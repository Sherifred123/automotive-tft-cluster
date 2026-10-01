/**
 * @file gfx_engine.h
 * @brief High-Speed 2D Graphics Engine with Bresenham Needle & Arc Rasterizers.
 * 
 * Provides integer-only drawing primitives optimized for streaming color TFT controllers
 * with zero-heap memory usage and optional dirty-rectangle tracking.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef GFX_ENGINE_H
#define GFX_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include "gfx_types.h"
#include "gfx_trig_lut.h"
#include "gfx_dirty_rect.h"
#include "font_embedded.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Graphics Engine and bind to HAL display driver.
 */
void gfx_engine_init(void);

/**
 * @brief Draw a single pixel on screen.
 * @param x Coordinate X (0 to DISPLAY_WIDTH - 1)
 * @param y Coordinate Y (0 to DISPLAY_HEIGHT - 1)
 * @param color16 RGB565 color value.
 */
void gfx_draw_pixel(int16_t x, int16_t y, uint16_t color16);

/**
 * @brief Draw a 1-pixel line using integer Bresenham algorithm.
 */
void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color16);

/**
 * @brief Draw a multi-pixel thick line segment.
 * @param x0 Starting X
 * @param y0 Starting Y
 * @param x1 Ending X
 * @param y1 Ending Y
 * @param thickness Line thickness in pixels (1 to 8)
 * @param color16 RGB565 color
 */
void gfx_draw_thick_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t thickness, uint16_t color16);

/**
 * @brief Draw an automotive gauge needle from center hub to tip.
 * @param xc Center X
 * @param yc Center Y
 * @param radius Needle length in pixels
 * @param angle_deg Needle angle in degrees [0-359]
 * @param thickness Needle base width
 * @param color16 Needle color
 * @param p_dirty_out Optional pointer to record dirty bounding box
 */
void gfx_draw_needle(int16_t xc, int16_t yc, uint16_t radius, int16_t angle_deg, uint8_t thickness, uint16_t color16, gfx_rect_t *p_dirty_out);

/**
 * @brief Draw a circular dial arc bar (e.g. for Speed or Battery SOC).
 * @param xc Center X
 * @param yc Center Y
 * @param radius Outer radius
 * @param thickness Arc band thickness
 * @param start_angle Starting angle in degrees
 * @param end_angle Ending angle in degrees
 * @param color16 Arc color
 */
void gfx_draw_arc_bar(int16_t xc, int16_t yc, uint16_t radius, uint8_t thickness, int16_t start_angle, int16_t end_angle, uint16_t color16);

/**
 * @brief Draw a rectangular outline.
 */
void gfx_draw_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color16);

/**
 * @brief Draw a solid filled rectangle.
 */
void gfx_fill_rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color16);

/**
 * @brief Render a single ASCII character glyph from Flash font.
 * @param x Top-left X
 * @param y Top-left Y
 * @param c ASCII character
 * @param p_font Pointer to font descriptor
 * @param fg_color Foreground color
 * @param bg_color Background color (transparent if equal to fg_color)
 * @return Width in pixels rendered
 */
uint16_t gfx_draw_char(int16_t x, int16_t y, char c, const gfx_font_t *p_font, uint16_t fg_color, uint16_t bg_color);

/**
 * @brief Render a null-terminated string.
 * @param x Top-left X
 * @param y Top-left Y
 * @param p_str String pointer
 * @param p_font Pointer to font descriptor
 * @param fg_color Foreground color
 * @param bg_color Background color
 * @return Total width in pixels rendered
 */
uint16_t gfx_draw_string(int16_t x, int16_t y, const char *p_str, const gfx_font_t *p_font, uint16_t fg_color, uint16_t bg_color);

/**
 * @brief Render an integer number with zero-padding option.
 * @param x Top-left X
 * @param y Top-left Y
 * @param val Signed integer value
 * @param min_digits Minimum digits to format (padded with leading zeros if > 0)
 * @param p_font Pointer to font descriptor
 * @param fg_color Foreground color
 * @param bg_color Background color
 * @return Total width in pixels rendered
 */
uint16_t gfx_draw_number(int16_t x, int16_t y, int32_t val, uint8_t min_digits, const gfx_font_t *p_font, uint16_t fg_color, uint16_t bg_color);

#ifdef __cplusplus
}
#endif

#endif /* GFX_ENGINE_H */
