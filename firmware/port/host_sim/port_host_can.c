/**
 * @file port_host_can.c
 * @brief Host PC CAN 2.0B FIFO Simulator.
 * 
 * Simulates hardware CAN mailboxes with an in-memory ring buffer.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "../../hal/hal_can.h"
#include <string.h>

#define HOST_CAN_FIFO_SIZE (32U)

static can_frame_t s_rx_fifo[HOST_CAN_FIFO_SIZE];
static uint8_t s_rx_head = 0;
static uint8_t s_rx_tail = 0;
static uint8_t s_rx_count = 0;
static bool s_bus_off = false;

hal_can_status_t hal_can_init(can_baud_rate_t baud)
{
    (void)baud;
    s_rx_head = 0;
    s_rx_tail = 0;
    s_rx_count = 0;
    s_bus_off = false;
    return HAL_CAN_OK;
}

hal_can_status_t hal_can_transmit(const can_frame_t *p_frame)
{
    if (p_frame == (void *)0) {
        return HAL_CAN_ERR_PARAM;
    }
    if (s_bus_off) {
        return HAL_CAN_ERR_BUS_OFF;
    }
    return HAL_CAN_OK;
}

hal_can_status_t hal_can_receive(can_frame_t *p_frame)
{
    if (p_frame == (void *)0) {
        return HAL_CAN_ERR_PARAM;
    }
    if (s_rx_count == 0U) {
        return HAL_CAN_ERR_BUFFER_EMPTY;
    }

    *p_frame = s_rx_fifo[s_rx_tail];
    s_rx_tail = (uint8_t)((s_rx_tail + 1U) % HOST_CAN_FIFO_SIZE);
    s_rx_count--;

    return HAL_CAN_OK;
}

hal_can_status_t hal_can_set_filter(uint32_t filter_id, uint32_t filter_mask, bool is_extended)
{
    (void)filter_id;
    (void)filter_mask;
    (void)is_extended;
    return HAL_CAN_OK;
}

bool hal_can_is_bus_off(void)
{
    return s_bus_off;
}

void hal_can_bus_recovery(void)
{
    s_bus_off = false;
}

/* Host Test Injection Helper */
bool port_host_can_push_rx(const can_frame_t *p_frame)
{
    if ((p_frame == (void *)0) || (s_rx_count >= HOST_CAN_FIFO_SIZE)) {
        return false;
    }

    s_rx_fifo[s_rx_head] = *p_frame;
    s_rx_head = (uint8_t)((s_rx_head + 1U) % HOST_CAN_FIFO_SIZE);
    s_rx_count++;
    return true;
}
