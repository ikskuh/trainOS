#pragma once

#include <inttypes.h>
#include "new.hpp"

namespace ker
{
	template<typename T>
	class Vector
	{
	public:
		static const size_t initialCap = 32;
	private:
		T *mData;
		size_t mLength;
		size_t mReserved;
	public:
		Vector() :
			mData(nullptr),
			mLength(0),
			mReserved(0)
		{
			this->reserve(Vector<T>::initialCap);
		}

		explicit Vector(size_t initialReserve) :
			Vector()
		{
			this->reserve(initialReserve);
		}

		size_t length() const
		{
			return this->mLength;
		}

		T &at(size_t index)
		{
			return this->mData[index];
		}

		const T &at(size_t index) const
		{
			return this->mData[index];
		}

		void append(const T &value)
		{
			this->reserve(this->mLength + 1);
			new (&this->mData[this->mLength - 1]) T(value);
			this->mLength += 1;
		}

		void resize(size_t size)
		{
			size_t current = this->mLength;
			this->reserve(size);

			if(current > size) {
				// "Downgrade"
				for(size_t i = this->mLength - 1; i > size; i--) {
					this->mData[i].~T();
				}
			} else {
				// "Upgrade"
				for(size_t i = this->mLength; i < size; i++) {
					new (&this->mData[i]) T ();
				}
			}
		}

		void reserve(size_t space)
		{
			if(this->mReserved >= space) {
				return;
			}
			const size_t newSize = sizeof(T) * space;

			T *newData = (T*)malloc(newSize);
			if(this->mData != nullptr) {
				memcpy(newData, this->mData, newSize);
				free(this->mData);
			}
			this->mData = newData;
		}

		T& operator [](size_t idx)
		{
			return this->at(idx);
		}

		const T& operator [](size_t idx) const
		{
			return this->at(idx);
		}

		T* begin()
		{
			return &this->mData[0];
		}

		T* end()
		{
			return &this->mData[this->mLength];
		}

		const T* begin() const
		{
			return &this->mData[0];
		}

		const T* end() const
		{
			return &this->mData[this->mLength];
		}
	};
}
