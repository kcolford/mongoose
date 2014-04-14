/* This is the routine that runs all the different passes in the
   correct order.

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

static void
destroy_ast (struct ast *s)
{
  while (s != NULL)
    {
      switch (s->type)
	{
	case function_type:
	  FREE (s->op.function.type);
	  FREE (s->op.function.name);
	  break;

	case variable_type:
	  FREE (s->op.variable.type);
	  FREE (s->op.variable.name);
	  break;

	case string_type:
	  FREE (s->op.string.val);
	  break;

	case label_type:
	  FREE (s->op.label.name);
	  break;

	case jump_type:
	  FREE (s->op.jump.name);
	  break;
	}
      int i;
      for (i = 0; i < s->num_ops; i++)
	destroy_ast (s->ops[i]);
      if (s->flags & AST_THROW_AWAY)
	FREE (s->loc);
      struct ast *t = s->next;
      FREE (s);
      s = t;
    }
}

int
run_compilation_passes (struct ast **ss)
{
  int ret = 0;
  ret = ret || semantic (*ss);
  ret = ret || dealias (ss);
  ret = ret || optimizer (ss);
  ret = ret || gen_code (*ss);
  destroy_ast (*ss);
  return ret;
}
