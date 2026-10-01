/**
 * @file font_embedded.h
 * @brief Flash-Resident 1-Bit Monospaced Font Descriptors for Automotive Display.
 * 
 * Provides compact bitmap font tables stored entirely in Program Flash (ROM).
 * Zero RAM consumption during glyph lookup and rendering.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef FONT_EMBEDDED_H
#define FONT_EMBEDDED_H

#include <stdint.h>
#include "gfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 5x7 Small ASCII Font (Full Printable ASCII 0x20 to 0x7E) */
extern const gfx_font_t g_font_5x7;

/* 11x18 Medium Automotive Numeric & Symbol Font ('0'-'9', 'k', 'm', 'h', '%', ':', '-', ' ', '.') */
extern const gfx_font_t g_font_num_11x18;

/* 16x26 Large Digital Speedometer Digits ('0'-'9', ' ') */
extern const gfx_font_t g_font_speedo_16x26;

#ifdef __cplusplus
}
#endif

#endif /* FONT_EMBEDDED_H */
