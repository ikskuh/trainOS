#include <io.h>
#include <serial.h>
#include <kstdlib.h>
#include <varargs.h>

// Funktion zum initialisieren eines COM-Ports
void serial_init(uint16_t base, uint32_t baud, uint8_t parity, uint8_t bits)
{
	// Teiler berechnen
	union {
		uint8_t b[2];
		uint16_t w;
	} divisor;
	divisor.w = 115200/baud;

	// Interrupt ausschalten
	outb(base+SERIAL_IER,0x00);

	// DLAB-Bit setzen
	outb(base+SERIAL_LCR,0x80);

	// Teiler (low) setzen
	outb(base+0,divisor.b[0]);

	// Teiler (high) setzen
	outb(base+1,divisor.b[1]);

	// Anzahl Bits, Parität, usw setzen (DLAB zurücksetzen)
	outb(base+SERIAL_LCR,((parity&0x7)<<3)|((bits-5)&0x3));

	// Initialisierung abschließen
	outb(base+SERIAL_FCR,0xC7);
	outb(base+SERIAL_MCR,0x0B);
}

int serial_can_write(uint16_t base)
{
	return inb(base+SERIAL_LSR)&0x20;
}

// Byte senden
void write_com(uint16_t base, uint8_t chr) {
	while (serial_can_write(base)==0);
	outb(base,chr);
}

void serial_write(uint16_t port, const uint8_t *data, size_t length)
{
	while(length--) {
		write_com(port, *data++);
	}
}

// Prüft, ob man bereits lesen kann
int serial_can_read(uint16_t base)
{
	return inb(base+SERIAL_LSR)&1;
}

// Byte empfangen
static uint8_t read_serial(uint16_t base)
{
	while (!serial_can_read(base));
	return inb(base);
}

void serial_read(uint16_t port, uint8_t *data, size_t length)
{
	while(length--) {
		*data++ = read_serial(port);
	}
}

void serial_printf(uint16_t port, const char *format, ...)
{
	static char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	va_list vl;
	va_start(vl, format);
	vsprintf(buffer, format, vl);
	va_end(vl);

	serial_write_str(port, buffer);
}
