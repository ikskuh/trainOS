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

        Vector(const Vector &other) :
            mData(nullptr),
            mLength(0),
            mReserved(0)
        {
            this->mLength = other.mLength;
            if(this->mLength > 0) {
                this->reserve(this->mLength);
                for(size_t i = 0; i < this->mLength; i++) {
                    new (&this->mData[i]) T(other.mData[i]);
                }
            }
        }

        Vector(Vector &&other) :
            mData(other.mData),
            mLength(other.mLength),
            mReserved(other.mReserved)
        {
            other.mData = nullptr;
            other.mLength = 0;
            other.mReserved = 0;
        }

        Vector & operator = (const Vector &other)
        {
            this->resize(other.mLength);
            for(size_t i = 0; i < this->mLength; i++)
            {
                this->mData[i] = other.mData[i];
            }
            return *this;
        }

		explicit Vector(size_t initialReserve) :
			Vector()
		{
			this->reserve(initialReserve);
		}

        ~Vector()
        {
            if(this->mData != nullptr) {
                free(this->mData);
            }
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

		T& append(const T &value)
		{
			this->reserve(this->mLength + 1);
            new (&this->mData[this->mLength]) T(value);
			this->mLength += 1;
			return this->mData[this->mLength - 1];
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
            this->mLength = size;
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
            this->mReserved = space;
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
