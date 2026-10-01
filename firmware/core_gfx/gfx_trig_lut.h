/**
 * @file gfx_trig_lut.h
 * @brief Integer Fixed-Point Q15 Trigonometry Engine for Needle and Arc Math.
 * 
 * Provides fast integer sine/cosine calculation without linking floating-point
 * libraries (<math.h>). Essential for zero-heap, resource-constrained 8-bit/32-bit MCUs.
 * 
 * Q15 format: 1.0 = +32767, -1.0 = -32768.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef GFX_TRIG_LUT_H
#define GFX_TRIG_LUT_H

#include <stdint.h>
#include <stdbool.h>
#include "gfx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRIG_Q15_ONE   (32767)

/**
 * @brief Get sine value in Q15 format for an angle in degrees [0 to 359].
 * @param degrees Angle from 0 to 359 (values >= 360 are automatically wrapped).
 * @return Signed 16-bit Q15 sine value [-32768 to +32767].
 */
int16_t gfx_trig_sin_q15(int16_t degrees);

/**
 * @brief Get cosine value in Q15 format for an angle in degrees [0 to 359].
 * @param degrees Angle from 0 to 359 (values >= 360 are automatically wrapped).
 * @return Signed 16-bit Q15 cosine value [-32768 to +32767].
 */
int16_t gfx_trig_cos_q15(int16_t degrees);

/**
 * @brief Calculate 2D vector endpoint from center (xc, yc), radius, and angle.
 * 
 * Computes:
 *   x = xc + (radius * cos(angle)) >> 15
 *   y = yc + (radius * sin(angle)) >> 15
 * 
 * @param xc Center X
 * @param yc Center Y
 * @param radius Radius in pixels
 * @param angle_deg Angle in degrees [0-359]
 * @param p_out Pointer to output point structure
 */
void gfx_trig_calc_endpoint(int16_t xc, int16_t yc, uint16_t radius, int16_t angle_deg, gfx_point_t *p_out);

/**
 * @brief Map a linear value (e.g. Speed 0-140 km/h) to an angular gauge range.
 * @param value Current input value
 * @param val_min Minimum value of range (e.g. 0)
 * @param val_max Maximum value of range (e.g. 140)
 * @param angle_start_deg Start angle on dial (e.g. 135 deg)
 * @param angle_end_deg End angle on dial (e.g. 405 / 45 deg)
 * @return Interpolated angle in degrees [0 to 359].
 */
int16_t gfx_trig_map_to_angle(int32_t value, int32_t val_min, int32_t val_max, int16_t angle_start_deg, int16_t angle_end_deg);

#ifdef __cplusplus
}
#endif

#endif /* GFX_TRIG_LUT_H */
