#pragma once

#include <inttypes.h>

#if defined(__cplusplus)
extern "C"  {
#endif

typedef uint32_t time_t;

/**
 * @brief Initializes the hardware timer.
 */
void timer_init();

/**
 * @brief Gets the number of ticks the timer has ticked.
 * @return
 */
time_t __cdecl timer_get();

/**
 * @brief Sets the timer to a given value.
 * @param time The number of ticks the timer should have now.
 */
void __cdecl timer_set(time_t time);

/**
 * @brief Waits until a certain time elapsed.
 * @param ticks The number of ticks to wait
 */
void __cdecl sleep(time_t ticks);

#if defined(__cplusplus)
}
#endif
