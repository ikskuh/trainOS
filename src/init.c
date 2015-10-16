#include <kernel.h>
#include <kstdlib.h>
#include <console.h>
#include <interrupts.h>
#include <pmm.h>
#include <vmm.h>
#include <timer.h>
#include <config.h>
#include <serial.h>

static void halt()
{
	while(1) {
		__asm__ volatile ("cli; hlt;");
	}
}

void die(const char *msg)
{
	die_extra(msg, "");
}

void die_extra(const char *msg, const char *extra)
{
	kputs("\n");
	ksetcolor(COLOR_LIGHTMAGENTA, COLOR_BLACK);
	kputs(msg);
	if((extra != nullptr) && (strlen(extra) > 0)) {
		kputs(": '");
		kputs(extra);
		kputc('\'');
	}
	halt();
}

extern size_t mallocCount;
extern size_t freeCount;
extern size_t allocatedMemory;

void update_statusbar(time_t t)
{
    size_t a, b;
    static char tmp[64];
    static char buffer[CONSOLE_WIDTH];
    memset(buffer, ' ', sizeof(char) * CONSOLE_WIDTH);

    memcpy(&buffer[0], "Malloc: ", 8);

    b = 8;
    a = strlen(itoa(freeCount, tmp, 10));
    memcpy(&buffer[8], tmp, a);
    b += a;

    buffer[b] = '/'; b++;

    a = strlen(itoa(mallocCount, tmp, 10));
    memcpy(&buffer[b], tmp, a);
    b += a;

    b += 2;

    a = strlen(itoa(allocatedMemory, tmp, 10));
    memcpy(&buffer[b], tmp, a);
    b += a;

    memcpy(&buffer[b], " Byte", 5);

    console_setstate(buffer);
}

static void debug_test()
{
	char buffer[64];
	kputs("itoa test:\n");
	kputs("10 =   "); kputs(itoa(10, buffer, 10)); kputc('\n');
	kputs("10 = 0x"); kputs(itoa(10, buffer, 16)); kputc('\n');
	kputs("10 = 0b"); kputs(itoa(10, buffer, 2)); kputc('\n');
	kputs("10 = 0o"); kputs(itoa(10, buffer, 8)); kputc('\n');

	kputs("printf test:\n");
	kprintf("This %s %c test line.\n", "is", 'a');
	kprintf("Numbers: %d %i %x %b\n", 15, 15, 15, 15);

    ///*
	kputs("scroll-test:\n");
    for(int i = 0; i < 10; i++)
	{
		kprintf("They see me scrolling, they hating! %i\n", i);
    }
    /*
	for(int i = 0; i < 272; i++)
	{
		kprintf("x");
	}
	*/
}

static void dumpMB(const MultibootStructure *mbHeader)
{
	serial_printf(SERIAL_COM1, "Multiboot Information:\n");
    if(mbHeader->flags & MB_MEMSIZE)
    {
		serial_printf(SERIAL_COM1, "  Lower Memory: %d kB\n", mbHeader->memLower);
		serial_printf(SERIAL_COM1, "  Upper Memory: %d kB\n", mbHeader->memUpper);
    }
    // TODO: MB_BOOTDEVICE
    if(mbHeader->flags & MB_COMMANDLINE)
    {
		serial_printf(SERIAL_COM1, "  Commandline: %s\n", (const char*)mbHeader->commandline);
    }
    if(mbHeader->flags & MB_MODULES)
    {
        const MultibootModule *mod = (const MultibootModule *)mbHeader->modules;
        for(size_t i = 0; i < mbHeader->moduleCount; i++)
        {
			serial_printf(SERIAL_COM1, "  Module %s [%d - %d]\n", (const char*)mod[i].name, mod[i].start, mod[i].end);
        }
    }
    if(mbHeader->flags & MB_SYMS_AOUT)
    {
		serial_printf(SERIAL_COM1, "  Kernel File Format: a.out\n");
    }
    if(mbHeader->flags & MB_SYMS_ELF)
    {
		serial_printf(SERIAL_COM1, "  Kernel File Format: ELF\n");
    }
    if(mbHeader->flags & MB_MEMORYMAP)
    {
        uintptr_t it = mbHeader->memoryMap;
		serial_printf(SERIAL_COM1, "  Memory Map: %d entries\n", mbHeader->memoryMapLength);
        for(size_t i = 0; i < mbHeader->memoryMapLength; i++)
        {
            const MultibootMemoryMap *mmap = (const MultibootMemoryMap *)it;
            if(mmap->type == 1)
				serial_printf(SERIAL_COM1, "    Memory Map: [%d + %d] %s\n", (uint32_t)mmap->base, (uint32_t)mmap->length, mmap->type == 1 ? "free" : "preserved");
            it += mmap->entry_size + 4; // Stupid offset :P
        }
    }
    // TODO: MB_DRIVES
    // TODO: MB_CONFIG_TABLE
    if(mbHeader->flags & MB_BOOTLOADER_NAME)
    {
		serial_printf(SERIAL_COM1, "  Bootloader Name: %s\n", (const char*)mbHeader->bootLoaderName);
    }
    // TODO: MB_APS_TABLE
}

void cpp_init();

void cpp_exit();

void putsuccess()
{
	int y; kgetpos(nullptr, &y);
	ksetpos(CONSOLE_WIDTH - 9, y);
	kputs("[success]");
}

extern void vm_start(const MultibootStructure *mbHeader);

void init(const MultibootStructure *mbHeader)
{
    (void)debug_test;
    (void)dumpMB;

	ksetcolor(COLOR_BLACK, COLOR_LIGHTGRAY);
	kclear();
    kputs("Welcome to \x12\005CircuitOS\x12\007!\n");

	serial_init(SERIAL_COM1, 9600, SERIAL_PARITY_NONE, 8);
	serial_write_str(SERIAL_COM1, "Debug Console Ready\n");

	dumpMB(mbHeader);

	if(((mbHeader->flags & MB_MODULES) == 0) || (mbHeader->moduleCount == 0)) {
		die_extra("BootProcessFailed", "No initial OS modules found.");
	}

    kputs("Initialize physical memory management: ");
	pmm_init(mbHeader);
	putsuccess();

	uint32_t freeMem = pmm_calc_free();
	kprintf("Free memory: %d B, %d kB, %d MB\n", freeMem, freeMem >> 10, freeMem >> 20);

#if defined(USE_VIRTUAL_MEMORY_MANAGEMENT)
	kputs("Initialize virtual memory management: ");
	vmm_init();
	putsuccess();
#endif

	kputs("Initialize interrupts:");
	intr_init();
	putsuccess();

	kputs("Enable hw interrupts:");
	irq_enable();
	putsuccess();

#if defined(USE_VIRTUAL_MEMORY_MANAGEMENT)
	kputs("Prepare heap memory:");
	for(uintptr_t ptr = 0x400000; ptr < 0x800000; ptr += 4096)
	{
		vmm_map(ptr, (uintptr_t)pmm_alloc(), VM_PROGRAM);
	}
	putsuccess();
#endif

	kputs("Initialize timer:");
	timer_init();
	putsuccess();

	kputs("Initialize C++ objects: ");
	cpp_init();
    putsuccess();

	timer_add_callback(1, update_statusbar);

    kputs("\n");

	vm_start(mbHeader);

	irq_disable();

    cpp_exit();

    kputs("\x12\x04" "CircuitOS stopped." "\x12\x07" "\n");

#if defined(ENABLE_MALLOC_MONITORING)
	malloc_print_list(0);
#endif

	halt();
}

int main(int argc, char **argv)
{
    while(1) {
        kputs("x");
    }
    return 0;
}

void __init_array_start() { }
void __init_array_end() { }





