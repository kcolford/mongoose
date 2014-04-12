/* This is the main header file for Compiler.

Copyright (C) 2014 Kieran Colford

This file is part of Compiler.

Compiler is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Compiler is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with Compiler; see the file COPYING.  If not see
<http://www.gnu.org/licenses/>. */

#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"
#include "parse.h"
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <error.h>

#include "gettext.h"

/* Special macros for making it easier to mark strings for
   translation. */
#define _(s) gettext (s)
#define N_(s) gettext_noop (s)

extern FILE *yyin;		/* The input stream for the lexer. */
extern FILE *outfile;		/* The output stream for the gen_code
				   routine. */

extern char *file_name;		/* The current file name as determined
				   by the lexer.  This is always
				   either NULL or a dynamically
				   allocated string. */
extern int lineno;		/* The current line number as
				   determined by the lexer. */

extern int optimize;		/* A flag describing the optimization
				   levels that each phase must adhere
				   to. */
extern int debug;		/* A flag that if true says that all
				   assembly will be echoed to
				   stdout. */

/* **************************************************************** */
/* These are the compilation passes that manipulate the AST and verify
   its correctness.  They all signal an error through their return
   values so that the parser can identify where the problem occured.
   A return value of 0 means that it successfully completed the pass,
   while any other return value indicates error. */

/* The code generation routine. */
extern int gen_code (struct ast *);

/* The optimizer. */
extern int optimizer (struct ast **);

/* The pass that translates all aliases into what they really mean. */
extern int dealias (struct ast **);

/* The pass that verifies the integrity of the AST and that it
   satisfies the semantics of the C language. */
extern int semantic (struct ast *); 

/* This runs all the above routines in order and collects their return
   values. */
extern int run_compilation_passes (struct ast **);

/* **************************************************************** */
/* These are the general utility functions that provide things that
   the C library doesn't. */

/* This concatenates two dynamically allocated strings and frees its
   arguments. */
extern char *my_strcat (char *, char *);

/* This creates a unique string to act as a place holder when one
   isn't already provided. */
extern char *place_holder (void);

/* This is a printf like function that returns a dynamically allocated
   string based on the format specifier. */
extern char *my_printf (const char *, ...);

/* This creates an appropriate file name that can be used as a
   temporary file. */
extern char *tmpfile_name (void);

/* This is a routine that forks the calling process and then calls
   exec to run another program (while the original program waits for
   it to return). */
extern int safe_system (const char **);

/* This concatenates two lists of ASTs. */
extern struct ast *ast_cat (struct ast *, struct ast *);

#endif
