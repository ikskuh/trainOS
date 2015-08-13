#pragma once

#include <inttypes.h>

#if defined(__cplusplus)
extern "C"  {
#endif

/**
 * @brief Initializes the hardware timer.
 */
void timer_init();

/**
 * @brief Gets the number of ticks the timer has ticked.
 * @return
 */
uint64_t timer_get();

/**
 * @brief Sets the timer to a given value.
 * @param time The number of ticks the timer should have now.
 */
void timer_set(uint64_t time);


/**
 * @brief Waits until a certain time elapsed.
 * @param ticks The number of ticks to wait
 */
void sleep(uint64_t ticks);

#if defined(__cplusplus)
}
#endif
