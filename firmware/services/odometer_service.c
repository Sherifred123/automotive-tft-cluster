/**
 * @file odometer_service.c
 * @brief Implementation of Automotive Odometer Wear-Leveling Service.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "odometer_service.h"
#include <string.h>

#define ODO_SYNC_INTERVAL_METERS  (100U) /* Write to NVM every 100 meters */

static odo_slot_t s_current_record;
static uint8_t    s_active_slot_idx;
static uint64_t   s_sub_meter_accumulator;
static uint32_t   s_uncommitted_meters;

uint16_t odometer_calc_crc16(const uint8_t *p_data, size_t length)
{
    uint16_t crc = 0xFFFFU;
    if (p_data == (void *)0) {
        return 0;
    }

    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)((uint16_t)p_data[i] << 8U);
        for (uint8_t bit = 0; bit < 8U; bit++) {
            if ((crc & 0x8000U) != 0U) {
                crc = (uint16_t)((crc << 1U) ^ 0x1021U);
            } else {
                crc = (uint16_t)(crc << 1U);
            }
        }
    }
    return crc;
}

bool odometer_service_init(void)
{
    (void)hal_nvm_init();

    s_sub_meter_accumulator = 0;
    s_uncommitted_meters = 0;
    s_active_slot_idx = 0;

    bool found_valid = false;
    uint32_t highest_seq = 0;
    odo_slot_t best_slot;
    (void)memset(&best_slot, 0, sizeof(best_slot));

    /* Scan all 16 slots across NVM ring partition */
    for (uint8_t slot = 0; slot < ODO_NUM_SLOTS; slot++) {
        odo_slot_t candidate;
        uint16_t offset = (uint16_t)(ODO_NVM_BASE_OFFSET + ((uint16_t)slot * ODO_SLOT_SIZE_BYTES));

        if (hal_nvm_read(offset, &candidate, sizeof(candidate)) == HAL_NVM_OK) {
            /* Verify CRC over first 12 bytes (total_meters, trip_meters, sequence_id) */
            uint16_t calculated_crc = odometer_calc_crc16((const uint8_t *)&candidate, 12);
            if ((calculated_crc == candidate.crc16) && (candidate.sequence_id > 0U)) {
                if (!found_valid || (candidate.sequence_id > highest_seq)) {
                    highest_seq = candidate.sequence_id;
                    s_active_slot_idx = slot;
                    best_slot = candidate;
                    found_valid = true;
                }
            }
        }
    }

    if (found_valid) {
        s_current_record = best_slot;
    } else {
        /* Initialize fresh slot at index 0 */
        s_current_record.total_odometer_meters = 0;
        s_current_record.trip_distance_meters = 0;
        s_current_record.sequence_id = 1;
        s_current_record.padding = 0;
        s_current_record.crc16 = odometer_calc_crc16((const uint8_t *)&s_current_record, 12);
        s_active_slot_idx = 0;
        (void)hal_nvm_write(ODO_NVM_BASE_OFFSET, &s_current_record, sizeof(s_current_record));
    }

    return found_valid;
}

void odometer_service_update(uint16_t speed_kmh_x10, uint32_t delta_time_ms)
{
    /* Distance delta: speed (0.1 km/h) * time (ms) / 36000 = meters */
    s_sub_meter_accumulator += ((uint64_t)speed_kmh_x10 * (uint64_t)delta_time_ms);

    while (s_sub_meter_accumulator >= 36000ULL) {
        s_sub_meter_accumulator -= 36000ULL;
        s_current_record.total_odometer_meters++;
        s_current_record.trip_distance_meters++;
        s_uncommitted_meters++;
    }

    if (s_uncommitted_meters >= ODO_SYNC_INTERVAL_METERS) {
        (void)odometer_service_sync_nvm(false);
    }
}

bool odometer_service_sync_nvm(bool force_sync)
{
    if (!force_sync && (s_uncommitted_meters < ODO_SYNC_INTERVAL_METERS)) {
        return false;
    }

    /* Advance to next wear-leveling slot in round-robin ring */
    uint8_t next_slot = (uint8_t)((s_active_slot_idx + 1U) % ODO_NUM_SLOTS);
    s_current_record.sequence_id++;
    s_current_record.padding = 0;
    s_current_record.crc16 = odometer_calc_crc16((const uint8_t *)&s_current_record, 12);

    uint16_t offset = (uint16_t)(ODO_NVM_BASE_OFFSET + ((uint16_t)next_slot * ODO_SLOT_SIZE_BYTES));
    hal_nvm_status_t status = hal_nvm_write(offset, &s_current_record, sizeof(s_current_record));

    if (status == HAL_NVM_OK) {
        s_active_slot_idx = next_slot;
        s_uncommitted_meters = 0;
        return true;
    }

    return false;
}

void odometer_service_reset_trip(void)
{
    s_current_record.trip_distance_meters = 0;
    (void)odometer_service_sync_nvm(true);
}

uint32_t odometer_service_get_total_km_x10(void)
{
    /* 1 km = 1000 meters -> 0.1 km = 100 meters */
    return s_current_record.total_odometer_meters / 100U;
}

uint32_t odometer_service_get_trip_km_x10(void)
{
    return s_current_record.trip_distance_meters / 100U;
}

uint8_t odometer_service_get_active_slot(void)
{
    return s_active_slot_idx;
}
