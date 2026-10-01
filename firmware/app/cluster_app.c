/**
 * @file cluster_app.c
 * @brief Implementation of Automotive Cluster Application Coordinator.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "cluster_app.h"
#include "cluster_screens.h"
#include "../core_gfx/gfx_engine.h"
#include "../services/can_telemetry.h"
#include "../services/odometer_service.h"
#include "../services/telltale_manager.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_gpio.h"
#include <string.h>

static cluster_app_state_t s_app_state;

/* Scheduling Timestamps */
static uint32_t s_last_10ms_tick;
static uint32_t s_last_20ms_tick;
static uint32_t s_last_33ms_tick;
static uint32_t s_last_50ms_tick;
static uint32_t s_last_500ms_tick;

void cluster_app_init(void)
{
    (void)memset(&s_app_state, 0, sizeof(s_app_state));
    s_app_state.prev_speed_angle_deg = -1;
    s_app_state.prev_soc_angle_deg = -1;
    s_app_state.gear = GEAR_DRIVE;
    s_app_state.needs_full_redraw = true;

    /* Initialize low-level peripherals & services */
    hal_timer_init();
    hal_gpio_init();
    gfx_engine_init();
    can_telemetry_init();
    (void)odometer_service_init();
    telltale_manager_init();

    s_last_10ms_tick = hal_timer_get_ms();
    s_last_20ms_tick = s_last_10ms_tick;
    s_last_33ms_tick = s_last_10ms_tick;
    s_last_50ms_tick = s_last_10ms_tick;
    s_last_500ms_tick = s_last_10ms_tick;
}

void cluster_app_request_redraw(void)
{
    s_app_state.needs_full_redraw = true;
}

void cluster_app_step(uint32_t current_time_ms)
{
    telltale_status_t telltales;
    telltale_manager_get_status(&telltales);
    vehicle_telemetry_t telem;
    can_telemetry_get_data(&telem);

    /* 0. Full Redraw on Startup or Theme Change */
    if (s_app_state.needs_full_redraw) {
        cluster_screens_draw_background(telltales.theme);
        s_app_state.prev_speed_angle_deg = -1;
        s_app_state.prev_soc_angle_deg = -1;
        s_app_state.needs_full_redraw = false;
    }

    /* 1. 10 ms Task: CAN Telemetry Ingestion & Timeout Watchdog */
    if ((current_time_ms - s_last_10ms_tick) >= 10U) {
        can_telemetry_poll(current_time_ms);
        can_telemetry_get_data(&telem);
        s_last_10ms_tick = current_time_ms;
    }

    /* 2. 20 ms Task: Odometer Accumulation */
    if ((current_time_ms - s_last_20ms_tick) >= 20U) {
        odometer_service_update(telem.speed_kmh_x10, current_time_ms - s_last_20ms_tick);
        s_last_20ms_tick = current_time_ms;
    }

    /* 3. 33 ms Task (30 Hz): Gauge Needle Partial Display Render */
    if ((current_time_ms - s_last_33ms_tick) >= 33U) {
        cluster_screens_update_speedo(telem.speed_kmh_x10, &s_app_state.prev_speed_angle_deg, telltales.theme);
        cluster_screens_update_battery(telem.battery_soc_pct, &s_app_state.prev_soc_angle_deg, telltales.theme);
        s_last_33ms_tick = current_time_ms;
    }

    /* 4. 50 ms Task (20 Hz): Tell-tales & Blinker Status */
    if ((current_time_ms - s_last_50ms_tick) >= 50U) {
        telltale_manager_update(&telem, current_time_ms);
        telltale_manager_get_status(&telltales);
        cluster_screens_update_telltales(&telltales, &telem, telltales.theme);
        s_last_50ms_tick = current_time_ms;
    }

    /* 5. 500 ms Task (2 Hz): Footer Trip / Odo / Gear Sync */
    if ((current_time_ms - s_last_500ms_tick) >= 500U) {
        uint32_t odo = odometer_service_get_total_km_x10();
        uint32_t trip = odometer_service_get_trip_km_x10();
        cluster_screens_update_footer(odo, trip, s_app_state.gear, telltales.theme);
        s_last_500ms_tick = current_time_ms;
    }
}

void cluster_app_get_state(cluster_app_state_t *p_out)
{
    if (p_out != (void *)0) {
        *p_out = s_app_state;
    }
}
