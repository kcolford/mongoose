/**
 * @file   compilation_passes.c
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief This is the routine that runs all the different passes in
 * the correct order.
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

#include "ast.h"
#include "compiler.h"

int
run_compilation_passes (struct ast **ss)
{
  int ret = 0;
  ret = ret || semantic (*ss);
  ret = ret || dealias (ss);
  ret = ret || collect_vars (*ss);
  ret = ret || transform (ss);
  ret = ret || optimizer (ss);
  ret = ret || gen_code (*ss);
  AST_FREE (*ss);
  return ret;
}
