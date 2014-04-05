#include "config.h"

#include "compiler.h"
#include "gl_oset.h"
#include "gl_avltree_oset.h"
#include "xalloc.h"

#include <stdbool.h>
#include <assert.h>

struct cons
{
  gl_oset_t tree;
  struct cons *prev;
};

#define CREATE_CONS(ALIAS, PREV)					\
  struct cons ALIAS;							\
  (ALIAS).tree = gl_oset_create_empty (GL_AVLTREE_OSET, NULL, NULL);	\
	 (ALIAS).prev = (PREV)

bool
contains (struct cons *c, struct ast *s)
{
  while (c != NULL)
    {
      if (gl_oset_search (c->tree, s))
	return true;
      else
	c = c->prev;
    }
  return false;
}

void
dealias_r (struct ast **ss, struct cons *c)
{
  assert (ss != NULL);
#define s (*ss)
  if (s == NULL)
    return;
  switch (s->type)
    {
    case block_type:
      dealias_r (&s->op.block.val, c);
      dealias_r (&s->op.block.next, c);
      break;
      
    case function_type:
      CREATE_CONS (cc, c);
      dealias_r (&s->op.function.args, &cc);
      dealias_r (&s->op.funciton.body, &cc);
      gl_oset_free (cc.tree);
      break;

    case variable_type:
      if (s->op.variable.type != NULL)
	{
	  
