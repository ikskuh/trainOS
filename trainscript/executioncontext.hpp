#pragma once
#include <inttypes.h>
#include <ker/string.hpp>
#include <ker/dictionary.hpp>

#include "variable.hpp"

namespace trainscript
{
	class Module;

	class ExecutionContext  :
			public ker::Dictionary<ker::String, Variable*>
	{
	public:
		Module * const module = nullptr;

		ExecutionContext(Module *mod) :
			ker::Dictionary<ker::String, Variable*>(),
			module(mod)
		{

		}

		Variable *get(const ker::String &name)
		{
			if(this->contains(name)) {
				return this->at(name);
			} else {
				return nullptr;
			}
		}
	};
}
