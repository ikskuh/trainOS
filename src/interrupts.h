#pragma once

#define GDTF_DATASEG 0x02
#define GDTF_CODESEG 0x0a
#define GDTF_TSS     0x09

#define GDTF_SEGMENT 0x10
#define GDTF_RING0   0x00
#define GDTF_RING3   0x60
#define GDTF_PRESENT 0x80

#define GDTF_4K_GRAN 0x800
#define GDTF_32_BIT  0x400

#define INTR_GATE 6
#define INTR_TRAP_GATE 7
#define INTR_TASK_GATE 5

/**
 * Initializes interrupt handling and the global descriptor table.
 */
void intr_init();

/**
 * @brief Enables physical interrupts.
 */
static inline void irq_enable(void)
{
	__asm__ volatile("sti");
}

/**
 * @brief Disables physical interrupts.
 */
static inline void irq_disable(void)
{
	__asm__ volatile("cli");
}

