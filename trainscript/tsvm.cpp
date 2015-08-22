#include <stdlib.h>
#include <console.h>

#include "common.h"

#include "tsvm.hpp"

#include "trainscript.tab.hpp"
#include "trainscript.l.h"

namespace trainscript
{
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

	Module *VM::create(const ker::String &)
	{
		return nullptr;
	}

	Module *VM::load(const void *buffer, size_t length)
	{
		char *internalStorage = (char*)malloc(length);
		memcpy(internalStorage, buffer, length);

		Module *module = new Module();

		ParserData data;
		memset(&data, 0, sizeof(data));
		data.buffer = reinterpret_cast<char*>(internalStorage);
		data.index = 0;
		data.length = length;
		data.module = module;

		yylex_init_extra(&data, &data.scanner);
        bool valid = yyparse(&data) == 0;
		yylex_destroy(data.scanner);

		free(internalStorage);

		for(char *ptr : data.strings) {
			free(ptr);
		}

		for(const ker::Pair<ker::String, ker::String> &mod : data.objects)
		{
			Module *obj = this->create(mod.second);
			if(obj == nullptr) {
				kprintf("Module \"%s\" not found.\n", mod.second.str());
				delete module;
				return nullptr;
			}
			module->objects.add(mod.first, obj);
		}

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
		for(auto obj : objects) {
			delete obj.second;
		}
	}

    Variable ScriptMethod::invoke(ker::Vector<Variable> arguments)
	{
		ExecutionContext context(this->module);

		ker::Vector<Variable*> temporaries;

		for(auto var : this->module->variables)
		{
			context.add(var.first, var.second);
		}

		Variable returnVariable = this->mReturnValue.second.createInstance();

		if(this->mReturnValue.second.usable()) {
			context.add(this->mReturnValue.first, &returnVariable);
		}
		if(arguments.length() != this->mArguments.length()) {
			die_extra("ScriptMethod::invoke", "Invalid argument count.");
		}
		for(size_t i = 0; i < this->mArguments.length(); i++) {
			if(this->mArguments[i].second != arguments[i].type()) {
				die_extra("ScriptMethod::invoke", "Invalid argument type.");
			}
			auto *v = new Variable(arguments[i]);
			temporaries.append(v);
			context.add(this->mArguments[i].first, v);
		}
		for(auto local : this->mLocals) {
			auto *v = new Variable(local.second.createInstance());
			temporaries.append(v);
			context.add(local.first, v);
		}

		this->block->execute(context);

		for(auto *var : temporaries) {
			delete var;
		}

		return returnVariable;
	}

	bool ScriptMethod::validate(ker::String &errorCode) const
	{
		if(this->block == nullptr) {
			errorCode = "Method block is not set.";
			return false;
		}

		ExecutionContext context(this->module);

		for(auto var : this->module->variables)
		{
			context.add(var.first, var.second);
		}

		Variable returnVariable = this->mReturnValue.second.createInstance();

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
			context.add(this->mArguments[i].first, new Variable(this->mArguments[i].second.createInstance()));
		}
		for(auto local : this->mLocals) {
			if(context.get(local.first) != nullptr) {
				errorCode = "Local variable overlaps a variable.";
				return false;
			}
			context.add(local.first, new Variable(local.second.createInstance()));
		}

		if(this->block->validate(context, errorCode) == false) {
			return false;
		}

		return true;
	}
}
