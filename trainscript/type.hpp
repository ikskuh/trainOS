#pragma once
#include <inttypes.h>

#include "types.hpp"
#include "typeid.hpp"

namespace trainscript
{
	class Variable;

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

		Variable createInstance() const;

		static const Type Invalid;
		static const Type Void;
		static const Type Int;
		static const Type Real;
		static const Type Text;
		static const Type Bool;
	};
}
