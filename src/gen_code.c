/* This is the code generation routine of the compiler.

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

#include "compiler.h"
#include "xalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <errno.h>

#include <error.h>

#include <assert.h>

#define IS_REGISTER(S) ((S) != NULL && *((const char *) S) == '%')
#define IS_LITERAL(S) ((S) != NULL && *((const char *) S) == '$')
#define IS_MEMORY(S) ((S) != NULL && !IS_REGISTER (S) && !IS_LITERAL (S))

#define ALLOC_REGISTER(X) do {				\
    (X) = xstrdup (regis (general_regis (avail++)));	\
  } while (0)

#define FREE_REGISTER(X) do {			\
    if (IS_REGISTER (X))			\
      avail -= 1;				\
  } while (0);

#define PUT(...) do {				\
    fprintf (outfile, __VA_ARGS__);		\
    if (debug)					\
      fprintf (stderr, __VA_ARGS__);		\
  } while (0)

/* In case of error, fail and then jump out to the top level to return
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

int str_labelno = 0;
char *data_section = NULL;

int branch_labelno = 0;

char *
give_register_how (const char *i, const char *s)
{
  char *r;
  ALLOC_REGISTER (r);
  PUT ("\t%s\t%s, %s\n", i, s, r);
  return r;
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
    if (IS_LITERAL (s->op.cond.cond->op.binary.left->loc))	\
      s->op.cond.cond->op.binary.left->loc =			\
	give_register (s->op.cond.cond->op.binary.left->loc);	\
    PUT ("\t%s\t%s, %s\n\t%s\t.LB%d\n", (X),			\
	 s->op.cond.cond->op.binary.right->loc,			\
	 s->op.cond.cond->op.binary.left->loc, (Y), n);		\
    FREE_REGISTER (s->op.cond.cond->op.binary.left->loc);	\
    DO_RECURSIVE;						\
    PUT (".LB%d:\n", n);					\
  } while (0)

#define ENSURE_DESTINATION_REGISTER(N, X, Y)	\
  ENSURE_DESTINATION_REGISTER##N (X, Y)

#define ENSURE_DESTINATION_REGISTER_UNI(X) do {	\
    if (!IS_REGISTER (X))			\
      (X) = give_register (X);			\
  } while (0)

/* Designed for the addition and subtraction operations.  These
   require one of the destination/source operands to be in a register
   while the other can be memory or (if it's the source operand) an
   immediate. */
#define ENSURE_DESTINATION_REGISTER1(X, Y) do {	\
    if (!IS_REGISTER (X))			\
      {						\
	if (!IS_REGISTER (Y))			\
	  (X) = give_register (X);		\
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
    if (!IS_REGISTER (Y))			\
      (Y) = give_register (Y);			\
  } while (0)

/* Designed for use with the right and left shift operators.  These
   require the right-hand argument to either be and immediate value or
   placed in the %cl register. */
#define ENSURE_DESTINATION_REGISTER3(X, Y) do {	\
    if (!IS_LITERAL (Y))			\
      {						\
	PUT ("\tmov\t%s, %%rcx\n", (Y));	\
	(Y) = "%cl";				\
      }						\
    if (IS_LITERAL (X))				\
      (X) = give_register (X);			\
  } while (0)

void
gen_code_r (struct ast *s)
{
  int done_next = 0;

#define DO_RECURSIVE do {			\
    if (!done_next)				\
      gen_code_r (s->next);			\
    done_next = 1;				\
  } while (0)

  if (s == NULL)
    return;
  switch (s->type)
    {
    case block_type:
      gen_code_r (s->op.block.val);
      break;

    case function_type:
      /* Enter the .text section and declare this symbol as global. */
      PUT ("\t.global\t%s\n", s->op.function.name);
      PUT ("%s:\n", s->op.function.name);

      /* Set up the stack frame. */
      PUT ("\tpush\t%%rbp\n\tmov\t%%rsp, %%rbp\n");
      
      /* Walk over the list of arguments and push them into the
	 stack. */
      struct ast *i;
      int argnum = 0;
      for (i = s->op.function.args; i != NULL; i = i->next)
	{
	  assert (i->type == variable_type);
	  PUT ("\tsub\t$%d, %%rsp\n", i->op.variable.alloc);
	  PUT ("\tmov\t%s, %s\n", regis(call_regis(argnum)), i->loc);
	  argnum++;
	}

      /* Generate the body of the function. */
      DO_RECURSIVE;
#if 0
      assert (avail == 0);
#endif
#if 0
      /* Generate a second function epilogue just in case a
	 return-statement wasn't included in the input source.  This
	 can be toggled for testing. */
      PUT ("\tmov\t%%rbp, %%rsp\n\tpop\t%%rbp\n\tret\n\n");
#endif
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
      n = branch_labelno++;
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
	  PUT ("\tcmp\t%s, $0\n\tjz\t.LB%d\n", s->op.cond.cond->loc, n);
	  DO_RECURSIVE;
	  PUT (".LB%d:\n", n);
	}
      break;

    case label_type:
      PUT ("%s:\n", s->loc);
      break;

    case jump_type:
      PUT ("\tjmp\t%s\n", s->loc);
      break;

    case integer_type:
      s->loc = my_printf ("$%lld", s->op.integer.i);
      break;

    case variable_type:
      if (s->op.variable.type != NULL && s->op.variable.alloc != 0)
	PUT ("\tsub\t$%d, %%rsp\n", s->op.variable.alloc);
      break;

    case string_type:
      s->loc = my_printf ("$.LS%d", str_labelno++);
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
	  if (IS_MEMORY (from->loc))
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
	  PUT ("\tidiv\t%s\n\tmov\t%%rdx, %s\n", from->loc, s->loc);
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
	  ERROR (_("Not Implemented: '['"));
	  assert (IS_MEMORY (s->loc));

	  /* TODO: Type checking semantics will also be necessary to
	           distinguish between an automaticly allocated array
	           and a pointer. */

	  /* TODO: Implement array indexing using the
	           `$offset(%base,%index,$scale)' semantics.  This will
	           require the use of regexp searching in all likely
	           hood, unless we can alter the location tracking
	           technique. */
	  break;

	default:
	  ERROR (_("Invalid binary operator op-code: %d\n"), 
		 s->op.binary.op);
	}
      /* Release the previously allocated register. */
      FREE_REGISTER (from->loc);
      break;

    case unary_type:
      gen_code_r (s->op.unary.arg);
      s->loc = s->op.unary.arg->loc;
      if (!(s->op.unary.op & AST_UNARY_PREFIX))
	{
	  s->loc = give_register (s->loc);
	  s->op.unary.op &= ~AST_UNARY_PREFIX;
	}
      switch (s->op.unary.op)
	{
	case '*':
	  ENSURE_DESTINATION_REGISTER_UNI (s->loc);
	  s->loc = my_printf ("(%s)", s->loc);
	  break;

	case '&':
	  assert (!IS_REGISTER (s->loc));
#if 1
	  assert (!IS_LITERAL (s->loc));
#else
	  if (*s->loc == '$')
	    s->loc = s->loc + 1;
	  else
#endif
	    s->loc = give_register_how ("lea", s->loc);
	  break;

	case '-':
	  ENSURE_DESTINATION_REGISTER_UNI (s->loc);
	  PUT ("\tnegq\t%s\n", s->loc);
	  break;

	case INC:
	  PUT ("\tinc\t%s\n", s->op.unary.arg->loc);
	  break;
	  
	case DEC:
	  PUT ("\tdec\t%s\n", s->op.unary.arg->loc);
	  break;

	default:
	  ERROR (_("Invalid unary operator opcode: %d\n"), s->op.unary.op);
	}
      break;      

    case function_call_type:
      ;
      /* TODO: Don't clobber other registers when making a function
	       call. */
      int a = 0;
      DO_RECURSIVE;
      for (i = s->op.function_call.args; i != NULL; i = i->next)
	{
	  PUT ("\tmov\t%s, %s\n", i->loc, regis(call_regis(a++)));
	  FREE_REGISTER (i->loc);
	}
      assert (s->op.function_call.name->type == variable_type);
      PUT ("\tmov\t$0, %%rax\n\tcall\t%s\n", s->op.function_call.name->loc);
      s->loc = "%rax";
      if (!(s->flags & AST_THROW_AWAY))
	s->loc = give_register (s->loc);
      break;

    default:
      ERROR (_("Invalid AST type %d"), s->type);
    }
  DO_RECURSIVE;
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
  free (data_section);
  return 0;
}
