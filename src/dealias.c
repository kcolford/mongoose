/**
 * @file   dealias.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief This is the routine that translates variable names, etc. to
 * what they really mean.
 * 
 * Copyright (C) 2014 Kieran Colford
 *
 * This file is part of Mongoose.
 *
 * Mongoose is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mongoose is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mongoose; see the file COPYING.  If not see
 * <http://www.gnu.org/licenses/>.
 *
 * @note This pass makes use of gnulib's @c GL_RBTREE_LIST to ensure
 * O(log N) time for all name lookups and insertions.  This is a
 * significant inprovement on the earlier versions which had to
 * perform a linear search through an array to lookup a name.  An
 * alternative would be to use @c GL_AVLTREE_LIST (which is the same
 * but uses an AVL-Tree instead of an RB-Tree) or @c GL_*TREEHASH_LIST
 * (which is a hashtable but requires a good hashing function).
 * 
 */

#include "config.h"

#include "ast.h"
#include "ast_util.h"
#include "compiler.h"
#include "free.h"
#include "gl_rbtree_list.h"
#include "gl_xlist.h"
#include "lib.h"
#include "my_printf.h"
#include "parse.h"
#include "xalloc.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

/**
 * An entry in the state table.
 * 
 */

struct state_entry
{
  char *label;			/**< The label. */
  struct loc *meaning;		/**< The location that
				   state_entry::label becomes. */
};

static inline struct state_entry *
create_entry (const char *label, struct loc *meaning)
{
  struct state_entry *out = xmalloc (sizeof *out);
  out->label = xstrdup (label);
  out->meaning = loc_dup (meaning);
  return out;
}

static inline void
free_entry (const void *ss)
{
  struct state_entry *s = (struct state_entry *) ss;
  if (s != NULL)
    {
      FREE (s->label);
      FREE_LOC (s->meaning);
    }
  FREE (s);
}

static inline int
compare_entry (const void *a, const void *b)
{
  return strcmp (((const struct state_entry *) a)->label,
		 ((const struct state_entry *) b)->label);
}

static bool
eq_entry (const void *a, const void *b)
{
  return compare_entry (a, b) == 0;
}

/**
 * This is a stack like structure that stores a record of each mapping
 * from variable name to location.
 * 
 * The previous TODO entry here was fixed by using gnulib's @c
 * gl_list_t as the data type.  This allows us to use much higher
 * level functions on it than we previously had access to.  Such as a
 * search function (which only requires a function [eq_entry] to
 * describe equality among entries in the structure).
 * 
 */

struct state_stack
{
  struct state_stack *prev;	/**< The previous state. */
  gl_list_t state;
};

static inline struct state_stack *
create_state (struct state_stack *p)
{
  struct state_stack *s = xmalloc (sizeof *s);
  s->prev = p;
  s->state = gl_list_create_empty (GL_RBTREE_LIST, eq_entry, NULL,
				   free_entry, 0);
  return s;
}

static inline struct state_stack *
free_state (struct state_stack *s)
{
  if (s == NULL)
    return NULL;
  gl_list_free (s->state);
  struct state_stack *t = s->prev;
  FREE (s);
  return t;
}

static struct state_stack *state = NULL; /**< The current state
					    level. */

static int func_allocd = 0;	/**< The amount of memory that has so
				   far been allocated for the
				   function. */
static int curr_labelno = 1;	/**< The number of the next label that
				   will be identified. */

/**
 * Add a variable to the state, noting the amount of memory that is
 * allocated to it.
 *
 * @param v The variable name to be added.
 * @param s The size of @c v (the amount of memory to allocate).
 */
static inline void
add_to_state (const char *v, size_t s)
{
  func_allocd += s;
  struct loc *l;
  MAKE_BASE_LOC (l, memory_loc, xstrdup ("%rbp"));
  l->offset = -func_allocd;
  gl_sortedlist_add (state->state, compare_entry, create_entry (v, l));
  FREE_LOC (l);
}

/**
 * Search the state for an entry with the same key as l.  If one can't
 * be found, then it is an externally linked in symbol and is simply
 * returned as is.
 *
 * We now use the gnulib's @c gl_sortedlist_* functions to manage the
 * structure, and we have modified it to use the tree based @c
 * gl_list_t.  This gives our lookups and insertions O(log N) time,
 * and it can grow without restriction.
 *
 * @param l The variable name to access from the state.
 *
 * @return The location that @c l refers to.
 */
static inline struct loc *
get_from_state (char *l)
{
  struct state_stack *p;
  for (p = state; p != NULL; p = p->prev)
    {
      struct state_entry t = { 0 };
      t.label = l;
      gl_list_node_t n = gl_sortedlist_search (p->state, compare_entry, &t);
      if (n != NULL)
	return loc_dup (((struct state_entry *) 
			 gl_list_node_value (p->state, n))->meaning);
    }
  struct loc *s;
  MAKE_BASE_LOC (s, literal_loc, xstrdup (l));
  return s;
}

/**
 * A similar routine as above, but if a symbol meaning can't be found
 * then create one and return it.
 *
 * @see get_from_state
 */
static inline struct loc *
get_label (char *l)
{
  struct loc *s = get_from_state (l);
  if (s->kind != symbol_loc)
    {
      struct state_stack *p = state;
      /* Jump up to the function level state. */
      while (p->prev->prev != NULL)
	p = p->prev;

      /* Add the label. */
      s->kind = symbol_loc;
      FREE (s->base);
      s->base = my_printf (".LJ%d", curr_labelno++);
      gl_sortedlist_add (p->state, compare_entry, create_entry (l, s));
    }
  return s;
}

static void
dealias_r (struct ast **ss)
{
  assert (ss != NULL);
#define s (*ss)

  if (s == NULL)
    return;
  switch (s->type)
    {
    case block_type:
      state = create_state (state);
      dealias_r (&s->ops[0]);
      state = free_state (state);
      break;

    case function_type:
      func_allocd = 0;
      state = create_state (state);
      dealias_r (&s->ops[0]);
      dealias_r (&s->ops[1]);
      state = free_state (state);
      break;

    case variable_type:
      if (s->op.variable.type != NULL)
	{
	  s->next = ast_cat (make_alloc (make_integer (8)), s->next);
	  s->op.variable.alloc = 8;
	  add_to_state (s->op.variable.name, 8);
	}
      s->loc = get_from_state (s->op.variable.name);
      assert (s->loc != NULL);
      break;

    case label_type:
      s->loc = get_label (s->op.label.name);
      assert (s->loc != NULL);
      break;

    case jump_type:
      s->loc = get_label (s->op.jump.name);
      assert (s->loc != NULL);
      break;

    case cond_type:
      dealias_r (&s->ops[0]);
      s->loc = get_label (s->op.cond.name);
      assert (s->loc != NULL);
      break;	  

    default:
      ;
      int j;
      for (j = 0; j < s->num_ops; j++)
	dealias_r (&s->ops[j]);
    }
  dealias_r (&s->next);
}

int
dealias (struct ast **ss)
{
  /* Nullify the global vars. */
  free_state (state);
  state = create_state (NULL);
  func_allocd = 0;
  curr_labelno = 1;

  dealias_r (ss);
  return 0;
}
