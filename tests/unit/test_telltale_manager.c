/**
 * @file test_telltale_manager.c
 * @brief Unit Tests for Tell-Tale Manager and Blinker Cadences.
 */

#include "../unity/unity.h"
#include "../../firmware/services/telltale_manager.h"
#include <string.h>

void test_telltale_blinker_cadence(void)
{
    telltale_manager_init();

    vehicle_telemetry_t telem;
    memset(&telem, 0, sizeof(telem));
    telem.is_comm_active = true;
    telem.flag_turn_left = true;

    /* At t = 0 ms: Phase ON */
    telltale_manager_update(&telem, 0);
    telltale_status_t status;
    telltale_manager_get_status(&status);
    TEST_ASSERT_EQUAL_INT(TELLTALE_STATE_BLINKING, status.turn_left);
    TEST_ASSERT_TRUE(status.blink_phase_on);

    /* At t = 350 ms: Phase OFF (1.5 Hz half period = 333 ms) */
    telltale_manager_update(&telem, 350);
    telltale_manager_get_status(&status);
    TEST_ASSERT_FALSE(status.blink_phase_on);

    /* At t = 700 ms: Phase ON */
    telltale_manager_update(&telem, 700);
    telltale_manager_get_status(&status);
    TEST_ASSERT_TRUE(status.blink_phase_on);
}

void test_telltale_theme_toggle(void)
{
    telltale_manager_init();
    telltale_status_t status;
    telltale_manager_get_status(&status);
    TEST_ASSERT_EQUAL_INT(THEME_NIGHT, status.theme);

    telltale_manager_toggle_theme();
    telltale_manager_get_status(&status);
    TEST_ASSERT_EQUAL_INT(THEME_DAY, status.theme);
}
