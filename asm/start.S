.section .text

// C-Einstiegspunkt
.extern init

// .global für Sichtbarkeit im Linker (invers zu static aus C)
.global _trainOS_start
_trainOS_start:
	// Init stack
	mov $kernel_stack, %esp

	// Start Kernel
	push %ebx
	call init

	// Halte an, no matter what
_stop:
	cli
	hlt
	jmp _stop

	// 16 KB Stackspace
.section .bss
.space 16384
kernel_stack:
