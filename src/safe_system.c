/**
 * @file   safe_system.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  The implementation of safe_system.
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

#include "errno.h"
#include "error.h"
#include "lib.h"
#include "safe_system.h"

#include <assert.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef SYSTEM_EMIT_DEBUGING
#define SYSTEM_EMIT_DEBUGING 0
#endif

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
