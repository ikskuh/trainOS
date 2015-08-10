#pragma once

namespace trainscript
{
	enum class TypeID
	{
		Invalid = 0,
		Void = 1,
		Int = 2,
		Real = 3,
		Text = 4,
	};

	static const char *typeName(TypeID id) {
		switch(id) {
			case TypeID::Invalid: return "INVALID";
			case TypeID::Void: return "VOID";
			case TypeID::Int: return "INT";
			case TypeID::Real: return "REAL";
			case TypeID::Text: return "TEXT";
			default: return "unknown";
		}
	}
}
