#pragma once
#include <inttypes.h>

#include "executioncontext.hpp"

namespace trainscript
{
	class Instruction
	{
	public:
		virtual ~Instruction() { }

		virtual Variable execute(ExecutionContext &context) const = 0;

		virtual Type expectedResult(ExecutionContext &) const {
			return Type::Void;
		}

		virtual bool validate(ExecutionContext &, ker::String &errorCode) const {
			errorCode = "";
			return true;
		}
	};
}
