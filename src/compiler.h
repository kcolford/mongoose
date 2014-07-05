/**
 * @file   compiler.h
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the main header file for Compiler.
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
 * The methods that return int are the compilation passes that
 * manipulate the AST and verify its correctness.  They all signal an
 * error through their return values so that the parser can identify
 * where the problem occured.  A return value of 0 means that it
 * successfully completed the pass, while any other return value
 * indicates error.
 *
 * The other routines are for the general purpose use of managing the
 * compiler.
 */

#ifndef COMPILER_H
#define COMPILER_H

#include "gl_xlist.h"

#include <stdio.h>

/** 
 * Create a string for a builtin name.
 * 
 * @param NAME The name to be transformed into a builtin.
 * 
 * @return The builtin version of @c NAME.
 */
#define BUILTIN(NAME) ("__builtin_" #NAME)

extern FILE *yyin;		/**< The input stream for the
				   lexer. */
extern FILE *outfile;		/**< The output stream for the
				   gen_code routine. */

extern gl_list_t infile_name;	/**< The current input file as
				   determined by the command line. */
extern const char *outfile_name; /**< The current output file as
				    determined by the command line. */

extern char *file_name;		/**< The current file name as
				   determined by the lexer.  This is
				   always either NULL or a dynamically
				   allocated string. */
extern int yylineno;		/**< The current line number as
				   determined by the lexer. */

extern int optimize;		/**< A flag describing the
				   optimization levels that each phase
				   must adhere to. */
extern int yydebug;		/**< A flag that if true will cause
				   the Yacc parser to issue debugging
				   output. */
extern int debug;		/**< A flag that if true says that all
				   assembly will be echoed to
				   stdout. */

extern char stop;		/**< A character that defines how far
				   the compiler should go during its
				   compilation routines. */

struct ast;

/** 
 * The code generation phase.
 * 
 * @param s The AST structure that will become the code.
 * 
 * @return Error code.
 */
extern int gen_code (struct ast *s);

/** 
 * The optimization pass.
 * 
 * @param ss A reference to the AST structure that must be optimized.
 * 
 * @return Error code.
 */
extern int optimizer (struct ast **ss);

/** 
 * The transformation pass for the lower level passes.
 * 
 * @param ss A reference to the AST structure that must be transformed.
 * 
 * @return Error code.
 */
extern int transform (struct ast **ss);

/** 
 * This pass collects all the allocated variables into the start of
 * the function definition, making it easier to optimize them into a
 * single allocation.
 * 
 * @param s The AST to operate on.
 * 
 * @return Error code.
 */
extern int collect_vars (struct ast *s);

/** 
 * The de-alias pass translates variable names into something we can
 * understand.
 * 
 * @param ss A reference to the AST to operate on.
 * 
 * @return Error code.
 */
extern int dealias (struct ast **ss);

/** 
 * The pass that verifies the integrity of the AST and that it
 * satisfies the semantics of the C language.
 * 
 * @param s The AST to operate on.
 * 
 * @return Error code.
 */
extern int semantic (struct ast *s);

/** 
 * This runs all the above routines in order and collects their return
 * values.
 * 
 * @param ss A reference to the AST to operate on.
 * 
 * @return Error code.
 */
extern int run_compilation_passes (struct ast **ss);

/** 
 * This initializes the global variables.
 * 
 */
extern void vars_init (void);

/**
 * This is the one routine that the triggers the entire compilation
 * unit to run. 
 *
 */
extern void run_unit (void);

#endif
