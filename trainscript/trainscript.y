%{
#include <stdio.h>
#include <stdlib.h>
#include <console.h>

#include "common.h"

#include "tsvm.hpp"

using namespace trainscript;

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
	ExpressionList *expressions;
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

%token OP_LT
%token OP_LE
%token OP_GT
%token OP_GE
%token OP_EQ
%token OP_NEQ

%token <fval> REAL
%token <ival> INT
%token <text> IDENTIFIER
%token <text> TEXT

%token KW_PUB
%token KW_PRI
%token KW_VAR
%token KW_PTR

%token KW_VOID
%token KW_INT
%token KW_REAL
%token KW_TEXT
%token KW_BOOL

%token KW_BEGIN
%token KW_END

%token KW_IF
%token KW_THEN
%token KW_ELSE
%token KW_ELSEIF
%token KW_REPEAT
%token KW_FROM
%token KW_TO
%token KW_UNTIL
%token KW_WHILE
%token KW_DO

%type <type> typeName
%type <varDecl> variableDeclaration

// %type <indentation> indentation

%type <method> method
%type <methodHeader> methodDeclaration
%type <instruction> block
%type <body> body

%type <local> argument
%type <local> arguments
%type <local> argumentList

%type <local> methodLocals
%type <local> methodLocalList

%type <instruction> instruction
%type <instruction> expression
%type <instruction> condition
%type <instruction> elseIfLoop
%type <instruction> loop

%type <expressions> expressionList

%start input

%left PLUS MINUS MULTIPLY DIVIDE MODULO RARROW
%left OP_LT OP_LE OP_GT OP_GE OP_EQ OP_NEQ

%right KW_IF KW_THEN KW_ELSEIF KW_ELSE

%%
input:
	%empty
|   input variableDeclaration SEMICOLON {
		auto *var = new Variable($2.type.createInstance());
        context->module->variables.add( ker::String($2.name), var );
	}
|   input method {
		using namespace trainscript;
		auto *mod = context->module;

        ScriptMethod *method = new ScriptMethod(mod, $2.body);
		method->isPublic = $2.header.isPublic;
		if($2.header.returnValue) {
			method->mReturnValue = ker::Pair<ker::String, Type>(
				$2.header.returnValue->name,
				$2.header.returnValue->type);
		}
		LocalVariable *local = $2.header.locals;
		while(local) {
			method->mLocals.add( local->name, local->type );
			LocalVariable *tmp = local;
			local = local->next;
			delete tmp;
		}
		LocalVariable *arg = $2.header.arguments;
		while(arg) {
			method->mArguments.append( { arg->name, arg->type} );
			LocalVariable *tmp = local;
			arg = arg->next;
			delete tmp;
		}

        context->module->methods.add( $2.header.name, method );
	}
;

method:
	methodDeclaration block {
		$$.header = $1;
		$$.body = $2;
	}
;

block:
	KW_BEGIN body KW_END {
		auto *block = new Block();
		MethodBody *mb = $2;
		while(mb) {
			if(mb->instruction != nullptr) {
                block->instructions.append(mb->instruction);
			}
			MethodBody *tmp = mb;
			mb = mb->next;
			delete mb;
		}
		$$ = block;
	}
;

body:
	%empty { $$ = nullptr; }
|   body instruction {
		auto *body = new MethodBody();
		body->indentation = 0;
		body->instruction = $2;
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
		$$->type = $3;
		$$->next = nullptr;
	}
;

instruction:
	block { $$ = $1; }
|	expression SEMICOLON { $$ = $1; }
|	condition { $$ = $1; }
|	loop { $$ = $1; }
;

loop:
	KW_REPEAT instruction { $$ = new RepeatEndlessExpression($2); }
|	KW_WHILE expression KW_DO instruction { $$ = new RepeatWhileExpression($2, $4); }
;

condition:
	KW_IF expression KW_THEN instruction elseIfLoop { $$ = new IfExpression($2, $4, $5); }
;

elseIfLoop:
	%empty { $$ = nullptr; }
|	elseIfLoop KW_ELSEIF expression KW_THEN instruction {
		if($$ == nullptr) {
			$$ = new IfExpression($3, $5, nullptr);
		} else {
			IfExpression *exp = (IfExpression*)$1;
			while(exp->blockFalse != nullptr) {
				exp = (IfExpression*)exp->blockFalse;
			}
			exp->blockFalse = new IfExpression($3, $5, nullptr);
			$$ = $1;
		}
	}
|	elseIfLoop KW_ELSE instruction {
		if($$ == nullptr) {
			$$ = $3;
		} else {
			IfExpression *exp = (IfExpression*)$1;
			while(exp->blockFalse != nullptr) {
				exp = (IfExpression*)exp->blockFalse;
			}
			exp->blockFalse = $3;
			$$ = $1;
		}
	}
;

expression:
	INT                                         { $$ = new ConstantExpression(Variable::fromInt($1)); }
|	REAL                                        { $$ = new ConstantExpression(Variable::fromReal($1)); }
|   TEXT                                        { $$ = new ConstantExpression(Variable::fromText($1)); }
|   IDENTIFIER                                  { $$ = new VariableExpression($1); }
|   IDENTIFIER LBRACKET expressionList RBRACKET {
		auto *call = new MethodInvokeExpression($1);
		auto *list = $3;
		while(list) {
            call->parameters.append(list->instruction);
			list = list->next;
		}
		$$ = call;
	}
|   LBRACKET expression RBRACKET                { $$ = $2; }
|   expression PLUS expression                  { $$ = new ArithmeticExpression($1, $3, Operation::Add); }
|   expression MINUS expression                 { $$ = new ArithmeticExpression($1, $3, Operation::Subtract); }
|   expression MULTIPLY expression              { $$ = new ArithmeticExpression($1, $3, Operation::Multiply); }
|   expression DIVIDE expression                { $$ = new ArithmeticExpression($1, $3, Operation::Divide); }
|   expression MODULO expression                { $$ = new ArithmeticExpression($1, $3, Operation::Modulo); }
|   expression OP_LT expression                 { $$ = new ArithmeticExpression($1, $3, Operation::Less); }
|   expression OP_LE expression                 { $$ = new ArithmeticExpression($1, $3, Operation::LessEquals); }
|   expression OP_GT expression                 { $$ = new ArithmeticExpression($1, $3, Operation::Greater); }
|   expression OP_GE expression                 { $$ = new ArithmeticExpression($1, $3, Operation::GreaterEquals); }
|   expression OP_EQ expression                 { $$ = new ArithmeticExpression($1, $3, Operation::Equals); }
|   expression OP_NEQ expression                { $$ = new ArithmeticExpression($1, $3, Operation::Inequals); }
|   expression RARROW IDENTIFIER                { $$ = new VariableAssignmentExpression($3, $1); }
;

expressionList:
	%empty { $$ = nullptr; }
|   expression {
		$$ = new ExpressionList();
		$$->instruction = $1;
		$$->next = nullptr;
	}
|   expressionList COMMA expression {
		auto *list = new ExpressionList();
		list->instruction = $3;
		list->next = nullptr;

		auto *it = $1;
		while(it->next) { it = it->next; }
		it->next = list;
		$$ = $1;

	}
;

variableDeclaration:
	KW_VAR IDENTIFIER COLON typeName {
		$$.name = $2;
		$$.type = $4;
	}
;

typeName:
	KW_VOID                           { $$ = Type::Void; }
|   KW_INT                            { $$ = Type::Int; }
|   KW_REAL                           { $$ = Type::Real; }
|   KW_TEXT                           { $$ = Type::Text; }
|   KW_BOOL                           { $$ = Type::Bool; }
|   KW_PTR LBRACKET typeName RBRACKET { $$ = $3.reference(); }
;

%%

#undef scanner

#include "trainscript.l.h"

void yyerror(void *scanner, const char *s) {
	if(scanner == nullptr) {
		kprintf("Error: %s\n", s);
		return;
	}
	int line = 0; // yyget_lineno(scanner);
	int col = 0; //yyget_column(scanner);
	char *text = yyget_text(scanner);
	kprintf(
		"[%d:%d] Error: %s at '%s'\n",
		line, col,
		s,
		text);
}
