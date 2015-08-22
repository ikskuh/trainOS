#include <kernel.h>
#include <console.h>
#include "variable.hpp"
#include "type.hpp"

namespace trainscript
{
	const Variable Variable::Invalid = { Type::Invalid, 0 };
	const Variable Variable::Void = { Type::Void, 0 };
	const Variable Variable::Int = { Type::Int, 0 };
	const Variable Variable::Real = { Type::Real, 0 };
	const Variable Variable::Text = { Type::Text, 0 };
	const Variable Variable::Bool = { Type::Bool, 0 };

	Variable::~Variable()
	{
		// Only free non-pointer types.
		if(this->mType.pointer == 0) {
			switch(this->mType.id) {
#define DELETE(type) case TypeID::type: \
					delete reinterpret_cast<trainscript::type*>(this->mValue); \
					break
				DELETE(Bool);
				DELETE(Int);
				DELETE(Real);
				DELETE(Text);
#undef DELETE
			}
		} else {
			delete reinterpret_cast<trainscript::Pointer*>(this->mValue); \
		}
	}

	void Variable::replace(const Variable &other)
	{
		this->setType(other.mType);
		*this = other;
	}

	void Variable::setType(const Type &type)
	{
		if(this->mValue != nullptr) {
			die_extra("Variable::setType.AlreadyInitialized", "Variable was already initialized!");
		}
		this->mType = type;
		this->mValue = nullptr;
		if(mType.pointer == 0) {
			switch(mType.id) {
#define CREATE(type, initial) case TypeID::type: \
					this->mValue = new trainscript::type(initial); \
					break
				CREATE(Bool, false);
				CREATE(Int, 0);
				CREATE(Real, 0.0);
				CREATE(Text, "");
#undef CREATE
				case TypeID::Invalid:
				case TypeID::Void:
					// Ignore INVALID or VOID type.
					break;
				default:
					die_extra("Variable::setType.InvalidTypeID", mType.name());
					break;
			}
		} else {
			this->mValue = new trainscript::Pointer(nullptr); \
		}
	}

	Variable &Variable::operator =(const Variable &other)
	{
		if(this->mType != other.mType) {
			kprintf("[%s -> %s]", other.mType.name(), this->mType.name());
			die_extra("Variable.operator=", "Type mismatch.");
		}
		if(this->mType == Type::Void) {
			// Just don't do anything when assigning void.
			return *this;
		}
		if(this->mValue == nullptr) {
			kprintf("[Variable.type: %s]", this->mType.name());
			die_extra("Variable.operator=", "Invalid variable: value == nullptr");
		}
		if(this->mType.pointer == 0) {
			switch(this->mType.id) {
	#define COPY(type) case TypeID::type: \
					*reinterpret_cast<trainscript::type*>(this->mValue) = *reinterpret_cast<trainscript::type*>(other.mValue); \
					break
			COPY(Bool);
			COPY(Int);
			COPY(Real);
			COPY(Text);
	#undef COPY
			}
		} else {
			*reinterpret_cast<trainscript::Pointer*>(this->mValue) = *reinterpret_cast<trainscript::Pointer*>(other.mValue); \
		}
		return *this;
	}


#define fromType(type) Variable Variable::from##type(trainscript::type i) \
	{ \
		Variable var = Type::type.createInstance(); \
		var.value<trainscript::type>() = i; \
		return var; \
	}

	fromType(Int)
	fromType(Real)
	fromType(Text)
	fromType(Bool)

	ker::String Variable::toString() const
	{
		if(this->mType.pointer > 0) {
			return "0x" + ker::String::fromNumber(*reinterpret_cast<int32_t*>(this->mValue), 16);
		}
		trainscript::Int iVal;
		trainscript::Real rVal;
		switch(this->mType.id) {
			case TypeID::Bool:
				if(*reinterpret_cast<trainscript::Bool*>(this->mValue) == true)
					return "TRUE";
				else
					return "FALSE";
			case TypeID::Int:
				iVal = *reinterpret_cast<trainscript::Int*>(this->mValue);
				return ker::String::fromNumber(iVal, 10);
			case TypeID::Real:
				rVal = *reinterpret_cast<trainscript::Real*>(this->mValue);
				// return ker::String::fromNumber(rVal);
				return "<REAL missing>";
			case TypeID::Text:
				return *reinterpret_cast<trainscript::Text*>(this->mValue);
			case TypeID::Void:
				return "VOID";
			case TypeID::Invalid:
				return "INVALID";
		}
		return "???";
	}
}
