/**
 * @file cluster_types.h
 * @brief Application Layer Types and Cluster Layout Geometry Constants.
 * 
 * Defines physical gauge centers, radii, dial sweep angles, and display layout boundaries.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef CLUSTER_TYPES_H
#define CLUSTER_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Gauge Center & Radius Geometries (320x240 Display) */
#define SPEEDO_CENTER_X         (80)
#define SPEEDO_CENTER_Y         (115)
#define SPEEDO_RADIUS           (52U)
#define SPEEDO_MIN_ANGLE_DEG    (135)
#define SPEEDO_MAX_ANGLE_DEG    (405) /* 45 deg */
#define SPEEDO_MAX_VAL_KMH      (140)

#define BATT_CENTER_X           (240)
#define BATT_CENTER_Y           (115)
#define BATT_RADIUS             (52U)
#define BATT_MIN_ANGLE_DEG      (135)
#define BATT_MAX_ANGLE_DEG      (405)
#define BATT_MAX_VAL_PCT        (100)

/* Drive Gear Mode */
typedef enum {
    GEAR_PARK = 0,
    GEAR_REVERSE,
    GEAR_NEUTRAL,
    GEAR_DRIVE,
    GEAR_SPORT
} vehicle_gear_t;

/* Cluster Runtime State */
typedef struct {
    uint16_t current_speed_kmh_x10;
    uint16_t prev_speed_kmh_x10;
    int16_t  prev_speed_angle_deg;

    uint8_t  current_soc_pct;
    uint8_t  prev_soc_pct;
    int16_t  prev_soc_angle_deg;

    uint32_t current_odo_km_x10;
    uint32_t prev_odo_km_x10;

    uint32_t current_trip_km_x10;
    uint32_t prev_trip_km_x10;

    vehicle_gear_t gear;
    bool needs_full_redraw;
} cluster_app_state_t;

#ifdef __cplusplus
}
#endif

#endif /* CLUSTER_TYPES_H */
