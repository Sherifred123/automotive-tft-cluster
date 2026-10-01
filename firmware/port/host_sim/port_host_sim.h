/**
 * @file port_host_sim.h
 * @brief Host PC Simulation Port Interface and Test Helpers.
 * 
 * Provides function declarations for host-based virtual hardware drivers
 * and test harness injection interfaces.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef PORT_HOST_SIM_H
#define PORT_HOST_SIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../hal/hal_can.h"
#include "../../hal/hal_display.h"
#include "../../hal/hal_gpio.h"
#include "../../hal/hal_nvm.h"
#include "../../hal/hal_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --- Host Virtual Display Inspector --- */
uint16_t port_host_display_get_pixel(uint16_t x, uint16_t y);

/* --- Host CAN Mailbox Injection --- */
bool port_host_can_push_rx(const can_frame_t *p_frame);

/* --- Host NVM (EEPROM) Test Control --- */
void port_host_nvm_reset(void);

/* --- Host System Timer Test Control --- */
void port_host_timer_set_manual(bool enable);
void port_host_timer_advance_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* PORT_HOST_SIM_H */
