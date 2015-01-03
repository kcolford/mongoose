/**
 * @file   my_printf.h
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief This is the declaration of my_printf.
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

#ifndef MY_PRINTF_H
#define MY_PRINTF_H

#include "attributes.h"

/** 
 * This is a function that dynamically allocates and returns a string
 * according to a printf-format specifier.
 * 
 * @param fmt The printf-format string.
 * 
 * @return The resultant string.
 */
extern char *my_printf (const char *fmt, ...)
  ATTRIBUTE ((__format__ (gnu_printf, 1, 2)))
  ATTRIBUTE_MALLOC
  ;

#endif
