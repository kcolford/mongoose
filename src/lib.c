#include "config.h"

#include "compiler.h"
#include "xalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/wait.h>

#include <error.h>

char *
my_strcat (char *l, char *r)
{
  l = realloc (l, strlen (l) + strlen (r) + 1);
  strcat (l, r);
  xfree (r);
  return l;
}

char *
place_holder ()
{
  static int var = 1;
  return my_printf ("place$holder%d", var++);
}

#define NOT_NULL(X) do {			\
    if ((X) == NULL)				\
      abort ();					\
  } while (0)

char *
my_printf (const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  char *out = NULL;
  int val = vasprintf (&out, fmt, args);
  NOT_NULL (out);
  return out;
}

void
xfree (void *s)
{
  free (s);
}

char *
tmpfile_name ()
{
  return xstrdup (tmpnam (NULL));
}

int
try_copy (const char *from, const char *to)
{
  pid_t p = fork ();
  if (p < 0)
    return -1;
  else if (p == 0)
    execl ("/bin/cp", "cp", "-f", from, to, NULL);
  else
    {
      int r;
      waitpid (p, &r, 0);
      return r;
    }
}
