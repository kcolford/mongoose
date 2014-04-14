/* This is the semantic analizer.

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
#include "parse.h"

#define ERROR(VAL, ...) do {			\
    if (!(VAL))					\
      {						\
	error (0, 0, __VA_ARGS__);		\
	return 1;				\
      }						\
  } while (0)


static inline int
is_lval (struct ast *s)
{
  switch (s->type)
    {
    case binary_type:
      return s->op.binary.op == '[';
    case unary_type:
      return s->op.unary.op == '*';
    case variable_type:
      return 1;
    default:
      return 0;
    }
}

#define CHECK_LVAL(VAL)							\
  ERROR (is_lval (VAL), _("Syntax Error, operand is not an lval"))

int
semantic (struct ast *s)
{
  int ret = 0;
  if (s == NULL)
    return ret;
  switch (s->type)
    {
    case binary_type:
      if (s->op.binary.op == '=')
	CHECK_LVAL (s->ops[0]);
      break;

    case unary_type:
      if (s->op.binary.op & ~AST_UNARY_PREFIX == INC
	  || s->op.binary.op & ~AST_UNARY_PREFIX == DEC)
	CHECK_LVAL (s->ops[0]);
      break;
    }
  int i;
  for (i = 0; i < s->num_ops; i++)
    ret = ret || semantic (s->ops[i]);
  ret = ret || semantic (s->next);
  return ret;
}
