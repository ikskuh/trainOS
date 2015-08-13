
#include "timer.h"
#include "interrupts.h"

static volatile uint64_t ticks = 0;

static void timer_irq()
{
	ticks++;
}

void timer_init()
{
	ticks = 0;
	intr_set_handler(0x20, timer_irq);
}

uint64_t timer_get()
{
	return ticks;
}

void timer_set(uint64_t time)
{
	ticks = time;
}

void sleep(uint64_t t)
{
	uint64_t end = timer_get() + t;
	while(timer_get() < end);
}

