/* This is the parser for the program.

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

%error-verbose
%define parse.lac full
%expect 6

%{
#include "config.h"

#include "compiler.h"

#include <stdlib.h>
#include <string.h>

int yydebug = 0;

void yyerror (const char *);

#define YYDEBUG 1
%}

%token END 0 "end of file"

%token RETURN "return"
%token IF "if"
%token GOTO "goto"
%token WHILE "while"
%token EQ "=="
%token NE "!="
%token LE "<="
%token GE ">="
%token RS ">>"
%token LS "<<"
%token OR "||"
%token AND "&&"
%token INC "++"
%token DEC "--"
%token MUT_ADD "+="
%token MUT_SUB "-="
%token MUT_MUL "*="
%token MUT_DIV "/="
%token MUT_MOD "%="
%token MUT_RS ">>="
%token MUT_LS "<<="
%token MUT_AND "&="
%token MUT_OR "|="
%token MUT_XOR "^="

%union { long long i; }
%token <i> INT

%union { char *str; }
%token <str> STR STRING
%type <str> str

%right '=' MUT_ADD MUT_SUB MUT_MUL MUT_DIV MUT_MOD MUT_RS MUT_LS MUT_AND MUT_OR MUT_XOR
%left OR
%left AND
%left '|'
%left '^'
%left '&'
%left EQ NE
%left '<' LE '>' GE
%left RS LS
%left '+' '-'
%left '*' '/' '%'
%right '!' '~' INC DEC SIZEOF
%nonassoc '(' ')' '[' ']' '.'

%union { struct ast *ast_val; }
%type <ast_val> expr file def defargs body statement constrval callargs

%%

input:		file END { if (run_compilation_passes (&$1)) YYERROR; }
	;

file:		/* empty */   { $$ = NULL; }
	|	def file      { $$ = make_block (0, $2, $1); }
	;

/* Function definitions. */
def:		STR STR '(' defargs ')' '{' body '}' { $$ = make_function (0, $7, $1, $2, $4); }
	|	STR STR '('         ')' '{' body '}' { $$ = make_function (0, $6, $1, $2, NULL); }
	;

defargs:	STR STR             { $$ = make_variable (0, NULL, $1, $2); }
	|	STR STR ',' defargs { $$ = make_variable (0, $4, $1, $2); }
	;

body:		/* empty */         { $$ = NULL; }
	|	statement body      { $$ = make_block (0, $2, $1); }
	;

/* Code statements. */
statement:	';'                             { $$ = NULL; }
	|	STR expr ';'                    { $$ = $2; $$->op.variable.type = $1; }
	|	expr ';'                        { $$ = $1; $$->flags |= AST_THROW_AWAY; }
	|	IF '(' expr ')' statement       { $$ = make_cond (0, $5, $3); }
	|	IF '(' expr ')' '{' body '}'    { $$ = make_cond (0, $6, $3); }
	|	STR ':' statement               { $$ = make_label (0, $3, $1); }
	|	GOTO STR ';'                    { $$ = make_jump (0, NULL, $2); }
	|	WHILE '(' expr ')' statement    { $$ = make_whileloop ($3, $5); }
	|	WHILE '(' expr ')' '{' body '}' { $$ = make_whileloop ($3, $6); }
	|	RETURN ';'                      { $$ = make_ret (0, NULL, NULL); }
	|	RETURN expr ';'                 { $$ = make_ret (0, NULL, $2); }
	;

/* Adjacent strings are concatenated together. */
str:		STRING     { $$ = $1; }
	|	str STRING { $$ = my_strcat ($1, $2); }
	;

/* These are all the constant expressions. */
constrval:	INT { $$ = make_integer (0, NULL, $1); }
	|	str { $$ = make_string (0, NULL, $1); }
	;

/* Expressions. */
expr:		STR                   { $$ = make_variable (0, NULL, NULL, $1); }
	|	expr '(' ')'          { $$ = make_function_call (0, NULL, $1, NULL); }
	|	expr '(' callargs ')' { $$ = make_function_call (0, NULL, $1, $3); }
	|	expr '=' expr         { $$ = make_binary (0, NULL, '=', $1, $3); }
	|	expr '<' expr         { $$ = make_binary (0, NULL, '<', $1, $3); }
	|	expr '>' expr         { $$ = make_binary (0, NULL, '>', $1, $3); }
	|	expr '&' expr         { $$ = make_binary (0, NULL, '&', $1, $3); }
	|	expr '|' expr         { $$ = make_binary (0, NULL, '|', $1, $3); }
	|	expr '^' expr         { $$ = make_binary (0, NULL, '^', $1, $3); }
	|	expr '+' expr         { $$ = make_binary (0, NULL, '+', $1, $3); }
	|	expr '-' expr         { $$ = make_binary (0, NULL, '-', $1, $3); }
	|	expr '*' expr         { $$ = make_binary (0, NULL, '*', $1, $3); }
	|	expr '/' expr         { $$ = make_binary (0, NULL, '/', $1, $3); }
	|	expr '%' expr         { $$ = make_binary (0, NULL, '%', $1, $3); }
	|	expr '[' expr ']'     { $$ = make_binary (0, NULL, '[', $1, $3); }
	|	expr EQ expr          { $$ = make_binary (0, NULL, EQ, $1, $3); }
	|	expr NE expr          { $$ = make_binary (0, NULL, NE, $1, $3); }
	|	expr LE expr          { $$ = make_binary (0, NULL, LE, $1, $3); }
	|	expr GE expr          { $$ = make_binary (0, NULL, GE, $1, $3); }
	|	expr RS expr          { $$ = make_binary (0, NULL, RS, $1, $3); }
	|	expr LS expr          { $$ = make_binary (0, NULL, LS, $1, $3); }
	|	'&'expr %prec SIZEOF  { $$ = make_unary (0, NULL, '&', $2); }
	|	'*'expr %prec SIZEOF  { $$ = make_unary (0, NULL, '*', $2); }
	|	expr INC              { $$ = make_unary (0, NULL, INC, $1); }
	|	expr DEC              { $$ = make_unary (0, NULL, DEC, $1); }
	|	INC expr              { $$ = make_unary (0, NULL, AST_UNARY_PREFIX | INC, $2); }
	|	DEC expr              { $$ = make_unary (0, NULL, AST_UNARY_PREFIX | DEC, $2); }
	|	'-'expr %prec SIZEOF  { $$ = make_unary (0, NULL, '-', $2); }
	|	'(' expr ')'          { $$ = $2; }
	|	constrval             { $$ = $1; }
	;

/* Function call arguments. */
callargs:	expr              { $$ = $1; }
	|	expr ',' callargs { $$ = $1; $$->next = $3; }
	;

%%

void
yyerror (const char *msg)
{
  error_at_line (0, 0, infile_name, lineno, "%s", msg);
}
