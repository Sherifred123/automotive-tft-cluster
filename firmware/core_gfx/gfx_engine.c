/**
 * @file gfx_engine.c
 * @brief Implementation of High-Speed 2D Graphics Engine.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "gfx_engine.h"
#include "../hal/hal_display.h"
#include <string.h>

void gfx_engine_init(void)
{
    (void)hal_display_init();
}

void gfx_draw_pixel(int16_t x, int16_t y, uint16_t color16)
{
    if ((x < 0) || (x >= (int16_t)DISPLAY_WIDTH) || (y < 0) || (y >= (int16_t)DISPLAY_HEIGHT)) {
        return;
    }
    (void)hal_display_set_window((uint16_t)x, (uint16_t)y, (uint16_t)x, (uint16_t)y);
    (void)hal_display_stream_pixels(&color16, 1);
}

void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color16)
{
    /* Integer Bresenham's Line Algorithm */
    int16_t dx = (x1 >= x0) ? (x1 - x0) : (x0 - x1);
    int16_t dy = (y1 >= y0) ? (y1 - y0) : (y0 - y1);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    for (;;) {
        gfx_draw_pixel(x0, y0, color16);
        if ((x0 == x1) && (y0 == y1)) {
            break;
        }
        int16_t e2 = (int16_t)(err << 1);
        if (e2 > -dy) {
            err = (int16_t)(err - dy);
            x0 = (int16_t)(x0 + sx);
        }
        if (e2 < dx) {
            err = (int16_t)(err + dx);
            y0 = (int16_t)(y0 + sy);
        }
    }
}

void gfx_draw_thick_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t thickness, uint16_t color16)
{
    if (thickness <= 1) {
        gfx_draw_line(x0, y0, x1, y1, color16);
        return;
    }

    int16_t half = (int16_t)(thickness >> 1);
    for (int16_t i = -half; i <= half; i++) {
        gfx_draw_line((int16_t)(x0 + i), y0, (int16_t)(x1 + i), y1, color16);
        gfx_draw_line(x0, (int16_t)(y0 + i), x1, (int16_t)(y1 + i), color16);
    }
}

void gfx_draw_needle(int16_t xc, int16_t yc, uint16_t radius, int16_t angle_deg, uint8_t thickness, uint16_t color16, gfx_rect_t *p_dirty_out)
{
    gfx_point_t tip;
    gfx_trig_calc_endpoint(xc, yc, radius, angle_deg, &tip);

    /* Draw thick needle line */
    gfx_draw_thick_line(xc, yc, tip.x, tip.y, thickness, color16);

    /* Draw center pivot circle cap (3x3 pixels) */
    gfx_fill_rect((int16_t)(xc - 2), (int16_t)(yc - 2), (int16_t)(xc + 2), (int16_t)(yc + 2), COLOR_WHITE);

    /* Record dirty bounding box if requested */
    if (p_dirty_out != (void *)0) {
        gfx_dirty_rect_reset(p_dirty_out);
        gfx_dirty_rect_add_line(p_dirty_out, xc, yc, tip.x, tip.y, (uint8_t)(thickness + 4));
        (void)gfx_dirty_rect_clamp(p_dirty_out, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    }
}

void gfx_draw_arc_bar(int16_t xc, int16_t yc, uint16_t radius, uint8_t thickness, int16_t start_angle, int16_t end_angle, uint16_t color16)
{
    if (radius < thickness) {
        return;
    }

    /* Normalize sweep direction */
    int16_t step = (start_angle <= end_angle) ? 2 : -2;
    int16_t curr = start_angle;

    while (1) {
        for (uint8_t r = 0; r < thickness; r++) {
            gfx_point_t pt;
            gfx_trig_calc_endpoint(xc, yc, (uint16_t)(radius - r), curr, &pt);
            gfx_draw_pixel(pt.x, pt.y, color16);
        }

        if (curr == end_angle) {
            break;
        }
        if (((step > 0) && ((curr + step) > end_angle)) ||
            ((step < 0) && ((curr + step) < end_angle))) {
            curr = end_angle;
        } else {
            curr = (int16_t)(curr + step);
        }
    }
}

void gfx_draw_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color16)
{
    gfx_draw_line(x0, y0, x1, y0, color16);
    gfx_draw_line(x1, y0, x1, y1, color16);
    gfx_draw_line(x1, y1, x0, y1, color16);
    gfx_draw_line(x0, y1, x0, y0, color16);
}

void gfx_fill_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color16)
{
    if (x0 > x1) {
        int16_t t = x0; x0 = x1; x1 = t;
    }
    if (y0 > y1) {
        int16_t t = y0; y0 = y1; y1 = t;
    }
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= (int16_t)DISPLAY_WIDTH) x1 = (int16_t)(DISPLAY_WIDTH - 1);
    if (y1 >= (int16_t)DISPLAY_HEIGHT) y1 = (int16_t)(DISPLAY_HEIGHT - 1);

    if ((x0 <= x1) && (y0 <= y1)) {
        (void)hal_display_fill_rect((uint16_t)x0, (uint16_t)y0, (uint16_t)x1, (uint16_t)y1, color16);
    }
}

uint16_t gfx_draw_char(int16_t x, int16_t y, char c, const gfx_font_t *p_font, uint16_t fg_color, uint16_t bg_color)
{
    if ((p_font == (void *)0) || (p_font->p_bitmap == (void *)0)) {
        return 0;
    }

    if (((uint8_t)c < p_font->first_char) || ((uint8_t)c > p_font->last_char)) {
        c = ' ';
    }

    uint16_t char_index = (uint16_t)((uint8_t)c - p_font->first_char);
    uint16_t bytes_per_glyph = (uint16_t)p_font->width;

    const uint8_t *p_glyph = &p_font->p_bitmap[char_index * bytes_per_glyph];

    for (uint8_t col = 0; col < p_font->width; col++) {
        uint8_t line = p_glyph[col];
        for (uint8_t row = 0; row < p_font->height; row++) {
            if ((line & (1U << row)) != 0U) {
                gfx_draw_pixel((int16_t)(x + col), (int16_t)(y + row), fg_color);
            } else if (bg_color != fg_color) {
                gfx_draw_pixel((int16_t)(x + col), (int16_t)(y + row), bg_color);
            } else {
                /* Transparent */
            }
        }
    }

    return (uint16_t)(p_font->width + 1U); /* Glyph width + 1 pixel character spacing */
}

uint16_t gfx_draw_string(int16_t x, int16_t y, const char *p_str, const gfx_font_t *p_font, uint16_t fg_color, uint16_t bg_color)
{
    if ((p_str == (void *)0) || (p_font == (void *)0)) {
        return 0;
    }

    int16_t curr_x = x;
    while (*p_str != '\0') {
        curr_x = (int16_t)(curr_x + (int16_t)gfx_draw_char(curr_x, y, *p_str, p_font, fg_color, bg_color));
        p_str++;
    }
    return (uint16_t)(curr_x - x);
}

uint16_t gfx_draw_number(int16_t x, int16_t y, int32_t val, uint8_t min_digits, const gfx_font_t *p_font, uint16_t fg_color, uint16_t bg_color)
{
    char buf[16];
    char temp[16];
    uint8_t idx = 0;
    bool is_neg = false;

    if (val < 0) {
        is_neg = true;
        val = -val;
    }

    /* Extract digits in reverse order */
    do {
        temp[idx++] = (char)('0' + (val % 10));
        val /= 10;
    } while (val > 0);

    /* Pad leading zeros if requested */
    while (idx < min_digits) {
        temp[idx++] = '0';
    }

    uint8_t out_idx = 0;
    if (is_neg) {
        buf[out_idx++] = '-';
    }

    /* Reverse into buffer */
    while (idx > 0) {
        buf[out_idx++] = temp[--idx];
    }
    buf[out_idx] = '\0';

    return gfx_draw_string(x, y, buf, p_font, fg_color, bg_color);
}
