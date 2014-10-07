/**
 * @file   compiler.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the main driver for the program.
 * 
 * Copyright (C) 2014 Kieran Colford
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

#include "argp-version-etc.h"
#include "compiler.h"
#include "copy-file.h"
#include "extendf.h"
#include "free.h"
#include "gl_xlist.h"
#include "lib.h"
#include "progname.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <argp.h>
#include <unistd.h>

#include "configmake.h"

/** Bug reporting address displayed by the argp parser. */
const char *argp_program_bug_address = "<" PACKAGE_BUGREPORT ">";

/** List of authors to be displayed along with --version. */
const char *authors[] = {
  "Kieran Colford",
  NULL
};

/** Copyright notice to be displayed along with --version.  It is a
    printf format string that first accepts a string for "(C)"
    (translated to the current locale) followed by a number (the
    current year).

    @note Do not mark this string for translation.  English is the
    standard requested by the Berne convention and thus this should
    remain in that language. */
const char version_etc_copyright[] =
  "Copyright %s %d Kieran Colford";

const char *doc[] = {
  N_("This is an experimental compiler that compiles a Turing Complete"
     " subset of C.  Where FILE is the input file to be compiled.  All C"
     " functions are supported except for the ones which require complicated"
     " structures or support from the compiler."),
  "\v",
  N_("The current subset supports the following: all arithmetic operators,"
     " goto-statements and labels, if-statements, the comparison operators"
     " (<, >, <=, >=, ==, !=) can be used in the test for an if-statement,"
     " the bit wise operators (|, &, ^) are available, as well as pointers"
     " (but there is no pointer data type), and more..."),
  NULL
}; /**< Help message. */

struct argp_option opts[] = {
  { "outfile",  'o', "FILE",                   0,
    N_("Write output to FILE") },
  { "verbose",  'v',   NULL,                   0,
    N_("Give output verbosely (intended for debugging purposes only)") },
  { "debug",    'd',   NULL,                   0,
    N_("Issue debuging data on the parser.") },
  { NULL,       'O',    "n", OPTION_ARG_OPTIONAL,
    N_("Control the optimization level (starts at 0, default is 1)") },
  { "echo",     'e',   NULL,                   0,
    N_("Echo all assembly to stdout") },
  { NULL,       'c',   NULL,                   0,
    N_("Only compile to object file") },
  { NULL,       'S',   NULL,                   0,
    N_("Only compile to assembly") },
  { NULL,       'E',   NULL,                   0,
    N_("Only run the preprocessor") },
  { "quiet",    'q',   NULL,                   0,
    N_("Don't print anything (disables -d and -v)") },
#if 0
  { "link",     'l',  "LIB",                   0,
    N_("Add LIB to the list of linked-in libraries") },
#endif
  { 0 }
};				/**< The program options recognized by
				   this compiler. */

/** 
 * The ARGP parser function.
 * 
 * @param key The key that maps to an option.
 * @param arg The argument supplied to the option (or NULL).
 * @param state A pointer describing the state of the parser.
 * 
 * @return Error code as described by the ARGP info docs.
 */
error_t
arg_parse (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'o':
      outfile_name = arg;
      break;

    case 'v':
      debug = -1;
      break;
      
    case 'd':
      yydebug = -1;
      break;

    case 'O':
      if (arg == NULL)
	optimize = 1;
      else
	optimize = strtol (arg, NULL, 0);
      break;

    case 'e':
      debug = -1;
      break;

    case 'c':
      stop = 'o';
      break;

    case 'S':
      stop = 's';
      break;

    case 'E':
      stop = 'i';
      break;

    case 'q':
      debug = 0;
      yydebug = 0;
      break;

    case ARGP_KEY_ARG:
      gl_list_add_last (infile_name, arg);
      break;

    case ARGP_KEY_NO_ARGS:
      argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/** 
 * The main entry point.
 * 
 * @param argc Number of program arguments.
 * @param argv The argument vector supplied to this program.
 * 
 * @return The exit status of the program.
 */
int main (int argc, char *argv[])
{
  set_program_name (argv[0]);
  setlocale (LC_ALL, "");

#if ENABLE_NLS
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
#endif

  vars_init ();
  
  /* Initialize the version string. */
  argp_version_setup (PACKAGE, authors);

  /* Initialize the help string. */
  const char *totaldoc = NULL, **ptr;
  for (ptr = doc; *ptr != NULL; ptr++)
    EXTENDF (totaldoc, "%s", *ptr);

  struct argp args = { opts, arg_parse, N_("FILE"), totaldoc };
  argp_parse (&args, argc, argv, 0, NULL, NULL);
  FREE (totaldoc);

  run_unit ();

  return 0;
}
