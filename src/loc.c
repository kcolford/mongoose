/**
 * @file   loc.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief This is the implementation of the location tracking
 * structure.
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

#include "extendf.h"
#include "free.h"
#include "loc.h"
#include "my_printf.h"
#include "xalloc.h"

#include <assert.h>
#include <stdlib.h>

const char *
print_loc (struct loc *l)
{
  assert (l != NULL);

  FREE (l->string);
  switch (l->kind)
    {
    case literal_loc:
      l->string = my_printf ("$%s", l->base);
      break;
    case memory_loc:
      if (l->offset != 0)
	EXTENDF (l->string, "%d", l->offset);
      EXTENDF (l->string, "(%s", l->base);
      if (l->index != NULL)
	EXTENDF (l->string, ",%s,%d", l->index, l->scale);
      EXTENDF (l->string, "%s", ")");
      break;
    case register_loc:
      l->string = my_printf ("%s", l->base);
      break;
    case symbol_loc:
      l->string = my_printf ("%s", l->base);
      break;
    default:
      assert (! "this should not have been reached");
      abort ();
    }
  return l->string;
}

struct loc *
loc_dup (const struct loc *l)
{
  assert (l != NULL);

  struct loc *out = xmemdup (l, sizeof *l);
  out->base = xstrdup (out->base);
  if (out->index != NULL)
    out->index = xstrdup (out->index);
  out->string = NULL;
  return out;
}
