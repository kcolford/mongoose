%option noyywrap

%{
#include "config.h"

#include "compiler.h"
  
#include <stdlib.h>
#include <string.h>
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

[ \t\r\n]+             ;
-?[0-9]+               { yylval.i = atoi (yytext); return INT; }
[_a-zA-Z][_a-zA-Z0-9]* { yylval.str = xstrdup (yytext); return STR; }
\"([^\\\"]|\\[^\"])*\" { yylval.str = xmemdup (yytext + 1, yyleng - 2); return STRING; }

"/*"                   { BEGIN (BLOCK_COMMENT); }
<BLOCK_COMMENT>.|\n    ;
<BLOCK_COMMENT>"*/"    { BEGIN (INITIAL); }

"//"                   { BEGIN (LINE_COMMENT); }
<LINE_COMMENT>.        ;
<LINE_COMMENT>\n       { BEGIN (INITIAL); }

.                      { return yytext[0]; }
