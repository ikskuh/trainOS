#include <config.h>
#include <vmm.h>
#include <pmm.h>
#include <stdlib.h>
#include <console.h>
#include <kernel.h>

#if defined(USE_VIRTUAL_MEMORY_MANAGEMENT)

typedef struct
{
    uintptr_t *directory;
} VmmContext;

VmmContext *context = nullptr;

VmmContext* vmm_create_context(void)
{
    VmmContext* context = (VmmContext*)pmm_alloc();

    // Initialize page directory
    context->directory = pmm_alloc();
    for (uint32_t i = 0; i < 1024; i++)
    {
        context->directory[i] = 0;
    }
    return context;
}

static inline void vmm_activate_context(VmmContext* context)
{
    __asm__ volatile("mov %0, %%cr3" : : "r" (context->directory));
}

void vmm_map(uintptr_t virtual, uintptr_t physical, uint32_t flags)
{
    // Prepare parameters
    // Align 12 bit
    virtual = (virtual >> 12) << 12;
    physical = (physical >> 12) << 12;
    flags &= 0x0007;

    uint32_t pageIndex = virtual / 4096;

    uint32_t dirIdx = pageIndex / 1024;    // Index in page directory
    uint32_t tableIdx = pageIndex % 1024; // Index in page table

    uintptr_t *table = nullptr;
    if((context->directory[dirIdx] & VM_PRESENT) == 0)
    {
        // Allocate page table if not exists.
        table = pmm_alloc();
        for(uint32_t i = 0; i < 1024; i++)
        {
            table[i] = 0;
        }
        context->directory[dirIdx] = (uintptr_t)table | VM_PRESENT | VM_WRITABLE;
    }
    else
    {
        table = (uintptr_t*)(context->directory[dirIdx] & 0xfffff000);
    }
    table[tableIdx] = physical | flags;

    __asm__ volatile("invlpg %0" : : "m" (*(char*)virtual));
}

void vmm_init(void)
{
    context = vmm_create_context();

    /* Die ersten 4 MB an dieselbe physische wie virtuelle Adresse mappen */
    for (uintptr_t ptr = 0; ptr < 0x400000; ptr += 0x1000)
    {
        vmm_map(ptr, ptr, VM_PRESENT | VM_WRITABLE);
    }

    vmm_activate_context(context);

    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= (1 << 31);
    __asm__ volatile("mov %0, %%cr0" : : "r" (cr0));
}

#endif
