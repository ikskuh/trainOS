%{
#include <stdlib.h>
#include "common.h"
#include "trainscript.tab.h"

%}
%option noyywrap
%option never-interactive
%option reentrant
%option bison-bridge
%option yylineno
%%
\#[^\n]*        ; // Eat all the comments!
[ ]+            ; // Eat all the whitespace!
\t              { return TAB; }
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
VAR             { return KW_VAR; }
PUB             { return KW_PUB; }
PRI             { return KW_PRI; }
PTR             { return KW_PTR; }
VOID            { return KW_VOID; }
INT             { return KW_INT; }
REAL            { return KW_REAL; }
TEXT            { return KW_TEXT; }
[0-9]+          { yylval->ival = atoi(yytext); return INT; }
[a-zA-Z0-9]+    { yylval->text = strdup(yytext); return IDENTIFIER; }
.               { yyerror(NULL, "illegal token"); }
%%

/*
[0-9]+          { yylval.ival = atoi(yytext); return INT; }
[a-zA-Z0-9]+    { yylval.sval = strdup(yytext); return STRING; }
[0-9]+\.[0-9]+  { yylval->fval = atof(yytext); return FLOAT; }

*/
