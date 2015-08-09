#pragma once

#include <stddef.h>
#include <inttypes.h>
#include "varargs.h"

char *itoa(int value, char *str, int base);
int atoi(const char *str);
float atof(const char *str);

/**
 * Allocates a block of memory
 * @param size Minimum size of the memory block
 * @return Pointer to the allocated memory area
 */
void *malloc(size_t size);

/**
 * Frees a previously allocated block of memory.
 * @param mem The block of memory.
 */
void free(void *mem);

static inline void *memset(void *ptr, int value, size_t num)
{
    uint8_t *it = (uint8_t*)ptr;
	while((num--) > 0)
	{
		*(it++) = (uint8_t)(value & 0xFF);
	}
	return ptr;
}

static inline void *memcpy(void *destination, const void *source, size_t num)
{
	uint8_t *to = (uint8_t*)destination;
	uint8_t *from = (uint8_t*)source;
	while((num--) > 0)
	{
		*(to++) = *(from++);
	}
	return destination;
}

void *memmove(void *destination, const void *source, size_t num);

static inline char *strcpy(char *destination, const char *source)
{
    while(*source)
	{
		*(destination++) = *(source++);
	}
	return destination;
}

static inline char *strcat(char *destination, const char *source)
{
    char *it = destination;
    while(*it++);
    it--;
    while(*source)
    {
        *it++ = *source++;
    }
    *it = 0;
    return destination;
}

static inline size_t strlen(const char *str)
{
	size_t size = 0;
	while(*(str++) != 0) size++;
	return size;
}

static inline void *calloc(size_t size)
{
    void *mem = malloc(size);
    memset(mem, 0, size);
    return mem;
}
