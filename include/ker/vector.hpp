#pragma once

#include <inttypes.h>
#include <stddef.h>
#include <initializer_list>

#if defined(CIRCUIT_OS)
#include "kstdlib.h"
#include "new.hpp"
#else
#include <stdlib.h>
#include <string.h>
#include <new>
#endif

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

        Vector(const std::initializer_list<T> &init) :
            Vector()
        {
            this->reserve(init.size());
            for(auto & value : init) {
                this->append(value);
            }
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
				for(size_t i = 0; i < this->mLength; i++) {
					this->mData[i].~T();
				}
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

        /**
         * @brief Removes the last element.
         */
        void pop()
        {
            if(this->length() > 0) {
                this->resize(this->length() - 1);
            }
        }

        void clear()
        {
            this->resize(0);
        }

        T& insert(size_t index, const T& value)
		{
			this->resize(this->mLength + 1);
			for(int32_t i = this->mLength - 2; i >= static_cast<int32_t>(index); i--) {
                // Move every item backwards
                this->mData[i+1] = this->mData[i];
            }
            // then override
            this->mData[index] = value;
            return this->mData[index];
        }

        void remove(size_t index)
        {
            for(uint32_t i = index; i < this->mLength; i++) {
                // Move every item backwards
                this->mData[i] = this->mData[i+1];
            }
            // then override
            this->mData[this->mLength - 1].~T();
            this->mLength -= 1;
        }

		void resize(size_t size)
		{
			size_t current = this->mLength;
			this->reserve(size);

			if(current > size) {
				// "Downgrade"
                for(int32_t i = static_cast<int32_t>(this->mLength) - 1; i >= static_cast<int32_t>(size); i--) {
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
			if((this->mReserved + initialCap) > space) {
				space = this->mReserved + initialCap;
			}
			const size_t newSize = sizeof(T) * space;

			T *newData = (T*)malloc(newSize);
			if(this->mData != nullptr) {
				// Ermahgerd, what a leak. Not obvious but it leaks.
				// memcpy(newData, this->mData, newSize);
				// Fix: copy construct all objects into the new memory, then destroy the original data.
				for(size_t i = 0; i < this->mLength; i++) {
					new (&newData[i]) T (this->mData[i]);
					this->mData[i].~T();
				}

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

        const T &front() const {
            return this->mData[0];
        }

        const T &back() const {
            return this->mData[this->mLength - 1];
        }

		T &front() {
			return this->mData[0];
		}

		T &back() {
			return this->mData[this->mLength - 1];
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

        const T *data() const
        {
            return &this->mData[0];
        }
	};
}
