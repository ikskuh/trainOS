#pragma once

#include <inttypes.h>

#define FIXED_STRING_AVAILABLE

#include <ker/string.hpp>

struct FixedString
{
	uint32_t len;
	const char *front;

    FixedString() :
        len(0),
        front(nullptr)
    {
    }

    FixedString(const char *first, uint32_t length) :
        len(length),
        front(first)
    {
    }

#if defined(_STRING_H)
    FixedString(const char *str) :
        len(strlen(str)),
        front(str)
    {

    }
#endif
#if defined(KER_STRING_AVAILABLE)
    FixedString(const ker::String &text) :
        FixedString(text.str(), text.length()) {
    }
    explicit operator ker::String() {
        return ker::String(reinterpret_cast<const uint8_t*>(front), len);
    }
#endif
#if defined(_GLIBCXX_STRING)
    FixedString(const std::string &text) :
        FixedString(text.c_str(), text.size()) {
    }
    explicit operator std::string() {
        return std::string(front, len);
	}
#endif
};
