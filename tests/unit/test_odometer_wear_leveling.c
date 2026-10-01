/**
 * @file test_odometer_wear_leveling.c
 * @brief Unit Tests for Odometer Multi-Slot Wear-Leveling & CRC Integrity.
 */

#include "../unity/unity.h"
#include "../../firmware/services/odometer_service.h"
#include "../../firmware/port/host_sim/port_host_sim.h"

void test_odometer_crc_calculation(void)
{
    uint8_t sample_data[12] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C };
    uint16_t crc = odometer_calc_crc16(sample_data, 12);
    TEST_ASSERT_TRUE(crc != 0);
    TEST_ASSERT_TRUE(crc != 0xFFFF);
}

void test_odometer_initialization_fresh(void)
{
    port_host_nvm_reset();
    bool recovered = odometer_service_init();

    TEST_ASSERT_FALSE(recovered); /* Fresh init */
    TEST_ASSERT_EQUAL_UINT32(0, odometer_service_get_total_km_x10());
    TEST_ASSERT_EQUAL_UINT32(0, odometer_service_get_trip_km_x10());
    TEST_ASSERT_EQUAL_UINT(0, odometer_service_get_active_slot());
}

void test_odometer_accumulation_and_sync(void)
{
    port_host_nvm_reset();
    (void)odometer_service_init();

    /* Simulate driving at 72.0 km/h for 10 seconds = 200 meters */
    /* speed = 720 (0.1 km/h), time = 10,000 ms */
    odometer_service_update(720, 10000);

    /* 200 meters = 2 x 0.1 km = 0.2 km */
    TEST_ASSERT_EQUAL_UINT32(2, odometer_service_get_total_km_x10());
    TEST_ASSERT_EQUAL_UINT32(2, odometer_service_get_trip_km_x10());

    /* Verify wear leveling rotated to slot 1 after 200m */
    TEST_ASSERT_EQUAL_UINT(1, odometer_service_get_active_slot());
}

void test_odometer_power_loss_recovery(void)
{
    port_host_nvm_reset();
    (void)odometer_service_init();

    /* Drive 300 meters */
    odometer_service_update(720, 15000);
    uint32_t saved_odo = odometer_service_get_total_km_x10();
    TEST_ASSERT_EQUAL_UINT32(3, saved_odo);

    /* Simulate power reboot: Re-initialize from NVM */
    bool recovered = odometer_service_init();
    TEST_ASSERT_TRUE(recovered);
    TEST_ASSERT_EQUAL_UINT32(saved_odo, odometer_service_get_total_km_x10());
}
