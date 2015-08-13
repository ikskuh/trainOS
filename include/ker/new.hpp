#pragma once

#include <inttypes.h>

inline void* operator new(size_t, void* __p)
{
	return __p;
}

inline void* operator new[](size_t, void* __p)
{
	return __p;
}

inline void operator delete  (void*, void*) { }
inline void operator delete[](void*, void*) { }
