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
	public:
		Module();
		~Module();

		Method *method(const char *name)
		{
			return this->methods.get(name);
		}

		Variable *variable(const char *name)
		{
			return this->variables.get(name);
		}

		bool validate(ker::String &errorCode) const;
	};
}
