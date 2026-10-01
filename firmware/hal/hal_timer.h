/**
 * @file hal_timer.h
 * @brief Hardware Abstraction Layer for High-Resolution System Ticks & Timestamps.
 * 
 * Provides monotonic millisecond and microsecond timebases for multi-rate scheduling,
 * CAN timeout tracking, and animation frame-rate timing.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize hardware timer peripheral (e.g. SysTick, Timer1/Timer2).
 */
void hal_timer_init(void);

/**
 * @brief Get monotonic uptime in milliseconds.
 * @return Milliseconds elapsed since boot.
 */
uint32_t hal_timer_get_ms(void);

/**
 * @brief Get monotonic uptime in microseconds.
 * @return Microseconds elapsed since boot.
 */
uint64_t hal_timer_get_us(void);

/**
 * @brief Blocking delay in milliseconds.
 * @param ms Duration to wait.
 */
void hal_timer_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* HAL_TIMER_H */
