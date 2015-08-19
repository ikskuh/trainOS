#pragma once
#include <inttypes.h>
#include <ker/string.hpp>

#include "type.hpp"

namespace trainscript
{
	class Method
	{
	public:
		virtual ~Method() { }

		virtual Variable invoke(ker::Vector<Variable> arguments) = 0;

		virtual bool validate(ker::String &errorCode) const = 0;

		virtual ker::Vector<Type> arguments() const = 0;

		virtual Type returnType() const = 0;
	};
}
