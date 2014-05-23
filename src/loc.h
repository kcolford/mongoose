/**
 * @file   loc.h
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the location tracking structure.
 * 
 * Copyright (C) 2014 Kieran Colford
 *
 * This file is part of Compiler.
 *
 * Compiler is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Compiler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Compiler; see the file COPYING.  If not see
 * <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef LOC_H
#define LOC_H

#include "lib.h"
#include "xalloc.h"

/**
 * The type of location.
 * 
 */

enum loc_code {
  literal_loc,			/**< A literal (like an integer). */
  memory_loc,			/**< A memory operand. */
  register_loc,			/**< A register operand. */
  symbol_loc			/**< A symbol (like a funciton
				   name). */
};

/**
 * A structure that represents an operand's location.
 *
 * It can be a register, a literal in the assembly code, a memory
 * reference, or a symbol.  Each of these needs to be handled
 * uniquely.
 * 
 */

struct loc
{
  enum loc_code kind;		/**< The type of location. */
  int offset;			/**< The offset from the base
				   register. */
  const char *base;		/**< The base register/the
				   register/the string representation
				   of a literal. */
  const char *index;		/**< An index operation in a memory
				   access. */
  int scale;			/**< A scale to multiply the index
				   by. */
  const char *string;		/**< The cached value of the
				   serialized string. */
};

/** 
 * Test if the loction @c S is of type @c T.
 * 
 * @param S The location to check.
 * @param T The type that @c S should be.
 * 
 * @return true if @c S is a @c T, false otherwise.
 */
#define IS_LOC_TYPEOF(S, T)				\
    ((S) != NULL && ((struct loc *) (S))->kind == (T))

#define IS_REGISTER(S) IS_LOC_TYPEOF (S, register_loc)
#define IS_LITERAL(S) IS_LOC_TYPEOF (S, literal_loc)
#define IS_MEMORY(S) IS_LOC_TYPEOF (S, memory_loc)
#define IS_SYMBOL(S) IS_LOC_TYPEOF (S, symbol_loc)

/** 
 * A hook macro to be invoked when ever @c FREE_LOC is called.
 *
 * @see FREE_LOC
 * 
 * @param X The location that is about to be freed.
 * 
 * @return This is ignored, it might as well return void.
 */
#ifndef FREE_LOC_HOOK
# define FREE_LOC_HOOK(X)
#endif

/** 
 * Free a location, replacing the value with NULL.  Before it frees @c
 * X, it calls the @c FREE_LOC_HOOK function/macro with the argument
 * @c X.
 *
 * @see FREE_LOC_HOOK
 * 
 * @param X The location to be freed and then overwritten with NULL.
 */
#define FREE_LOC(X) do {			\
    if ((X) != NULL)				\
      {						\
	FREE_LOC_HOOK (X);			\
	FREE ((X)->base);			\
	FREE ((X)->index);			\
	FREE ((X)->string);			\
      }						\
    FREE (X);					\
  } while (0)

/** 
 * Initialize a location variable @c L with type @c K with the
 * loc::base field set to @c S.
 * 
 * @param L The variable to initialize.
 * @param K The type of location.
 * @param S The initial value of loc::base.
 */
#define MAKE_BASE_LOC(L, K, S) do {		\
    (L) = xzalloc (sizeof (struct loc));	\
    (L)->kind = (K);				\
    (L)->base = (S);				\
  } while (0)

/** 
 * Print out the location into a cannonical form understood by the
 * preprocessor.  This routine is dependant only on the type of
 * assembler and is mostly protable from one architeture to another.
 * Although, it will not work on the intel assembler as it uses AT\&T
 * syntax.
 * 
 * @param l The location to serialize.
 * 
 * @return The string representation of @c l.
 */
static inline const char *
print_loc (struct loc *l)
{
  if (l == NULL)
    return NULL;
  FREE (l->string);
  switch (l->kind)
    {
    case literal_loc:
      l->string = my_printf ("$%s", l->base);
      break;
    case memory_loc:
      if (l->offset != 0)
	EXTENDF (l->string, "%d", l->offset);
      EXTENDF (l->string, "(%s", l->base);
      if (l->index != NULL)
	EXTENDF (l->string, ",%s,%d", l->index, l->scale);
      EXTENDF (l->string, "%s", ")");
      break;
    case register_loc:
      l->string = my_printf ("%s", l->base);
      break;
    case symbol_loc:
      l->string = my_printf ("%s", l->base);
      break;
    default:
      abort ();
    }
  return l->string;
}

/** 
 * Make a copy of location @c l.
 * 
 * @param l The location to be copied.
 * 
 * @return The copy of @c l.
 */
static inline struct loc *
loc_dup (const struct loc *l)
{
  struct loc *out = xmemdup (l, sizeof *l);
  out->base = xstrdup (out->base);
  if (out->index != NULL)
    out->index = xstrdup (out->index);
  out->string = NULL;
  return out;
}

#endif
