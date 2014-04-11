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

#define _(s) gettext (s)
#define N_(s) gettext_noop (s)

extern FILE *yyin;
extern FILE *outfile;

extern char *file_name;
extern int lineno;

extern int optimize;
extern int debug;

extern int gen_code (struct ast *);
extern int optimizer (struct ast **);
extern int dealias (struct ast **);
extern int semantic (struct ast *);

extern int run_compilation_passes (struct ast **);

extern char *my_strcat (char *, char *);
extern char *place_holder (void);
extern char *my_printf (const char *, ...);
extern char *tmpfile_name (void);
extern int safe_system (const char **);
extern struct ast *ast_cat (struct ast *, struct ast *);

#endif
