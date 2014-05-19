/**
 * @file   lib.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief This is a utility library that holds generic functions/APIs
 * that aren't offered by the C library.
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

#include "config.h"

#include "ast.h"
#include "fatal-signal.h"
#include "gl_xlist.h"
#include "gl_array_list.h"
#include "lib.h"
#include "xalloc.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <fcntl.h>
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

static gl_list_t tmpfiles = NULL;

static void
del_tmpfile (const void *_name)
{
  char *name = (char *) _name;
  if (name != NULL)
    unlink (name);
  free (name);
}

static void
free_tmpfiles ()
{
  if (tmpfiles != NULL)
    gl_list_free (tmpfiles);
  tmpfiles = NULL;
}

char *
tmpfile_name ()
{
  /* If this is the first time that this routine is run, set up the
     list and add the destructors to the cleanup functions. */
  if (tmpfiles == NULL)
    {
      tmpfiles = gl_list_create_empty (GL_ARRAY_LIST, NULL, NULL,
				       del_tmpfile, 1);
      atexit (free_tmpfiles);
      at_fatal_signal (free_tmpfiles);
    }

  /* Add an entry to the end of the array, this will then be
     replaced. */
  gl_list_add_last (tmpfiles, NULL);
  char *out = NULL;

  int t = 0;
  do
    {
      FREE (out);
      out = xstrdup (tmpnam (NULL));
      gl_list_set_at (tmpfiles, gl_list_size (tmpfiles) - 1, out);
      errno = 0;
      /* Create the file so that it can't be opened by another process
	 or if it is, it will cause us to go and select a new
	 file name. */
      int t = creat (out, S_IRWXU);
      if (t >= 0)
	close (t);
    }
  while (errno == EEXIST);

  return out;
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
