/*  Bei _start soll die Ausfuehrung losgehen */
ENTRY(_trainOS_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

/*
 * Hier wird festgelegt, in welcher Reihenfolge welche Sektionen in die Binary
 * geschrieben werden sollen
 */
SECTIONS
{
    /*
     * Startpunkt des Kernels
     */
    . = 0x100000;

    /* Kernel Start Punkt */
    kernelStart = .;

    /*
     * Two text sections: .text and .multiboot
     * Multiboot must be at the start of the kernel
     */
    .text : {
        *(multiboot)
        *(.text)
    }
    .data ALIGN(4096) : {
        start_ctors = .;
        KEEP(*( .init_array ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
        end_ctors = .;

        *(.data)
    }
    .rodata ALIGN(4096) : {
        *(.rodata)
    }
    .bss ALIGN(4096) : {
        *(.bss)
    }

    /* Align the end of the kernel to the page size */
    . = ALIGN(4096);
    kernelEnd = .;
}
