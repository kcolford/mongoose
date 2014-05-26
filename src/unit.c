/**
 * @file   unit.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief These are the routines that select which phase of the
 * compiler to run.
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

#include "compiler.h"
#include "copy-file.h"
#include "gl_array_list.h"
#include "gl_xlist.h"
#include "lib.h"
#include "xalloc.h"

#include <stdlib.h>
#include <string.h>

extern int yyparse (void);

/** @todo This is the GCC's preprocessor which we hope to
    replace with our own. */
static const char *cppargs[] =
  { CC, "-o", NULL, NULL, "-E", NULL };

/** @todo Determine the needed assembler program in a portable way. */
static const char *asargs[] =
  { "as", "-o", NULL, NULL, NULL };

/** @todo The GCC seems to link in some additional object files that
    we can't duplicate or get the program working without.  We have to
    use the GCC to link our programs for the time being. */
const char *ldargs_template[] =
  { CC, "-o", NULL };

const char *extra_libs[] =
  { "-lm" };

void
run_unit (void)
{
  gl_list_t name = NULL;
  if (stop == 0)
    /** @todo Consider using a linked list instead of an array list
	for managing the queue of compiled object files. */
    name = gl_list_create_empty (GL_ARRAY_LIST, NULL, NULL, NULL, 1);

  unsigned i;
  const char *out;
  for (i = 0; i < gl_list_size (infile_name); i++)
    {
      const char *in = gl_list_get_at (infile_name, i), *_in = in;
      switch (in[strlen (in) - 1])
	{
	case 'c':
	  out = tmpfile_name ();
	  cppargs[2] = out;
	  cppargs[3] = in;
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
	  asargs[2] = out;
	  asargs[3] = in;
	  if (safe_system (asargs))
	    error (1, 0, _("assembler failed"));
	  in = out;

	default:
	  break;
	}

      if (stop == 0)
	gl_list_add_last (name, in);
      else
	{
	  /* If the output file wasn't specified, we'll decide on one
	     by changing the extension of the input file.*/
	  if (outfile_name == NULL)
	    {
	      char *t = xstrdup (_in);
	      t[strlen (t) - 1] = stop;
	      out = t;
	    }
	  else
	    out = outfile_name;

	  /* The reason that we wait until now to set up the output
	     file is that one of the programs could clobber the output
	     file, but then fail.  This would break any Makefiles due
	     to new timestamps being applied and the file having
	     corrupt data. */
	  copy_file_preserving (in, out);
	}
    }

  /* If an output file name wasn't specified, then we need to
     determine one from the name of the source file.  If that can't be
     done though, we just set it to "a.out". */
  if (outfile_name == NULL)
    outfile_name = "a.out";

  if (stop == 0)
    {
      ldargs_template[2] = outfile_name;

      const char **ldargs = xcalloc (LEN (ldargs_template) +
				     gl_list_size (name) +
				     LEN (extra_libs) + 1,
				     sizeof *ldargs);
      const char **ldargsptr = ldargs;

      memcpy (ldargsptr, ldargs_template, sizeof ldargs_template);
      ldargsptr += LEN (ldargs_template);

      for (i = 0; i < gl_list_size (name); i++)
	*ldargsptr++ = gl_list_get_at (name, i);

      memcpy (ldargsptr, extra_libs, sizeof extra_libs);
      ldargsptr = ldargsptr + LEN (extra_libs);

      if (safe_system (ldargs))
	error (1, 0, _("linker failed"));

      FREE (ldargs);
      gl_list_free (name);
    }
}
