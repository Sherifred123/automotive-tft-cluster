/**
 * @file main_dashboard_sim.c
 * @brief Interactive Host PC Automotive TFT Cluster Dashboard Simulator.
 * 
 * Provides an interactive ANSI virtual LCD dashboard in the terminal
 * with real-time keyboard stimulus, needle motion, and CAN fault injection.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard ANSI C99
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#endif

#include "../../firmware/app/cluster_app.h"
#include "../../firmware/app/cluster_screens.h"
#include "../../firmware/services/can_telemetry.h"
#include "../../firmware/services/odometer_service.h"
#include "../../firmware/services/telltale_manager.h"
#include "../../firmware/hal/hal_timer.h"

/* Non-blocking keyboard read */
static int check_keypress(void)
{
#if defined(_WIN32) || defined(_WIN64)
    if (_kbhit()) {
        return _getch();
    }
    return -1;
#else
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    if (select(1, &fds, NULL, NULL, &tv) > 0) {
        return getchar();
    }
    return -1;
#endif
}

static void draw_ansi_dashboard(uint16_t speed_x10, uint16_t rpm, uint8_t soc, uint16_t volt_x10,
                                uint32_t odo_x10, uint32_t trip_x10, const telltale_status_t *p_t,
                                bool comm_active, uint8_t active_slot)
{
    /* Clear screen and move cursor to top-left */
    printf("\033[H");

    printf("\033[1;36m========================================================================================\033[0m\n");
    printf("\033[1;37m        AUTOMOTIVE DIGITAL TFT INSTRUMENT CLUSTER SIMULATOR  [320x240 RGB565]          \033[0m\n");
    printf("\033[1;36m========================================================================================\033[0m\n");

    /* 1. Header Tell-Tale Banner */
    printf(" | ");
    if ((p_t->turn_left == TELLTALE_STATE_ON) || ((p_t->turn_left == TELLTALE_STATE_BLINKING) && p_t->blink_phase_on)) {
        printf("\033[1;32m[◀◀ LEFT]\033[0m ");
    } else {
        printf("         ");
    }

    printf("\033[1;32m[READY]\033[0m ");

    if (p_t->high_beam == TELLTALE_STATE_ON) {
        printf("\033[1;34m[HIGH BEAM]\033[0m ");
    } else {
        printf("           ");
    }

    if ((p_t->battery_warning == TELLTALE_STATE_ON) || ((p_t->battery_warning == TELLTALE_STATE_BLINKING) && p_t->blink_phase_on)) {
        printf("\033[1;31m[BATT LOW!]\033[0m ");
    } else {
        printf("           ");
    }

    if ((p_t->can_comm_loss == TELLTALE_STATE_BLINKING) && p_t->blink_phase_on) {
        printf("\033[1;33m[CAN FAULT!]\033[0m ");
    } else {
        printf("            ");
    }

    if (p_t->theme == THEME_NIGHT) {
        printf("\033[0;35m[NIGHT]\033[0m ");
    } else {
        printf("\033[1;33m[DAY]\033[0m   ");
    }

    if ((p_t->turn_right == TELLTALE_STATE_ON) || ((p_t->turn_right == TELLTALE_STATE_BLINKING) && p_t->blink_phase_on)) {
        printf("\033[1;32m[RIGHT ▶▶]\033[0m");
    } else {
        printf("          ");
    }
    printf(" |\n");
    printf("\033[1;30m----------------------------------------------------------------------------------------\033[0m\n");

    /* 2. Dials / Gauges ASCII Art */
    int speed_val = speed_x10 / 10;
    int speed_bars = (speed_val * 20) / 140;
    int soc_bars = (soc * 20) / 100;

    printf(" | \033[1;36m%-38s\033[0m | \033[1;32m%-38s\033[0m |\n", "  << SPEEDOMETER (km/h) >>", "  << BATTERY PACK (SOC %) >>");
    printf(" |   0      40      80     120   140   |   0%%     25%%     50%%     75%%    100%%  |\n");
    printf(" |  [");
    for (int i = 0; i < 20; i++) {
        if (i < speed_bars) printf("\033[1;31m#\033[0m");
        else printf(".");
    }
    printf("]           |  [");
    for (int i = 0; i < 20; i++) {
        if (i < soc_bars) {
            if (soc <= 20) printf("\033[1;31m#\033[0m");
            else printf("\033[1;32m#\033[0m");
        } else printf(".");
    }
    printf("]           |\n");

    printf(" |                                         |                                         |\n");
    printf(" |     \033[1;37mDIGITAL: \033[1;33m%3d km/h\033[0m                    |     \033[1;37mSTATE OF CHARGE: \033[1;32m%3d %%%%\033[0m            |\n", speed_val, soc);
    printf(" |     \033[0;37mMOTOR:   \033[1;36m%4d RPM\033[0m                     |     \033[0;37mPACK VOLTAGE:    \033[1;32m%4.1f V\033[0m            |\n", rpm, (float)volt_x10 / 10.0f);
    printf(" |                                         |                                         |\n");
    printf("\033[1;30m----------------------------------------------------------------------------------------\033[0m\n");

    /* 3. Footer Trip, Odo, Gear */
    printf(" | \033[1;37mTRIP: \033[1;36m%6.1f km\033[0m   |  \033[1;37mODO: \033[1;33m%06.1f km\033[0m   |  \033[1;37mGEAR: \033[1;32m[ DRIVE ]\033[0m   |  \033[1;37mTEMP: \033[0;36m28 C\033[0m  |\n",
           (float)trip_x10 / 10.0f, (float)odo_x10 / 10.0f);
    printf("\033[1;36m========================================================================================\033[0m\n");

    /* 4. Diagnostics & Wear-Leveling Status */
    printf("\033[0;37m [DIAGNOSTICS]\033[0m CAN Bus: %s\033[0m | Active EEPROM Slot: \033[1;33m[%d / 16]\033[0m | Wear-Level: \033[1;32mOK\033[0m\n",
           comm_active ? "\033[1;32mONLINE (500 kbps)" : "\033[1;31mOFFLINE (COMM LOSS)", (int)active_slot);
    printf("\033[0;37m [CONTROLS]\033[0m [W/S] Speed +/- | [A/D] Blinker L/R | [H] HighBeam | [B] Batt Drain | [T] Trip Reset\n");
    printf("            [Z] Hazard | [M] Theme Night/Day | [C] Inject CAN Loss | [Q] Quit\n");
}

int main(void)
{
    printf("\033[2J"); /* Clear terminal */
    cluster_app_init();

    uint16_t sim_speed = 450; /* 45.0 km/h */
    uint16_t sim_rpm = 2400;
    uint8_t  sim_soc = 88;
    uint16_t sim_volt = 518;
    bool     sim_left = false;
    bool     sim_right = false;
    bool     sim_high = false;
    bool     sim_hazard = false;
    bool     sim_can_loss = false;

    can_telemetry_inject_speed(sim_speed, sim_rpm);
    can_telemetry_inject_battery(sim_soc, sim_volt);

    bool running = true;
    while (running) {
        int key = check_keypress();
        if (key != -1) {
            switch (key) {
                case 'w':
                case 'W':
                    if (sim_speed < 1400) { sim_speed += 50; sim_rpm += 250; }
                    break;
                case 's':
                case 'S':
                    if (sim_speed >= 50) { sim_speed -= 50; sim_rpm = (sim_rpm >= 250) ? sim_rpm - 250 : 0; }
                    break;
                case 'a':
                case 'A':
                    sim_left = !sim_left;
                    sim_right = false;
                    sim_hazard = false;
                    break;
                case 'd':
                case 'D':
                    sim_right = !sim_right;
                    sim_left = false;
                    sim_hazard = false;
                    break;
                case 'z':
                case 'Z':
                    sim_hazard = !sim_hazard;
                    sim_left = false;
                    sim_right = false;
                    break;
                case 'h':
                case 'H':
                    sim_high = !sim_high;
                    break;
                case 'b':
                case 'B':
                    if (sim_soc > 5) sim_soc -= 5;
                    else sim_soc = 100;
                    break;
                case 't':
                case 'T':
                    odometer_service_reset_trip();
                    break;
                case 'm':
                case 'M':
                    telltale_manager_toggle_theme();
                    cluster_app_request_redraw();
                    break;
                case 'c':
                case 'C':
                    sim_can_loss = !sim_can_loss;
                    break;
                case 'q':
                case 'Q':
                    running = false;
                    break;
                default:
                    break;
            }

            if (!sim_can_loss) {
                can_telemetry_inject_speed(sim_speed, sim_rpm);
                can_telemetry_inject_battery(sim_soc, sim_volt);
                can_telemetry_inject_telltales(sim_left || sim_hazard, sim_right || sim_hazard, sim_high, sim_soc <= 15, false);
            }
        }

        uint32_t now = hal_timer_get_ms();
        cluster_app_step(now);

        vehicle_telemetry_t telem;
        can_telemetry_get_data(&telem);
        telltale_status_t telltales;
        telltale_manager_get_status(&telltales);
        uint32_t odo = odometer_service_get_total_km_x10();
        uint32_t trip = odometer_service_get_trip_km_x10();
        uint8_t slot = odometer_service_get_active_slot();

        draw_ansi_dashboard(telem.speed_kmh_x10, telem.motor_rpm, telem.battery_soc_pct,
                            telem.battery_voltage_x10, odo, trip, &telltales,
                            telem.is_comm_active, slot);

        hal_timer_delay_ms(50); /* 20 FPS UI refresh */
    }

    printf("\nSimulator stopped.\n");
    return 0;
}
