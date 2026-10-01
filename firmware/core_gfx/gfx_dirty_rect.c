/**
 * @file gfx_dirty_rect.c
 * @brief Implementation of Dirty-Rectangle Bounding Box Calculations.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "gfx_dirty_rect.h"

#define GFX_COORD_MAX_POS (32767)
#define GFX_COORD_MIN_NEG (-32768)

void gfx_dirty_rect_reset(gfx_rect_t *p_rect)
{
    if (p_rect != (void *)0) {
        p_rect->x0 = GFX_COORD_MAX_POS;
        p_rect->y0 = GFX_COORD_MAX_POS;
        p_rect->x1 = GFX_COORD_MIN_NEG;
        p_rect->y1 = GFX_COORD_MIN_NEG;
    }
}

void gfx_dirty_rect_add_point(gfx_rect_t *p_rect, int16_t x, int16_t y)
{
    if (p_rect == (void *)0) {
        return;
    }

    if (x < p_rect->x0) {
        p_rect->x0 = x;
    }
    if (x > p_rect->x1) {
        p_rect->x1 = x;
    }
    if (y < p_rect->y0) {
        p_rect->y0 = y;
    }
    if (y > p_rect->y1) {
        p_rect->y1 = y;
    }
}

void gfx_dirty_rect_add_line(gfx_rect_t *p_rect, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t thickness)
{
    if (p_rect == (void *)0) {
        return;
    }

    int16_t half_thick = (int16_t)(thickness >> 1U);
    if (half_thick < 1) {
        half_thick = 1;
    }

    gfx_dirty_rect_add_point(p_rect, (int16_t)(x0 - half_thick), (int16_t)(y0 - half_thick));
    gfx_dirty_rect_add_point(p_rect, (int16_t)(x0 + half_thick), (int16_t)(y0 + half_thick));
    gfx_dirty_rect_add_point(p_rect, (int16_t)(x1 - half_thick), (int16_t)(y1 - half_thick));
    gfx_dirty_rect_add_point(p_rect, (int16_t)(x1 + half_thick), (int16_t)(y1 + half_thick));
}

void gfx_dirty_rect_union(gfx_rect_t *p_dest, const gfx_rect_t *p_a, const gfx_rect_t *p_b)
{
    if ((p_dest == (void *)0) || (p_a == (void *)0) || (p_b == (void *)0)) {
        return;
    }

    if (!gfx_dirty_rect_is_valid(p_a)) {
        *p_dest = *p_b;
        return;
    }
    if (!gfx_dirty_rect_is_valid(p_b)) {
        *p_dest = *p_a;
        return;
    }

    p_dest->x0 = (p_a->x0 < p_b->x0) ? p_a->x0 : p_b->x0;
    p_dest->y0 = (p_a->y0 < p_b->y0) ? p_a->y0 : p_b->y0;
    p_dest->x1 = (p_a->x1 > p_b->x1) ? p_a->x1 : p_b->x1;
    p_dest->y1 = (p_a->y1 > p_b->y1) ? p_a->y1 : p_b->y1;
}

bool gfx_dirty_rect_clamp(gfx_rect_t *p_rect, uint16_t screen_w, uint16_t screen_h)
{
    if (p_rect == (void *)0) {
        return false;
    }

    if (!gfx_dirty_rect_is_valid(p_rect)) {
        return false;
    }

    if (p_rect->x0 < 0) {
        p_rect->x0 = 0;
    }
    if (p_rect->y0 < 0) {
        p_rect->y0 = 0;
    }
    if (p_rect->x1 >= (int16_t)screen_w) {
        p_rect->x1 = (int16_t)(screen_w - 1U);
    }
    if (p_rect->y1 >= (int16_t)screen_h) {
        p_rect->y1 = (int16_t)(screen_h - 1U);
    }

    /* Check if clamped rect has positive width and height */
    return (p_rect->x0 <= p_rect->x1) && (p_rect->y0 <= p_rect->y1);
}

bool gfx_dirty_rect_is_valid(const gfx_rect_t *p_rect)
{
    if (p_rect == (void *)0) {
        return false;
    }
    return (p_rect->x0 <= p_rect->x1) && (p_rect->y0 <= p_rect->y1);
}
