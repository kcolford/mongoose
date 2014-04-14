/* This is the routine that translates variable names, etc. to what
   they really mean.

Copyright (C) 2014 Kieran Colford

This file is part of Compiler.

Compiler is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Compiler is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with Compiler; see the file COPYING.  If not see
<http://www.gnu.org/licenses/>. */

#include "config.h"

#include "ast.h"
#include "compiler.h"
#include "lib.h"
#include "parse.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

struct state_entry
{
  char *label;
  char *meaning;
};

struct state_stack
{
  struct state_stack *prev;
  int state_end;
  struct state_entry state[0x10000];
};

static struct state_stack absolute_top = { NULL, 0 };
static struct state_stack *state = &absolute_top;

static int func_allocd = 0;
static int curr_labelno = 1;

/* Add a variable to the state, noting the amount of memory that is
   allocated to it. */
static inline void
add_to_state (char *v, size_t s)
{
  func_allocd += s;
  state->state[state->state_end].label = v;
  state->state[state->state_end].meaning = my_printf ("-%d(%%rbp)", func_allocd);
  state->state_end++;
}

/* Linearly search the state array for an entry with the same key as
   l.  If one can't be found, then it is an externally linked in
   symbol and is simply returned as is. */
static inline char *
get_from_state (char *l)
{
  struct state_stack *p;
  for (p = state; p != NULL; p = p->prev)
    {
      int i;
      for (i = p->state_end - 1; i >= 0; i--)
	{
	  assert (p->state[i].label != NULL);
	  if (strcmp (l, p->state[i].label) == 0)
	    return p->state[i].meaning;
	}
    }
  return l;
} 

/* A similar routine as above, but if a symbol meaning can't be found
   then create one and return it. */
static inline char *
get_label (char *l)
{
  char *s = get_from_state (l);
  if (*s != '.')
    {
      struct state_stack *p = state;
      /* Jump up to the function level state. */
      while (p->prev->prev != NULL)
	p = p->prev;

      /* Add the label. */
      p->state[p->state_end].label = l;
      p->state[p->state_end].meaning = s = my_printf (".LJ%d", curr_labelno++);
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
  struct state_stack *t;
  int i;
  switch (s->type)
    {
    case block_type:
      t = alloca (sizeof *t);
      t->prev = state;
      t->state_end = 0;
      state = t;
      dealias_r (&s->ops[0]);
      state = state->prev;
      break;

    case function_type:
      func_allocd = 0;
      t = alloca (sizeof *t);
      t->prev = state;
      t->state_end = 0;
      state = t;
      dealias_r (&s->ops[0]);
      dealias_r (&s->ops[1]);
      state = state->prev;
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
  dealias_r (ss);
  return 0;
}
