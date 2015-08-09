#include <stdlib.h>
#include <string.h>

#include "common.h"

#include "tsvm.hpp"

#include "trainscript.tab.h"
#include "trainscript.l.h"

namespace trainscript
{
	Module *VM::load(const void *buffer, size_t length)
	{
		void *internalStorage = malloc(length);
		memcpy(internalStorage, buffer, length);

		Module *module = new Module();

		ParserData data;
		data.buffer = reinterpret_cast<char*>(internalStorage);
		data.index = 0;
		data.length = length;
		data.module = module;
		yylex_init_extra(&data, &data.scanner);

		bool valid = yyparse(&data) == 0;

		yylex_destroy(data.scanner);
		free(internalStorage);

		if(valid) {
			return module;
		} else {
			delete module;
			return nullptr;
		}
	}

	Module *VM::load(const char *text)
	{
		return VM::load(reinterpret_cast<const void*>(text), static_cast<size_t>(strlen(text)));
	}

	Module::Module() :
		variables()
	{

	}

	Module::~Module()
	{
		for(auto val : methods) {
			delete val.second;
		}
		for(auto val : variables) {
			delete val.second;
		}
	}
}
