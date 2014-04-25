/* This is the location tracking structure.

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

#ifndef LOC_H
#define LOC_H

#include "lib.h"
#include "xalloc.h"

enum loc_code {
  literal_loc, 
  memory_loc, 
  register_loc, 
  symbol_loc };

struct loc
{
  enum loc_code kind;
  int offset;
  char *base;
  char *index;
  int scale;
  char *string;
};

#define IS_REGISTER(S) ((S) != NULL && ((struct loc *) S)->kind == register_loc)
#define IS_LITERAL(S) ((S) != NULL && ((struct loc *) S)->kind == literal_loc)
#define IS_MEMORY(S) ((S) != NULL && ((struct loc *) S)->kind == memory_loc)

#ifndef FREE_LOC_HOOK
# define FREE_LOC_HOOK(X) NULL
#endif

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

#define MAKE_BASE_LOC(L, K, S) do {		\
    (L) = xzalloc (sizeof (struct loc));	\
    (L)->kind = (K);				\
    (L)->base = (S);				\
  } while (0)

static inline char *
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
    }
  return l->string;
}

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
