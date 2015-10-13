#include <config.h>
#include <kernel.h>
#include <kstdlib.h>
#include <stdint.h>
#include <console.h>
#include <serial.h>

#undef malloc
#undef free

static const uint32_t magic = 0xDEADBEEF;

#define STACKDEPTH 8

typedef struct List
{
#if defined(USE_MAGIC_SECURED_MALLOC)
	size_t hash;
#endif
	size_t length;
	size_t used;
	struct List *next;
#if defined(ENABLE_MALLOC_MONITORING)
	const char *allocationFile;
	int allocationLine;
	void *stacktrace[STACKDEPTH];
#endif
} List;

static uint32_t hash(List *list)
{
	uint32_t h = magic;
	h ^= list->length;
	h ^= list->used;
	h ^= (uint32_t)list->next;
#if defined(ENABLE_MALLOC_MONITORING)
	h ^= (uint32_t)list->allocationFile;
	h ^= list->allocationLine;
#endif
	return h;
}

static int isValid(List *list)
{
#if defined(USE_MAGIC_SECURED_MALLOC)
	return list->hash == hash(list);
#else
    return 1; // Yes, it is always valid. What a pity....
#endif
}

size_t mallocCount = 0, freeCount = 0;
size_t allocatedMemory = 0;

static const size_t minimumAllocSize = 1; // 2 * sizeof(List);
static char * const malloc_heap_start = (char *)0x400000;
static char * const malloc_heap_end = (char *)0x800000;

List *listBegin = nullptr;

#if defined(ENABLE_MALLOC_MONITORING)
void malloc_print_list(int freeList)
{
	List *list = listBegin;
	serial_printf(SERIAL_COM1, "malloc list: \n");
	size_t count = 0;
	while(list != nullptr)
	{
#if defined(USE_MAGIC_SECURED_MALLOC)
		if(isValid(list) == 0) {
			die("malloc::print_list.InvalidMagicNumber");
		}
#endif
		if(freeList || list->used) {
			serial_printf(SERIAL_COM1,
					"[%x -> %x] (%s:%d) %d %d\n",
					list,
					list->next,
					list->used ? list->allocationFile : "",
					list->used ? list->allocationLine : 0,
					list->used,
					list->length);
			serial_printf(SERIAL_COM1,"    ");
			for(int i = 0; i < STACKDEPTH; i++) {
				serial_printf(SERIAL_COM1, "%x ", list->stacktrace[i]);
			}
			serial_printf(SERIAL_COM1,"\n");

			serial_printf(SERIAL_COM1,"[");
			serial_write(SERIAL_COM1, (const uint8_t*)(list) + sizeof(List), list->length);
			serial_printf(SERIAL_COM1,"]");

			/*
			kprintf("[%x -> %x] (%s:%d) %d %d\n",
					list,
					list->next,
					list->used ? list->allocationFile : "",
					list->used ? list->allocationLine : 0,
					list->used,
					list->length);
			*/
			if(list->used) {
				count++;
			}
		}
		list = list->next;
	}
	kprintf("Unfreed objects: %d\n", count);
	serial_printf(SERIAL_COM1, "Unfreed objects: %d\n", count);
}
#else
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
#endif

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

#if defined(USE_MAGIC_SECURED_MALLOC)
		list->hash = hash(list);
#endif
	}
}

#if defined(ENABLE_MALLOC_MONITORING)
void *_malloc(size_t len, const char *file, int line)
#else
void *malloc(size_t len)
#endif
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
		listBegin->hash = hash(listBegin);
#endif
	}

	List *cursor = listBegin;

	// Find the first non-used List entry
	while((cursor != nullptr) && ((cursor->used != 0) || (cursor->length < (len + sizeof(List))))) {
		cursor = cursor->next;
	}
	if(cursor == nullptr) {
#if defined(ENABLE_MALLOC_MONITORING)
		malloc_print_list(1);
#else
		print_list();
#endif
		die_extra("malloc.OutOfMemory", itoa(len, nullptr, 10));
	}

	if(cursor->length < (sizeof(List) + len)) {
		die("malloc.FragmentationFailure");
	}

	if(cursor->length != len)
	{
		// Store total length
		size_t newLength = cursor->length - sizeof(List) - len;

		// Allocate the memory
		cursor->length = len;

		// Fragment list
		List *newl = (List*)((char*)cursor + (sizeof(List) + cursor->length));
		newl->length = newLength;
		newl->used = 0;
		newl->next = cursor->next;
#if defined(USE_MAGIC_SECURED_MALLOC)
		newl->hash = hash(newl);
		newl->stacktrace[0] = __builtin_return_address(1);
		newl->stacktrace[1] = __builtin_return_address(2);
		newl->stacktrace[2] = __builtin_return_address(3);
		newl->stacktrace[3] = __builtin_return_address(4);
		newl->stacktrace[4] = __builtin_return_address(5);
		newl->stacktrace[5] = __builtin_return_address(6);
		newl->stacktrace[6] = __builtin_return_address(7);
		newl->stacktrace[7] = __builtin_return_address(8);
#endif

		cursor->next = newl;
	}

	cursor->used = 1;
#if defined(ENABLE_MALLOC_MONITORING)
	cursor->allocationFile = file;
	cursor->allocationLine = line;
#endif

#if defined(USE_MAGIC_SECURED_MALLOC)
	cursor->hash = hash(cursor);
#endif

	allocatedMemory += len;
	mallocCount++;

	return (void*)((char*)cursor + sizeof(List));
}

#if defined(ENABLE_MALLOC_MONITORING)
void* _realloc (void* ptr, size_t size, const char *file, int line)
#else
void* realloc (void* ptr, size_t size)
#endif
{
#if defined(ENABLE_MALLOC_MONITORING)
	void *n = _malloc(size, file, line);
#else
	void *n = malloc(size);
#endif
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
		die_extra("free.InvalidFree", itoa((int)ptr, nullptr, 16));
	}
	if((uintptr_t)ptr >= (uintptr_t)malloc_heap_end) {
		die_extra("free.InvalidFree", itoa((int)ptr, nullptr, 16));
	}
	freeCount++;

	List *entry = (List*)((char*)ptr - sizeof(List));
	if(entry->used == 0) {
		die_extra("free.InvalidBlock", itoa((int)ptr, nullptr, 16));
	}
#if defined(USE_MAGIC_SECURED_MALLOC)
	if(isValid(entry) == 0) {
		die_extra("free.InvalidBlockMagic: ", itoa(entry->hash, nullptr, 16));
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

#if defined(ENABLE_MALLOC_MONITORING)
	void *ptr = _malloc(len, file, line);
#else
	void *ptr = malloc(len);
#endif
	serial_write_str(SERIAL_COM1, itoa((int)ptr, nullptr, 16));
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
	serial_write_str(SERIAL_COM1, itoa((int)ptr, nullptr, 16));
	serial_write_str(SERIAL_COM1, " in ");
	serial_write_str(SERIAL_COM1, file);
	serial_write_str(SERIAL_COM1, ":");
	serial_write_str(SERIAL_COM1, itoa(line, nullptr, 10));
	serial_write_str(SERIAL_COM1, ".\n");
	free(ptr);
}
