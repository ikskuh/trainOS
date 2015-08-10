#include <stdlib.h>
#include <string.h>

#include "common.h"

#include "tsvm.hpp"

#include "trainscript.tab.h"
#include "trainscript.l.h"

namespace trainscript
{
	bool verbose = false;

	const Type Type::Invalid = { TypeID::Invalid, 0 };
	const Type Type::Void = { TypeID::Void, 0 };
	const Type Type::Int = { TypeID::Int, 0 };
	const Type Type::Real = { TypeID::Real, 0 };
	const Type Type::Text = { TypeID::Text, 0 };

	const Variable Variable::Invalid = { Type::Invalid };
	const Variable Variable::Void = { Type::Void };
	const Variable Variable::Int = { Type::Int };
	const Variable Variable::Real = { Type::Real };
	const Variable Variable::Text = { Type::Text };

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

	Variable Method::invoke(std::vector<Variable> arguments)
	{
		LocalContext context(this->module);

		for(auto var : this->module->variables)
		{
			context.insert({ var.first, var.second });
		}

		if(this->returnValue.second.type.usable()) {
			context.insert({ this->returnValue.first, &this->returnValue.second });
		}
		if(arguments.size() != this->arguments.size()) {
			printf("MECKER anzahl!\n");
			return Variable();
		}
		for(size_t i = 0; i < this->arguments.size(); i++) {
			if(this->arguments[i].second.type != arguments[i].type) {
				printf("MECKER argtyp!\n");
				return Variable();
			}
			context.insert({this->arguments[i].first, new Variable(arguments[i]) });
		}
		for(auto local : this->locals) {
			context.insert({ local.first, new Variable(local.second) });
		}

		if(verbose) {
			printf("executing with local context:\n");
			for(auto &ref : context)
			{
				printf("  %s : %s\n", ref.first.c_str(), typeName(ref.second->type.id));
			}
		}

		this->block->execute(context);

		return this->returnValue.second;
	}



	namespace ops
	{
		Variable add(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int:return  mkvar(lhs.integer + rhs.integer);
				case TypeID::Real: return mkvar(lhs.real + rhs.real);
				default: printf("addition not supported for %s.\n", typeName(lhs.type.id)); break;
			}
		}

		Variable subtract(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkvar(lhs.integer - rhs.integer);
				case TypeID::Real:return  mkvar(lhs.real - rhs.real);
				default: printf("subtraction not supported for %s.\n", typeName(lhs.type.id)); return Variable::Invalid;
			}
		}

		Variable multiply(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkvar(lhs.integer * rhs.integer);
				case TypeID::Real: return mkvar(lhs.real * rhs.real);
				default: printf("multiplication not supported for %s.\n", typeName(lhs.type.id)); return Variable::Invalid;
			}
		}

		Variable divide(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkvar(lhs.integer / rhs.integer);
				case TypeID::Real: return mkvar(lhs.real / rhs.real);
				default: printf("division not supported for %s.\n", typeName(lhs.type.id)); return Variable::Invalid;
			}
		}

		Variable modulo(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkvar(lhs.integer % rhs.integer);
				// case TypeID::Real: mkvar(lhs.real % rhs.real);
				default: printf("modulo not supported for %s.\n", typeName(lhs.type.id));  return Variable::Invalid;
			}
		}
	}
}
