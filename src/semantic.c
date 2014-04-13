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

#if 0
struct state
{
  char *label;
  char *meaning;
};

struct state var_info[0x10000] = { {NULL, NULL} };
int var_info_off = 0;

int
semantic (struct ast *s)
{
  int ret = 0;
  if (s == NULL)
    return ret;
  switch (s->type)
    {
    case block_type:
      ret = ret || semantic (s->op.block.val);
      ret = ret || semantic (s->op.block.next);
      break;

    case function_type:
      break;
    }
  return ret;
}

#else

int
semantic (struct ast *s)
{
  return 0;
}

#endif
