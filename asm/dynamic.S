.section .text
.intel_syntax noprefix

.extern _callee

/**
 * void __cdecl dynamic_call(void *func, void *stack, size_t stacksize)
 */
.global dynamic_call
dynamic_call:
	/* save everything except EAX, ECX, EDX */
	push ebx
	push esi
	push edi

	/* setup call */
	mov ebx, [esp+0x18] /* stacksize */
	mov eax, [esp+0x10] /* function */
	mov esi /* src */, [esp+0x14] /* stackptr */

	sub esp, ebx /* adjust stack pointer for call */
	mov edi /* dst */, esp
	mov ecx /* num */, ebx

	/* jump to label if we don't need to copy anything */
	cmp ebx, 0x00
	jz label

	/* copy virtual stack data */
	cld
	rep movsb

label:
	/* call function and restore stack */
	call eax
	add esp, ebx /* ebx = stacksize */

	/* clean up */
	pop edi
	pop esi
	pop ebx
	ret
