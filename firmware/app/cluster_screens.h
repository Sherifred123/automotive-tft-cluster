/**
 * @file cluster_screens.h
 * @brief Screen Layout & Display Rendering Procedures for Automotive Cluster.
 * 
 * Implements high-speed dirty-rectangle partial rendering for analog dials,
 * numeric readouts, and animated tell-tale status bars.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef CLUSTER_SCREENS_H
#define CLUSTER_SCREENS_H

#include <stdint.h>
#include <stdbool.h>
#include "cluster_types.h"
#include "../services/telltale_manager.h"
#include "../services/can_telemetry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Paint full static background, gauge dial arcs, and fixed labels.
 * @param theme Active color theme (THEME_NIGHT / THEME_DAY).
 */
void cluster_screens_draw_background(cluster_theme_t theme);

/**
 * @brief Partially update Speedometer analog needle and digital readout.
 * @param speed_kmh_x10 Vehicle speed in 0.1 km/h.
 * @param p_prev_angle Pointer to previous needle angle for delta erasure.
 * @param theme Active color theme.
 */
void cluster_screens_update_speedo(uint16_t speed_kmh_x10, int16_t *p_prev_angle, cluster_theme_t theme);

/**
 * @brief Partially update Battery SOC analog needle / arc and percentage readout.
 * @param soc_pct Battery State of Charge (0-100%).
 * @param p_prev_angle Pointer to previous needle angle.
 * @param theme Active color theme.
 */
void cluster_screens_update_battery(uint8_t soc_pct, int16_t *p_prev_angle, cluster_theme_t theme);

/**
 * @brief Partially update top header tell-tale indicators and turn signal blinkers.
 * @param p_telltales Active tell-tale states.
 * @param p_telem Active vehicle telemetry.
 * @param theme Active color theme.
 */
void cluster_screens_update_telltales(const telltale_status_t *p_telltales, const vehicle_telemetry_t *p_telem, cluster_theme_t theme);

/**
 * @brief Partially update bottom footer with Odometer, Trip distance, and Gear mode.
 * @param odo_km_x10 Lifetime odometer in 0.1 km.
 * @param trip_km_x10 Trip distance in 0.1 km.
 * @param gear Active vehicle gear.
 * @param theme Active color theme.
 */
void cluster_screens_update_footer(uint32_t odo_km_x10, uint32_t trip_km_x10, vehicle_gear_t gear, cluster_theme_t theme);

#ifdef __cplusplus
}
#endif

#endif /* CLUSTER_SCREENS_H */
