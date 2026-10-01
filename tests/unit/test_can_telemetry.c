/**
 * @file test_can_telemetry.c
 * @brief Unit Tests for CAN Telemetry Decoding & Loss-of-Comm Watchdog.
 */

#include "../unity/unity.h"
#include "../../firmware/services/can_telemetry.h"

void test_can_telemetry_decode_speed(void)
{
    can_telemetry_init();

    /* Inject 85.0 km/h (850 x 0.1) and 4500 RPM */
    can_telemetry_inject_speed(850, 4500);

    vehicle_telemetry_t data;
    can_telemetry_get_data(&data);

    TEST_ASSERT_TRUE(data.is_comm_active);
    TEST_ASSERT_EQUAL_UINT16(850, data.speed_kmh_x10);
    TEST_ASSERT_EQUAL_UINT16(4500, data.motor_rpm);
}

void test_can_telemetry_decode_battery(void)
{
    can_telemetry_init();

    /* Inject 78% SOC and 51.2V (512 x 0.1) */
    can_telemetry_inject_battery(78, 512);

    vehicle_telemetry_t data;
    can_telemetry_get_data(&data);

    TEST_ASSERT_EQUAL_UINT(78, data.battery_soc_pct);
    TEST_ASSERT_EQUAL_UINT16(512, data.battery_voltage_x10);
    TEST_ASSERT_FALSE(data.flag_battery_low);

    /* Test Low Battery Threshold (<= 15%) */
    can_telemetry_inject_battery(12, 460);
    can_telemetry_get_data(&data);
    TEST_ASSERT_TRUE(data.flag_battery_low);
}

void test_can_telemetry_decode_telltales(void)
{
    can_telemetry_init();

    /* Inject Left Turn ON, High Beam ON */
    can_telemetry_inject_telltales(true, false, true, false, false);

    vehicle_telemetry_t data;
    can_telemetry_get_data(&data);

    TEST_ASSERT_TRUE(data.flag_turn_left);
    TEST_ASSERT_FALSE(data.flag_turn_right);
    TEST_ASSERT_TRUE(data.flag_high_beam);
}

void test_can_telemetry_loss_of_comm_watchdog(void)
{
    can_telemetry_init();
    can_telemetry_inject_speed(800, 4000);

    vehicle_telemetry_t data;
    can_telemetry_get_data(&data);
    TEST_ASSERT_TRUE(data.is_comm_active);

    /* Poll at 100ms (within 500ms timeout) */
    can_telemetry_poll(100);
    can_telemetry_get_data(&data);
    TEST_ASSERT_TRUE(data.is_comm_active);

    /* Poll at 600ms (exceeds 500ms timeout) */
    can_telemetry_poll(700);
    can_telemetry_get_data(&data);
    TEST_ASSERT_FALSE(data.is_comm_active);

    /* Verify needle glide decay begins */
    TEST_ASSERT_TRUE(data.speed_kmh_x10 < 800);
}
