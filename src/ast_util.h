/**
 * @file   ast_util.h
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  A series of utility functions for use with the AST.
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

#ifndef AST_UTIL_H
#define AST_UTIL_H

#include "ast.h"
#include "lib.h"

/** 
 * This swaps to ASTs in seperate AST chains.
 * 
 * @param X 
 * @param Y 
 * 
 */
#define SWAP_AST(X, Y) do {			\
    SWAP ((X)->next, (Y)->next);		\
    SWAP (X, Y);				\
  } while (0)

/** 
 * This concatenates two lists of ASTs.
 * 
 * @param l Left AST.
 * @param r Right AST.
 * 
 * @return Concatenation of them both.
 */
static inline struct ast *
ast_cat (struct ast *l, struct ast *r)
{
  if (l == NULL)
    return r;
  else
    {
      struct ast *t = l;
      while (t->next != NULL)
	t = t->next;
      t->next = r;
      return l;
    }
}

#endif
