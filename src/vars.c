/**
 * @file   vars.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief This object file stores all the global variables used
 * throughout the program that require external linkage.
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

#include "compiler.h"
#include "gl_array_list.h"

FILE *outfile = NULL;
char stop = 0;

int optimize = 0;
int debug = 0;

gl_list_t infile_name = NULL;
const char *outfile_name = NULL;

char *file_name = NULL;

void
vars_init (void)
{
  infile_name = gl_list_create_empty (GL_ARRAY_LIST, NULL, NULL, NULL, 1);
}
