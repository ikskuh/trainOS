#pragma once

#include <inttypes.h>

#if defined(__cplusplus)
extern "C"  {
#endif


/**
 * @brief Outputs a byte on the given port.
 * @param port The port number.
 * @param data The byte to send.
 */
static inline void outb(uint16_t port, uint8_t data)
{
	__asm__ volatile ("outb %0, %1" : : "a" (data), "Nd" (port));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t data;
	__asm__ volatile ("inb %1, %0" : "=a" (data) : "d" (port));
	return data;
}

#if defined(__cplusplus)
}
#endif
