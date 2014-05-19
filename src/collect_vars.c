/**
 * @file   collect_vars.c
 * @author Kieran Colford <colfordk@gmail.com>
 * @date   Mon May 19 11:02:15 2014
 * 
 * @brief This is compiler pass that collects all the variable
 * declarations at the top of the function.
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
 * @note This pass should be run after the dealias pass so that we
 * preserve the scoping of variables.
 *
 */

#include "config.h"

#include "ast.h"
#include "ast_util.h"
#include "compiler.h"
#include "lib.h"
#include "xalloc.h"

#include <assert.h>

/** 
 * Recursive version of @c collect_vars.
 * 
 * @param s Input structure.
 * 
 * @return Concatenation of just the allocation routines that have
 * been collected.
 */
static struct ast *
collect_vars_r (struct ast *s)
{
  if (s == NULL)
    return NULL;
  struct ast *vars = NULL;
  switch (s->type)
    {
    case function_type:
      assert (s->ops[1]->type == block_type);
      struct ast **t = &s->ops[1]->ops[0];
      *t = ast_cat (collect_vars_r (*t), *t);
      break;

    case variable_type:
      if (s->op.variable.alloc != 0)
	{
	  assert (s->op.variable.type != NULL);
	  vars = make_variable (xstrdup (s->op.variable.type),
				xstrdup (s->op.variable.name));
	  vars->op.variable.alloc = s->op.variable.alloc;
	  s->op.variable.alloc = 0;
	}
      break;

    default:
      ;
      int i;
      for (i = 0; i < s->num_ops; i++)
	vars = ast_cat (vars, collect_vars_r (s->ops[i]));
    }
  vars = ast_cat (vars, collect_vars_r (s->next));
  return vars;
}

int
collect_vars (struct ast *s)
{
  collect_vars_r (s);
  return 0;
}
