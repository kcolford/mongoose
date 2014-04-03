#include "config.h"

#include "compiler.h"
#include "xalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <argp.h>
#include <unistd.h>
#include <error.h>

FILE *outfile = NULL;
int optimize = 0;
int debug = 0;

char *infile_name = NULL;
char *outfile_name = NULL;

char *name = NULL;

extern int yyparse (void);

extern int yydebug;

enum stop_point {
  compile_point,
  assemble_point,
  linker_point, 
  unknown_point } stop = unknown_point;

void
del_name ()
{
  if (strcmp (name, infile_name) != 0)
    unlink (name);
}

char *
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

char *
assemble (const char *in)
{
  char *out = tmpfile_name ();
  pid_t p = fork ();
  if (p < 0)
    return NULL;
  else if (p == 0)
    execl ("/usr/bin/as", "as", "-o", out, in, NULL);
  else
    {
      int r = 0;;
      waitpid (p, &r, 0);
      if (r)
	return NULL;
      else
	return out;
    }
}

char *
linker (const char *in)
{
  char *out = tmpfile_name ();
  pid_t p = fork ();
  if (p < 0)
    return NULL;
  else if (p == 0)
    execl ("/usr/bin/gcc", "gcc", "-o", out, in, NULL);
  else
    {
      int r;
      waitpid (p, &r, 0);
      if (r)
	return NULL;
      else
	return out;
    }
}

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;

const char *doc = 
  "This is an experimental compiler that compiles an almost Turing "
  "complete subset of C.  It currently lacks an \"infinite tape\".  "
  "Where FILE is the input file to be compiled.\v"

  "The current subset supports the following: all arithmetic operators, "
  "goto-statements and labels, if-statements, the comparison operators "
  "(<, >, <=, >=, ==, !=) can be used in the test for an if-statement, "
  "the bitwise operators (|, &, ^) are available, as well as pointers "
  "(but there is no pointer data type), and more...";

struct argp_option opts[] = {
  { "outfile",  'o', "FILE",                   0, 
    "Write output to FILE" },
  { "verbose",  'v',   NULL,                   0, 
    "Give output verbosely (intended for debugging purposes only)" },
  { NULL,       'O',    "n", OPTION_ARG_OPTIONAL, 
    "Control the optimization level (starts at 0, default is 1)" },
  { "debug",    'd',   NULL,                   0,
    "Run the compiler in debug mode" },
  { NULL,       'c',   NULL,                   0,
    "Only compile to object file"},
  { NULL,       'S',   NULL,                   0,
    "Only compile to assembly"},
  { "quiet",    'q',   NULL,                   0,
    "Don't print anything (disables -d and -v)" },
#if 0
  { "link",     'l',  "LIB",                   0,
    "Add LIB to the list of linked-in libraries" },
#endif
  { 0 }
};

error_t 
arg_parse (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case 'o':
      outfile_name = arg;
      break;

    case 'v':
      yydebug = -1;
      break;

    case 'O':
      if (arg == NULL)
	optimize = 1;
      else
	optimize = atoi (arg);
      break;

    case 'd':
      debug = -1;
      break;

    case 'c':
      stop = assemble_point;
      break;

    case 'S':
      stop = compile_point;
      break;

    case 'q':
      debug = 0;
      yydebug = 0;
      break;
      
    case ARGP_KEY_ARG:
      infile_name = arg;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

int main (int argc, char *argv[])
{
  struct argp args = { opts, arg_parse, "FILE", doc };
  argp_parse (&args, argc, argv, 0, NULL, NULL);

  if (infile_name == NULL)
    error (1, 0, "No input file given");

  name = infile_name;
  atexit (del_name);

#define CHECK(C, S)						\
    do {							\
      char *r = C (name);					\
      del_name ();						\
      name = r;							\
      if (r == NULL)						\
	error (1, errno, "failure");				\
    } while (0);						\
    if (stop == C##_point)					\
      {								\
	if (outfile_name == NULL)				\
	  {							\
	    outfile_name = xstrdup (infile_name);		\
	    outfile_name[strlen (outfile_name) - 1] = S;	\
	  }							\
	break;							\
      }								\

  switch (name[strlen (name) - 1])
    {
    case 'c':
      CHECK (compile, 's');
    case 's':
      CHECK (assemble, 'o');
    case 'o':
    default:
      CHECK (linker, outfile_name[strlen (outfile_name) - 1]);
    }

#undef CHECK

  if (outfile_name == NULL)
    outfile_name = "a.out";

  copy_file_preserving (name, outfile_name);

  return 0;
}
