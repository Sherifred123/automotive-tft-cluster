/**
 * @file hal_gpio.h
 * @brief Hardware Abstraction Layer for GPIO Pins and Vehicle Status Indicators.
 * 
 * Manages physical discrete pins such as Display Control (CS, DC, RST),
 * Turn Signal Status LEDs, High Beam input sense, and Trip Reset Pushbutton.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_PIN_DISP_CS = 0,    /**< Display SPI Chip Select */
    HAL_PIN_DISP_DC,        /**< Display Data/Command */
    HAL_PIN_DISP_RST,       /**< Display Hardware Reset */
    HAL_PIN_DISP_BL_PWM,    /**< Display Backlight PWM */
    HAL_PIN_BTN_TRIP_RESET, /**< Pushbutton for Trip Reset */
    HAL_PIN_IGNITION_SENSE, /**< Key Ignition Switched 12V Sense */
    HAL_PIN_LED_HEARTBEAT,  /**< System Diagnostic Heartbeat */
    HAL_PIN_MAX
} hal_gpio_pin_t;

typedef enum {
    HAL_PIN_LOW = 0,
    HAL_PIN_HIGH = 1
} hal_gpio_state_t;

/**
 * @brief Initialize all board GPIO pins.
 */
void hal_gpio_init(void);

/**
 * @brief Write digital state to a GPIO pin.
 * @param pin Target pin identifier.
 * @param state Logic level (HIGH/LOW).
 */
void hal_gpio_write(hal_gpio_pin_t pin, hal_gpio_state_t state);

/**
 * @brief Read digital state of a GPIO pin.
 * @param pin Target pin identifier.
 * @return Pin logic level.
 */
hal_gpio_state_t hal_gpio_read(hal_gpio_pin_t pin);

/**
 * @brief Toggle digital state of a GPIO output pin.
 * @param pin Target pin identifier.
 */
void hal_gpio_toggle(hal_gpio_pin_t pin);

#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIO_H */
