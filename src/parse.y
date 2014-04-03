%error-verbose

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
%left '(' ')' '[' ']' '.'

%union { struct ast *ast_val; }
%type <ast_val> file def defargs body statement constrval lval rval callargs

%%

input:		file END { if (semantic ($1) || optimizer (&$1) || gen_code ($1)) YYERROR; }
	;

file:		/* empty */   { $$ = NULL; }
	|	def file      { $$ = make_block ($1, $2); }
	;

/* Function definitions. */
def:		STR STR '(' defargs ')' '{' body '}' { $$ = make_function ($1, $2, $4, $7); }
	|	STR STR '('         ')' '{' body '}' { $$ = make_function ($1, $2, NULL, $6); }
	;

defargs:	STR STR             { $$ = make_block (make_variable ($1, $2), NULL); }
	|	STR STR ',' defargs { $$ = make_block (make_variable ($1, $2), $4); }
	;

body:		/* empty */         { $$ = NULL; }
	|	statement body      { $$ = make_block ($1, $2); }
	;

/* Code statements. */
statement:	';'                             { $$ = NULL; }
	|	rval ';'                        { $$ = make_statement ($1); }
	|	IF '(' rval ')' statement       { $$ = make_cond ($3, $5); }
	|	IF '(' rval ')' '{' body '}'    { $$ = make_cond ($3, $6); }
	|	STR ':' statement               { $$ = make_label ($1, $3); }
	|	GOTO STR ';'                    { $$ = make_jump ($2); }
	|	WHILE '(' rval ')' statement    { $$ = make_whileloop ($3, $5); }
	|	WHILE '(' rval ')' '{' body '}' { $$ = make_whileloop ($3, $6); }
	|	RETURN ';'                      { $$ = make_ret (NULL); }
	|	RETURN rval ';'                 { $$ = make_ret ($2); }
	;

/* Adjacent strings are concatenated together. */
str:		STRING     { $$ = $1; }
	|	str STRING { $$ = my_strcat ($1, $2); }
	;

/* These are all the constant expressions. */
constrval:	INT { $$ = make_integer ($1); }
	|	str { $$ = make_string ($1); }
	;

/* Expressions that can be written to. */
lval:		STR STR              { $$ = make_variable ($1, $2); }
	|	STR STR '[' INT ']'  { $$ = make_array ($1, $2, $4); }
	|	STR                  { $$ = make_variable (NULL, $1); }
	|	rval '[' rval ']'    { $$ = make_binary ('[', $1, $3); }
	|	'*'rval %prec SIZEOF { $$ = make_unary ('*', $2); }
	;

/* Expressions that can be read from. */
rval:		STR '(' ')'           { $$ = make_function_call (make_variable (NULL, $1), NULL); }
	|	STR '(' callargs ')'  { $$ = make_function_call (make_variable (NULL, $1), $3); }
	|	lval '=' rval         { $$ = make_binary ('=', $1, $3); }
	|	rval '<' rval         { $$ = make_binary ('<', $1, $3); }
	|	rval '>' rval         { $$ = make_binary ('>', $1, $3); }
	|	rval '&' rval         { $$ = make_binary ('&', $1, $3); }
	|	rval '|' rval         { $$ = make_binary ('|', $1, $3); }
	|	rval '^' rval         { $$ = make_binary ('^', $1, $3); }
	|	rval '+' rval         { $$ = make_binary ('+', $1, $3); }
	|	rval '-' rval         { $$ = make_binary ('-', $1, $3); }
	|	rval '*' rval         { $$ = make_binary ('*', $1, $3); }
	|	rval '/' rval         { $$ = make_binary ('/', $1, $3); }
	|	rval '%' rval         { $$ = make_binary ('%', $1, $3); }
	|	rval EQ rval          { $$ = make_binary (EQ, $1, $3); }
	|	rval NE rval          { $$ = make_binary (NE, $1, $3); }
	|	rval LE rval          { $$ = make_binary (LE, $1, $3); }
	|	rval GE rval          { $$ = make_binary (GE, $1, $3); }
	|	rval RS rval          { $$ = make_binary (RS, $1, $3); }
	|	rval LS rval          { $$ = make_binary (LS, $1, $3); }
	|	'&'lval %prec SIZEOF  { $$ = make_unary ('&', $2); }
	|	lval INC              { $$ = make_crement (0, 1, $1); }
	|	lval DEC              { $$ = make_crement (0, 0, $1); }
	|	INC lval              { $$ = make_crement (1, 1, $2); }
	|	DEC lval              { $$ = make_crement (1, 0, $2); }
	|	'-'rval %prec SIZEOF  { $$ = make_unary ('-', $2); }
	|	'(' rval ')'          { $$ = $2; }
	|	lval                  { $$ = $1; }
	|	constrval             { $$ = $1; }
	;

/* Function call arguments. */
callargs:	rval              { $$ = make_block ($1, NULL); }
	|	rval ',' callargs { $$ = make_block ($1, $3); }
	;

%%

void
yyerror (const char *s)
{
  fprintf (stderr, "%s\n", s);
}
