/**
 * @file   
 * @author Kieran Colford <colfordk@gmail.com>
 * 
 * @brief  This is the lexer.
 * 
 * Copyright (C) 2014 Kieran Colford
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

%option noyywrap
%option yylineno

%{
#include "config.h"

#include "ast.h"
#include "compiler.h"
#include "free.h"
#include "parse.h"
#include "xalloc.h"

#include <stdlib.h>
#include <string.h>

#define static			/**< Eliminate generated warnings. */
#define YY_NO_INPUT		/**< We have no use for it. */

%}

%%

 /* As one can see, there are a number of multi-character lexemes that
    have to be supported. */
"=="                   { return EQ; }
"!="                   { return NE; }
">="                   { return GE; }
"<="                   { return LE; }
">>"                   { return RS; }
"<<"                   { return LS; }
"++"                   { return INC; }
"--"                   { return DEC; }
"&&"                   { return AND; }
"||"                   { return OR; }
"+="                   { return MUT_ADD; }
"-="                   { return MUT_SUB; }
"*="                   { return MUT_MUL; }
"/="                   { return MUT_DIV; }
"%="                   { return MUT_MOD; }
">>="                  { return MUT_RS; }
"<<="                  { return MUT_LS; }
"&="                   { return MUT_AND; }
"|="                   { return MUT_OR; }
"^="                   { return MUT_XOR; }

 /* These are the handful of keywords that are currently supported. */
"return"               { return RETURN; }
"if"                   { return IF; }
"goto"                 { return GOTO; }
"while"                { return WHILE; }
"static"               { return STATIC; }
"extern"               { return EXTERN; }
"inline"               { return INLINE; }
"for"                  { return FOR; }
"do"                   { return DO; }
"else"                 { return ELSE; }
"const"                { return CONST; }
"struct"               { return STRUCT; }
"typedef"              { return TYPEDEF; }
"union"                { return UNION; }
"enum"                 { return ENUM; }
"switch"               { return SWITCH; }
"case"                 { return CASE; }
"default"              { return DEFAULT; }
"break"                { return BREAK; }
"continue"             { return CONTINUE; }

 /* Whitespace is thrown out. */
[ \t\r\n]              ;

 /* Different styles of inputting numbers. */
[-+]?0[0-8]*           { yylval.i = strtoll (yytext, NULL, 8); return INT; }
[-+]?[1-9][0-9]*       { yylval.i = strtoll (yytext, NULL, 10); return INT; }
[-+]?0x[0-9a-f]+       { yylval.i = strtoll (yytext, NULL, 16); return INT; }
[-+]?0x[0-9A-F]+       { yylval.i = strtoll (yytext, NULL, 16); return INT; }

 /* Symbols are copied and passed over to parser to decide their
    fate. */
[_a-zA-Z][_a-zA-Z0-9]* { yylval.str = xstrdup (yytext); return STR; }

 /* Strings are extracted and passed unformated to the assembler so
    that it can deal with any escape sequences. */
\"([^\\\"]|\\[^\"])*\" {
  yylval.str = xmemdup (yytext + 1, yyleng - 1);
  yylval.str[yyleng - 2] = '\0';
  return STRING;
}

 /* Extract the current line number and file name from the directives
    left by the C preprocessor. */
"# "[0-9]+.*           {
#if defined HAVE_C_VARARRAYS
  char t[yyleng + 1];
#elif defined HAVE_ALLOCA
  char *t = alloca (yyleng + 1);
#else
  char *t = xmalloc (yyleng + 1);
#endif
  strcpy (t, yytext);
  yylineno = strtol (strtok (t, "# \""), NULL, 10);
  FREE (file_name);
  file_name = xstrdup (strtok (NULL, "# \""));

#if !defined HAVE_C_VARARRAYS && !defined HAVE_ALLOCA
  FREE (t);
#endif
}

 /* Ignore any pragmas found in the source. */
"#"[ \t]*"pragma".*    ;

 /* If something didn't fit into any of the above categories, we'll
    return it to the parser to let it deal with whether it's valid or
    should throw an error. */
.                      { return yytext[0]; }
