/**
 * @file   dealias.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief This is the routine that translates variable names, etc. to
 * what they really mean.
 * 
 * Copyright (C) 2014 Kieran Colford
 *
 * This file is part of Compiler.
 *
 * Compiler is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Compiler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Compiler; see the file COPYING.  If not see
 * <http://www.gnu.org/licenses/>.
 * 
 */

#include "config.h"

#include "ast.h"
#include "compiler.h"
#include "lib.h"
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

/**
 * This is a stack like structure that stores a record of each mapping
 * from variable name to location.
 * 
 * @todo Our implementation only allows 0x10000 variable names per
 * scope and allocates space for that many variable names every time
 * it enters a new scope level.  This should be fixed to make the
 * variable name array more dynamic.
 * 
 */

struct state_stack
{
  struct state_stack *prev;	/**< The previous state. */
  int state_end;
  struct state_entry state[0x10000];
};

static inline struct state_stack *
create_state (struct state_stack *p)
{
  struct state_stack *s = xzalloc (sizeof *s);
  s->prev = p;
  return s;
}

static inline struct state_stack *
free_state (struct state_stack *s)
{
  if (s == NULL)
    return NULL;
  int i;
  for (i = 0; i < s->state_end; i++)
    {
      FREE (s->state[i].label);
      FREE_LOC (s->state[i].meaning);
    }
  struct state_stack *t = s->prev;
  FREE (s);
  return t;
}

static struct state_stack absolute_top = { 0 };	/**< The file level
						   state. */
static struct state_stack *state = &absolute_top; /**< The current
						     state level. */

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
  state->state[state->state_end].label = xstrdup (v);
  struct loc *l;
  MAKE_BASE_LOC (l, memory_loc, xstrdup ("%rbp"));
  l->offset = -func_allocd;
  state->state[state->state_end].meaning = l;
  state->state_end++;
}

/**
 * Linearly search the state array for an entry with the same key as
 * l.  If one can't be found, then it is an externally linked in
 * symbol and is simply returned as is.
 *
 * @todo We currently use a linear search to find the meaning of a
 * particular string.  This should be altered to use something more
 * scalable and more efficient.  Either Knuth's Algorithm T or
 * something similar would be ideal.
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
      int i;
      for (i = p->state_end - 1; i >= 0; i--)
	{
	  assert (p->state[i].label != NULL);
	  if (STREQ (l, p->state[i].label))
	    return loc_dup (p->state[i].meaning);
	}
    }
  struct loc *s;
  MAKE_BASE_LOC (s, literal_loc, xstrdup (l));
  return s;
}

/**
 * A similar routine as above, but if a symbol meaning can't be found
 * then create one and return it.
 *
 * @copydoc get_from_state
 */
static inline struct loc *
get_label (char *l)
{
  struct loc *s = get_from_state (l);
  if (*s->base != '.')
    {
      struct state_stack *p = state;
      /* Jump up to the function level state. */
      while (p->prev->prev != NULL)
	p = p->prev;

      /* Add the label. */
      p->state[p->state_end].label = xstrdup (l);
      s->kind = symbol_loc;
      FREE (s->base);
      s->base = my_printf (".LJ%d", curr_labelno++);
      p->state[p->state_end].meaning = loc_dup (s);
      p->state_end++;
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
  int i;
  struct loc *l;
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
  memset (&absolute_top, 0, sizeof absolute_top);
  state = &absolute_top;
  func_allocd = 0;
  curr_labelno = 1;

  dealias_r (ss);
  return 0;
}
