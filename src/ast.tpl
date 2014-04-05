[+ AutoGen5 template -*- mode: c -*-
h 
c 
+]

/* This is the template file for the AST.

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

[+ CASE (suffix) +]

[+ == h +]
#ifndef AST_H
#define AST_H

#include <stddef.h>

struct ast
{
  enum { [+ FOR types ',
         ' +][+name+]_type[+ ENDFOR types +] } type;
  char *loc;
  union
  {
    [+ FOR types '
    '+]struct
    {
      [+ FOR cont '
      ' +][+type+] [+call+];[+ ENDFOR cont +]
    } [+name+];[+ ENDFOR types +]
  } op;
};

[+ FOR types '
' +]extern struct ast *make_[+name+] ([+ FOR cont ", " +][+type+][+ ENDFOR cont +]);[+ ENDFOR types +]

extern struct ast *make_whileloop (struct ast *, struct ast *);

#endif
[+ == c +]
#include "config.h"

#include "compiler.h"
#include "xalloc.h"

[+ FOR types +]
struct ast *
make_[+name+] ([+ FOR cont ',' +][+type+] [+call+][+ ENDFOR cont +])
{
  struct ast *out = xmalloc (sizeof *out);
  out->type = [+name+]_type;
  out->loc = NULL;
  [+ FOR cont ';
  ' +]out->op.[+name+].[+call+] = [+call+][+ ENDFOR cont +];
  return out;
}
[+ ENDFOR types +]
struct ast *
make_whileloop (struct ast *cond, struct ast *body)
{
  char *t = place_holder ();
  return make_label (t, make_cond (cond, make_block (body, make_jump (t))));
}
[+ ESAC +]
