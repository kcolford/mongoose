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

#include "gettext.h"
#include "xalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <error.h>

/* **************************************************************** */
/* Start of Macros. */

/* String comparision utilities. */
#define STREQ(X, Y) ((X) == (Y) || (X) != NULL && (Y) != NULL && strcmp (X, Y) == 0)
#define STRNEQ(X, Y) ((X) != (Y) && (X) != NULL && (Y) != NULL && strcmp (X, Y) != 0)

/* Special macros for making it easier to mark strings for
   translation. */
#define _(s) gettext (s)
#define N_(s) gettext_noop (s)

/* Macro for specially freeing dynamically allocated memory and
   setting the location to NULL so that it's safe from a
   double-free. */
#define FREE(X) do {				\
    free (X);					\
    (X) = NULL;					\
  } while (0)

/* Macro for determining the length of an array. */
#define LEN(X) (sizeof (X) / sizeof *(X))

/* Macro for swapping two pointer values. */
#define SWAP(X, Y) do {				\
    void *_t = (X);				\
    (X) = (Y);					\
    (Y) = _t;					\
  } while (0)

/* **************************************************************** */
/* Start of linked in functions. */

/* This creates a unique string to act as a place holder when one
   isn't already provided. */
extern char *place_holder (void);

/* This is a routine that forks the calling process and then calls
   exec to run another program (while the original program waits for
   it to return). */
extern int safe_system (const char **);

/* This is function that dynamically allocates and returns a string
   according to a printf-format specifier. */
extern char *my_printf (const char *, ...);

/* These are comparision functions for use with "bsearch" and
   "qsort". */
extern int compare (const void *, const void *);
extern int compares (const void *, const void *);

/* **************************************************************** */
/* Start of inlined functions. */

/* This creates an appropriate file name that can be used as a
   temporary file. */
static inline char *
tmpfile_name ()
{
  return xstrdup (tmpnam (NULL));
}

#endif
