%{
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#include "tsvm.hpp"

using namespace trainscript;

namespace trainscript {
	namespace ops {
		Variable add(Variable lhs, Variable rhs);
		Variable subtract(Variable lhs, Variable rhs);
		Variable multiply(Variable lhs, Variable rhs);
		Variable divide(Variable lhs, Variable rhs);
		Variable modulo(Variable lhs, Variable rhs);
	}
}

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
	trainscript::Type type;
	VariableDeclaration varDecl;
	MethodDeclaration method;
	MethodBody *body;
	MethodHeader methodHeader;
	trainscript::Instruction *instruction;
	LocalVariable *local;
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

%token <fval> REAL
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

%type <type> typeName
%type <varDecl> variableDeclaration
%type <indentation> indentation

%type <method> method
%type <methodHeader> methodDeclaration
%type <body> body

%type <local> argument
%type <local> arguments
%type <local> argumentList

%type <local> methodLocals
%type <local> methodLocalList

%type <instruction> instruction
%type <instruction> expression

%start input

%left PLUS MINUS MULTIPLY DIVIDE MODULO RARROW

%%
input:
	%empty
|   input variableDeclaration SEMICOLON {
		context->module->variables.insert( { $2.name, new Variable($2.variable) } );
	}
|   input method {
		using namespace trainscript;
		auto *mod = context->module;
		Block *body = new Block(mod);

		// Translate body here
		MethodBody *mb = $2.body;
		while(mb) {
			if(mb->instruction != nullptr) {
				body->instructions.push_back(mb->instruction);
			} else {
				printf("invalid instruction in %s\n", $2.header.name);
			}
			mb = mb->next;
		}

		Method *method = new Method(mod, body);
		method->isPublic = $2.header.isPublic;
		if($2.header.returnValue) {
			method->returnValue = std::pair<std::string, Variable>(
				$2.header.returnValue->name,
				$2.header.returnValue->variable);
		}
		LocalVariable *local = $2.header.locals;
		while(local) {
			method->locals.insert( { local->name, local->variable } );
			local = local->next;
		}
		LocalVariable *arg = $2.header.arguments;
		while(arg) {
			method->arguments.push_back( { arg->name, arg->variable } );
			arg = arg->next;
		}

		context->module->methods.insert( { $2.header.name, method } );
	}
;

method:
	methodDeclaration body {
		$$.header = $1;
		$$.body = $2;
	}
;

body:
	%empty { $$ = nullptr; }
|   body indentation instruction SEMICOLON {
		auto *body = new MethodBody();
		body->indentation = $2;
		body->instruction = $3;
		if($1 == nullptr) {
			$$ = body;
		} else {
			$$ = $1;
			auto *it = $1;
			while(it->next) { it = it->next; }
			it->next = body;
		}
	}
;


methodDeclaration:
	KW_PUB IDENTIFIER LBRACKET arguments RBRACKET methodLocals {
		$$.isPublic = true;
		$$.arguments = $4;
		$$.locals = $6;
		$$.returnValue = nullptr;
		$$.name = $2;
	}
|   KW_PRI IDENTIFIER LBRACKET arguments RBRACKET methodLocals {
		$$.isPublic = false;
		$$.arguments = $4;
		$$.locals = $6;
		$$.returnValue = nullptr;
		$$.name = $2;
	}
|   KW_PUB IDENTIFIER LBRACKET arguments RBRACKET RARROW argument methodLocals {
		$$.isPublic = true;
		$$.arguments = $4;
		$$.locals = $8;
		$$.returnValue = $7;
		$$.name = $2;
	}
|   KW_PRI IDENTIFIER LBRACKET arguments RBRACKET RARROW argument methodLocals {
		$$.isPublic = false;
		$$.arguments = $4;
		$$.locals = $8;
		$$.returnValue = $7;
		$$.name = $2;
	}
;

methodLocals:
	%empty { $$ = nullptr; }
|   PIPE methodLocalList { $$ = $2; }
;

methodLocalList:
	argument { $$ = $1; }
|   methodLocalList COMMA argument {
		auto *it = $1;
		while(it->next) { it = it->next; }
		it->next = $3;
		$$ = $1;
	}
;

arguments:
	%empty { $$ = nullptr; }
|   argumentList { $$ = $1; }
;

argumentList:
	argument { $$ = $1; }
|   argumentList COMMA argument {
		auto *it = $1;
		while(it->next) { it = it->next; }
		it->next = $3;
		$$ = $1;
	}
;

argument:
	IDENTIFIER COLON typeName {
		$$ = new LocalVariable();
		$$->name = $1;
		$$->variable.type = $3;
		$$->variable.integer = 0; // zero value
		$$->next = nullptr;
	}
;

instruction:
	expression { $$ = $1; }
;

expression:
	INT                                         { $$ = new ConstantExpression(context->module, mkvar($1)); }
|   REAL                                        { $$ = new ConstantExpression(context->module, mkvar($1)); }
// |   TEXT                                     { $$ = new ConstantExpression(context->module, mkvar($1)); }
|   IDENTIFIER                                  { $$ = new VariableExpression(context->module, $1); }
|   IDENTIFIER LBRACKET expressionList RBRACKET { $$ = nullptr; yyerror(nullptr, "missing instruction."); }
|   LBRACKET expression RBRACKET                { $$ = $2; }
|   expression PLUS expression                  { $$ = new ArithmeticExpression<trainscript::ops::add>(context->module, $1, $3); }
|   expression MINUS expression                 { $$ = nullptr; yyerror(nullptr, "missing instruction.");}
|   expression MULTIPLY expression              { $$ = nullptr; yyerror(nullptr, "missing instruction.");}
|   expression DIVIDE expression                { $$ = nullptr; yyerror(nullptr, "missing instruction.");}
|   expression MODULO expression                { $$ = nullptr; yyerror(nullptr, "missing instruction.");}
|   expression RARROW IDENTIFIER                { $$ = new VariableAssignmentExpression(context->module, $3, $1); }
;

expressionList:
	%empty
|   expression
|   expressionList COMMA expression
;

variableDeclaration:
	KW_VAR IDENTIFIER COLON typeName {
		$$.name = $2;
		$$.variable.type = $4;
		$$.variable.integer = 0; // Initialize with zeroes
	}
;

typeName:
	KW_VOID                           { $$.id = TypeID::Void; $$.pointer = 0; }
|   KW_INT                            { $$.id = TypeID::Int; $$.pointer = 0; }
|   KW_REAL                           { $$.id = TypeID::Real; $$.pointer = 0; }
|   KW_TEXT                           { $$.id = TypeID::Text; $$.pointer = 0; }
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
	printf("Error: %s\n", s);
}
