#pragma once

#include <stddef.h>
#include <inttypes.h>

#define SERIAL_IER 1
#define SERIAL_IIR 2
#define SERIAL_FCR 2
#define SERIAL_LCR 3
#define SERIAL_MCR 4
#define SERIAL_LSR 5
#define SERIAL_MSR 6

#define SERIAL_COM1	0x3F8
#define SERIAL_COM2	0x2F8
#define SERIAL_COM3	0x3E8
#define SERIAL_COM4	0x2E8

#define SERIAL_PARITY_NONE 0b000
#define SERIAL_PARITY_ODD  0b100
#define SERIAL_PARITY_EVEN 0b110
#define SERIAL_PARITY_HIGH 0b101
#define SERIAL_PARITY_LOW  0b111

#if defined(__cplusplus)
extern "C"  {
#endif

void serial_init(uint16_t port, uint32_t baud, uint8_t parity, uint8_t bits);

int serial_can_read(uint16_t port);

int serial_can_write(uint16_t port);

void serial_write(uint16_t port, const uint8_t *data, size_t length);

void serial_read(uint16_t port, uint8_t *data, size_t length);

void serial_printf(uint16_t port, const char *format, ...);

static inline void serial_write_str(uint16_t port, const char *text)
{
	while(*text) {
		serial_write(port, (uint8_t*)text, 1);
		text++;
	}
}

#if defined(__cplusplus)
}
#endif
