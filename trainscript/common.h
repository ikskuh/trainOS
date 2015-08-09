#pragma once

#include <stddef.h>
#include <inttypes.h>

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

namespace trainscript {
	class Module;
}

typedef struct 
{
	char *buffer;
	size_t index;
	size_t length;
	trainscript::Module *module;
	void *scanner;
} ParserData;

typedef enum
{
	tidUNKNOWN = 0,
	tidVOID = 1,
	tidINT = 2,
	tidREAL = 3,
	tidTEXT = 4,
} typeid_t;

typedef struct
{
	typeid_t type;
	int pointer;
} type_t;

typedef struct
{
	type_t type;
	char *name;
} VariableDeclaration;
