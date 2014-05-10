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
#include "gl_array_list.h"
#include "gl_xlist.h"
#include "lib.h"
#include "xalloc.h"

#include <stdlib.h>
#include <string.h>

extern int yyparse (void);

void
run_unit ()
{
  /* If an output file name wasn't specified, then we need to
     determine one from the name of the source file.  If that can't be
     done though, we just set it to "a.out". */
  if (outfile_name == NULL)
    outfile_name = "a.out";

  gl_list_t name = NULL;
  if (stop == 0)
    name = gl_list_create_empty (GL_ARRAY_LIST, NULL, NULL, NULL, 1);

  int i;
  for (i = 0; i < gl_list_size (infile_name); i++)
    {
      const char *in = gl_list_get_at (infile_name, i), *_in = in;
      char *out;
      switch (in[strlen (in) - 1])
	{
	case 'c':
	  out = tmpfile_name ();
	  /* TODO: This is the GCC's preprocessor which we hope to replace
	     with our own. */
	  const char *cppargs[] =
	    { "/usr/bin/cpp", in, out, NULL };
	  if (safe_system (cppargs))
	    error (1, 0, _("preprocessor failed"));
	  in = out;

	case 'i':
	  if (stop == 'i')
	    break;
	  out = tmpfile_name ();
	  outfile = fopen (out, "w");
	  yyin = fopen (in, "r");
	  yyparse ();
	  fclose (outfile);
	  fclose (yyin);
	  in = out;

	case 's':
	  if (stop == 's')
	    break;
	  out = tmpfile_name ();
	  const char *asargs[] =
	    { "/usr/bin/as", "-o", out, in, NULL };
	  if (safe_system (asargs))
	    error (1, 0, _("assembler failed"));
	  in = out;
	}
      if (stop == 0)
	gl_list_add_last (name, in);
      else
	{
	  char *out = xstrdup (_in);
	  out[strlen (out) - 1] = stop;

	  copy_file_preserving (in, out);
	}
    }

  if (stop == 0)
    {
      const char *ldargs_template[] =
	{ "/usr/bin/gcc", "-o", outfile_name };

      const char **ldargs = xnmalloc (LEN (ldargs_template) +
				      gl_list_size (name) + 2,
				      sizeof *ldargs);

      memcpy (ldargs, ldargs_template, sizeof ldargs_template);

      const char **ldargsptr = ldargs + LEN (ldargs_template);
      int i;
      for (i = 0; i < gl_list_size (name); i++)
	*ldargsptr++ = gl_list_get_at (name, i);
      *ldargsptr++ = "-lm";
      *ldargsptr++ = NULL;

      if (safe_system (ldargs))
	error (1, 0, _("linker failed"));

      FREE (ldargs);
      gl_list_free (name);
    }
}
