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

#include <string.h>
#include <stdbool.h>
#include <assert.h>

struct state_entry
{
  char *label;
  char *meaning;
};

static struct state_entry state[0x10000] = { {NULL, NULL} };
static int state_end = 0;

static int func_allocd = 0;
static int curr_labelno = 1;

static inline void
clear_state ()
{
  memset (state, 0, sizeof state);
  state_end = 0;
  func_allocd = 0;
}

static inline void
add_to_state (char *v, size_t s)
{
  func_allocd += s;
  state[state_end].label = v;
  state[state_end].meaning = my_printf ("-%d(%%rbp)", func_allocd);
  ++state_end;
}

static inline char *
get_from_state (char *l)
{
  int i;
  for (i = state_end - 1; i >= 0; i--)
    {
      assert (state[i].label != NULL);
      if (strcmp (l, state[i].label) == 0)
	return state[i].meaning;
    }
  return l;
} 

static inline char *
get_label (char *l)
{
  char *s = get_from_state (l);
  if (*s != '.')
    {
      state[state_end].label = l;
      state[state_end].meaning = s = my_printf (".LJ%d", curr_labelno++);
      ++state_end;
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
    case function_type:
      clear_state ();
      dealias_r (&s->ops[0]);
      dealias_r (&s->ops[1]);
      clear_state ();
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
