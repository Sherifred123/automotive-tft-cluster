/**
 * @file cluster_screens.c
 * @brief Implementation of Automotive Cluster Screen Rendering.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "cluster_screens.h"
#include "../hal/hal_display.h"
#include "../core_gfx/gfx_engine.h"
#include "../core_gfx/gfx_trig_lut.h"
#include "../core_gfx/font_embedded.h"

void cluster_screens_draw_background(cluster_theme_t theme)
{
    uint16_t bg_color = (theme == THEME_NIGHT) ? COLOR_AUTO_BG_NIGHT : COLOR_AUTO_BG_DAY;
    uint16_t fg_color = (theme == THEME_NIGHT) ? COLOR_WHITE : COLOR_BLACK;
    uint16_t dim_color = (theme == THEME_NIGHT) ? COLOR_MID_GRAY : COLOR_DARK_GRAY;

    /* 1. Clear entire screen with background theme color */
    gfx_fill_rect(0, 0, (int16_t)(DISPLAY_WIDTH - 1), (int16_t)(DISPLAY_HEIGHT - 1), bg_color);

    /* 2. Top Header & Bottom Footer Dividers */
    gfx_draw_line(0, 24, (int16_t)(DISPLAY_WIDTH - 1), 24, dim_color);
    gfx_draw_line(0, 200, (int16_t)(DISPLAY_WIDTH - 1), 200, dim_color);

    /* 3. Speedometer Outer Dial Ring & Tick Arcs */
    gfx_draw_arc_bar(SPEEDO_CENTER_X, SPEEDO_CENTER_Y, SPEEDO_RADIUS, 2, SPEEDO_MIN_ANGLE_DEG, SPEEDO_MAX_ANGLE_DEG, COLOR_AUTO_CYAN);
    gfx_draw_string((int16_t)(SPEEDO_CENTER_X - 14), (int16_t)(SPEEDO_CENTER_Y - 32), "SPEED", &g_font_5x7, fg_color, bg_color);
    gfx_draw_string((int16_t)(SPEEDO_CENTER_X - 12), (int16_t)(SPEEDO_CENTER_Y + 36), "km/h", &g_font_5x7, dim_color, bg_color);
    gfx_draw_string((int16_t)(SPEEDO_CENTER_X - 44), (int16_t)(SPEEDO_CENTER_Y + 24), "0", &g_font_5x7, dim_color, bg_color);
    gfx_draw_string((int16_t)(SPEEDO_CENTER_X - 6),  (int16_t)(SPEEDO_CENTER_Y - 46), "70", &g_font_5x7, dim_color, bg_color);
    gfx_draw_string((int16_t)(SPEEDO_CENTER_X + 32), (int16_t)(SPEEDO_CENTER_Y + 24), "140", &g_font_5x7, dim_color, bg_color);

    /* 4. Battery SOC Outer Dial Ring */
    gfx_draw_arc_bar(BATT_CENTER_X, BATT_CENTER_Y, BATT_RADIUS, 2, BATT_MIN_ANGLE_DEG, BATT_MAX_ANGLE_DEG, COLOR_AUTO_GREEN);
    gfx_draw_string((int16_t)(BATT_CENTER_X - 18), (int16_t)(BATT_CENTER_Y - 32), "BATTERY", &g_font_5x7, fg_color, bg_color);
    gfx_draw_string((int16_t)(BATT_CENTER_X - 12), (int16_t)(BATT_CENTER_Y + 36), "SOC %", &g_font_5x7, dim_color, bg_color);
    gfx_draw_string((int16_t)(BATT_CENTER_X - 44), (int16_t)(BATT_CENTER_Y + 24), "0", &g_font_5x7, dim_color, bg_color);
    gfx_draw_string((int16_t)(BATT_CENTER_X - 6),  (int16_t)(BATT_CENTER_Y - 46), "50", &g_font_5x7, dim_color, bg_color);
    gfx_draw_string((int16_t)(BATT_CENTER_X + 32), (int16_t)(BATT_CENTER_Y + 24), "100", &g_font_5x7, dim_color, bg_color);

    /* 5. Center Gear Mode Box */
    gfx_draw_rect(142, 100, 178, 130, dim_color);
    gfx_draw_string(148, 108, "DRV", &g_font_5x7, COLOR_AUTO_CYAN, bg_color);
}

void cluster_screens_update_speedo(uint16_t speed_kmh_x10, int16_t *p_prev_angle, cluster_theme_t theme)
{
    uint16_t bg_color = (theme == THEME_NIGHT) ? COLOR_AUTO_BG_NIGHT : COLOR_AUTO_BG_DAY;
    uint16_t fg_color = (theme == THEME_NIGHT) ? COLOR_WHITE : COLOR_BLACK;

    int32_t speed_int = (int32_t)(speed_kmh_x10 / 10U);
    int16_t new_angle = gfx_trig_map_to_angle(speed_int, 0, SPEEDO_MAX_VAL_KMH, SPEEDO_MIN_ANGLE_DEG, SPEEDO_MAX_ANGLE_DEG);

    if (*p_prev_angle != new_angle) {
        /* 1. Erase old needle */
        if (*p_prev_angle >= 0) {
            gfx_draw_needle(SPEEDO_CENTER_X, SPEEDO_CENTER_Y, (uint16_t)(SPEEDO_RADIUS - 8U), *p_prev_angle, 3, bg_color, (void *)0);
        }

        /* 2. Draw new needle */
        gfx_draw_needle(SPEEDO_CENTER_X, SPEEDO_CENTER_Y, (uint16_t)(SPEEDO_RADIUS - 8U), new_angle, 3, COLOR_AUTO_NEEDLE, (void *)0);
        *p_prev_angle = new_angle;

        /* 3. Update Digital Speed Readout */
        gfx_fill_rect((int16_t)(SPEEDO_CENTER_X - 16), (int16_t)(SPEEDO_CENTER_Y + 12), (int16_t)(SPEEDO_CENTER_X + 16), (int16_t)(SPEEDO_CENTER_Y + 24), bg_color);
        gfx_draw_number((int16_t)(SPEEDO_CENTER_X - 10), (int16_t)(SPEEDO_CENTER_Y + 14), speed_int, 0, &g_font_5x7, fg_color, bg_color);
    }
}

void cluster_screens_update_battery(uint8_t soc_pct, int16_t *p_prev_angle, cluster_theme_t theme)
{
    uint16_t bg_color = (theme == THEME_NIGHT) ? COLOR_AUTO_BG_NIGHT : COLOR_AUTO_BG_DAY;
    uint16_t fg_color = (theme == THEME_NIGHT) ? COLOR_WHITE : COLOR_BLACK;
    uint16_t needle_color = (soc_pct <= 20U) ? COLOR_AUTO_RED : COLOR_AUTO_GREEN;

    int16_t new_angle = gfx_trig_map_to_angle(soc_pct, 0, BATT_MAX_VAL_PCT, BATT_MIN_ANGLE_DEG, BATT_MAX_ANGLE_DEG);

    if (*p_prev_angle != new_angle) {
        /* 1. Erase old needle */
        if (*p_prev_angle >= 0) {
            gfx_draw_needle(BATT_CENTER_X, BATT_CENTER_Y, (uint16_t)(BATT_RADIUS - 8U), *p_prev_angle, 3, bg_color, (void *)0);
        }

        /* 2. Draw new needle */
        gfx_draw_needle(BATT_CENTER_X, BATT_CENTER_Y, (uint16_t)(BATT_RADIUS - 8U), new_angle, 3, needle_color, (void *)0);
        *p_prev_angle = new_angle;

        /* 3. Update Digital SOC Readout */
        gfx_fill_rect((int16_t)(BATT_CENTER_X - 16), (int16_t)(BATT_CENTER_Y + 12), (int16_t)(BATT_CENTER_X + 16), (int16_t)(BATT_CENTER_Y + 24), bg_color);
        gfx_draw_number((int16_t)(BATT_CENTER_X - 12), (int16_t)(BATT_CENTER_Y + 14), (int32_t)soc_pct, 0, &g_font_5x7, fg_color, bg_color);
    }
}

void cluster_screens_update_telltales(const telltale_status_t *p_telltales, const vehicle_telemetry_t *p_telem, cluster_theme_t theme)
{
    if ((p_telltales == (void *)0) || (p_telem == (void *)0)) {
        return;
    }

    uint16_t bg_color = (theme == THEME_NIGHT) ? COLOR_AUTO_BG_NIGHT : COLOR_AUTO_BG_DAY;

    /* 1. Left Blinker [ < ] */
    if ((p_telltales->turn_left == TELLTALE_STATE_ON) || 
       ((p_telltales->turn_left == TELLTALE_STATE_BLINKING) && p_telltales->blink_phase_on)) {
        gfx_draw_string(8, 8, "<--", &g_font_5x7, COLOR_AUTO_GREEN, bg_color);
    } else {
        gfx_fill_rect(8, 8, 28, 18, bg_color);
    }

    /* 2. Right Blinker [ > ] */
    if ((p_telltales->turn_right == TELLTALE_STATE_ON) || 
       ((p_telltales->turn_right == TELLTALE_STATE_BLINKING) && p_telltales->blink_phase_on)) {
        gfx_draw_string(292, 8, "-->", &g_font_5x7, COLOR_AUTO_GREEN, bg_color);
    } else {
        gfx_fill_rect(292, 8, 312, 18, bg_color);
    }

    /* 3. Status Indicators */
    /* READY */
    gfx_draw_string(45, 8, "READY", &g_font_5x7, COLOR_AUTO_ECO, bg_color);

    /* High Beam */
    if (p_telltales->high_beam == TELLTALE_STATE_ON) {
        gfx_draw_string(90, 8, "[HIGH]", &g_font_5x7, COLOR_AUTO_BLUE, bg_color);
    } else {
        gfx_fill_rect(90, 8, 126, 18, bg_color);
    }

    /* Battery Low Warning */
    if ((p_telltales->battery_warning == TELLTALE_STATE_ON) ||
       ((p_telltales->battery_warning == TELLTALE_STATE_BLINKING) && p_telltales->blink_phase_on)) {
        gfx_draw_string(140, 8, "[BATT!]", &g_font_5x7, COLOR_AUTO_RED, bg_color);
    } else {
        gfx_fill_rect(140, 8, 184, 18, bg_color);
    }

    /* CAN Comm Loss */
    if ((p_telltales->can_comm_loss == TELLTALE_STATE_BLINKING) && p_telltales->blink_phase_on) {
        gfx_draw_string(195, 8, "[CAN FAULT]", &g_font_5x7, COLOR_AUTO_AMBER, bg_color);
    } else {
        gfx_fill_rect(195, 8, 265, 18, bg_color);
    }
}

void cluster_screens_update_footer(uint32_t odo_km_x10, uint32_t trip_km_x10, vehicle_gear_t gear, cluster_theme_t theme)
{
    uint16_t bg_color = (theme == THEME_NIGHT) ? COLOR_AUTO_BG_NIGHT : COLOR_AUTO_BG_DAY;
    uint16_t fg_color = (theme == THEME_NIGHT) ? COLOR_WHITE : COLOR_BLACK;
    uint16_t dim_color = (theme == THEME_NIGHT) ? COLOR_MID_GRAY : COLOR_DARK_GRAY;

    /* Trip: e.g. "TRIP: 142.8 km" */
    gfx_draw_string(10, 214, "TRIP:", &g_font_5x7, dim_color, bg_color);
    gfx_fill_rect(42, 212, 90, 224, bg_color);
    gfx_draw_number(44, 214, (int32_t)(trip_km_x10 / 10U), 0, &g_font_5x7, fg_color, bg_color);
    gfx_draw_string(72, 214, "km", &g_font_5x7, dim_color, bg_color);

    /* ODO: e.g. "ODO: 014820 km" */
    gfx_draw_string(110, 214, "ODO:", &g_font_5x7, dim_color, bg_color);
    gfx_fill_rect(136, 212, 195, 224, bg_color);
    gfx_draw_number(138, 214, (int32_t)(odo_km_x10 / 10U), 6, &g_font_5x7, fg_color, bg_color);
    gfx_draw_string(178, 214, "km", &g_font_5x7, dim_color, bg_color);

    /* Gear Mode & Temp */
    const char *gear_str = "D";
    if (gear == GEAR_PARK) gear_str = "P";
    else if (gear == GEAR_REVERSE) gear_str = "R";
    else if (gear == GEAR_NEUTRAL) gear_str = "N";
    else if (gear == GEAR_SPORT) gear_str = "S";

    gfx_draw_string(220, 214, "GEAR:[", &g_font_5x7, dim_color, bg_color);
    gfx_draw_string(254, 214, gear_str, &g_font_5x7, COLOR_AUTO_CYAN, bg_color);
    gfx_draw_string(262, 214, "]", &g_font_5x7, dim_color, bg_color);

    gfx_draw_string(280, 214, "28 C", &g_font_5x7, fg_color, bg_color);
}
