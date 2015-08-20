#include <kernel.h>
#include <console.h>
#include "type.hpp"
#include "variable.hpp"

namespace trainscript
{
	extern TypeOps InvalidOperators;
	extern TypeOps IntOperators;
	extern TypeOps RealOperators;
	extern TypeOps TextOperators;
	extern TypeOps BoolOperators;

	const Type Type::Invalid = { TypeID::Invalid, 0, &InvalidOperators };
	const Type Type::Void = { TypeID::Void, 0, &InvalidOperators };
	const Type Type::Int = { TypeID::Int, 0, &IntOperators };
	const Type Type::Real = { TypeID::Real, 0, &RealOperators };
	const Type Type::Text = { TypeID::Text, 0, &TextOperators };
	const Type Type::Bool = { TypeID::Bool, 0, &BoolOperators };

	bool Type::hasOperator(Operation op) const
	{
		return this->operators->ops[(int)op] != nullptr;
	}

	Variable Type::apply(const Variable &lhs, Operation op, const Variable &rhs) const
	{
		if(this->hasOperator(op) == false) {
			kprintf("[invalid op: apply %d to %s]", (int)op, this->name());
			return Variable::Invalid;
		}
		return this->operators->ops[(int)op](lhs, rhs);
	}

	Variable Type::createInstance() const
	{
		Variable v;
		v.setType(*this);
		return v;
	}
}
