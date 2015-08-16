%{
#include <stdlib.h>
#include "common.h"
#include "trainscript.tab.hpp"

%}
%option noyywrap
%option never-interactive
%option reentrant
%option bison-bridge
%option yylineno
%%
\#[^\n]*        ; // Eat all the comments!
[ \t]+          ; // Eat all the whitespace!
\;              { return SEMICOLON; }
\:              { return COLON; }
\,              { return COMMA; }
\|              { return PIPE; }
\(              { return LBRACKET; }
\)              { return RBRACKET; }
\+              { return PLUS; }
\-              { return MINUS; }
\*              { return MULTIPLY; }
\/              { return DIVIDE; }
\%              { return MODULO; }
\-\>|→          { return RARROW; }
\<\-|←          { return LARROW; }

\<              { return OP_LT; }
\<\=            { return OP_LE; }
\>              { return OP_GT; }
\>\=            { return OP_GE; }
\=              { return OP_EQ; }
\≠|\=\/\=       { return OP_NEQ; }

VAR             { return KW_VAR; }
PUB             { return KW_PUB; }
PRI             { return KW_PRI; }
PTR             { return KW_PTR; }

VOID            { return KW_VOID; }
INT             { return KW_INT; }
REAL            { return KW_REAL; }
TEXT            { return KW_TEXT; }
BOOL            { return KW_BOOL; }

BEGIN           { return KW_BEGIN; }
END             { return KW_END; }
IF              { return KW_IF; }
THEN            { return KW_THEN; }
ELSE            { return KW_ELSE; }
ELSEIF          { return KW_ELSEIF; }
REPEAT          { return KW_REPEAT; }
FROM            { return KW_FROM; }
TO              { return KW_TO; }
UNTIL           { return KW_UNTIL; }
WHILE           { return KW_WHILE; }
DO              { return KW_DO; }
[0-9]+\.[0-9]*  { yylval->fval = atof(yytext); return REAL; }
[0-9]+          { yylval->ival = atoi(yytext); return INT; }
[a-zA-Z0-9'_]+  { yylval->text = yyextra->strdup(yytext); return IDENTIFIER; }
.               { yyerror(NULL, "illegal token"); }
%%
/*
\t              { return TAB; }
*/
