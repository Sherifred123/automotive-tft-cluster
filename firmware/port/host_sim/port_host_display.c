/**
 * @file port_host_display.c
 * @brief Host PC Virtual LCD Display Driver for Simulation and Automated Testing.
 * 
 * Simulates ST7789 / ILI9341 display controller registers and GRAM in host memory.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "../../hal/hal_display.h"
#include <string.h>

/* Host 320x240 Virtual GRAM */
static uint16_t s_virtual_gram[DISPLAY_HEIGHT][DISPLAY_WIDTH];
static uint16_t s_win_x0 = 0;
static uint16_t s_win_y0 = 0;
static uint16_t s_win_x1 = DISPLAY_WIDTH - 1;
static uint16_t s_win_y1 = DISPLAY_HEIGHT - 1;
static uint16_t s_cur_x = 0;
static uint16_t s_cur_y = 0;
static uint8_t  s_backlight_pct = 100;

hal_display_status_t hal_display_init(void)
{
    (void)memset(s_virtual_gram, 0, sizeof(s_virtual_gram));
    s_win_x0 = 0;
    s_win_y0 = 0;
    s_win_x1 = DISPLAY_WIDTH - 1;
    s_win_y1 = DISPLAY_HEIGHT - 1;
    s_cur_x = 0;
    s_cur_y = 0;
    s_backlight_pct = 100;
    return HAL_DISPLAY_OK;
}

hal_display_status_t hal_display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    if ((x0 >= DISPLAY_WIDTH) || (x1 >= DISPLAY_WIDTH) || (y0 >= DISPLAY_HEIGHT) || (y1 >= DISPLAY_HEIGHT) ||
        (x0 > x1) || (y0 > y1)) {
        return HAL_DISPLAY_ERR_PARAM;
    }
    s_win_x0 = x0;
    s_win_y0 = y0;
    s_win_x1 = x1;
    s_win_y1 = y1;
    s_cur_x = x0;
    s_cur_y = y0;
    return HAL_DISPLAY_OK;
}

hal_display_status_t hal_display_stream_pixels(const uint16_t *p_pixels, uint32_t count)
{
    if (p_pixels == (void *)0) {
        return HAL_DISPLAY_ERR_PARAM;
    }

    for (uint32_t i = 0; i < count; i++) {
        if ((s_cur_y < DISPLAY_HEIGHT) && (s_cur_x < DISPLAY_WIDTH)) {
            s_virtual_gram[s_cur_y][s_cur_x] = p_pixels[i];
        }

        s_cur_x++;
        if (s_cur_x > s_win_x1) {
            s_cur_x = s_win_x0;
            s_cur_y++;
            if (s_cur_y > s_win_y1) {
                s_cur_y = s_win_y0;
            }
        }
    }
    return HAL_DISPLAY_OK;
}

hal_display_status_t hal_display_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color16)
{
    if ((x0 >= DISPLAY_WIDTH) || (x1 >= DISPLAY_WIDTH) || (y0 >= DISPLAY_HEIGHT) || (y1 >= DISPLAY_HEIGHT) ||
        (x0 > x1) || (y0 > y1)) {
        return HAL_DISPLAY_ERR_PARAM;
    }

    for (uint16_t y = y0; y <= y1; y++) {
        for (uint16_t x = x0; x <= x1; x++) {
            s_virtual_gram[y][x] = color16;
        }
    }
    return HAL_DISPLAY_OK;
}

void hal_display_set_backlight(uint8_t brightness_pct)
{
    s_backlight_pct = (brightness_pct > 100U) ? 100U : brightness_pct;
}

bool hal_display_is_busy(void)
{
    return false;
}

/* Host Inspector Helper */
uint16_t port_host_display_get_pixel(uint16_t x, uint16_t y)
{
    if ((x < DISPLAY_WIDTH) && (y < DISPLAY_HEIGHT)) {
        return s_virtual_gram[y][x];
    }
    return 0;
}
