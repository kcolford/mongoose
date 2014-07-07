/**
 * @file   transform.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief This is the routine that transforms different AST structures
 * into ones that make more sense for the low level passes.
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

#include <assert.h>

#define SWAP_AST(X, Y) do {			\
    SWAP ((X)->next, (Y)->next);		\
    SWAP (X, Y);				\
  } while (0)

static void
transform_r (struct ast **ss)
{
  assert (ss != NULL);
#define s (*ss)
  if (s == NULL)
    return;
  switch (s->type)
    {
    case binary_type:
      switch (s->op.binary.op)
	{
	  /* Turn this into an equality statement. */
	case NE:
	  s->op.binary.op = EQ;
	  s->boolean_not ^= 1;
	case EQ:
	case '<':
	case '>':
	case GE:
	case LE:
	  if (!s->noreturnint)
	    {
	      s->noreturnint = 1;
	      struct ast *t = make_cond_move (s,
					      make_integer (!s->boolean_not));
	      SWAP_AST (t, s);
	      transform_r (ss);
	    }
	  break;
	}
      break;

    case cond_type:
      s->ops[0]->noreturnint = 1;
      break;

    case function_call_type:
      /* Certain functions are considered builtin and thus require
	 special treatment. */
#define IS_BUILTIN(A, B)				\
      STREQ ((A)->ops[0]->loc->base, BUILTIN (B))
      if (IS_BUILTIN (s, alloca))
	{
	  struct ast *t = make_alloc (ast_dup (s->ops[1]));
	  SWAP_AST (t, s);
	  AST_FREE (t);
	}
    
    default:
      break;
    }
  int i;
  for (i = 0; i < s->num_ops; i++)
    transform_r (&s->ops[i]);
  transform_r (&s->next);
#undef s
}

int
transform (struct ast **ss)
{
  transform_r (ss);
  return 0;
}
