#include "config.h"

#include "compiler.h"

int
run_compilation_passes (struct ast **ss)
{
  int ret = 0;
  ret = ret || semantic (*ss);
  ret = ret || dealias (ss);
  ret = ret || optimizer (ss);
  ret = ret || gen_code (*ss);
  return ret;
}
