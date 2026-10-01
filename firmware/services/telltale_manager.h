/**
 * @file telltale_manager.h
 * @brief Automotive Indicator & Warning Light State Manager.
 * 
 * Manages tell-tale indicators, blinker cadences (1.5 Hz / 333 ms), warning priorities,
 * and Day/Night color theme transitions.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef TELLTALE_MANAGER_H
#define TELLTALE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "can_telemetry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TELLTALE_STATE_OFF = 0,
    TELLTALE_STATE_ON,
    TELLTALE_STATE_BLINKING
} telltale_state_t;

typedef enum {
    THEME_NIGHT = 0,
    THEME_DAY
} cluster_theme_t;

typedef struct {
    telltale_state_t turn_left;
    telltale_state_t turn_right;
    telltale_state_t high_beam;
    telltale_state_t battery_warning;
    telltale_state_t motor_fault;
    telltale_state_t can_comm_loss;
    telltale_state_t brake_active;
    
    bool blink_phase_on;        /**< 1.5 Hz periodic blinker state */
    cluster_theme_t theme;      /**< Current active color theme */
} telltale_status_t;

/**
 * @brief Initialize Tell-Tale Manager.
 */
void telltale_manager_init(void);

/**
 * @brief Periodic update (e.g. 50 ms rate) to compute blink phases and evaluate telemetry flags.
 * @param p_telemetry Pointer to active vehicle telemetry.
 * @param current_time_ms Monotonic timestamp in milliseconds.
 */
void telltale_manager_update(const vehicle_telemetry_t *p_telemetry, uint32_t current_time_ms);

/**
 * @brief Get active tell-tale states and blinker phase.
 */
void telltale_manager_get_status(telltale_status_t *p_out);

/**
 * @brief Toggle between Day and Night color themes.
 */
void telltale_manager_toggle_theme(void);

#ifdef __cplusplus
}
#endif

#endif /* TELLTALE_MANAGER_H */
