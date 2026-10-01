/**
 * @file hal_display.h
 * @brief Hardware Abstraction Layer for Color TFT Display Controller (ST7789 / ILI9341).
 * 
 * Defines standard SPI interface for display window addressing and pixel streaming.
 * Designed for zero-heap, streaming architectures with dirty-rectangle support.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef HAL_DISPLAY_H
#define HAL_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Display Physical Dimensions (Pixels) */
#define DISPLAY_WIDTH   (320U)
#define DISPLAY_HEIGHT  (240U)

/* Display Interface Return Codes */
typedef enum {
    HAL_DISPLAY_OK = 0,
    HAL_DISPLAY_ERR_PARAM,
    HAL_DISPLAY_ERR_TIMEOUT,
    HAL_DISPLAY_ERR_BUSY,
    HAL_DISPLAY_ERR_IO
} hal_display_status_t;

/* Display Orientation */
typedef enum {
    DISPLAY_ORIENT_PORTRAIT = 0,
    DISPLAY_ORIENT_LANDSCAPE,
    DISPLAY_ORIENT_PORTRAIT_FLIP,
    DISPLAY_ORIENT_LANDSCAPE_FLIP
} display_orientation_t;

/**
 * @brief Initialize display controller peripheral (SPI, GPIOs, Reset sequence).
 * @return HAL_DISPLAY_OK on success.
 */
hal_display_status_t hal_display_init(void);

/**
 * @brief Set drawing window / bounding box for streaming writes (CASET/RASET).
 * @param x0 Starting X coordinate (0 to DISPLAY_WIDTH - 1)
 * @param y0 Starting Y coordinate (0 to DISPLAY_HEIGHT - 1)
 * @param x1 Ending X coordinate (inclusive)
 * @param y1 Ending Y coordinate (inclusive)
 * @return HAL_DISPLAY_OK on success.
 */
hal_display_status_t hal_display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/**
 * @brief Stream RGB565 pixel buffer into current active window.
 * @param p_pixels Pointer to 16-bit RGB565 color data array.
 * @param count Number of 16-bit pixels to transfer.
 * @return HAL_DISPLAY_OK on success.
 */
hal_display_status_t hal_display_stream_pixels(const uint16_t *p_pixels, uint32_t count);

/**
 * @brief Fill a contiguous rectangular area with a solid color.
 * @param x0 Starting X
 * @param y0 Starting Y
 * @param x1 Ending X
 * @param y1 Ending Y
 * @param color16 RGB565 color value.
 * @return HAL_DISPLAY_OK on success.
 */
hal_display_status_t hal_display_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color16);

/**
 * @brief Set Display Backlight Brightness (0 - 100% via PWM).
 * @param brightness_pct Brightness percentage (0 = Off, 100 = Max).
 */
void hal_display_set_backlight(uint8_t brightness_pct);

/**
 * @brief Poll if display DMA / SPI transfer is active (non-blocking for RTOS).
 * @return true if busy transmitting, false if ready.
 */
bool hal_display_is_busy(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_DISPLAY_H */
