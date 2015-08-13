extern "C"  {
#include <stdlib.h>
#include <console.h>
}

#include "common.h"

#include "tsvm.hpp"

#include "trainscript.tab.hpp"
#include "trainscript.l.h"

namespace trainscript
{
	bool verbose = false;

	const Type Type::Invalid = { TypeID::Invalid, 0 };
	const Type Type::Void = { TypeID::Void, 0 };
	const Type Type::Int = { TypeID::Int, 0 };
	const Type Type::Real = { TypeID::Real, 0 };
	const Type Type::Text = { TypeID::Text, 0 };
	const Type Type::Boolean = { TypeID::Bool, 0 };

    const Variable Variable::Invalid = { Type::Invalid, 0 };
    const Variable Variable::Void = { Type::Void, 0 };
    const Variable Variable::Int = { Type::Int, 0 };
    const Variable Variable::Real = { Type::Real, 0 };
    const Variable Variable::Text = { Type::Text, 0 };
    const Variable Variable::Boolean = { Type::Boolean, 0 };

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

        int error = yyparse(&data);
        kprintf("[E:%d]", error);
        bool valid = error == 0;

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

	Variable Method::invoke(ker::Vector<Variable> arguments)
	{
		LocalContext context(this->module);

		for(auto var : this->module->variables)
		{
			context.add(var.first, var.second);
		}

		if(this->returnValue.second.type.usable()) {
			context.add(this->returnValue.first, &this->returnValue.second);
		}
		if(arguments.length() != this->arguments.length()) {
			return Variable::Invalid;
		}
		for(size_t i = 0; i < this->arguments.length(); i++) {
			if(this->arguments[i].second.type != arguments[i].type) {
				return Variable::Invalid;
			}
			context.add(this->arguments[i].first, new Variable(arguments[i]));
		}
		for(auto local : this->locals) {
			context.add(local.first, new Variable(local.second));
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
				default: kprintf("addition not supported for %s.\n", typeName(lhs.type.id)); return Variable::Invalid;
			}
		}

		Variable subtract(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkvar(lhs.integer - rhs.integer);
				case TypeID::Real:return  mkvar(lhs.real - rhs.real);
				default: kprintf("subtraction not supported for %s.\n", typeName(lhs.type.id)); return Variable::Invalid;
			}
		}

		Variable multiply(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkvar(lhs.integer * rhs.integer);
				case TypeID::Real: return mkvar(lhs.real * rhs.real);
				default: kprintf("multiplication not supported for %s.\n", typeName(lhs.type.id)); return Variable::Invalid;
			}
		}

		Variable divide(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkvar(lhs.integer / rhs.integer);
				case TypeID::Real: return mkvar(lhs.real / rhs.real);
				default: kprintf("division not supported for %s.\n", typeName(lhs.type.id)); return Variable::Invalid;
			}
		}

		Variable modulo(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkvar(lhs.integer % rhs.integer);
				// case TypeID::Real: mkvar(lhs.real % rhs.real);
				default: kprintf("modulo not supported for %s.\n", typeName(lhs.type.id));  return Variable::Invalid;
			}
		}

		Variable equals(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkbool(lhs.integer == rhs.integer);
				case TypeID::Real: return mkbool(lhs.real == rhs.real);
				case TypeID::Bool: return mkbool(lhs.boolean == rhs.boolean);
				default:
					kprintf("equals not supported for %s.\n", typeName(lhs.type.id));
					return Variable::Invalid;
			}
		}

		Variable inequals(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkbool(lhs.integer != rhs.integer);
				case TypeID::Real: return mkbool(lhs.real != rhs.real);
				case TypeID::Bool: return mkbool(lhs.boolean != rhs.boolean);
				default:
					kprintf("inequals not supported for %s.\n", typeName(lhs.type.id));
					return Variable::Invalid;
			}
		}


		Variable less(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkbool(lhs.integer < rhs.integer);
				case TypeID::Real: return mkbool(lhs.real < rhs.real);
				default:
					kprintf("equals not supported for %s.\n", typeName(lhs.type.id));
					return Variable::Invalid;
			}
		}

		Variable lessEqual(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkbool(lhs.integer <= rhs.integer);
				case TypeID::Real: return mkbool(lhs.real <= rhs.real);
				default:
					kprintf("equals not supported for %s.\n", typeName(lhs.type.id));
					return Variable::Invalid;
			}
		}

		Variable greater(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkbool(lhs.integer > rhs.integer);
				case TypeID::Real: return mkbool(lhs.real > rhs.real);
				default:
					kprintf("equals not supported for %s.\n", typeName(lhs.type.id));
					return Variable::Invalid;
			}
		}

		Variable greaterEqual(Variable lhs, Variable rhs)
		{
			switch(lhs.type.id) {
				case TypeID::Int: return mkbool(lhs.integer >= rhs.integer);
				case TypeID::Real: return mkbool(lhs.real >= rhs.real);
				default:
					kprintf("equals not supported for %s.\n", typeName(lhs.type.id));
					return Variable::Invalid;
			}
		}
	}
}
