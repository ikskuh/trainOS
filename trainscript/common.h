#pragma once

#include <stddef.h>
#include <inttypes.h>

#include "tsvm.hpp"

#define ECHO do { } while(0)

void yyerror(void *scanner, const char *s);

#define YY_EXTRA_TYPE ParserData*

#define YY_INPUT(buf,result,max_size)   \
{ \
	if(yyextra->index >= yyextra->length) \
		result = YY_NULL; \
	else { \
		buf[0] = yyextra->buffer[yyextra->index++]; \
		result = 1; \
	} \
}

typedef struct 
{
	char *buffer;
	size_t index;
	size_t length;
	trainscript::Module *module;
	void *scanner;
} ParserData;

struct VariableDeclaration
{
	char *name;
	trainscript::Variable variable;
};

struct LocalVariable
{
	char *name;
	trainscript::Type type;
	LocalVariable *next;
};

struct MethodHeader
{
	bool isPublic;
	char *name;
	LocalVariable *returnValue;
	LocalVariable *locals;
	LocalVariable *arguments;
};

struct MethodBody
{
	int indentation;
	trainscript::Instruction *instruction;
	MethodBody *next;
};

struct MethodDeclaration
{
	MethodHeader header;
	trainscript::Instruction *body;
};

struct ExpressionList
{
	trainscript::Instruction *instruction;
	ExpressionList *next;
};

// Variable declaration

