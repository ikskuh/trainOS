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

        String(const String &other) :
            String(other.mText, other.mLength)
        {

        }

        String(String &&other) :
            mText(other.mText),
            mLength(other.mLength)
        {
            other.mText = nullptr;
            other.mLength = 0;
		}

		String(const char *text) :
			mText(nullptr),
			mLength(0)
        {
            this->copyFrom(reinterpret_cast<const uint8_t*>(text), strlen(text));
		}

		String(const uint8_t *bytes, size_t length) :
            mText(nullptr),
			mLength(length)
		{
            this->copyFrom(bytes, length);
		}

		~String()
		{
            if(this->mText != nullptr) {
				free(this->mText);
            }
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
            return memcmp(this->mText, other.mText, this->mLength) == 0;
		}

		String append(const String &other) const
		{
			uint8_t *data = (uint8_t*)malloc(this->mLength + other.mLength);
			memcpy(&data[0], this->mText, this->mLength);
			memcpy(&data[this->mLength], other.mText, other.mLength);
			String cat(data, this->mLength + other.mLength);
			free(data);
			return cat;
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

		String & operator = (const String &other)
		{
			this->copyFrom(other.mText, other.mLength);
			return *this;
		}

		bool operator ==(const String &other) const
		{
			return this->equals(other);
		}

		bool operator !=(const String &other) const
		{
			return !this->equals(other);
		}

		String operator +(const String &other) const
		{
			return this->append(other);
		}

    private:
        void copyFrom(const uint8_t *bytes, size_t length)
        {
            if(this->mText != nullptr) {
				// free(this->mText);
            }
            this->mText = (uint8_t*)malloc(length + 1);
            memcpy(this->mText, bytes, length);
            this->mLength = length;
            this->mText[this->mLength] = 0; // last byte is always 0
        }
	public:
		static String concat(const String &lhs, const String &rhs)
		{
			return lhs.append(rhs);
		}

		static String fromNumber(int32_t number, int radix = 10)
		{
			static char buffer[64];
			itoa(number, buffer, radix);
			return String(buffer);
		}
	};


	static String operator + (const char *lhs, const String &rhs)
	{
		return String::concat(lhs, rhs);
	}

	static String operator + (const String &lhs, const char *rhs)
	{
		return String::concat(lhs, rhs);
	}
};
