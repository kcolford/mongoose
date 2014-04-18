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

[+added_code+]

struct ast
{
  enum { [+ FOR types ',
         ' +][+name+]_type[+ ENDFOR types +] } type;
  [+ FOR top_level +]
  [+type+] [+call+];
  [+ ENDFOR top_level +];
  union
  {
    [+ FOR types +]
    [+ IF (or (exist? "cont") (exist? "extra")) +]
    struct
    {
      [+ FOR cont +]
      [+type+] [+call+];
      [+ ENDFOR cont +]
      [+ FOR extra +]
      [+type+] [+call+];
      [+ ENDFOR extra +]
    } [+name+];
    [+ ENDIF +]
    [+ ENDFOR types +]
  } op;
  const int num_ops;
  struct ast *ops[1];
};

[+ FOR types +]
extern struct ast *make_[+name+] ([+ FOR cont ', ' +][+type+][+ ENDFOR cont +]
				  [+ IF (and (exist? "cont") (exist? "sub")) +], [+ ENDIF +]
				  [+ FOR sub ', ' +]struct ast *[+ ENDFOR sub +]);
[+ ENDFOR types +]

extern struct ast *ast_dup (const struct ast *);
extern void ast_free (struct ast *);

#define AST_FREE(S) do {			\
    ast_free (S);				\
    (S) = NULL;					\
  } while (0)

#endif
[+ == c +]
#include "config.h"

#include "ast.h"
#include "compiler.h"
#include "lib.h"
#include "xalloc.h"

#include <stdlib.h>

[+ FOR types +]
struct ast *
make_[+name+] ([+ FOR cont ', ' +][+type+] [+call+][+ ENDFOR cont +]
	       [+ IF (and (exist? "cont") (exist? "sub")) +], [+ ENDIF +]
	       [+ FOR sub ', ' +]struct ast *[+sub+][+ ENDFOR sub +])
{
  struct ast template = { [+name+]_type[+ FOR top_level +], ([+type+]) 0[+ ENDFOR +], 
			  {0}, [+ (count "sub") +], NULL };
  struct ast *out = xmemdup (&template, sizeof *out + sizeof out->ops[0] * ([+ (count "sub") +] - 1));
  [+ FOR extra +]
    out->op.[+name+].[+call+] = ([+type+]) 0;
  [+ ENDFOR extra +];
  [+ FOR cont +]
    out->op.[+name+].[+call+] = [+call+];
  [+ ENDFOR cont +];
  [+ FOR sub +]
    out->ops[[+ (for-index) +]] = [+sub+];
  [+ ENDFOR sub +];
  return out;
}
[+ ENDFOR types +]

#define USE_RETURN(X, F) if ((X) != NULL) (X) = F (X)

struct ast *
ast_dup (const struct ast *s)
{
  struct ast *out = xmemdup (s, sizeof *s + sizeof s->ops[0] * (s->num_ops - 1));
  [+ FOR top_level +]
    [+ IF (== "char *" (get "type")) +]
    USE_RETURN (out->[+call+], xstrdup);
  [+ ELIF (== "struct ast *" (get "type")) +]
    USE_RETURN (out->[+call+], ast_dup);
  [+ ENDIF +]
    [+ ENDFOR top_level+];
  switch (out->type)
    {
      [+ FOR types +]
    case [+name+]_type:
      [+ FOR cont +]
	[+ IF (== "char *" (get "type")) +]
	USE_RETURN (out->op.[+name+].[+call+], xstrdup);
      [+ ENDIF +]
	[+ ENDFOR cont +]
	break;
      [+ ENDFOR types+]
    }
  int i;
  for (i = 0; i < out->num_ops; i++)
    USE_RETURN (out->ops[i], ast_dup);
  return out;
}

void
ast_free (struct ast *s)
{
  if (s == NULL)
    return;
  [+ FOR top_level +]
    [+ IF (== "char *" (get "type")) +]
    FREE (s->[+call+]);
  [+ ELIF (== "struct ast *" (get "type")) +]
    AST_FREE (s->[+call+]);
  [+ ENDIF +]
    [+ ENDFOR top_level +];
  switch (s->type)
    {
      [+ FOR types +]
    case [+name+]_type:
      [+ FOR cont +]
	[+ IF (== "char *" (get "type")) +]
	FREE (s->op.[+name+].[+call+]);
      [+ ENDIF +]
	[+ ENDFOR cont +]
	break;
      [+ ENDFOR types +]
	}
  int i;
  for (i = 0; i < s->num_ops; i++)
    AST_FREE (s->ops[i]);
  FREE (s);
}

[+ ESAC +]
