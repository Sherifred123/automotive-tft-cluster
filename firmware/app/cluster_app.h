/**
 * @file cluster_app.h
 * @brief High-Level Automotive Cluster Application Coordinator.
 * 
 * Manages the multi-rate execution cycle, telemetry ingestion, odometer synchronization,
 * and high-speed partial display rendering.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef CLUSTER_APP_H
#define CLUSTER_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "cluster_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize all cluster subsystems (HAL, Gfx, Telemetry, Odometer, Tell-Tales).
 */
void cluster_app_init(void);

/**
 * @brief Single non-blocking multi-rate execution step.
 * 
 * Rates:
 *   - 10 ms (100 Hz): CAN Telemetry & Hardware FIFO poll
 *   - 20 ms (50 Hz) : Odometer incremental distance update
 *   - 33 ms (30 Hz) : Analog Gauge Partial Display Render (Speed & Battery)
 *   - 50 ms (20 Hz) : Tell-tale & Blinker indicator update
 *   - 500 ms (2 Hz) : Footer Trip/Odo/Gear synchronization
 * 
 * @param current_time_ms Monotonic millisecond timestamp.
 */
void cluster_app_step(uint32_t current_time_ms);

/**
 * @brief Force full screen redraw (e.g. after theme toggle).
 */
void cluster_app_request_redraw(void);

/**
 * @brief Get copy of active cluster application state.
 */
void cluster_app_get_state(cluster_app_state_t *p_out);

#ifdef __cplusplus
}
#endif

#endif /* CLUSTER_APP_H */
