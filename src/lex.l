/* This is the lexer.

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

%option noyywrap

%{
#include "config.h"

#include "compiler.h"
#include "xalloc.h"
  
#include <stdlib.h>
#include <string.h>

char *file_name = NULL;
int lineno = 1;

%}

%x LINE_COMMENT
%x BLOCK_COMMENT

%%

"=="                   { return EQ; }
"!="                   { return NE; }
">="                   { return GE; }
"<="                   { return LE; }
">>"                   { return RS; }
"<<"                   { return LS; }
"++"                   { return INC; }
"--"                   { return DEC; }

"return"               { return RETURN; }
"if"                   { return IF; }
"goto"                 { return GOTO; }
"while"                { return WHILE; }

[ \t\r]                ;
-?[0-9]+               { yylval.i = atoi (yytext); return INT; }
[_a-zA-Z][_a-zA-Z0-9]* { yylval.str = xstrdup (yytext); return STR; }
\"([^\\\"]|\\[^\"])*\" { yylval.str = xmemdup (yytext + 1, yyleng - 2); return STRING; }

"/*"                   { BEGIN (BLOCK_COMMENT); }
<BLOCK_COMMENT>.       ;
<BLOCK_COMMENT>"*/"    { BEGIN (INITIAL); }

"//"                   { BEGIN (LINE_COMMENT); }
<LINE_COMMENT>.        ;
<LINE_COMMENT>\n       { lineno++; BEGIN (INITIAL); }

<*>\n                  { lineno++; }

.                      { return yytext[0]; }
