#pragma once

#include <stddef.h>
#include <inttypes.h>

class ReferenceCounted
{
public:
	static int32_t counter;
private:
	size_t mReferenceCount;
protected:
	ReferenceCounted() :
		mReferenceCount(1)
	{
		counter++;
	}

	virtual ~ReferenceCounted() { counter--; }
public:
	void aquire() {
		this->mReferenceCount += 1;
	}

	void release() {
		this->mReferenceCount -= 1;
		if(this->mReferenceCount == 0) {
			delete this;
		}
	}
};
