#pragma once
#include <inttypes.h>

#include "types.hpp"
#include "typeid.hpp"

namespace trainscript
{
	struct Type
	{
		TypeID id;
		int pointer;

		Type reference() const {
			return { id, pointer + 1 };
		}

		Type dereference() const {
			return { id, pointer - 1 };
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
			switch(id) {
				case TypeID::Invalid: return "INVALID";
				case TypeID::Void: return "VOID";
				case TypeID::Int: return "INT";
				case TypeID::Real: return "REAL";
				case TypeID::Text: return "TEXT";
				case TypeID::Bool: return "BOOL";
				default: return "unknown";
			}
		}

		static const Type Invalid;
		static const Type Void;
		static const Type Int;
		static const Type Real;
		static const Type Text;
		static const Type Boolean;
	};
}
