/**
 * @file   xalloc_die.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the implementation of the xalloc_die function.
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

#include "lib.h"
#include "xalloc.h"

#include <stdlib.h>

/** 
 * The function that is called when any of the x*alloc functions fail
 * to allocate memory.  The suggested method of handling this is by
 * just calling abort but there are cleanups and error messages that
 * need to be delt with.  So a call to @c error is made so that it
 * exits instead.
 * 
 */
void
xalloc_die (void)
{
  error (1, ENOMEM, _("out of memory"));
  abort ();
}
