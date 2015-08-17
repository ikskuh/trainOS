#include <kernel.h>
#include <stdlib.h>
#include <stdint.h>
#include <console.h>
#include <serial.h>

#undef malloc
#undef free

typedef struct List
{
#if defined(USE_MAGIC_SECURED_MALLOC)
	size_t magic;
#endif
	size_t length;
	size_t used;
	struct List *next;
} List;

size_t mallocCount = 0, freeCount = 0;
size_t allocatedMemory = 0;

static const size_t minimumAllocSize = 2 * sizeof(List);
static char * const malloc_heap_start = (char *)0x400000;
static char * const malloc_heap_end = (char *)0x10000000;

List *listBegin = nullptr;

static void print_list()
{
	List *list = listBegin;
	serial_printf(SERIAL_COM1, "malloc list: \n");
	while(list != nullptr)
	{
#if defined(USE_MAGIC_SECURED_MALLOC)
		if(list->magic != 0xDEADBEEF) {
			die("malloc::print_list.InvalidMagicNumber");
		}
#endif
		serial_printf(SERIAL_COM1, "[%x -> %x] %d %d\n", list, list->next, list->used, list->length);
		list = list->next;
	}
}

static void defragment()
{
	List *list = listBegin;
	if(list == nullptr) {
		return;
	}
	for(; list->next != nullptr; list = list->next)
	{
		if(list->used != 0) {
			continue;
		}
		while(list->next->used == 0) {
			List *n = list->next->next;
			list->length += list->next->length + sizeof(List);
			list->next = n;
		}
	}
}

void *malloc(size_t len)
{
	// Prevent fragmentation, sacrifice memory
	if(len < minimumAllocSize) {
		len = minimumAllocSize;
	}

	if(listBegin == nullptr) {
		listBegin = (List*)malloc_heap_start;
		listBegin->length = (intptr_t)malloc_heap_end - (intptr_t)malloc_heap_start - sizeof(List);
		listBegin->used = 0;
		listBegin->next = nullptr;
#if defined(USE_MAGIC_SECURED_MALLOC)
		listBegin->magic = 0xDEADBEEF;
#endif
	}

	List *cursor = listBegin;

	// Find the first non-used List entry
	while((cursor != nullptr) && ((cursor->used != 0) || (cursor->length < (len + sizeof(List))))) {
		cursor = cursor->next;
	}
	if(cursor == nullptr) {
		print_list();
		die_extra("malloc.OutOfMemory", itoa(len, nullptr, 10));
	}

	if(cursor->length < (sizeof(List) + len)) {
		die("malloc.FragmentationFailure");
	}

	if(cursor->length == len)
	{
		cursor->used = 1;
	}
	else
	{
		// Store total length
		size_t newLength = cursor->length - sizeof(List) - len;

		// Allocate the memory
		cursor->used = 1;
		cursor->length = len;

		// Fragment list
		List *newl = (List*)((char*)cursor + (sizeof(List) + cursor->length));
		newl->length = newLength;
		newl->used = 0;
		newl->next = cursor->next;
#if defined(USE_MAGIC_SECURED_MALLOC)
		newl->magic = 0xDEADBEEF;
#endif

		cursor->next = newl;
	}

	allocatedMemory += len;
	mallocCount++;

	return (void*)((char*)cursor + sizeof(List));
}

void* realloc (void* ptr, size_t size)
{
	void *n = malloc(size);
	memcpy(n, ptr, size);
	free(ptr);
	return n;
}

void free(void *ptr)
{
	if(ptr == nullptr) {
		// Valid behaviour!
		return;
	}
	if((uintptr_t)ptr < (uintptr_t)malloc_heap_start) {
		die_extra("free.InvalidFree", itoa(ptr, nullptr, 16));
	}
	freeCount++;

	List *entry = (List*)((char*)ptr - sizeof(List));
	if(entry->used == 0) {
		die_extra("free.InvalidBlock", itoa(ptr, nullptr, 16));
	}
#if defined(USE_MAGIC_SECURED_MALLOC)
	if(entry->magic != 0xDEADBEEF) {
		die_extra("free.InvalidBlockMagic: ", itoa(entry->magic, nullptr, 16));
	}
#endif

	if(entry->length > 0x5000) {
		die_extra("free.InvalidSizedBlock: ", itoa(entry->length, nullptr, 10));
	}

	allocatedMemory -= entry->length;
	entry->used = 0;

	defragment();
}



void *malloc_d(size_t len, const char *file, int line)
{
	serial_write_str(SERIAL_COM1, "Allocate ");
	serial_write_str(SERIAL_COM1, itoa(len, nullptr, 10));
	serial_write_str(SERIAL_COM1, " bytes at ");
	serial_write_str(SERIAL_COM1, file);
	serial_write_str(SERIAL_COM1, ":");
	serial_write_str(SERIAL_COM1, itoa(line, nullptr, 10));
	serial_write_str(SERIAL_COM1, ": ");

	void *ptr = malloc(len);
	serial_write_str(SERIAL_COM1, itoa(ptr, nullptr, 16));
	serial_write_str(SERIAL_COM1, "\n");

	return ptr;
}

void free_d(void *ptr, const char *file, int line)
{
	if(ptr == nullptr) {
		return;
	}
	List *entry = (List*)((char*)ptr - sizeof(List));

	serial_write_str(SERIAL_COM1, "Free ");
	serial_write_str(SERIAL_COM1, itoa(entry->length, nullptr, 10));
	serial_write_str(SERIAL_COM1, " bytes at ");
	serial_write_str(SERIAL_COM1, itoa(ptr, nullptr, 16));
	serial_write_str(SERIAL_COM1, " in ");
	serial_write_str(SERIAL_COM1, file);
	serial_write_str(SERIAL_COM1, ":");
	serial_write_str(SERIAL_COM1, itoa(line, nullptr, 10));
	serial_write_str(SERIAL_COM1, ".\n");
	free(ptr);
}
