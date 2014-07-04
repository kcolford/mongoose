/**
 * @file   
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the parser for the program.
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

%error-verbose
%define parse.lac full

%expect 1

%{
#include "config.h"

#include "ast.h"
#include "ast_util.h"
#include "compiler.h"
#include "lib.h"
#include "xalloc.h"

#include <stdlib.h>
#include <string.h>

int yydebug = 0;

extern int yylex (void);
void yyerror (const char *);
static struct ast *make_ifstatement (struct ast *, struct ast *);
static struct ast *make_dowhileloop (struct ast *, struct ast *);
static struct ast *make_whileloop (struct ast *, struct ast *);
static struct ast *make_array (char *, char *, struct ast *);
static struct ast *make_forloop (struct ast *, struct ast *, struct ast *, struct ast *);
static struct ast *make_ifelse (struct ast *, struct ast *, struct ast *);

#ifndef YYDEBUG
#define YYDEBUG 1
#endif
%}

%token END 0 "end of file"

%token RETURN "return"
%token IF "if"
%token GOTO "goto"
%token WHILE "while"
%token STATIC "static"
%token EXTERN "extern"
%token INLINE "inline"
%token FOR "for"
%token DO "do"
%token ELSE "else"
%token CONST "const"
%token STRUCT "struct"
%token TYPEDEF "typedef"
%token UNION "union"
%token ENUM "enum"
%token SWITCH "switch"
%token CASE "case"
%token DEFAULT "default"
%token BREAK "break"
%token CONTINUE "continue"

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

%left ','
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
%type <ast_val> body callargs constrval def defargs expr file maybe_expr scoped_body statement sub_body

%token MAX_TOKEN		/*
This is maximum value of any token, this is why it is placed last in
the list. */


%%

input:		file END { if (run_compilation_passes (&$1)) YYERROR; }
	;

file:		/* empty */   { $$ = NULL; }
	|	def file      { $$ = ast_cat ($1, $2); }
	;

/* Function definitions. */
def:		STR STR '(' defargs ')' scoped_body { $$ = make_function ($1, $2, $4, $6); }
	|	STR STR '('         ')' scoped_body { $$ = make_function ($1, $2, NULL, $5); }
	;

defargs:	STR STR             { $$ = make_variable ($1, $2); }
	|	defargs ',' defargs { $$ = ast_cat ($1, $3); }
	;

body:		/* empty */         { $$ = NULL; }
	|	statement body      { $$ = ast_cat ($1, $2); }
	;

scoped_body:    '{' body '}' { $$ = make_block ($2); }
        ;

sub_body:       scoped_body { $$ = $1; }
	|	statement   { $$ = $1; }
        ;

maybe_expr:     /* empty */ { $$ = NULL; }
	|	expr        { $$ = $1; }
        ;

/* Code statements. */
statement:	';'                             { $$ = NULL; }
	|	STR STR ';'                     { $$ = make_variable ($1, $2); }
	|	STR STR '[' expr ']' ';'        { $$ = make_array ($1, $2, $4); }
	|	STR STR '=' expr ';'            { $$ = make_binary ('=', make_variable ($1, $2), $4); $$->throw_away = 1; }
	|	expr ';'                        { $$ = $1; $$->throw_away = 1; }
	|	IF '(' expr ')' sub_body        { $$ = make_ifstatement ($3, $5); }
	|	IF '(' expr ')' sub_body ELSE sub_body { $$ = make_ifelse ($3, $5, $7); }
	|	STR ':' statement               { $$ = ast_cat (make_label ($1), $3); }
	|	GOTO STR ';'                    { $$ = make_jump ($2); }
	|	WHILE '(' expr ')' sub_body     { $$ = make_whileloop ($3, $5); }
	|	DO sub_body WHILE '(' expr ')' ';' { $$ = make_dowhileloop ($5, $2); }
	|	FOR '(' maybe_expr ';' expr ';' maybe_expr ')' sub_body { $$ = make_forloop ($3, $5, $7, $9); }
	|	FOR '(' maybe_expr ';' ';' maybe_expr ')' sub_body { $$ = make_forloop ($3, make_integer (1), $6, $8); }
	|	RETURN ';'                      { $$ = make_ret (NULL); }
	|	RETURN expr ';'                 { $$ = make_ret ($2); }
	;

/* Adjacent strings are concatenated together. */
str:		STRING     { $$ = $1; }
	|	str STRING { $$ = my_printf ("%s%s", $1, $2); FREE ($1); FREE ($2); }
	;

/* These are all the constant expressions. */
constrval:	INT { $$ = make_integer ($1); }
	|	str { $$ = make_unary ('&', make_string ($1)); }
	;

/* Expressions. */
expr:		STR                   { $$ = make_variable (NULL, $1); }
	|	expr '(' ')'          { $$ = make_function_call ($1, NULL); }
	|	expr '(' callargs ')' { $$ = make_function_call ($1, $3); }
	|	expr '=' expr         { $$ = make_binary ('=', $1, $3); }
	|	expr MUT_ADD expr     { $$ = make_binary ('=', $1, make_binary ('+', ast_dup ($1), $3)); }
	|	expr MUT_SUB expr     { $$ = make_binary ('=', $1, make_binary ('-', ast_dup ($1), $3)); }
	|	expr MUT_MUL expr     { $$ = make_binary ('=', $1, make_binary ('*', ast_dup ($1), $3)); }
	|	expr MUT_DIV expr     { $$ = make_binary ('=', $1, make_binary ('/', ast_dup ($1), $3)); }
	|	expr MUT_MOD expr     { $$ = make_binary ('=', $1, make_binary ('%', ast_dup ($1), $3)); }
	|	expr MUT_LS expr      { $$ = make_binary ('=', $1, make_binary (LS, ast_dup ($1), $3)); }
	|	expr MUT_RS expr      { $$ = make_binary ('=', $1, make_binary (RS, ast_dup ($1), $3)); }
	|	expr MUT_AND expr     { $$ = make_binary ('=', $1, make_binary ('&', ast_dup ($1), $3)); }
	|	expr MUT_OR expr      { $$ = make_binary ('=', $1, make_binary ('|', ast_dup ($1), $3)); }
	|	expr MUT_XOR expr     { $$ = make_binary ('=', $1, make_binary ('^', ast_dup ($1), $3)); }
	|	expr '<' expr         { $$ = make_binary ('<', $1, $3); }
	|	expr '>' expr         { $$ = make_binary ('>', $1, $3); }
	|	expr '&' expr         { $$ = make_binary ('&', $1, $3); }
	|	expr '|' expr         { $$ = make_binary ('|', $1, $3); }
	|	expr '^' expr         { $$ = make_binary ('^', $1, $3); }
	|	expr '+' expr         { $$ = make_binary ('+', $1, $3); }
	|	expr '-' expr         { $$ = make_binary ('-', $1, $3); }
	|	expr '*' expr         { $$ = make_binary ('*', $1, $3); }
	|	expr '/' expr         { $$ = make_binary ('/', $1, $3); }
	|	expr '%' expr         { $$ = make_binary ('%', $1, $3); }
	|	expr '[' expr ']'     { $$ = make_binary ('[', $1, $3); }
	|	expr EQ expr          { $$ = make_binary (EQ, $1, $3); }
	|	expr NE expr          { $$ = make_binary (NE, $1, $3); }
	|	expr LE expr          { $$ = make_binary (LE, $1, $3); }
	|	expr GE expr          { $$ = make_binary (GE, $1, $3); }
	|	expr RS expr          { $$ = make_binary (RS, $1, $3); }
	|	expr LS expr          { $$ = make_binary (LS, $1, $3); }
	|	'&'expr %prec SIZEOF  { $$ = make_unary ('&', $2); }
	|	'*'expr %prec SIZEOF  { $$ = make_unary ('*', $2); }
	|	'!'expr %prec SIZEOF  { $$ = $2; $$->boolean_not ^= 1; }
	|	expr INC              { $$ = make_unary (INC, $1); }
	|	expr DEC              { $$ = make_unary (DEC, $1); }
	|	INC expr              { $$ = make_unary (INC, $2); $$->unary_prefix = 1; }
	|	DEC expr              { $$ = make_unary (DEC, $2); $$->unary_prefix = 1; }
	|	'-'expr %prec SIZEOF  { $$ = make_unary ('-', $2); }
	|	'(' expr ')'          { $$ = $2; }
	|	constrval             { $$ = $1; }
	;

/* Function call arguments. */
callargs:	expr                  { $$ = $1; }
	|	callargs ',' callargs { $$ = ast_cat ($1, $3); }
	;

%%

void
no_op (void)
{
  ;
}

/** 
 * Print error message with current file name and line number.
 * 
 * @param msg Error message to be displayed.
 */
void
yyerror (const char *msg)
{
  error_at_line (0, 0, file_name, yylineno, "%s", msg);
}

struct ast *
make_ifstatement (struct ast *cond, struct ast *body)
{
  char *t = place_holder (), *tt = xstrdup (t);
  cond->boolean_not ^= 1;
  return ast_cat (make_cond (t, cond) , ast_cat (body, make_label (tt)));
}

struct ast *
make_dowhileloop (struct ast *cond, struct ast *body)
{
  char *t = place_holder (), *tt = xstrdup (t);
  return ast_cat (make_label (t), ast_cat (body, make_cond (tt, cond)));
}

struct ast *
make_whileloop (struct ast *cond, struct ast *body)
{
  char *t = place_holder (), *tt = xstrdup (t);
  return ast_cat (make_label (t), make_ifstatement (cond, ast_cat (body, make_jump (tt))));
}

struct ast *
make_array (char *type, char *name, struct ast *size)
{
  char *newtype = my_printf ("%s * const", type);
  FREE (type);
  size = make_binary ('*', size, make_integer (8));
  return make_binary ('=', make_variable (newtype, name), make_alloc (size));
}

struct ast *
make_forloop (struct ast *init, struct ast *cond, struct ast *step, struct ast *body)
{
  step->throw_away = 1;
  init->throw_away = 1;
  struct ast *out = ast_cat (body, step);
  out = make_whileloop (cond, out);
  out = ast_cat (init, out);
  return out;
}

struct ast *
make_ifelse (struct ast *cond, struct ast *body, struct ast *elsebody)
{
  char *t = place_holder (), *tt = xstrdup (t);
  body = ast_cat (body, make_jump (t));
  struct ast *out = ast_cat (make_label (tt), elsebody);
  out = ast_cat (make_ifstatement (cond, body), out);
  return out;
}
