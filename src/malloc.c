#include <stdlib.h>
#include <stdint.h>

size_t mallocCount = 0, freeCount = 0;
size_t allocatedMemory = 0;

static char * const malloc_heap_start = (char *)0x400000;
static char * const malloc_heap_end = (char *)0x800000;

static char * current = nullptr;

void *malloc(size_t len)
{
    allocatedMemory += len;
	mallocCount++;

	if(current == nullptr) {
		current = malloc_heap_start;
	}

	void *ptr = current;
	current += len;

	if((uintptr_t)current >= (uintptr_t)malloc_heap_end) {
		die("malloc.OutOfMemory");
	}

	return ptr;
}




void free(void *__ptr)
{
	freeCount++;
}
