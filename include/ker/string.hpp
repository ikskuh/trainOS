#pragma once

#include <inttypes.h>
#if defined(CIRCUIT_OS)
#include <kstdlib.h>
#else
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#endif

#include "config.hpp"

#define KER_STRING_AVAILABLE

namespace ker
{
	class String
	{
	private:
#if defined(ENABLE_SHARED_STRING)
		uint32_t *mReferences;
#endif
		uint8_t *mText;
		size_t mLength;

#if !defined(ENABLE_SHARED_STRING)
		void copyFrom(const uint8_t *bytes, size_t length)
		{
			if(this->mText != nullptr) {
				free(this->mText);
			}
			this->mText = (uint8_t*)malloc(length + 1);
			memcpy(this->mText, bytes, length);
			this->mLength = length;
			this->mText[this->mLength] = 0; // last byte is always 0
		}
#endif
	public:
#if defined(ENABLE_SHARED_STRING)
		String() :
			mReferences(nullptr),
			mText(nullptr),
			mLength(0)
		{

		}

		// Here just plain copy the string resources and increase the reference counter
		String(const String &other) :
			mReferences(other.mReferences),
			mText(other.mText),
			mLength(other.mLength)
		{
			// Increase reference count
			if(this->mReferences != nullptr) {
				*this->mReferences += 1;
			}
		}

		// Here move the string resources and don't modify reference counter
		String(String &&other) :
			mReferences(other.mReferences),
			mText(other.mText),
			mLength(other.mLength)
		{
			other.mReferences = nullptr;
			other.mText = nullptr;
			other.mLength = 0;
		}

		String(const uint8_t *bytes, size_t length)
		{
			// Allocate memory for n bytes + nulltermination + length
			this->mReferences = reinterpret_cast<uint32_t*>(malloc(sizeof(*this->mReferences) + sizeof(uint8_t) * (length + 1)));
			this->mText = reinterpret_cast<uint8_t *>(&this->mReferences[1]);
			this->mLength = length;

			// We have a single reference
			*this->mReferences = 1;

			// Initialize string
			memcpy(this->mText, bytes, length);
			this->mText[length] = 0;
		}

		~String()
		{
			if(this->mReferences != nullptr) {
				*this->mReferences -= 1;
				if(this->mReferences == 0) {
					// Last reference was released, now destroy the memory.
					free(this->mReferences);
				}
			}
			this->mReferences = nullptr;
			this->mText = nullptr;
			this->mLength = 0;
		}

		String & operator = (const String &other)
		{
			if(this->mReferences == other.mReferences) {
				// We have copied the string in circles.
				// Everything is just fine
				return *this;
			}
			if(this->mReferences != nullptr) {
				*this->mReferences -= 1;
				if(this->mReferences == 0) {
					free(this->mReferences);
				}
			}

			this->mReferences = other.mReferences;
			this->mText = other.mText;
			this->mLength = other.mLength;

			if(this->mReferences != nullptr) {
				*this->mReferences += 1;
			}

			return *this;
		}
#else
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


		String & operator = (const String &other)
		{
			this->copyFrom(other.mText, other.mLength);
			return *this;
		}
#endif

		String(const char *text) :
			String(text, strlen(text))
		{

		}

		String(const char *bytes, size_t length) :
			String(reinterpret_cast<const uint8_t *>(bytes), length)
		{

		}

		uint8_t at(size_t index) const
		{
			return this->mText[index];
		}

		size_t length() const
		{
			return this->mLength;
		}

		bool equals(const String &other) const
		{
#if defined(ENABLE_SHARED_STRING)
			if(this->mText == other.mText) {
				// This is a quite useful equality test.
				return true;
			}
#endif
			if(this->mLength != other.mLength) {
				return false;
			}
            return memcmp(this->mText, other.mText, this->mLength) == 0;
		}

		bool equals(const char *other) const
		{
			return strcmp(this->str(), other) == 0;
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

		bool operator ==(const String &other) const
		{
			return this->equals(other);
		}

		bool operator ==(const char *other) const
		{
			return this->equals(other);
		}

		bool operator !=(const String &other) const
		{
			return !this->equals(other);
		}

		bool operator !=(const char *other) const
		{
			return !this->equals(other);
		}

		String operator +(const String &other) const
		{
			return this->append(other);
		}
	public:
		static String concat(const String &lhs, const String &rhs)
		{
			return lhs.append(rhs);
		}
/*
		static String fromNumber(int32_t number, int radix = 10)
		{
			static char buffer[64];
			itoa(number, buffer, radix);
			return String(buffer);
		}
*/

        template<typename T>
		static String from(const T &);
	};


    static inline String operator + (const char *lhs, const String &rhs)
	{
		return String::concat(lhs, rhs);
	}

    static inline String operator + (const String &lhs, const char *rhs)
	{
		return String::concat(lhs, rhs);
	}
};
