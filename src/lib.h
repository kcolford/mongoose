/**
 * @file   lib.h
 * @author Kieran Colford <colfordk@gmail.com>
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <error.h>

/**
 * Test if the strings @c X and @c Y are equal.
 *
 */
#define STREQ(X, Y) (strcmp (X, Y) == 0)

/**
 * Test if the strings @c X and @c Y are not equal
 *
 */
#define STRNEQ(X, Y) (strcmp (X, Y) != 0)

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
 * @warning This function casts the pointer @c X to void* and thus is
 * considered dangerous.
 *
 * @param X Pointer to free.
 */
#define FREE(X) do {				\
    void *_free_target = (void *) (X);		\
    free (_free_target);			\
    (X) = NULL;					\
  } while (0)

/**
 * Macro for determining the length of an array.
 *
 * @param X The array to check.
 *
 * @return The length of @c X.
 */
#define LEN(X) (sizeof (X) / sizeof *(X))

/** 
 * Macro for determining the signed size of an array.
 * 
 * @param X The array to check.
 * 
 * @return The signed length of @c X.
 */
#define SLEN(X) ((ssize_t) LEN (X))

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
    char *_v = my_printf ("%s" FMT, ((DEST) != NULL ? (DEST) : ""),	\
			  __VA_ARGS__);					\
    FREE (DEST);							\
    (DEST) = _v;							\
  } while (0)

/** 
 * Macro for checking that a particular value is within the bounds
 * specified.
 * 
 * @param ARRAY The array to check.
 * @param N The index we want to try.
 */
#define CHECK_BOUNDS(ARRAY, N) do {					\
    if ((N) < 0)							\
      error (1, 0, _("index out of bounds, %d less than zero"), (N));	\
    if ((N) >= SLEN (ARRAY))						\
      error (1, 0, _("index out of bounds, %d greater than or equal "	\
		     "to the maximum %ld"), (N), SLEN (ARRAY));		\
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
extern int safe_system (const char *args[]);

/** 
 * This is function that dynamically allocates and returns a string
 * according to a printf-format specifier.
 * 
 * @param fmt The printf-format string.
 * 
 * @return The resultant string.
 */
extern char *my_printf (const char *fmt, ...)
#if defined __GNUC__ && __GNUC__ > 2
  __attribute__ ((__format__ (gnu_printf, 1, 2)))
#endif
  ;

/** 
 * This creates an appropriate file name that can be used as a
 * temporary file.
 *
 * This temporary file is automatically deleted upon program
 * termination (either from a fatal signal that isn't SIGKILL or a
 * call to exit).
 * 
 * @return The temporary file name.
 */
extern const char *tmpfile_name (void);

#endif
