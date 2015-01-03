/**
 * @file   place_holder.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the implementation of the place_holder function.
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

#include "config.h"

#include "my_printf.h"
#include "place_holder.h"

char *
place_holder (void)
{
  static int var = 1;
  return my_printf ("place$holder%d", var++);
}
