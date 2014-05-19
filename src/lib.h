/**
 * @file   lib.h
 * @author Kieran Colford <colfordk@gmail.com>
 * @date   Sun May 18 18:01:03 2014
 * 
 * @brief  This is the header file for generic routines.
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

#ifndef LIB_H
#define LIB_H

#include "gettext.h"
#include "xalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <error.h>

/**
 * String comparision utilities.
 *
 */
#define STREQ(X, Y) ((X) == (Y) || (X) != NULL && (Y) != NULL	\
		     && strcmp (X, Y) == 0)

/**
 * String comparision utilities.
 *
 */
#define STRNEQ(X, Y) ((X) != (Y) && (X) != NULL && (Y) != NULL	\
		      && strcmp (X, Y) != 0)

/**
 * Special macros for making it easier to mark strings for
 * translation.
 *
 */
#define _(s) gettext (s)

/**
 * Special macros for making it easier to mark strings for
 * translation.
 *
 */
#define N_(s) gettext_noop (s)

/**
 * Macro for specially freeing dynamically allocated memory and
 * setting the location to NULL so that it's safe from a
 * double-free.
 *
 * @param X Pointer to free.
 */
#define FREE(X) do {				\
    free (X);					\
    (X) = NULL;					\
  } while (0)

/**
 * Macro for determining the length of an array.
 *
 * @param X The array to check.
 *
 * @return The length of X.
 */
#define LEN(X) (sizeof (X) / sizeof *(X))

/**
 * Macro for swapping two pointer values.
 *
 */
#define SWAP(X, Y) do {				\
    void *_t = (X);				\
    (X) = (Y);					\
    (Y) = _t;					\
  } while (0)

/** 
 * Macro for extending a dynamically allocated string by the format
 * specifier given.
 * 
 * @param DEST The destination.
 * @param FMT The format specifier.
 */
#define EXTENDF(DEST, FMT, ...) do {					\
    char *_v = (DEST);							\
    (DEST) = my_printf ("%s" FMT, ((DEST) != NULL ? (DEST) : ""),	\
			__VA_ARGS__);					\
    FREE (_v);								\
  } while (0)

/** 
 * Macro for checking that a particular value is within the bounds
 * specified.
 * 
 * @param ARRAY The array to check.
 * @param N The index we want to try.
 */
#define CHECK_BOUNDS(ARRAY, N) do {					\
    if (a < 0)								\
      error (1, 0, _("index out of bounds, %d less than zero"), a);	\
    if (a >= LEN (ARRAY))						\
      error (1, 0, _("index out of bounds, %d greater than or equal "	\
		     "to the maximum %lu"), a, LEN (storage));		\
  } while (0)

/** 
 * This creates a unique string to act as a place holder when one
 * isn't already provided.
 * 
 * 
 * @return Unique string.
 */
extern char *place_holder (void);

/** 
 * This is a routine that forks the calling process and then calls
 * exec to run another program (while the original program waits for
 * it to return).
 * 
 * @param args A NULL terminated argument vector.
 * 
 * @return The return of the program.
 */
extern int safe_system (const char **args);

/** 
 * This is function that dynamically allocates and returns a string
 * according to a printf-format specifier.
 * 
 * @param fmt The printf-format string.
 * 
 * @return The resultant string.
 */
extern char *my_printf (const char *fmt, ...);

/** 
 * This creates an appropriate file name that can be used as a
 * temporary file.
 * 
 * 
 * @return The temporary file name.
 */
extern char *tmpfile_name (void);

/** 
 * This is a comparision function for use with "bsearch" and "qsort".
 * This variant works on type @c int.
 * 
 * @see bsearch
 * @see qsort
 * 
 * @param a Pointer to @c int.
 * @param b Pointer to @c int.
 * 
 * @return A code that is understood by @c bsearch and @c qsort.
 */
extern int compare (const void *a, const void *b);

/** 
 * This is a comparision function for use with "bsearch" and "qsort".
 * This variant works on type @c char* .
 * 
 * @see bsearch
 * @see qsort
 * 
 * @param a Pointer to @c char* .
 * @param b Pointer to @c char* .
 * 
 * @return A code that is understood by @c bsearch and @c qsort.
 */
extern int compares (const void *a, const void *b);

#endif
