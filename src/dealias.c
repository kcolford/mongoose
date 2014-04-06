#include "config.h"

#include "compiler.h"

#include <string.h>
#include <stdbool.h>
#include <assert.h>

struct state_entry
{
  char *label;
  char *meaning;
};

struct state_entry state[0x10000] = { {NULL, NULL} };
int state_end = 0;

int func_allocd = 0;
int curr_labelno = 1;

void
clear_state ()
{
  memset (state, 0, sizeof state);
  state_end = 0;
  func_allocd = 0;
}

void
add_to_state (char *v, size_t s)
{
  func_allocd += s;
  state[state_end].label = v;
  state[state_end].meaning = my_printf ("-%d(%%rbp)", func_allocd);
  ++state_end;
}

char *
get_from_state (char *l)
{
  int i;
  for (i = state_end - 1; i >= 0; i--)
    {
      assert (state[i].label != NULL);
      if (strcmp (l, state[i].label) == 0)
	return state[i].meaning;
    }
  return l;
} 

char *
get_label (char *l)
{
  char *s = get_from_state (l);
  if (*s != '.')
    {
      state[state_end].label = l;
      state[state_end].meaning = s = my_printf (".LJ%d", curr_labelno++);
      ++state_end;
    }
  return s;
}

void
dealias_r (struct ast **ss)
{
  int done_recursive = 0;
  assert (ss != NULL);
#define s (*ss)

#define DO_RECURSIVE do {			\
    if (!done_recursive)			\
      dealias_r (&s->next);			\
    done_recursive = 1;				\
  } while (0)

  if (s == NULL)
    return;
  switch (s->type)
    {
    case block_type:
      dealias_r (&s->op.block.val);
      break;
      
    case function_type:
      clear_state ();
      dealias_r (&s->op.function.args);
      DO_RECURSIVE;
      clear_state ();
      break;

    case ret_type:
      dealias_r (&s->op.ret.val);
      break;

    case cond_type:
      dealias_r (&s->op.cond.cond);
      break;

    case variable_type:
      if (s->op.variable.type != NULL)
	{
	  s->op.variable.alloc = 8;
	  add_to_state (s->op.variable.name, 8);
	}
      s->loc = get_from_state (s->op.variable.name);
      assert (s->loc != NULL);
      break;

    case label_type:
      s->loc = get_label (s->op.label.name);
      assert (s->loc != NULL);
      break;
      
    case jump_type:
      s->loc = get_label (s->op.jump.name);
      assert (s->loc != NULL);
      break;

    case integer_type:
      break;

    case string_type:
      break;

    case binary_type:
      dealias_r (&s->op.binary.left);
      dealias_r (&s->op.binary.right);
      break;

    case unary_type:
      dealias_r (&s->op.unary.arg);
      break;
      
    case function_call_type:
      dealias_r (&s->op.function_call.args);
      dealias_r (&s->op.function_call.name);
      break;

#ifndef NDEBUG
    default:
      error (1, errno, _("default case, this shouldn't happen..."));
      break;
#endif
    }
  DO_RECURSIVE;
}

int
dealias (struct ast **ss)
{
  dealias_r (ss);
  return 0;
}
