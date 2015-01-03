/**
 * @file   lib.h
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the header file for generic routines.
 * 
 * Copyright (C) 2014, 2015 Kieran Colford
 *
 * This file is part of Mongoose.
 *
 * Mongoose is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mongoose is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mongoose; see the file COPYING.  If not see
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

#endif
