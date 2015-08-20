#pragma once
#include <inttypes.h>

#include "types.hpp"
#include "typeid.hpp"

namespace trainscript
{
	class Variable;

	using Operator = Variable (*)(const Variable & rhs, const Variable &lhs);

	enum class Operation
	{
		Add = 0,
		Subtract,
		Multiply,
		Divide,
		Modulo,
		Less,
		LessEquals,
		Greater,
		GreaterEquals,
		Equals,
		Inequals,
		LIMIT
	};

	struct TypeOps {
		Operator ops[(int)Operation::LIMIT];
	};

	struct Type
	{
		TypeID id;
		int pointer;
		const TypeOps *operators;

		Type reference() const {
			return Type { id, pointer + 1, operators };
		}

		Type dereference() const {
			return Type { id, pointer - 1, operators };
		}

		bool usable() const {
			return (this->id != TypeID::Invalid) &&
				((this->id != TypeID::Void) || (this->pointer > 0));
		}

		bool operator ==(const Type &other) const {
			return (this->id == other.id) &&
				   (this->pointer == other.pointer);
		}

		bool operator !=(const Type &other) const {
			return (this->id != other.id) ||
				   (this->pointer != other.pointer);
		}

		const char *name() const {
			if(this->pointer > 0) {
				static char str[64];
				strcpy(str, this->dereference().name());
				strcat(str, "*");
				return str;
			} else {
				switch(id) {
					case TypeID::Invalid: return "INVALID";
					case TypeID::Void: return "VOID";
					case TypeID::Int: return "INT";
					case TypeID::Real: return "REAL";
					case TypeID::Text: return "TEXT";
					case TypeID::Bool: return "BOOL";
					default: return "<UNKNOWN>";
				}
			}
		}

		size_t size() const
		{
			if(this->pointer > 0) {
				return sizeof(void*);
			} else {
				switch(id) {
					case TypeID::Invalid: return 0;
					case TypeID::Void: return 0;
					case TypeID::Int: return sizeof(Int);
					case TypeID::Real: return sizeof(Real);
					case TypeID::Text: return sizeof(Text);
					case TypeID::Bool: return sizeof(Bool);
					default: die("Type::size.UnknownTypeID");
				}
			}
			return 0;
		}

		/**
		 * @brief Creates an instance of this type.
		 * @return Variable with a value of this type.
		 */
		Variable createInstance() const;

		/**
		 * @brief Checks if the type has the operation defined.
		 * @param op Operation that should be checked
		 * @return True if the operation is defined, else false
		 */
		bool hasOperator(Operation op) const;

		/**
		 * @brief Applies the operation to the operands.
		 * @param lhs Left hand side
		 * @param op Operation
		 * @param rhs Right hand side
		 * @return Result of the operation or Variable::Invalid if operation is not defined.
		 */
		Variable apply(const Variable &lhs, Operation op, const Variable &rhs) const;

		static const Type Invalid;
		static const Type Void;
		static const Type Int;
		static const Type Real;
		static const Type Text;
		static const Type Bool;
	};
}
