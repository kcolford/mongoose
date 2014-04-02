#include "compiler.h"

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
