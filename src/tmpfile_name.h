/**
 * @file   tmpfile_name.h
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the header file for the place_holder function.
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

#ifndef TMPFILE_NAME_H
#define TMPFILE_NAME

#include "attributes.h"

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
extern const char *tmpfile_name (void)
  ATTRIBUTE_MALLOC
;

#endif
