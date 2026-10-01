/**
 * @file gfx_types.h
 * @brief Core 2D Graphics Data Structures and 16-bit RGB565 Color Palette.
 * 
 * Defines standard coordinate, rectangle, and bitmap structures for embedded
 * display rendering with zero dynamic memory allocation.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RGB565 16-bit Color Encoding Helper: R (5-bit), G (6-bit), B (5-bit) */
#define RGB565(r, g, b)  ((uint16_t)((((uint16_t)(r) & 0x1FU) << 11U) | \
                                     (((uint16_t)(g) & 0x3FU) << 5U)  | \
                                     (((uint16_t)(b) & 0x1FU))))

/* Standard Automotive Cluster Color Palette */
#define COLOR_BLACK         (0x0000U)
#define COLOR_WHITE         (0xFFFFU)
#define COLOR_DARK_GRAY     (0x2104U)
#define COLOR_MID_GRAY      (0x7BEFU)
#define COLOR_LIGHT_GRAY    (0xC618U)

/* Gauge & Theme Colors */
#define COLOR_AUTO_CYAN     (0x07FFU)
#define COLOR_AUTO_BLUE     (0x199FU)
#define COLOR_AUTO_GREEN    (0x07E0U)
#define COLOR_AUTO_AMBER    (0xFD20U)
#define COLOR_AUTO_RED      (0xF800U)
#define COLOR_AUTO_NEEDLE   (0xFA08U) /* Bright Red/Orange Needle */
#define COLOR_AUTO_BG_NIGHT (0x0841U) /* Deep Charcoal Night Background */
#define COLOR_AUTO_BG_DAY   (0xDEFBU) /* Crisp Slate Day Background */
#define COLOR_AUTO_ECO      (0x2FE5U) /* Vibrant Eco Green */

/* 2D Point Structure */
typedef struct {
    int16_t x;
    int16_t y;
} gfx_point_t;

/* 2D Bounding Rectangle */
typedef struct {
    int16_t x0;
    int16_t y0;
    int16_t x1;
    int16_t y1;
} gfx_rect_t;

/* 1-Bit Monospaced Embedded Font Descriptor */
typedef struct {
    uint8_t width;        /**< Character glyph width in pixels */
    uint8_t height;       /**< Character glyph height in pixels */
    uint8_t first_char;   /**< ASCII value of first glyph (e.g. ' ' = 0x20) */
    uint8_t last_char;    /**< ASCII value of last glyph */
    const uint8_t *p_bitmap; /**< Pointer to Flash font bitmap data */
} gfx_font_t;

#ifdef __cplusplus
}
#endif

#endif /* GFX_TYPES_H */
