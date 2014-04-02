[+ AutoGen5 template h c +]
/* -*- c-mode -*- */
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
