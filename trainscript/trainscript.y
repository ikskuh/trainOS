%{
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

typedef union YYSTYPE YYSTYPE;

// stuff from flex that bison needs to know about:
int yylex (YYSTYPE * yylval_param , void *yyscanner);

void yyerror(void *scanner, const char *s);

#define scanner context->scanner


%}

%pure-parser
%lex-param {void * scanner}
%parse-param {ParserData * context}


%union {
    float fval;
	int ival;
	char *text;
	int indentation;
	type_t type;
	VariableDeclaration declaration;
}

%token TAB
%token TYPENAME
%token SEMICOLON
%token COLON
%token COMMA
%token PIPE
%token PLUS
%token MINUS
%token MULTIPLY
%token DIVIDE
%token MODULO
%token LBRACKET
%token RBRACKET
%token RARROW
%token LARROW

%token <fval> FLOAT
%token <ival> INT
%token <text> IDENTIFIER

%token KW_PUB
%token KW_PRI
%token KW_VAR
%token KW_PTR
%token KW_VOID
%token KW_INT
%token KW_REAL
%token KW_TEXT

%type <ival> expression
%type <type> typeName
%type <declaration> variableDeclaration
%type <indentation> indentation

%start input

%left PLUS MINUS MULTIPLY DIVIDE MODULO

%%
input:
	%empty
|   input variableDeclaration SEMICOLON { printf("decl %s as %d^%d\n", $2.name, $2.type.type, $2.type.pointer); }
|   input method { printf("method declaration.\n"); }
;

method:
	methodDeclaration body
;

body:
	%empty
|   body indentation assignment SEMICOLON { printf("Indent: %d\n", $2); }
;


methodDeclaration:
	KW_PUB IDENTIFIER LBRACKET argumentList RBRACKET methodLocals
|   KW_PRI IDENTIFIER LBRACKET argumentList RBRACKET methodLocals
|   KW_PUB IDENTIFIER LBRACKET argumentList RBRACKET RARROW argument methodLocals
|   KW_PRI IDENTIFIER LBRACKET argumentList RBRACKET RARROW argument methodLocals
;

methodLocals:
	%empty
|   PIPE methodLocalList
;

methodLocalList:
	argument
|   methodLocalList COMMA argument


argumentList:
	%empty
|   argument
|   argumentList COMMA argument
;

argument:
	IDENTIFIER COLON typeName
;

assignment:
	expression RARROW IDENTIFIER { printf("%s := %d\n", $3, $1); }

expression:
	INT                            { $$ = $1; }
|   IDENTIFIER                     { printf("[access %s]", $1); $$ = 1; }
|   IDENTIFIER LBRACKET expressionList RBRACKET { printf("[call %s]", $1); $$ = 1; }
|   LBRACKET expression RBRACKET   { $$ = $2; }
|   expression PLUS expression     { $$ = $1 + $3; }
|   expression MINUS expression    { $$ = $1 - $3; }
|   expression MULTIPLY expression { $$ = $1 * $3; }
|   expression DIVIDE expression   { $$ = $1 / $3; }
|   expression MODULO expression   { $$ = $1 % $3; }
;

expressionList:
	%empty
|   expression
|   expressionList COMMA expression
;

variableDeclaration:
	KW_VAR IDENTIFIER COLON typeName  { $$.name = $2; $$.type = $4; }
;

typeName:
	KW_VOID                           { $$.type = tidVOID; $$.pointer = 0; }
|   KW_INT                            { $$.type = tidINT; $$.pointer = 0; }
|   KW_REAL                           { $$.type = tidREAL; $$.pointer = 0; }
|   KW_TEXT                           { $$.type = tidTEXT; $$.pointer = 0; }
|   KW_PTR LBRACKET typeName RBRACKET { $$ = $3; $$.pointer++; }
;

indentation:
	TAB { $$ = 1; }
|   indentation TAB { $$ = $1 + 1; }
;

%%

#undef scanner

#include "trainscript.l.h"

void yyerror(void *scanner, const char *s) {
    // printf("Error: %s\n", s);
}
