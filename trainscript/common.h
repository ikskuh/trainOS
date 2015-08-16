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

struct ParserData
{
	char *buffer;
	size_t index;
	size_t length;
	trainscript::Module *module;
	void *scanner;
	char* strings[256];

	char *strdup(const char *str)
	{
		for(size_t i = 0; i < 256; i++) {
			if(this->strings[i] == nullptr) {
				return this->strings[i] = ::strdup(str);
			}
			else if(strcmp(this->strings[i], str) == 0) {
				return this->strings[i];
			}
		}
		die_extra("ParserData::strdup", "out of strings");
	}

};

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

