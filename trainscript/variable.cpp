#include <kernel.h>
#include "variable.hpp"

namespace trainscript
{
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
		}
	}

	Variable &Variable::operator =(const Variable &other)
	{
		if(this->mType != other.mType) {
			die_extra("Variable.operator=", "Type mismatch.");
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
			this->mValue = other.mValue;
		}
		return *this;
	}

	Variable Type::createInstance() const
	{
		void *value = nullptr;
		if(this->pointer > 0) {
			switch(this->id) {
#define CREATE(type, initial) case TypeID::type: \
					value = new trainscript::type(initial); \
					break
				CREATE(Bool, false);
				CREATE(Int, 0);
				CREATE(Real, 0.0);
				CREATE(Text, "");
				default:
					die("Type::createInstance.InvalidTypeID");
			}
		}

		return Variable(*this, value);
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
}
