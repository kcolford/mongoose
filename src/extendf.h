/**
 * @file   extendf.h
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the header file for the EXTENDF macro.
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

#ifndef EXTENDF_H
#define EXTENDF_H

#include "free.h"
#include "my_printf.h"

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

#endif
