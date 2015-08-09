/*  Bei _start soll die Ausfuehrung losgehen */
ENTRY(_start)

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
