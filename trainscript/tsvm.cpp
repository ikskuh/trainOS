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

	bool Module::validate(ker::String &errorCode) const
	{
		errorCode = "";
		for(auto method : this->methods) {
			if(method.second->validate(errorCode) == false) {
				return false;
			}
		}
		return true;
	}

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

    Variable ScriptMethod::invoke(ker::Vector<Variable> arguments)
	{
		LocalContext context(this->module);

		for(auto var : this->module->variables)
		{
			context.add(var.first, var.second);
		}

		Variable returnVariable = {
			this->mReturnValue.second, 0
		};

		if(this->mReturnValue.second.usable()) {
			context.add(this->mReturnValue.first, &returnVariable);
		}
		if(arguments.length() != this->mArguments.length()) {
			return Variable::Invalid;
		}
		for(size_t i = 0; i < this->mArguments.length(); i++) {
			if(this->mArguments[i].second != arguments[i].type) {
				return Variable::Invalid;
			}
			context.add(this->mArguments[i].first, new Variable(arguments[i]));
		}
		for(auto local : this->mLocals) {
			context.add(local.first, new Variable { local.second, 0 });
		}

		this->block->execute(context);

		return returnVariable;
	}

	bool ScriptMethod::validate(ker::String &errorCode) const
	{
		if(this->block == nullptr) {
			errorCode = "Method block is not set.";
			return false;
		}

		LocalContext context(this->module);

		for(auto var : this->module->variables)
		{
			context.add(var.first, var.second);
		}

		Variable returnVariable = {
			this->mReturnValue.second, 0
		};

		if(this->mReturnValue.second.usable()) {
			if(context.get(this->mReturnValue.first) != nullptr) {
				errorCode = "Return variable overlaps a variable.";
				return false;
			}
			context.add(this->mReturnValue.first, &returnVariable);
		}

		for(size_t i = 0; i < this->mArguments.length(); i++) {
			if(context.get(this->mArguments[i].first) != nullptr) {
				errorCode = "Parameter overlaps a variable.";
				return false;
			}
			context.add(this->mArguments[i].first, new Variable { this->mArguments[i].second, 0 });
		}
		for(auto local : this->mLocals) {
			if(context.get(local.first) != nullptr) {
				errorCode = "Local variable overlaps a variable.";
				return false;
			}
			context.add(local.first, new Variable { local.second, 0 });
		}

		if(this->block->validate(context, errorCode) == false) {
			return false;
		}

		return true;
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
