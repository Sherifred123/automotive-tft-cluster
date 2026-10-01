/**
 * @file test_gfx_engine.c
 * @brief Unit Tests for Graphics Engine & Dirty Rectangle Math.
 */

#include "../unity/unity.h"
#include "../../firmware/core_gfx/gfx_engine.h"
#include "../../firmware/core_gfx/gfx_dirty_rect.h"
#include "../../firmware/port/host_sim/port_host_sim.h"

void test_gfx_dirty_rect_bounding_box(void)
{
    gfx_rect_t rect;
    gfx_dirty_rect_reset(&rect);
    TEST_ASSERT_FALSE(gfx_dirty_rect_is_valid(&rect));

    gfx_dirty_rect_add_point(&rect, 10, 20);
    gfx_dirty_rect_add_point(&rect, 50, 80);

    TEST_ASSERT_TRUE(gfx_dirty_rect_is_valid(&rect));
    TEST_ASSERT_EQUAL_INT16(10, rect.x0);
    TEST_ASSERT_EQUAL_INT16(20, rect.y0);
    TEST_ASSERT_EQUAL_INT16(50, rect.x1);
    TEST_ASSERT_EQUAL_INT16(80, rect.y1);
}

void test_gfx_dirty_rect_clamping(void)
{
    gfx_rect_t rect;
    rect.x0 = -20;
    rect.y0 = -10;
    rect.x1 = 400;
    rect.y1 = 300;

    TEST_ASSERT_TRUE(gfx_dirty_rect_clamp(&rect, DISPLAY_WIDTH, DISPLAY_HEIGHT));
    TEST_ASSERT_EQUAL_INT16(0, rect.x0);
    TEST_ASSERT_EQUAL_INT16(0, rect.y0);
    TEST_ASSERT_EQUAL_INT16(DISPLAY_WIDTH - 1, rect.x1);
    TEST_ASSERT_EQUAL_INT16(DISPLAY_HEIGHT - 1, rect.y1);
}

void test_gfx_draw_pixel_and_fill(void)
{
    gfx_engine_init();

    /* Clear screen to black */
    gfx_fill_rect(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, COLOR_BLACK);
    TEST_ASSERT_EQUAL_HEX16(COLOR_BLACK, port_host_display_get_pixel(100, 100));

    /* Draw white pixel at (100, 100) */
    gfx_draw_pixel(100, 100, COLOR_WHITE);
    TEST_ASSERT_EQUAL_HEX16(COLOR_WHITE, port_host_display_get_pixel(100, 100));
}

void test_gfx_string_and_number_rendering(void)
{
    gfx_engine_init();

    uint16_t w1 = gfx_draw_string(10, 10, "SPEED", &g_font_5x7, COLOR_WHITE, COLOR_BLACK);
    TEST_ASSERT_EQUAL_UINT16(30, w1); /* 5 chars * 6 px = 30 px */

    uint16_t w2 = gfx_draw_number(10, 20, 75, 0, &g_font_5x7, COLOR_WHITE, COLOR_BLACK);
    TEST_ASSERT_EQUAL_UINT16(12, w2); /* 2 digits * 6 px = 12 px */

    uint16_t w3 = gfx_draw_number(10, 30, 75, 4, &g_font_5x7, COLOR_WHITE, COLOR_BLACK);
    TEST_ASSERT_EQUAL_UINT16(24, w3); /* "0075" = 4 digits * 6 px = 24 px */
}
