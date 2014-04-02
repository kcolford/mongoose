#include "config.h"

#include "compiler.h"

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <errno.h>
#include <error.h>

#include <assert.h>

#define PUT(...) do {				\
    fprintf (outfile, __VA_ARGS__);		\
    if (debug)					\
      fprintf (stderr, __VA_ARGS__);		\
  } while (0)

/* Incase of error, fail and then jump out to the top level to return
   an error value.  This allows us to continue the program as we
   desire. */
jmp_buf error_jump;
#define ERROR(...)				\
  do {						\
    error (0, errno, __VA_ARGS__);		\
    longjmp (error_jump, 1);			\
  } while (0)

const char *
regis (int a)
{
  const char *storage[] =
    { "%rax", "%rbx", "%rcx", "%rdx", "%rdi", "%rsi", "%r8", "%r9", "%r10", 
      "%r11", "%r12", "%r13", "%r14", "%r15" };
  assert (a >= 0);
  assert (a < sizeof storage / sizeof *storage);
  return storage[a];
}

/* These are the order of registers that function arguments are passed
   through.  Currently, they seriously conflict with the general-use
   registers and a function call in the middle of an expression is
   likely to fail. */
const int 
call_regis(int a)
{
  const int storage[] = 
    { 4, 5, 2, 3, 6, 7 };
  assert (a >= 0);
  assert (a < sizeof storage / sizeof *storage);
  return storage[a];
}

/* These registers are available for use in expression calculation.
   Registers which are needed for special purposes have been left out.
   These include: %rax, %rdx, and %rcx. 

   The idea to make the allocation behave like a stack (and operations
   are done like an RPN calculator) so that we only need to increment
   and decrement the avail variable to check which register we should
   use. */
const int 
general_regis(int a)
{
  const int storage[] = 
    { 1, 8, 9, 10, 11, 12, 13
#if 1
      , 7, 6, 3, 2, 5, 4 
#endif
    };
  assert (a >= 0);
  assert (a < sizeof storage / sizeof *storage);
  return storage[a];
}

int avail = 0;

struct state_entry
{
  char *label;
  char *meaning;
};

int state_end = 0;
struct state_entry state[0x10000] = { {NULL, NULL} };

int func_allocd = 0;
int curr_labelno = 1;

char *data_section = NULL;

void
clear_state ()
{
  memset (state, 0, sizeof state);
  state_end = 0;
  func_allocd = 0;
  avail = 0;
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
      state[state_end].meaning = s = my_printf (".L%d", curr_labelno++);
      ++state_end;
    }
  return s;
}

char *
give_register_how (const char *i, const char *s)
{
  PUT ("\t%s\t%s, %s\n", i, s, regis(general_regis(avail)));
  return xstrdup (regis(general_regis(avail++)));
}

#define give_register(s)			\
  give_register_how ("mov", s)

#define SWAP(X, Y) do {				\
    void *_t = (X);				\
    (X) = (Y);					\
    (Y) = _t;					\
  } while (0)

/* This macro makes branching a whole lot easier. */
#define BRANCH_WITH_CODE_BIN(X, Y) do {				\
    gen_code_r (s->op.cond.cond->op.binary.left);		\
    gen_code_r (s->op.cond.cond->op.binary.right);		\
    if (*s->op.cond.cond->op.binary.left->loc == '$')		\
      s->op.cond.cond->op.binary.left->loc =			\
	give_register (s->op.cond.cond->op.binary.left->loc);	\
    PUT ("\t%s\t%s, %s\n\t%s\t.L%d\n", (X),			\
	 s->op.cond.cond->op.binary.right->loc,			\
	 s->op.cond.cond->op.binary.left->loc, (Y), n);		\
    if (*s->op.cond.cond->op.binary.left->loc == '%')		\
      avail--;							\
    gen_code_r (s->op.cond.body);				\
    PUT (".L%d:\n", n);						\
  } while (0)

#define ENSURE_DESTINATION_REGISTER(N, X, Y)	\
  ENSURE_DESTINATION_REGISTER##N (X, Y)

#define ENSURE_DESTINATION_REGISTER_UNI(X) do {	\
    if (*(X) != '%')				\
      (X) = give_register (X);			\
  } while (0)

/* Designed for the addition and subtraction operations.  These
   require one of the destination/source operands to be in a register
   while the other can be memory or (if it's the source operand) an
   immediate. */
#define ENSURE_DESTINATION_REGISTER1(X, Y) do {	\
    if (*(X) != '%')				\
      {						\
	if (*(Y) != '%')			\
	  (X) = give_register ((X));		\
	else					\
	  SWAP (X, Y);				\
      }						\
  } while (0)

/* Designed for the multiplication, division, and modulo operations.
   These require the %rdx register to be 0, the left hand argument to
   be in %rax and the result will be left in %rax or %rdx depending on
   the operation. */
#define ENSURE_DESTINATION_REGISTER2(X, Y) do {	\
    ENSURE_DESTINATION_REGISTER1 (X, Y);	\
    if (*(Y) != '%')				\
      (Y) = give_register ((Y));		\
  } while (0)

/* Designed for use with the right and left shift operators.  These
   require the right-hand argument to either be and immediate value or
   placed in the %cl register. */
#define ENSURE_DESTINATION_REGISTER3(X, Y) do {	\
    if (*(Y) != '$')				\
      {						\
	PUT ("\tmov\t%s, %%rcx\n", (Y));	\
	(Y) = "%cl";				\
      }						\
    if (*(X) == '$')				\
      (X) = give_register (X);			\
  } while (0)

void
gen_code_r (struct ast *s)
{
  if (s == NULL)
    return;
  switch (s->type)
    {
    case block_type:
      gen_code_r (s->op.block.val);
      gen_code_r (s->op.block.next);
      break;

    case function_type:
      /* We need a clean state for the function's variables. */
      clear_state ();

      /* Enter the .text section and declare this symbol as global. */
      PUT ("\t.global\t%s\n", s->op.function.name);
      PUT ("%s:\n", s->op.function.name);

      /* Set up the stack frame. */
      PUT ("\tpush\t%%rbp\n\tmov\t%%rsp, %%rbp\n");
      
      /* Walk over the list of arguments and push them into the
	 stack. */
      struct ast *i;
      int argnum = 0;
      for (i = s->op.function.args; i != NULL; i = i->op.block.next)
	{
	  struct ast *v = i->op.block.val;
	  add_to_state (v->op.variable.name, 8);
	  PUT ("\tsub\t$8, %%rsp\n");
	  PUT ("\tmov\t%s, %s\n", regis(call_regis(argnum)), 
	       get_from_state (v->op.variable.name));
	  argnum++;
	}

      /* Generate the body of the function. */
      gen_code_r (s->op.function.body);
#if 0
      /* Generate a second function epilog just incase a
	 return-statement wasn't included in the input source.  This
	 can be toggled for testing. */
      PUT ("\tmov\t%%rbp, %%rsp\n\tpop\t%%rbp\n\tret\n\n");
#endif
      clear_state ();
      break;

    case ret_type:
      /* Move the return value into the %rax register. */
      if (s->op.ret.val != NULL)
	{
	  gen_code_r (s->op.ret.val);
	  PUT ("\tmov\t%s, %%rax\n", s->op.ret.val->loc);
	}
      /* Function footer. */
      PUT ("\tmov\t%%rbp, %%rsp\n\tpop\t%%rbp\n\tret\n");
      break;

    case cond_type:
      ;
      int n;
      n = curr_labelno++;
      if (s->op.cond.cond->type == binary_type)
	{
	  switch (s->op.cond.cond->op.binary.op)
	    {
	    case '<':
	      BRANCH_WITH_CODE_BIN ("cmp", "jnl");
	      break;
	    case LE:
	      BRANCH_WITH_CODE_BIN ("cmp", "jnle");
	      break;
	    case '>':
	      BRANCH_WITH_CODE_BIN ("cmp", "jng");
	      break;
	    case GE:
	      BRANCH_WITH_CODE_BIN ("cmp", "jnge");
	      break;
	    case EQ:
	      BRANCH_WITH_CODE_BIN ("cmp", "jne");
	      break;
	    case NE:
	      BRANCH_WITH_CODE_BIN ("cmp", "je");
	      break;
	    default:
	      goto alt_condition;
	    }
	}
      else
	{
	alt_condition:
	  /* When in doubt, the C standard requires that if an
	     expression evaluates to 0 then it is false, otherwise it
	     is true. */
	  gen_code_r (s->op.cond.cond);
	  PUT ("\tcmp\t%s, $0\n\tjz\t.L%d\n", s->op.cond.cond->loc, n);
	  gen_code_r (s->op.cond.body);
	  PUT (".L%d:\n", n);
	}
      break;

    case label_type:
      PUT ("%s:\n", get_label (s->op.label.name));
      gen_code_r (s->op.label.stuff);
      break;

    case jump_type:
      PUT ("\tjmp\t%s\n", get_label (s->op.jump.name));
      break;

    case integer_type:
      s->loc = my_printf ("$%lld", s->op.integer.i);
      break;

    case variable_type:
      if (s->op.variable.type != NULL)
	{
	  add_to_state (s->op.variable.name, 8);
	  PUT ("\tsub\t$8, %%rsp\n");
	}
      s->loc = get_from_state (s->op.variable.name);
      break;

    case string_type:
      s->loc = my_printf ("$.L%d", curr_labelno++);
      char *out = my_printf ("%s:\n\t.string\t\"%s\"\n", s->loc + 1, 
			     s->op.string.val);
      data_section = my_strcat (data_section, out);
      break;

    case binary_type:
      gen_code_r (s->op.binary.left);
      gen_code_r (s->op.binary.right);
      s->loc = s->op.binary.left->loc;
      struct ast *from = s->op.binary.right;
      switch (s->op.binary.op)
	{
	case '=':
	  /* Either the source or the destination must be a
	     register/immediate, if that's not the case, then we have
	     to move the source operand into a register first. */
	  if (*from->loc != '%' && *from->loc != '$')
	    from->loc = give_register (from->loc);
	  PUT ("\tmovq\t%s, %s\n", from->loc, s->loc);
	  break;

	case '&':
	  ENSURE_DESTINATION_REGISTER (1, s->loc, from->loc);
	  PUT ("\tand\t%s, %s\n", from->loc, s->loc);
	  break;

	case '|':
	  ENSURE_DESTINATION_REGISTER (1, s->loc, from->loc);
	  PUT ("\tor\t%s, %s\n", from->loc, s->loc);
	  break;

	case '^':
	  ENSURE_DESTINATION_REGISTER (1, s->loc, from->loc);
	  PUT ("\txor\t%s, %s\n", from->loc, s->loc);
	  break;

	case '+':
	  ENSURE_DESTINATION_REGISTER (1, s->loc, from->loc);
	  PUT ("\tadd\t%s, %s\n", from->loc, s->loc);
	  break;

	case '-':
	  ENSURE_DESTINATION_REGISTER (1, s->loc, from->loc);
	  PUT ("\tsub\t%s, %s\n", from->loc, s->loc);
	  if (from != s->op.binary.right)
	    PUT ("\tneg\t%s\n", s->loc);
	  break;

	case '*':
	  ENSURE_DESTINATION_REGISTER (1, s->loc, from->loc);
	  PUT ("\tmov\t%s, %%rax\n\timul\t%s\n\tmov\t%%rax, %s\n", 
	       from->loc, s->loc, s->loc);
	  break;

	case '/':
	  PUT ("\tmov\t%s, %%rax\n\tmov\t$0, %%rdx\n", s->loc);
	  ENSURE_DESTINATION_REGISTER (2, s->loc, from->loc);
	  PUT ("\tidiv\t%s\n\tmov\t%%rax, %s\n", from->loc, s->loc);
	  break;

	case '%':
	  PUT ("\tmov\t%s, %%rax\n\tmov\t$0, %%rdx\n", s->loc);
	  ENSURE_DESTINATION_REGISTER (2, s->loc, from->loc);
	  PUT ("\tidiv\t%s\n", from->loc);
	  PUT ("\tmov\t%%rdx, %s\n", s->loc);
	  break;

	case RS:
	  ENSURE_DESTINATION_REGISTER (3, s->loc, from->loc);
	  PUT ("\tshr\t%s, %s\n", from->loc, s->loc);
	  from->loc = ""; 	/* Do this so that we don't free a
				   register when we haven't allocated
				   one. */
	  break;

	case LS:
	  ENSURE_DESTINATION_REGISTER (3, s->loc, from->loc);
	  PUT ("\tshl\t%s, %s\n", from->loc, s->loc);
	  from->loc = ""; 	/* Do this so that we don't free a
				   register when we haven't allocated
				   one. */
	  break;

	case '[':
	  ERROR ("Not Implemented: '['");
	  break;

	default:
	  ERROR ("Invalid binary operator opcode: %d\n", 
		 s->op.binary.op);
	}
      /* Release the previously allocated register. */
      if (*from->loc == '%')
	avail--;
      break;

    case unary_type:
      gen_code_r (s->op.unary.arg);
      s->loc = s->op.unary.arg->loc;
      switch (s->op.unary.op)
	{
	case '*':
	  ENSURE_DESTINATION_REGISTER_UNI (s->loc);
	  s->loc = my_printf ("(%s)", s->loc);
	  break;

	case '&':
	  assert (*s->loc != '%');
#if 1
	  assert (*s->loc != '$');
#endif
	  if (*s->loc == '$')
	    s->loc = s->loc + 1;
	  else
	    s->loc = give_register_how ("lea", s->loc);
	  break;

	case '-':
	  ENSURE_DESTINATION_REGISTER_UNI (s->loc);
	  PUT ("\tnegq\t%s\n", s->loc);
	  break;

	default:
	  ERROR ("Invalid unary operator opcode: %d\n", s->op.unary.op);
	}
      break;

    case crement_type:
      gen_code_r (s->op.crement.val);
      s->loc = s->op.crement.val->loc;
      if (!s->op.crement.isprefix)
	s->loc = give_register (s->loc);
      if (s->op.crement.isincrease)
	PUT ("\tinc\t%s\n", s->op.crement.val->loc);
      else
	PUT ("\tdec\t%s\n", s->op.crement.val->loc);
      break;	      

    case function_call_type:
      ;
      /* TODO: Don't clobber other registers when making a function
	       call. */
      int a = 0;
      for (i = s->op.function_call.args; i != NULL; i = i->op.block.next)
	gen_code_r (i->op.block.val);
      for (i = s->op.function_call.args; i != NULL; i = i->op.block.next)
	{
	  PUT ("\tmov\t%s, %s\n", i->op.block.val->loc, regis(call_regis(a++)));
	  if (*i->op.block.val->loc == '%')
	    avail--;
	}
      assert (s->op.function_call.name->type == variable_type);
      PUT ("\tmov\t$0, %%rax\n\tcall\t%s\n", s->op.function_call.name->op.variable.name);
      s->loc = "%rax";
      s->loc = give_register (s->loc);
      break;

    case statement_type:
      gen_code_r (s->op.statement.val);
      if (*s->op.statement.val->loc == '%')
	avail--;
      break;

    default:
      ERROR ("Invalid AST type %d", s->type);
    }
}

/* Top level entry point to the code generation phase. */
int 
gen_code (struct ast *s)
{
  if (setjmp (error_jump))
    return 1;
  PUT ("\t.text\n");
  data_section = xstrdup ("\t.data\n");
  gen_code_r (s);
  PUT ("%s", data_section);
  xfree (data_section);
  return 0;
}
