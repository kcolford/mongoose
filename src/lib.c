#include "config.h"

#include "compiler.h"
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
my_strcat (char *l, char *r)
{
  char *out = my_printf ("%s%s", l, r);
  free (l);
  free (r);
  return out;
}

char *
place_holder ()
{
  static int var = 1;
  return my_printf ("place$holder%d", var++);
}


char *
tmpfile_name ()
{
  return xstrdup (tmpnam (NULL));
}

int
safe_system (const char *args[])
{
  pid_t p = fork ();
  if (p < 0)
    error (1, errno, "Could not fork the process to run %s", args[0]);
  else if (p == 0)
    {
      if (execv (args[0], (char * const *) args))
	error (1, errno, "Could not exec to program %s", args[0]);
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
  error (1, ENOMEM, "Out of memory");
  abort ();
}
