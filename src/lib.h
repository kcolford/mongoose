/* This is the header file for generic routines.

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

#ifndef LIB_H
#define LIB_H

#include "ast.h"
#include "xalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* **************************************************************** */
/* Start of Macros. */

/* String comparision utilities. */
#define STREQ(X, Y) (strcmp (X, Y) == 0)
#define STRNEQ(X, Y) (strcmp (X, Y) != 0)

/* **************************************************************** */
/* Start of linked in functions. */

/* This creates a unique string to act as a place holder when one
   isn't already provided. */
extern char *place_holder (void);

/* This is a printf like function that returns a dynamically allocated
   string based on the format specifier. */
extern char *my_printf (const char *, ...);

/* This is a routine that forks the calling process and then calls
   exec to run another program (while the original program waits for
   it to return). */
extern int safe_system (const char **);

/* **************************************************************** */
/* Start of inlined functions. */

/* This creates an appropriate file name that can be used as a
   temporary file. */
static inline char *
tmpfile_name ()
{
  return xstrdup (tmpnam (NULL));
}

/* This concatenates two lists of ASTs. */
static inline struct ast *
ast_cat (struct ast *l, struct ast *r)
{
  if (l == NULL)
    return r;
  else
    {
      struct ast *t = l;
      while (t->next != NULL)
	t = t->next;
      t->next = r;
      return l;
    }
}

#endif
