/* These are the routines that select which phase of the compiler to
   run.

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
#include "copy-file.h"
#include "lib.h"
#include "xalloc.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

FILE *outfile = NULL;
char *name = NULL;
char stop = 0;
extern char *infile_name;
extern char *outfile_name;
extern int yyparse (void);

static void
del_name ()
{
  if (name != NULL && strcmp (name, infile_name) != 0)
    {
      unlink (name);
      FREE (name);
    }
}

static inline char *
preprocess (const char *in)
{
  char *out = tmpfile_name ();
  /* This is the GCC's preprocessor which we hope to replace with our
     own. */
  const char *args[] = { "/usr/bin/cpp", in, out, NULL };
  if (safe_system (args))
    return NULL;
  else
    return out;
}

static inline char *
compile (const char *in)
{
  if (in ==  NULL)
    return NULL;
  yyin = fopen (in, "r");
  char *out = tmpfile_name ();
  outfile = fopen (out, "w");
  int i = yyparse ();
  fclose (yyin);
  fclose (outfile);
  if (i)
    return NULL;
  else
    return out;
}

static inline char *
assemble (const char *in)
{
  char *out = tmpfile_name ();
  const char *args[] = { "/usr/bin/as", "-o", out, in, NULL };
  if (safe_system (args))
    return NULL;
  else
    return out;
}

static inline char *
linker (const char *in)
{
  char *out = tmpfile_name ();
  /* Hopefully we can move away from having to use the GCC to link our
     programs, but it seems to link in some other object files that we
     can't duplicate yet.  Once we can create our own, we will replace
     this with "ld". */
  const char *args[] = { "/usr/bin/gcc", "-o", out, in, NULL };
  if (safe_system (args))
    return NULL;
  else
    return out;
}

void
run_unit ()
{
  name = infile_name;
  atexit (del_name);

#define CHECK(C, S)						\
    do {							\
      char *r = (C) (name);					\
      del_name ();						\
      name = r;							\
      if (r == NULL)						\
	exit (1);						\
    } while (0);						\
    if (stop == (S))						\
      break;							\

  switch (name[strlen (name) - 1])
    {
    case 'c':
      CHECK (preprocess, 'i');
    case 'i':
      CHECK (compile, 's');
    case 's':
      CHECK (assemble, 'o');
    case 'o':
    default:
      CHECK (linker, 0);
    }

#undef CHECK

  /* If an output file name wasn't specified, then we need to
     determine one from the name of the source file.  If that can't be
     done though, we just set it to "a.out". */
  if (outfile_name == NULL)
    {
      if (stop != 0)
	{
	  assert (strchr ("iso", stop) != NULL);
	  outfile_name = xstrdup (infile_name);
	  outfile_name[strlen (outfile_name) - 1] = stop;
	}
      else
	outfile_name = "a.out";
    }

  copy_file_preserving (name, outfile_name);
}
