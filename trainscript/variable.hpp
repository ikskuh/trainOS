#pragma once
#include <inttypes.h>

#include "type.hpp"

namespace trainscript
{
	class Variable
	{
		friend class Type;
	private:
		Type mType;
		void *mValue;
	private:
		// To be called by Type::createInstance
		Variable(const Type &type, void *value) :
			mType(type),
			mValue(value)
		{

		}
	public:
		Variable() :
			mType(Type::Invalid),
			mValue(nullptr)
		{

		}

		Variable(const Variable &other) :
			mType(),
			mValue()
		{
			this->setType(other.mType);
			*this = other;
		}

		Variable(Variable &&other) :
			mType(other.mType),
			mValue(other.mValue)
		{
			other.mType = Type::Invalid;
			other.mValue = nullptr;
		}
		~Variable();

		Variable &operator =(const Variable &other);

		/**
		 * @brief Replaces the value and type of this variable.
		 * @param other The reference variable to copy.
		 */
		void replace(const Variable &other);

		void setType(const Type &type);

		void *data() const
		{
			return this->mValue;
		}

		template<typename T>
		T& value()
		{
			return *reinterpret_cast<T*>(this->mValue);
		}

		template<typename T>
		const T& value() const
		{
			return *reinterpret_cast<T*>(this->mValue);
		}

		const Type &type() const {
			return this->mType;
		}

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

		static Variable fromInt(trainscript::Int i);
		static Variable fromReal(trainscript::Real r);
		static Variable fromText(trainscript::Text t);
		static Variable fromBool(Bool b);

		static const Variable Invalid;
		static const Variable Void;
		static const Variable Int;
		static const Variable Real;
		static const Variable Text;
		static const Variable Bool;
	};
}
