#include <timer.h>
#include <stdio.h>
#include <kernel.h>
#include <interrupts.h>

#define CALLBACK_COUNT 64

static volatile time_t ticks = 0;

static struct {
    time_t interval;
    void (*callback)(time_t);
} callbacks[CALLBACK_COUNT];

static void timer_irq()
{
	ticks++;

    irq_disable();
    for(size_t i = 0; i < CALLBACK_COUNT; i++) {
        if(callbacks[i].callback != nullptr) {
            if((ticks % callbacks[i].interval) == 0) {
                callbacks[i].callback(ticks);
            }
            return;
        }
    }
    irq_enable();
}

void timer_init()
{
	ticks = 0;
	intr_set_handler(0x20, timer_irq);
}

time_t timer_get()
{
	return ticks;
}

void timer_set(time_t time)
{
	ticks = time;
}

void timer_add_callback(time_t interval, void (*callback)(time_t))
{
    for(size_t i = 0; i < CALLBACK_COUNT; i++) {
        if(callbacks[i].callback == nullptr) {
            callbacks[i].interval = interval;
            callbacks[i].callback = callback;
            return;
        }
    }
    die("no timer callbacks left.");
}

void sleep(time_t t)
{
	uint64_t end = timer_get() + t;
	while(timer_get() < end);
}

