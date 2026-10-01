/**
 * @file test_runner.c
 * @brief Master Test Runner for Automotive TFT Cluster Test Suite.
 */

#include "unity/unity.h"

/* Forward Declarations */
void test_trig_quadrant_cardinal_angles(void);
void test_trig_intermediate_angles(void);
void test_trig_calc_endpoint(void);
void test_trig_map_to_angle(void);

void test_gfx_dirty_rect_bounding_box(void);
void test_gfx_dirty_rect_clamping(void);
void test_gfx_draw_pixel_and_fill(void);
void test_gfx_string_and_number_rendering(void);

void test_can_telemetry_decode_speed(void);
void test_can_telemetry_decode_battery(void);
void test_can_telemetry_decode_telltales(void);
void test_can_telemetry_loss_of_comm_watchdog(void);

void test_odometer_crc_calculation(void);
void test_odometer_initialization_fresh(void);
void test_odometer_accumulation_and_sync(void);
void test_odometer_power_loss_recovery(void);

void test_telltale_blinker_cadence(void);
void test_telltale_theme_toggle(void);

void test_cluster_full_cycle_step(void);

int main(void)
{
    UnityBegin("Automotive TFT Cluster Test Suite");

    /* 1. Trigonometry & Math Tests */
    RUN_TEST(test_trig_quadrant_cardinal_angles);
    RUN_TEST(test_trig_intermediate_angles);
    RUN_TEST(test_trig_calc_endpoint);
    RUN_TEST(test_trig_map_to_angle);

    /* 2. Graphics Engine & Dirty Rect Tests */
    RUN_TEST(test_gfx_dirty_rect_bounding_box);
    RUN_TEST(test_gfx_dirty_rect_clamping);
    RUN_TEST(test_gfx_draw_pixel_and_fill);
    RUN_TEST(test_gfx_string_and_number_rendering);

    /* 3. CAN Telemetry & Watchdog Tests */
    RUN_TEST(test_can_telemetry_decode_speed);
    RUN_TEST(test_can_telemetry_decode_battery);
    RUN_TEST(test_can_telemetry_decode_telltales);
    RUN_TEST(test_can_telemetry_loss_of_comm_watchdog);

    /* 4. Odometer Wear-Leveling & NVM Tests */
    RUN_TEST(test_odometer_crc_calculation);
    RUN_TEST(test_odometer_initialization_fresh);
    RUN_TEST(test_odometer_accumulation_and_sync);
    RUN_TEST(test_odometer_power_loss_recovery);

    /* 5. Tell-Tale Manager Tests */
    RUN_TEST(test_telltale_blinker_cadence);
    RUN_TEST(test_telltale_theme_toggle);

    /* 6. System Integration Test */
    RUN_TEST(test_cluster_full_cycle_step);

    return UnityEnd();
}
