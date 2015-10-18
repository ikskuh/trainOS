#pragma once

#include <stddef.h>

#include "fixedstring.hpp"

class BinaryReader
{
public:
	const char * const start;
	const size_t length;
private:
	size_t current;
	size_t remaining;
public:
	BinaryReader(const void *src, size_t length) :
		start(reinterpret_cast<const char *>(src)),
		length(length),
		current(0),
		remaining(length)
	{

	}

	void jumpTo(size_t offset)
	{
		if(offset > this->length) {
			return;
		}
		this->current = offset;
		this->remaining = this->length - offset;
	}

	size_t position() const {
		return this->current;
	}

	template<typename T>
	T read()
	{
		if(this->remaining < sizeof(T)) {
			return T();
		}
		const T *ptr = reinterpret_cast<const T*>(this->start + this->current);
		this->current += sizeof(T);
		this->remaining -= sizeof(T);
		return *ptr;
	}


};

template<>
inline FixedString BinaryReader::read<FixedString>()
{
	FixedString str;
	str.len = this->read<uint32_t>();
	str.front = this->start + this->current;
	this->current += str.len;
	this->remaining -= str.len;
	return str;
}


