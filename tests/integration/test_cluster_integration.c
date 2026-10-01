/**
 * @file test_cluster_integration.c
 * @brief Integration Tests for Full Cluster Multi-Rate Lifecycle.
 */

#include "../unity/unity.h"
#include "../../firmware/app/cluster_app.h"
#include "../../firmware/services/can_telemetry.h"
#include "../../firmware/services/odometer_service.h"
#include "../../firmware/port/host_sim/port_host_sim.h"

void test_cluster_full_cycle_step(void)
{
    port_host_timer_set_manual(true);
    cluster_app_init();

    /* Inject Speed 60 km/h, Battery 85% */
    can_telemetry_inject_speed(600, 3200);
    can_telemetry_inject_battery(85, 520);
    can_telemetry_inject_telltales(false, true, false, false, false);

    /* Step through 1000 ms in 10 ms increments */
    for (uint32_t t = 0; t < 1000; t += 10) {
        port_host_timer_advance_ms(10);
        cluster_app_step(t);
    }

    cluster_app_state_t state;
    cluster_app_get_state(&state);
    TEST_ASSERT_TRUE(state.prev_speed_angle_deg > 0);
    TEST_ASSERT_TRUE(state.prev_soc_angle_deg > 0);
}
