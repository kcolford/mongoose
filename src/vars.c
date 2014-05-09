/* This object file stores all the global variables used throughout
   the program that require external linkage.

Copyright (C) 2014 Kieran Colford

This file is part of Compiler.

Compiler is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Compiler is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with Compiler; see the file COPYING.  If not see
<http://www.gnu.org/licenses/>. */

#include "config.h"

#include "compiler.h"

FILE *outfile = NULL;
char stop = 0;

int optimize = 0;
int debug = 0;

char *infile_name = NULL;
char *outfile_name = NULL;

char *file_name = NULL;
int lineno = 0;

void
vars_init ()
{
  ;
}
