#include "config.h"

#include "compiler.h"
#include "xalloc.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/wait.h>

#include <error.h>

char *
my_strcat (char *l, char *r)
{
  l = xrealloc (l, strlen (l) + strlen (r) + 1);
  strcat (l, r);
  free (r);
  return l;
}

char *
place_holder ()
{
  static int var = 1;
  return my_printf ("place$holder%d", var++);
}

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
tmpfile_name ()
{
  return xstrdup (tmpnam (NULL));
}
