/* This is function that dynamically allocates and returns a string
   according to a printf-format specifier.

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

#ifndef MY_PRINTF_H
#define MY_PRINTF_H

#include <stdlib.h>

extern char *my_printf (const char *, ...);

/* Macro for specially freeing dynamically allocated memory and
   setting the location to NULL so that it's safe from a
   double-free. */
#define FREE(X) do {				\
    free (X);					\
    (X) = NULL;					\
  } while (0)

#endif
