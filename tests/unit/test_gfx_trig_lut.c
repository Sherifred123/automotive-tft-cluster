/**
 * @file test_gfx_trig_lut.c
 * @brief Unit Tests for Fixed-Point Q15 Trigonometry Engine.
 */

#include "../unity/unity.h"
#include "../../firmware/core_gfx/gfx_trig_lut.h"

void setUp(void) {}
void tearDown(void) {}

void test_trig_quadrant_cardinal_angles(void)
{
    /* sin(0) = 0, cos(0) = 32767 */
    TEST_ASSERT_INT_WITHIN(10, 0, gfx_trig_sin_q15(0));
    TEST_ASSERT_INT_WITHIN(10, 32767, gfx_trig_cos_q15(0));

    /* sin(90) = 32767, cos(90) = 0 */
    TEST_ASSERT_INT_WITHIN(10, 32767, gfx_trig_sin_q15(90));
    TEST_ASSERT_INT_WITHIN(10, 0, gfx_trig_cos_q15(90));

    /* sin(180) = 0, cos(180) = -32767 */
    TEST_ASSERT_INT_WITHIN(10, 0, gfx_trig_sin_q15(180));
    TEST_ASSERT_INT_WITHIN(10, -32767, gfx_trig_cos_q15(180));

    /* sin(270) = -32767, cos(270) = 0 */
    TEST_ASSERT_INT_WITHIN(10, -32767, gfx_trig_sin_q15(270));
    TEST_ASSERT_INT_WITHIN(10, 0, gfx_trig_cos_q15(270));
}

void test_trig_intermediate_angles(void)
{
    /* sin(30) = 0.5 * 32767 = 16384 */
    TEST_ASSERT_INT_WITHIN(50, 16384, gfx_trig_sin_q15(30));

    /* sin(45) = 0.7071 * 32767 = 23170 */
    TEST_ASSERT_INT_WITHIN(50, 23170, gfx_trig_sin_q15(45));
    TEST_ASSERT_INT_WITHIN(50, 23170, gfx_trig_cos_q15(45));

    /* Angle wrapping: 450 deg = 90 deg */
    TEST_ASSERT_INT_WITHIN(10, 32767, gfx_trig_sin_q15(450));
    /* Negative angle: -90 deg = 270 deg */
    TEST_ASSERT_INT_WITHIN(10, -32767, gfx_trig_sin_q15(-90));
}

void test_trig_calc_endpoint(void)
{
    gfx_point_t pt;
    /* Center (100, 100), Radius 50, Angle 0 deg (pointing East) -> (150, 100) */
    gfx_trig_calc_endpoint(100, 100, 50, 0, &pt);
    TEST_ASSERT_EQUAL_INT16(150, pt.x);
    TEST_ASSERT_EQUAL_INT16(100, pt.y);

    /* Angle 90 deg (pointing South in screen coords) -> (100, 150) */
    gfx_trig_calc_endpoint(100, 100, 50, 90, &pt);
    TEST_ASSERT_EQUAL_INT16(100, pt.x);
    TEST_ASSERT_EQUAL_INT16(150, pt.y);

    /* Angle 180 deg (pointing West) -> (50, 100) */
    gfx_trig_calc_endpoint(100, 100, 50, 180, &pt);
    TEST_ASSERT_EQUAL_INT16(50, pt.x);
    TEST_ASSERT_EQUAL_INT16(100, pt.y);
}

void test_trig_map_to_angle(void)
{
    /* Map Speed 0-140 km/h to 135 deg - 405 deg */
    int16_t a_min = gfx_trig_map_to_angle(0, 0, 140, 135, 405);
    TEST_ASSERT_EQUAL_INT16(135, a_min);

    int16_t a_mid = gfx_trig_map_to_angle(70, 0, 140, 135, 405);
    TEST_ASSERT_EQUAL_INT16(270, a_mid);

    int16_t a_max = gfx_trig_map_to_angle(140, 0, 140, 135, 405);
    TEST_ASSERT_EQUAL_INT16(45, a_max); /* 405 normalized is 45 */
}
