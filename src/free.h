/**
 * @file   free.h
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the header file for the FREE macro.
 * 
 * Copyright (C) 2014 Kieran Colford
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

#ifndef FREE_H
#define FREE_H

#include <stddef.h>

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

#endif
