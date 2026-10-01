/**
 * @file port_host_gpio.c
 * @brief Host PC GPIO Pin Simulator.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "../../hal/hal_gpio.h"

static hal_gpio_state_t s_pin_states[HAL_PIN_MAX];

void hal_gpio_init(void)
{
    for (int i = 0; i < HAL_PIN_MAX; i++) {
        s_pin_states[i] = HAL_PIN_LOW;
    }
}

void hal_gpio_write(hal_gpio_pin_t pin, hal_gpio_state_t state)
{
    if (pin < HAL_PIN_MAX) {
        s_pin_states[pin] = state;
    }
}

hal_gpio_state_t hal_gpio_read(hal_gpio_pin_t pin)
{
    if (pin < HAL_PIN_MAX) {
        return s_pin_states[pin];
    }
    return HAL_PIN_LOW;
}

void hal_gpio_toggle(hal_gpio_pin_t pin)
{
    if (pin < HAL_PIN_MAX) {
        s_pin_states[pin] = (s_pin_states[pin] == HAL_PIN_LOW) ? HAL_PIN_HIGH : HAL_PIN_LOW;
    }
}
