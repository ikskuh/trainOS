#pragma once
#include <inttypes.h>
#include <ker/string.hpp>
#include <ker/dictionary.hpp>

#include "variable.hpp"
#include "method.hpp"

namespace trainscript
{
	class Module
	{
	public:
		ker::Dictionary<ker::String, Variable*> variables;
		ker::Dictionary<ker::String, Method*> methods;
		ker::Dictionary<ker::String, Module*> objects;
	public:
		Module();
		~Module();

		Module *object(const ker::String &name)
		{
			return this->objects.get(name);
		}

		Method *method(const ker::String &name)
		{
			return this->methods.get(name);
		}

		Variable *variable(const ker::String &name)
		{
			return this->variables.get(name);
		}

		bool validate(ker::String &errorCode) const;
	};
}
