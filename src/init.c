#include <kernel.h>
#include <stdlib.h>
#include <console.h>
#include <interrupts.h>
#include <pmm.h>
#include <vmm.h>

#include <timer.h>

void die(const char *msg)
{
    kputs("\n");
    ksetcolor(COLOR_RED, COLOR_WHITE);
    kputs(msg);
    while(1)
    {
        __asm__ volatile ("cli; hlt;");
    }
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

	/*
	kputs("scroll-test:\n");
	for(int i = 0; i < 100; i++)
	{
		kprintf("They see me scrolling, they hating! %i\n", i);
	}
	for(int i = 0; i < 272; i++)
	{
		kprintf("x");
	}
	*/
}

static void dumpMB(const MultibootStructure *mbHeader)
{
    kputs("Multiboot Information:\n");
    if(mbHeader->flags & MB_MEMSIZE)
    {
        kprintf("Lower Memory: %d kB\n", mbHeader->memLower);
        kprintf("Upper Memory: %d kB\n", mbHeader->memUpper);
    }
    // TODO: MB_BOOTDEVICE
    if(mbHeader->flags & MB_COMMANDLINE)
    {
        kprintf("Commandline: %s\n", mbHeader->commandline);
    }
    if(mbHeader->flags & MB_MODULES)
    {
        const MultibootModule *mod = (const MultibootModule *)mbHeader->modules;
        for(size_t i = 0; i < mbHeader->moduleCount; i++)
        {
            kprintf("Module %s [%d - %d]\n", mod[i].name, mod[i].start, mod[i].end);
        }
    }
    if(mbHeader->flags & MB_SYMS_AOUT)
    {
        kputs("Kernel File Format: a.out\n");
    }
    if(mbHeader->flags & MB_SYMS_ELF)
    {
        kputs("Kernel File Format: ELF\n");
    }
    if(mbHeader->flags & MB_MEMORYMAP)
    {
        uintptr_t it = mbHeader->memoryMap;
        kprintf("Memory Map: %d entries\n", mbHeader->memoryMapLength);
        for(size_t i = 0; i < mbHeader->memoryMapLength; i++)
        {
            const MultibootMemoryMap *mmap = (const MultibootMemoryMap *)it;
            if(mmap->type == 1)
                kprintf("Memory Map: [%d + %d] %s\n", (uint32_t)mmap->base, (uint32_t)mmap->length, mmap->type == 1 ? "free" : "preserved");
            it += mmap->entry_size + 4; // Stupid offset :P
        }
    }
    // TODO: MB_DRIVES
    // TODO: MB_CONFIG_TABLE
    if(mbHeader->flags & MB_BOOTLOADER_NAME)
    {
        kprintf("Bootloader Name: %s\n", mbHeader->bootLoaderName);
    }
    // TODO: MB_APS_TABLE
}

void cpp_init()
{

}

void putsuccess()
{
	int y; kgetpos(nullptr, &y);
	ksetpos(CONSOLE_WIDTH - 9, y);
	kputs("[success]");
}

void init(const MultibootStructure *mbHeader)
{
    (void)debug_test;
    (void)dumpMB;

	ksetcolor(COLOR_BLACK, COLOR_LIGHTGRAY);
	kclear();
	kputs("Welcome to \x12\x05nucleo\x12\x07!\n");

    //dumpMB(mbHeader);

    kputs("Initialize physical memory management: ");
	pmm_init(mbHeader);
	putsuccess();

    // uint32_t freeMem = pmm_calc_free();
    //kprintf("Free memory: %d B, %d kB, %d MB\n", freeMem, freeMem >> 10, freeMem >> 20);

    kputs("Initialize virtual memory management: ");
	vmm_init();
	putsuccess();

	kputs("Initialize interrupts:");
	intr_init();
	putsuccess();

	kputs("Enable hw interrupts:");
	irq_enable();
	putsuccess();

	kputs("Prepare heap memory:");
	for(uintptr_t ptr = 0x400000; ptr < 0x800000; ptr += 4096)
	{
		vmm_map(ptr, (uintptr_t)pmm_alloc(), VM_PROGRAM);
	}
	putsuccess();

	kputs("Initialize timer:");
	timer_init();
	putsuccess();

	kputs("Initialize C++ objects: ");
	cpp_init();
	putsuccess();

	while(1)
	{
		kputs("x");
		sleep(1);
	}
}
