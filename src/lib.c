/* This is a utility library that holds generic functions/APIs that
   aren't offered by the C library.

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

#include "ast.h"
#include "lib.h"
#include "xalloc.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>

char *
my_printf (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  char *out = NULL;
  int val = vasprintf (&out, fmt, args);
  if (out == NULL)
    xalloc_die ();
  return out;
}

char *
place_holder ()
{
  static int var = 1;
  return my_printf ("place$holder%d", var++);
}

int
safe_system (const char *args[])
{
  pid_t p = fork ();
  if (p < 0)
    error (1, errno, _("could not fork the process to run %s"), args[0]);
  else if (p == 0)
    {
      if (execv (args[0], (char * const *) args))
	error (1, errno, _("could not exec to program %s"), args[0]);
    }
  else
    {
      int r = 0;
      waitpid (p, &r, 0);
      return r;
    }
}

void
xalloc_die ()
{
  error (1, ENOMEM, _("out of memory"));
  abort ();
}

int
compare (const void *a, const void *b)
{
  return *(const int *) a - *(const int *) b;
}

int
compares (const void *a, const void *b)
{
  return strcmp (*(char * const *) a, *(char * const *) b);
}
