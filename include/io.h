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

#if defined(__cplusplus)
}
#endif
