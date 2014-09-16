[+ AutoGen5 template
h
c
+]

/**
 * @file
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief This file contains definitions/declarations for the entire
 * AST framework.
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

[+ CASE (suffix) +]

[+ == h +]
#ifndef AST_H
#define AST_H

[+added_code+]

/**
 * Data type that lists off all of the different kinds of ASTs.
 * 
 */
enum ast_code {
  [+ FOR types ',
  ' +][+name+]_type /**< [+doc+] */[+ ENDFOR types +]
};

/**
 * The AST structure that the compiler uses for its intermediate
 * representation.
 * 
 */
struct ast
{
  [+ FOR top_level +]
  [+type+] [+call+][+ IF (exist? "size") +]: [+size+][+ ENDIF +]; /**< [+doc+] */
  [+ ENDFOR top_level +];
  const int num_ops;		/**< Number of variable ops. */
  union
  {
    [+ FOR types +]
    [+ IF (or (exist? "cont") (exist? "extra")) +]
    struct
    {
      [+ FOR cont +]
      [+type+] [+call+];	/**< [+doc+] */
      [+ ENDFOR cont +]
      [+ FOR extra +]
      [+type+] [+call+];	/**< [+doc+] */
      [+ ENDFOR extra +]
    } [+name+];			/**< [+doc+] */
    [+ ENDIF +]
    [+ ENDFOR types +]
  } op;				/**< Wrapper around the anonymous
				   union. */
  struct ast *ops[1];		/**< Variable length array that stores
				   any number of ops. */
};

[+ FOR types +]
/**
 * Create an instance of the AST using the type: @c [+name+]_type.
 *
 * [+doc+]
 *
[+ IF (exist? "cont") +] * @see ast::[+name+]
[+ ENDIF +] * @see [+name+]_type
 * 
 * @return A pointer to an AST of type @c [+name+]_type.
 */
extern struct ast *make_[+name+]
([+ FOR cont ', ' +][+type+][+ ENDFOR cont +]
 [+ IF (and (exist? "cont") (exist? "sub")) +], [+ ENDIF +]
 [+ FOR sub ', ' +]struct ast *[+ ENDFOR sub +]);
[+ ENDFOR types +]

/** 
 * Create a duplicate of the AST structure s.
 * 
 * @param s AST to duplicate.
 * 
 * @return The copy of @c s.
 */
extern struct ast *ast_dup (const struct ast *s);

/** 
 * Free the AST @c s.
 * 
 * @param s The AST to free.
 * 
 * @return NULL
 */
extern struct ast *ast_free (struct ast *s);

/** 
 * Free an AST structure and overwrite it with NULL.
 * 
 * @param S AST to free.
 */
#define AST_FREE(S) do {			\
    (S) = ast_free (S);				\
  } while (0)

#endif
[+ == c +]
#include "config.h"

#include "ast.h"
#include "ast_util.h"
#include "free.h"
#include "xalloc.h"

#include <stdlib.h>

[+ FOR types +]
struct ast *
make_[+name+] ([+ FOR cont ', ' +][+type+] [+call+][+ ENDFOR cont +]
	       [+ IF (and (exist? "cont") (exist? "sub")) +], [+ ENDIF +]
	       [+ FOR sub ', ' +]struct ast *[+sub+][+ ENDFOR sub +])
{
  struct ast template = { [+ FOR top_level +]([+type+]) 0, [+ ENDFOR +]
			  [+ (count "sub") +] };
  struct ast *out = xmemdup (&template, sizeof *out +
			     sizeof out->ops[0] * ([+ (count "sub") +] - 1));
  out->type = [+name+]_type;
  [+ FOR extra +]
    out->op.[+name+].[+call+] = ([+type+]) 0;
  [+ ENDFOR extra +];
  [+ FOR cont +]
    out->op.[+name+].[+call+] = [+call+];
  [+ ENDFOR cont +];
  [+ FOR sub +]
    out->ops[[+ (for-index) +]] = [+sub+];
  [+ ENDFOR sub +];
  return out;
}
[+ ENDFOR types +]

/** 
 * Mutate a variable X according to the function F.
 *
 * Take the return value from the function F applied to X (so long as
 * X is not NULL) and then store it back in to X.
 * 
 * @param X The variable to mutate.
 * @param F The function to use.
 */
#define USE_RETURN(X, F) do { if ((X) != NULL) (X) = F (X); } while (0)

struct ast *
ast_dup (const struct ast *s)
{
  if (s == NULL)
    return NULL;

#if USE_REFCOUNT
  s->refs++;
  return s;
#else
  struct ast *out = xmemdup (s, sizeof *s +
			     sizeof s->ops[0] * (s->num_ops - 1));

  [+ FOR top_level +]
    [+ IF (== "char *" (get "type")) +]
    USE_RETURN (out->[+call+], xstrdup);
  [+ ELIF (== "struct ast *" (get "type")) +]
    USE_RETURN (out->[+call+], ast_dup);
  [+ ELIF (== "struct loc *" (get "type")) +]
    USE_RETURN (out->[+call+], loc_dup);
  [+ ENDIF +]
    [+ ENDFOR top_level+];

  switch (out->type)
    {
      [+ FOR types +]
    case [+name+]_type:
      [+ FOR cont +]
	[+ IF (== "char *" (get "type")) +]
	USE_RETURN (out->op.[+name+].[+call+], xstrdup);
      [+ ENDIF +]
	[+ ENDFOR cont +]
	break;
      [+ ENDFOR types+]
    default:
      break;
    }

  int i;
  for (i = 0; i < out->num_ops; i++)
    USE_RETURN (out->ops[i], ast_dup);
  return out;
#endif	/* USE_REFCOUNT */
}

struct ast *
ast_free (struct ast *s)
{
  if (s == NULL)
    return NULL;

#if USE_REFCOUNT
  if (s->refs != 0)
    {
      s->refs--;
      return s;
    }
#endif

  [+ FOR top_level +]
    [+ IF (== "char *" (get "type")) +]
    FREE (s->[+call+]);
  [+ ELIF (== "struct ast *" (get "type")) +]
    AST_FREE (s->[+call+]);
  [+ ELIF (== "struct loc *" (get "type")) +]
    FREE_LOC (s->[+call+]);
  [+ ENDIF +]
    [+ ENDFOR top_level +];

  switch (s->type)
    {
      [+ FOR types +]
    case [+name+]_type:
      [+ FOR cont +]
	[+ IF (== "char *" (get "type")) +]
	FREE (s->op.[+name+].[+call+]);
      [+ ENDIF +]
	[+ ENDFOR cont +]
	break;
      [+ ENDFOR types +]
    default:
      break;
    }

  int i;
  for (i = 0; i < s->num_ops; i++)
    AST_FREE (s->ops[i]);

  FREE (s);
  return NULL;
}

[+ ESAC +]

/* Hey Emacs!
Local Variables:
mode: c
End:
*/
