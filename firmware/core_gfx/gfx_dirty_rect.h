/**
 * @file gfx_dirty_rect.h
 * @brief Dirty-Rectangle Bounding Box Tracker for Partial Display Updates.
 * 
 * Computes exact minimum rectangular bounding boxes for changed screen elements
 * (speedometer needles, digits, status icons) to eliminate full-screen redraws
 * and screen tearing while using zero RAM framebuffers.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef GFX_DIRTY_RECT_H
#define GFX_DIRTY_RECT_H

#include <stdint.h>
#include <stdbool.h>
#include "gfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reset/invalidate dirty rectangle.
 * @param p_rect Pointer to rectangle to reset.
 */
void gfx_dirty_rect_reset(gfx_rect_t *p_rect);

/**
 * @brief Expand dirty rectangle to enclose a single 2D point (x, y).
 * @param p_rect Pointer to dirty rectangle.
 * @param x Coordinate X
 * @param y Coordinate Y
 */
void gfx_dirty_rect_add_point(gfx_rect_t *p_rect, int16_t x, int16_t y);

/**
 * @brief Expand dirty rectangle to enclose a line segment with a given pixel thickness.
 * @param p_rect Pointer to dirty rectangle.
 * @param x0 Starting X
 * @param y0 Starting Y
 * @param x1 Ending X
 * @param y1 Ending Y
 * @param thickness Line thickness in pixels (e.g. 1, 2, 3)
 */
void gfx_dirty_rect_add_line(gfx_rect_t *p_rect, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t thickness);

/**
 * @brief Merge two rectangles into their smallest enclosing bounding box (Union).
 * @param p_dest Destination union rectangle.
 * @param p_a First rectangle.
 * @param p_b Second rectangle.
 */
void gfx_dirty_rect_union(gfx_rect_t *p_dest, const gfx_rect_t *p_a, const gfx_rect_t *p_b);

/**
 * @brief Clamp dirty rectangle boundaries within screen physical limits (e.g. 320x240).
 * @param p_rect Pointer to dirty rectangle.
 * @param screen_w Screen width in pixels.
 * @param screen_h Screen height in pixels.
 * @return true if rectangle has valid area within screen bounds, false if off-screen.
 */
bool gfx_dirty_rect_clamp(gfx_rect_t *p_rect, uint16_t screen_w, uint16_t screen_h);

/**
 * @brief Check if rectangle contains a valid non-empty area.
 * @param p_rect Pointer to dirty rectangle.
 * @return true if valid area, false if empty.
 */
bool gfx_dirty_rect_is_valid(const gfx_rect_t *p_rect);

#ifdef __cplusplus
}
#endif

#endif /* GFX_DIRTY_RECT_H */
