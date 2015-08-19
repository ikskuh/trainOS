#pragma once
#include <inttypes.h>

#include "type.hpp"

namespace trainscript
{
	struct Variable
	{
		Type type;
		union {
			Int integer;
			Real real;
			// Text text;
			Bool boolean;
		};

#if defined(TSVM_PRINTVAL)
		void printval() const
		{
			switch(this->type.id) {
				case TypeID::Int: kprintf("%d", this->integer); break;
				case TypeID::Real: kprintf("%f", this->real); break;
				case TypeID::Bool: kprintf("%s", this->boolean ? "TRUE" : "FALSE"); break;
				default: kprintf("???"); break;
			}
		}
#endif

		static const Variable Invalid;
		static const Variable Void;
		static const Variable Int;
		static const Variable Real;
		static const Variable Text;
		static const Variable Boolean;
	};

	static inline Variable mkvar(Int value) {
		Variable v = Variable::Int;
		v.integer = value;
		return v;
	}

	static inline Variable mkvar(Real value) {
		Variable v = Variable::Real;
		v.real = value;
		return v;
	}

	static inline Variable mkbool(Bool value) {
		Variable v = Variable::Boolean;
		v.boolean = value;
		return v;
	}
}
