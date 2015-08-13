#pragma once

#include <stdlib.h>
#include <inttypes.h>

namespace ker
{
	class String
	{
	private:
		uint8_t *mText;
		size_t mLength;
	public:
		String() :
			mText(nullptr),
			mLength(0)
		{

		}

		String(const char *text) :
			mText(nullptr),
			mLength(0)
		{
			this->mLength = strlen(text);
			this->mText = (uint8_t*)malloc(this->mLength + 1);
			memcpy(this->mText, text, this->mLength);
		}

		String(const uint8_t *bytes, size_t length) :
			mText((uint8_t*)malloc(length)),
			mLength(length)
		{
			memcpy(this->mText, bytes, length);
		}

		~String()
		{
			free(this->mText);
		}

		size_t length() const
		{
			return this->mLength;
		}

		bool equals(const String &other) const
		{
			if(this->mLength != other.mLength) {
				return false;
			}
			return memcmp(this->mText, other.mText, this->mLength) != 0;
		}

		const uint8_t *text() const
		{
			static const uint8_t empty[] = { 0 };
			if(this->mText != nullptr) {
				return this->mText;
			} else {
				return empty;
			}
		}

		const char *str() const
		{
			return (char*)this->text();
		}

		operator const uint8_t *() const
		{
			return this->text();
		}

		operator const char *() const
		{
			return this->str();
		}

		uint8_t & operator [](size_t index)
		{
			return this->mText[index];
		}

		const uint8_t & operator [](size_t index) const
		{
			return this->mText[index];
		}

		bool operator ==(const String &other) const
		{
			return this->equals(other);
		}

		bool operator !=(const String &other) const
		{
			return !this->equals(other);
		}
	};
};
