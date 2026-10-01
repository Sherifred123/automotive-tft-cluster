/**
 * @file odometer_service.h
 * @brief Automotive Odometer Wear-Leveling and Anti-Tearing NVM Service.
 * 
 * Features a 16-slot round-robin wear-leveling ring buffer in EEPROM/Flash
 * with CRC-16-CCITT integrity verification and sudden power-loss recovery.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef ODOMETER_SERVICE_H
#define ODOMETER_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "../hal/hal_nvm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ODO_NUM_SLOTS           (16U)
#define ODO_SLOT_SIZE_BYTES     (16U)
#define ODO_NVM_BASE_OFFSET     (0U)

/* Odometer Slot Memory Structure (16 bytes) */
typedef struct {
    uint32_t total_odometer_meters; /**< Total lifetime distance in meters */
    uint32_t trip_distance_meters;  /**< Current trip distance in meters */
    uint32_t sequence_id;           /**< Monotonically increasing write sequence */
    uint16_t crc16;                 /**< CRC-16-CCITT of the preceding 12 bytes */
    uint16_t padding;               /**< 16-byte alignment padding */
} odo_slot_t;

/**
 * @brief Initialize Odometer service, scan all NVM slots, and recover latest valid record.
 * @return true if valid previous odometer record recovered, false if initialized fresh.
 */
bool odometer_service_init(void);

/**
 * @brief Accumulate incremental distance based on vehicle speed and time slice.
 * @param speed_kmh_x10 Current vehicle speed in 0.1 km/h.
 * @param delta_time_ms Time elapsed in milliseconds.
 */
void odometer_service_update(uint16_t speed_kmh_x10, uint32_t delta_time_ms);

/**
 * @brief Periodic NVM sync (e.g. executed every 1000m or on ignition power down).
 * Rotates to the next wear-leveling slot and writes with fresh CRC.
 * @param force_sync If true, forces write even if distance delta threshold not met.
 * @return true if write succeeded.
 */
bool odometer_service_sync_nvm(bool force_sync);

/**
 * @brief Reset Trip Distance to 0.0 km.
 */
void odometer_service_reset_trip(void);

/**
 * @brief Get total lifetime odometer reading in 0.1 km units (e.g. 148205 = 14820.5 km).
 */
uint32_t odometer_service_get_total_km_x10(void);

/**
 * @brief Get current trip odometer reading in 0.1 km units (e.g. 1428 = 142.8 km).
 */
uint32_t odometer_service_get_trip_km_x10(void);

/**
 * @brief Get active wear-leveling slot index (0 to 15) for diagnostic inspection.
 */
uint8_t odometer_service_get_active_slot(void);

/**
 * @brief Calculate CRC-16-CCITT checksum over a data buffer (Polynomial 0x1021).
 */
uint16_t odometer_calc_crc16(const uint8_t *p_data, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* ODOMETER_SERVICE_H */
