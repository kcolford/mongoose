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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#ifndef SYSTEM_EMIT_DEBUGING
#define SYSTEM_EMIT_DEBUGING 0
#endif

char *
my_printf (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  char *out = NULL;
  vasprintf (&out, fmt, args);
  if (out == NULL)
    xalloc_die ();
  return out;
}

char *
place_holder (void)
{
  static int var = 1;
  return my_printf ("place$holder%d", var++);
}

int
safe_system (const char *args[])
{
  assert (args[0] != NULL);

  /* Emit debuging info about external programs run. */
  if (SYSTEM_EMIT_DEBUGING)
    {
      const char **i;
      fprintf (stderr, "%s: ", _("Running Program"));
      for (i = args; i[1] != NULL; i++)
	fprintf (stderr, "%s ", *i);
      fprintf (stderr, "%s\n", *i);
    }

  pid_t p = fork ();
  if (p < 0)
    error (1, errno, _("could not fork the process to run %s"), args[0]);
  else if (p == 0)
    {
      if (execvp (args[0], (char * const *) args))
	error (1, errno, _("could not exec to program %s"), args[0]);
    }
  else
    {
      int r = 0;
      waitpid (p, &r, 0);
      return r;
    }
  return -1;
}

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

static gl_list_t tmpfiles = NULL; /**< A list of temporary files. */

/** 
 * Free all temporary files in the tmpfiles list.
 *
 * @note This function is called as the destructor of the tmpfiles
 * variable and isn't directly invoked on any file name.  This
 * destructor is called either by one of the signal handlers or by the
 * exit function (since it had been registered with the atexit
 * function).
 *
 * @see tmpfile_name
 * @see tmpfiles
 * 
 */
static void
free_tmpfiles (void)
{
  if (tmpfiles != NULL)
    {
      /** @todo Change this loop so that it uses @c gl_list_iterator
	  instead of array accesses. */
      unsigned i;
      for (i = 0; i < gl_list_size (tmpfiles); i++)
	{
	  const char *t = gl_list_get_at (tmpfiles, i);
	  unlink (t);
	  gl_list_set_at (tmpfiles, i, NULL);
	  FREE (t);
	}
    }
}

/** 
 * @see free_tmpfiles
 * @see tmpfiles
 *
 */
const char *
tmpfile_name (void)
{
  /* If this is the first time that this routine is run, set up the
     list and add the destructors to the cleanup functions. */
  if (tmpfiles == NULL)
    {
      /** @todo @parblock Incorperate GL_LINKED_LIST's feature to
	  make all actions on it signal safe.  This is turned on in
	  configure.ac by invoking:

	  @code
	  AC_DEFINE([SIGNAL_SAFE_LIST], [1],
	            [Define if lists must be signal-safe.])
	  @endcode

	  Thus this can be safely cleaned up while catching a fatal
	  signal.  @endparblock 
      */
      tmpfiles = gl_list_create_empty (GL_ARRAY_LIST, NULL, NULL, NULL, 1);
      /* Register the free_tmpfiles cleanup function so that it is
	 called when the program exits and whenever the program
	 recieves a fatal signal. */
      atexit (free_tmpfiles);
      at_fatal_signal (free_tmpfiles);
    }

  /* Add an entry to the end of the array, this will then be
     replaced. */
  gl_list_add_last (tmpfiles, NULL);
  char *out = NULL;

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
