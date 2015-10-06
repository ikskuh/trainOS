#include <stdlib.h>
#include <timer.h>
#include <dynamic.h>
#include <console.h>

extern "C" {
	extern const char mainscript_start;
	extern const char mainscript_end;
	extern const char mainscript_size;
}

extern "C" void vm_start()
{
	struct {
		const char *ptr;
		uint32_t size;
	} mainfile {
		&mainscript_start,
		(uint32_t)&mainscript_size
	};

    (void)mainfile;

    kprintf("No implementation of Conductance here yet.\n");
}

