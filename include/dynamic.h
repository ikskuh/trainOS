#pragma once
#include <inttypes.h>

#if defined(__cplusplus)
extern "C"  {
#endif

extern void __cdecl dynamic_call(void *func, void *stack, uint32_t stackSize);

#if defined(__cplusplus)
}
#endif
