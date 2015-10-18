#pragma once

#include <ker/vector.hpp>
#include <inttypes.h>

#include "fixedstring.hpp"

class BinaryWriter
{
private:
	ker::Vector<uint8_t> mData;
	uint32_t mPointer;
public:
	BinaryWriter() : mData(), mPointer(0)
	{
		this->mData.reserve(1024);
	}

	ker::Vector<uint8_t> &data() { return this->mData; }

	const ker::Vector<uint8_t> &data() const { return this->mData; }

	size_t size() const {
        return this->mData.length();
	}

	uint32_t tell() const {
		return this->mPointer;
	}

	void rewind() {
		this->mPointer = 0;
	}

	void fastForwad() {
        this->mPointer = this->mData.length();
	}

	void seek(uint32_t position) {
		this->mPointer = position;
	}

	template<typename T>
	uint32_t write(const T &bits)
	{
		uint32_t loc = this->mPointer;
        if((this->mPointer + sizeof(T)) > this->mData.length())
		{
			this->mData.resize(this->mPointer + sizeof(T));
		}
		this->mPointer += sizeof(T);

		uint8_t *data = &(this->mData[loc]);
		new (data) T (bits);
		return loc;
	}
};

template<>
inline uint32_t BinaryWriter::write<FixedString>(const FixedString &bits)
{
	uint32_t loc = this->write<uint32_t>(bits.len);
	for(uint32_t k = 0; k < bits.len; k++) {
		this->write<uint8_t>(bits.front[k]);
	}
	return loc;
}
