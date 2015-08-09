#include "interrupts.h"
#include "console.h"
#include "stdlib.h"
#include "cpustate.h"
#include "io.h"

#define GDT_ENTRIES 5
#define IDT_ENTRIES 256

static uint64_t gdt[GDT_ENTRIES];
static uint64_t idt[IDT_ENTRIES];

static const char *interruptNames[] = {
	"Divide-by-zero Error",//	0 (0x0)	Fault	#DE	No
	"Debug",//	1 (0x1)	Fault/Trap	#DB	No
	"Non-maskable Interrupt",//	2 (0x2)	Interrupt	-	No
	"Breakpoint",//	3 (0x3)	Trap	#BP	No
	"Overflow",//	4 (0x4)	Trap	#OF	No
	"Bound Range Exceeded",//	5 (0x5)	Fault	#BR	No
	"Invalid Opcode",//	6 (0x6)	Fault	#UD	No
	"Device Not Available",//	7 (0x7)	Fault	#NM	No
	"Double Fault",//	8 (0x8)	Abort	#DF	Yes
	"Coprocessor Segment Overrun",//	9 (0x9)	Fault	-	No
	"Invalid TSS",//	10 (0xA)	Fault	#TS	Yes
	"Segment Not Present",//	11 (0xB)	Fault	#NP	Yes
	"Stack-Segment Fault",//	12 (0xC)	Fault	#SS	Yes
	"General Protection Fault",//	13 (0xD)	Fault	#GP	Yes
	"Page Fault",//	14 (0xE)	Fault	#PF	Yes
	"Reserved",//	15 (0xF)	-	-	No
	"x87 Floating-Point Exception",//	16 (0x10)	Fault	#MF	No
	"Alignment Check",//	17 (0x11)	Fault	#AC	Yes
	"Machine Check",//	18 (0x12)	Abort	#MC	No
	"SIMD Floating-Point Exception",//	19 (0x13)	Fault	#XM/#XF	No
	"Virtualization Exception",//	20 (0x14)	Fault	#VE	No
	"Reserved 21",//	21-29 (0x15-0x1D)	-	-	No
	"Reserved 22",//	21-29 (0x15-0x1D)	-	-	No
	"Reserved 23",//	21-29 (0x15-0x1D)	-	-	No
	"Reserved 24",//	21-29 (0x15-0x1D)	-	-	No
	"Reserved 25",//	21-29 (0x15-0x1D)	-	-	No
	"Reserved 26",//	21-29 (0x15-0x1D)	-	-	No
	"Reserved 27",//	21-29 (0x15-0x1D)	-	-	No
	"Reserved 28",//	21-29 (0x15-0x1D)	-	-	No
	"Reserved 29",//	21-29 (0x15-0x1D)	-	-	No
	"Security Exception",//	30 (0x1E)	-	#SX	Yes
	"Reserved 31",//	31 (0x1F)	-	-	No
	"IRQ 0",
	"IRQ 1",
	"IRQ 2",
	"IRQ 3",
	"IRQ 4",
	"IRQ 5",
	"IRQ 6",
	"IRQ 7",
	"IRQ 8",
	"IRQ 9",
	"IRQ 10",
	"IRQ 11",
	"IRQ 12",
	"IRQ 13",
	"IRQ 14",
	"IRQ 15",
};

static const size_t interruptNameCount = sizeof(interruptNames) / sizeof(interruptNames[0]);

#include "intr_stubs.h"

void intr_routine(CpuState *state)
{
	const char *name = "Unknown";
	if(state->intr < interruptNameCount)
		name = interruptNames[state->intr];
	if(state->intr < 0x20)
	{
		kprintf("\n\x12\x04Exception [%d] %s!\x12\0x7\n", state->intr, name);
		while(1)
		{
			__asm__ volatile("cli; hlt");
		}
	}
	if (state->intr >= 0x20 && state->intr <= 0x2f)
	{
		if (state->intr >= 0x28)
		{
			// EOI an Slave-PIC
			outb(0xa0, 0x20);
		}
		// EOI an Master-PIC
		outb(0x20, 0x20);
	}
	else
	{
		kprintf("\n\x12\x04Interrupt [%d] %s occurred!\x12\0x7\n", state->intr, name);
		while(1)
		{
			// Prozessor anhalten
			__asm__ volatile("cli; hlt");
		}
	}
}

static void gdt_entry(int i, uint32_t base, uint32_t limit, int flags)
{
    gdt[i] = limit & 0xffffLL;
    gdt[i] |= (base & 0xffffffLL) << 16;
    gdt[i] |= (flags & 0xffLL) << 40;
    gdt[i] |= ((limit >> 16) & 0xfLL) << 48;
    gdt[i] |= ((flags >> 8 )& 0xffLL) << 52;
    gdt[i] |= ((base >> 24) & 0xffLL) << 56;
}

static void idt_entry(int i, void (*fn)(), unsigned int selector, int flags)
{
	unsigned long int handler = (unsigned long int) fn;
	idt[i] = handler & 0xffffLL;
	idt[i] |= (selector & 0xffffLL) << 16;
	idt[i] |= (flags & 0xffLL) << 40;
	idt[i] |= ((handler>> 16) & 0xffffLL) << 48;
}

static void init_gdt(void)
{
	memset(gdt, 0, sizeof(gdt));

    gdt_entry(0, 0, 0, 0);
    gdt_entry(1, 0, 0xfffff, GDTF_SEGMENT | GDTF_32_BIT |
        GDTF_CODESEG | GDTF_4K_GRAN | GDTF_PRESENT);
    gdt_entry(2, 0, 0xfffff, GDTF_SEGMENT | GDTF_32_BIT |
        GDTF_DATASEG | GDTF_4K_GRAN | GDTF_PRESENT);
    gdt_entry(3, 0, 0xfffff, GDTF_SEGMENT | GDTF_32_BIT |
        GDTF_CODESEG | GDTF_4K_GRAN | GDTF_PRESENT | GDTF_RING3);
    gdt_entry(4, 0, 0xfffff, GDTF_SEGMENT | GDTF_32_BIT |
        GDTF_DATASEG | GDTF_4K_GRAN | GDTF_PRESENT | GDTF_RING3);

	struct
	{
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) gdtp =
	{
		.limit = GDT_ENTRIES * 8 - 1,
		.pointer = gdt,
	};
	__asm__ volatile("lgdt %0" : : "m" (gdtp));
	__asm__ volatile(
		"mov $0x10, %ax;"
		"mov %ax, %ds;"
		"mov %ax, %es;"
		"mov %ax, %ss;"
		"ljmp $0x8, $.1;"
		".1:"
	);
}

#define IDT_FLAG_INTERRUPT_GATE 0xe
#define IDT_FLAG_PRESENT 0x80
#define IDT_FLAG_RING0 0x00
#define IDT_FLAG_RING3 0x60

static void init_pic(void)
{
	// Master-PIC initialisieren
	outb(0x20, 0x11); // Initialisierungsbefehl fuer den PIC
	outb(0x21, 0x20); // Interruptnummer fuer IRQ 0
	outb(0x21, 0x04); // An IRQ 2 haengt der Slave
	outb(0x21, 0x01); // ICW 4

	// Slave-PIC initialisieren
	outb(0xa0, 0x11); // Initialisierungsbefehl fuer den PIC
	outb(0xa1, 0x28); // Interruptnummer fuer IRQ 8
	outb(0xa1, 0x02); // An IRQ 2 haengt der Slave
	outb(0xa1, 0x01); // ICW 4

	// Alle IRQs aktivieren (demaskieren)
	outb(0x20, 0x0);
	outb(0xa0, 0x0);
}

static void init_idt(void)
{
	memset(idt, 0, sizeof(idt));

#define SET_ENTRY(i) idt_entry(i, intr_stub_ ## i, 0x08, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_PRESENT | IDT_FLAG_RING0)

	for(size_t i = 0; i < IDT_ENTRIES; i++) {
		idt_entry(i, intr_stub_0, 0x08, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_PRESENT | IDT_FLAG_RING0);
	}

	// System Interrupts
	SET_ENTRY(0);
	SET_ENTRY(1);
	SET_ENTRY(2);
	SET_ENTRY(3);
	SET_ENTRY(4);
	SET_ENTRY(6);
	SET_ENTRY(7);
	SET_ENTRY(8);
	SET_ENTRY(9);
	SET_ENTRY(10);
	SET_ENTRY(11);
	SET_ENTRY(12);
	SET_ENTRY(13);
	SET_ENTRY(14);
	SET_ENTRY(15);
	SET_ENTRY(16);
	SET_ENTRY(17);
	SET_ENTRY(18);

	// Hardware handler
	SET_ENTRY(32);
	SET_ENTRY(33);
	SET_ENTRY(34);
	SET_ENTRY(35);
	SET_ENTRY(36);
	SET_ENTRY(37);
	SET_ENTRY(38);
	SET_ENTRY(39);
	SET_ENTRY(40);
	SET_ENTRY(41);
	SET_ENTRY(42);
	SET_ENTRY(43);
	SET_ENTRY(44);
	SET_ENTRY(45);
	SET_ENTRY(46);
	SET_ENTRY(47);

	// System call
	SET_ENTRY(48);

#undef SET_ENTRY
	struct
	{
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) idtp =
	{
		.limit = IDT_ENTRIES * 8 - 1,
		.pointer = idt,
	};
	__asm__ volatile("lidt %0" : : "m" (idtp));
}

void intr_init(void)
{
	// Initialize global descriptor table
	init_gdt();

	// Initialize interrupt descriptor table
	init_idt();

	// Initialize Programmable Interrupt Controller
	init_pic();
}
