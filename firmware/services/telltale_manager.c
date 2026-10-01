/**
 * @file telltale_manager.c
 * @brief Implementation of Automotive Tell-Tale Manager.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "telltale_manager.h"
#include <string.h>

#define BLINK_HALF_PERIOD_MS  (333U) /* ~1.5 Hz blink rate */

static telltale_status_t s_telltale_status;

void telltale_manager_init(void)
{
    (void)memset(&s_telltale_status, 0, sizeof(s_telltale_status));
    s_telltale_status.theme = THEME_NIGHT;
    s_telltale_status.blink_phase_on = false;
}

void telltale_manager_update(const vehicle_telemetry_t *p_telemetry, uint32_t current_time_ms)
{
    if (p_telemetry == (void *)0) {
        return;
    }

    /* 1. Calculate 1.5 Hz blink phase */
    s_telltale_status.blink_phase_on = ((current_time_ms / BLINK_HALF_PERIOD_MS) % 2U) == 0U;

    /* 2. Turn Signals & Hazards */
    if (p_telemetry->flag_hazard) {
        s_telltale_status.turn_left = TELLTALE_STATE_BLINKING;
        s_telltale_status.turn_right = TELLTALE_STATE_BLINKING;
    } else {
        s_telltale_status.turn_left = p_telemetry->flag_turn_left ? TELLTALE_STATE_BLINKING : TELLTALE_STATE_OFF;
        s_telltale_status.turn_right = p_telemetry->flag_turn_right ? TELLTALE_STATE_BLINKING : TELLTALE_STATE_OFF;
    }

    /* 3. High Beam */
    s_telltale_status.high_beam = p_telemetry->flag_high_beam ? TELLTALE_STATE_ON : TELLTALE_STATE_OFF;

    /* 4. Battery Low Warning */
    if (p_telemetry->battery_soc_pct <= 8U) {
        s_telltale_status.battery_warning = TELLTALE_STATE_BLINKING;
    } else if (p_telemetry->battery_soc_pct <= 15U) {
        s_telltale_status.battery_warning = TELLTALE_STATE_ON;
    } else {
        s_telltale_status.battery_warning = TELLTALE_STATE_OFF;
    }

    /* 5. Motor Over-Temperature */
    s_telltale_status.motor_fault = p_telemetry->flag_motor_overtemp ? TELLTALE_STATE_ON : TELLTALE_STATE_OFF;

    /* 6. CAN Loss-of-Comm */
    s_telltale_status.can_comm_loss = (!p_telemetry->is_comm_active) ? TELLTALE_STATE_BLINKING : TELLTALE_STATE_OFF;

    /* 7. Brake Active */
    s_telltale_status.brake_active = p_telemetry->flag_brake_active ? TELLTALE_STATE_ON : TELLTALE_STATE_OFF;
}

void telltale_manager_get_status(telltale_status_t *p_out)
{
    if (p_out != (void *)0) {
        *p_out = s_telltale_status;
    }
}

void telltale_manager_toggle_theme(void)
{
    s_telltale_status.theme = (s_telltale_status.theme == THEME_NIGHT) ? THEME_DAY : THEME_NIGHT;
}
