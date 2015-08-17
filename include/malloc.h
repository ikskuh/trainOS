#pragma once

#include <stddef.h>
#include <inttypes.h>

#include "config.h"

#if defined(__cplusplus)
extern "C"  {
#endif

#if defined(USE_VERBOSE_MALLOC)
void *malloc_d(size_t,const char *,int);
#endif

#if defined(USE_VERBOSE_FREE)
void free_d(void *,const char*, int);
#endif

#if defined(ENABLE_MALLOC_MONITORING)
void *_malloc(size_t, const char *, int);

void* _realloc (void*, size_t, const char *, int);

#define malloc(size) _malloc((size), __FILE__, __LINE__)
#define realloc(ptr, size) _realloc((ptr), (size), __FILE__, __LINE__)

#else
/**
 * Allocates a block of memory
 * @param size Minimum size of the memory block
 * @return Pointer to the allocated memory area
 */
void *malloc(size_t size);

void* realloc (void* ptr, size_t size);
#endif

/**
 * Frees a previously allocated block of memory.
 * @param mem The block of memory.
 */
void free(void *mem);

#if defined(ENABLE_MALLOC_MONITORING)
void malloc_print_list(int freeList);
#endif

#if defined(__cplusplus)
}
#endif


#if defined(USE_VERBOSE_MALLOC)
#define malloc(size) malloc_d((size), __FILE__, __LINE__)
#endif

#if defined(USE_VERBOSE_FREE)
#define free(ptr) free_d((ptr), __FILE__, __LINE__)
#endif
