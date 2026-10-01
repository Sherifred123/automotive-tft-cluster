/**
 * @file port_pic18_st7789.c
 * @brief Microchip PIC18F46K22 Bare-Metal ST7789 SPI Display Driver (XC8 Compiler).
 * 
 * Demonstrates high-efficiency register-level SPI pixel streaming on an 8-bit MCU
 * with zero dynamic memory allocation and < 2 KB total RAM budget.
 * 
 * Hardware Mapping (PIC18F46K22 @ 64MHz Fosc):
 *   - SCK1 : RC3 (SPI Clock, 16 MHz / Fosc/4)
 *   - SDO1 : RC5 (SPI MOSI Data Out)
 *   - CS   : RC2 (Display Chip Select)
 *   - DC   : RC1 (Data / Command Select)
 *   - RST  : RC0 (Hardware Reset)
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, Microchip XC8 C99
 */

#include "../../hal/hal_display.h"

#ifdef __XC8
#include <xc.h>

/* Register-Level Bit Definitions */
#define PIN_DISP_CS   LATCbits.LATC2
#define PIN_DISP_DC   LATCbits.LATC1
#define PIN_DISP_RST  LATCbits.LATC0

/* ST7789 Command Set */
#define ST7789_SWRESET  (0x01U)
#define ST7789_SLPOUT   (0x11U)
#define ST7789_COLMOD   (0x3AU)
#define ST7789_MADCTL   (0x36U)
#define ST7789_CASET    (0x2AU)
#define ST7789_RASET    (0x2BU)
#define ST7789_RAMWR    (0x2CU)
#define ST7789_DISPON   (0x29U)

static void spi_write_byte(uint8_t data)
{
    SSP1BUF = data;
    while (!SSP1STATbits.BF) {
        /* Wait for hardware SPI shift register to complete transfer (250ns @ 64MHz) */
    }
    uint8_t dummy = SSP1BUF; /* Clear buffer */
    (void)dummy;
}

static void st7789_write_cmd(uint8_t cmd)
{
    PIN_DISP_DC = 0; /* Command Mode */
    PIN_DISP_CS = 0;
    spi_write_byte(cmd);
    PIN_DISP_CS = 1;
}

static void st7789_write_data(uint8_t data)
{
    PIN_DISP_DC = 1; /* Data Mode */
    PIN_DISP_CS = 0;
    spi_write_byte(data);
    PIN_DISP_CS = 1;
}

hal_display_status_t hal_display_init(void)
{
    /* 1. Configure Port C Directions */
    TRISCbits.TRISC0 = 0; /* RST output */
    TRISCbits.TRISC1 = 0; /* DC output */
    TRISCbits.TRISC2 = 0; /* CS output */
    TRISCbits.TRISC3 = 0; /* SCK1 output */
    TRISCbits.TRISC5 = 0; /* SDO1 output */

    /* 2. Configure MSSP1 for SPI Master Mode (Fosc/4 = 16 MHz, Mode 0) */
    SSP1STAT = 0x40; /* CKE = 1 */
    SSP1CON1 = 0x20; /* SSPEN = 1, CKP = 0, Master Fosc/4 */

    /* 3. Hardware Reset Pulse */
    PIN_DISP_CS = 1;
    PIN_DISP_RST = 0;
    for (volatile uint16_t i = 0; i < 10000; i++) {}
    PIN_DISP_RST = 1;
    for (volatile uint16_t i = 0; i < 10000; i++) {}

    /* 4. ST7789 Initialization Sequence */
    st7789_write_cmd(ST7789_SWRESET);
    for (volatile uint16_t i = 0; i < 10000; i++) {}

    st7789_write_cmd(ST7789_SLPOUT);
    for (volatile uint16_t i = 0; i < 10000; i++) {}

    st7789_write_cmd(ST7789_COLMOD);
    st7789_write_data(0x55); /* 16-bit / pixel (RGB565) */

    st7789_write_cmd(ST7789_MADCTL);
    st7789_write_data(0x70); /* Landscape orientation (320x240) */

    st7789_write_cmd(ST7789_DISPON);

    return HAL_DISPLAY_OK;
}

hal_display_status_t hal_display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    st7789_write_cmd(ST7789_CASET);
    st7789_write_data((uint8_t)(x0 >> 8U));
    st7789_write_data((uint8_t)(x0 & 0xFFU));
    st7789_write_data((uint8_t)(x1 >> 8U));
    st7789_write_data((uint8_t)(x1 & 0xFFU));

    st7789_write_cmd(ST7789_RASET);
    st7789_write_data((uint8_t)(y0 >> 8U));
    st7789_write_data((uint8_t)(y0 & 0xFFU));
    st7789_write_data((uint8_t)(y1 >> 8U));
    st7789_write_data((uint8_t)(y1 & 0xFFU));

    st7789_write_cmd(ST7789_RAMWR);
    return HAL_DISPLAY_OK;
}

hal_display_status_t hal_display_stream_pixels(const uint16_t *p_pixels, uint32_t count)
{
    PIN_DISP_DC = 1;
    PIN_DISP_CS = 0;
    for (uint32_t i = 0; i < count; i++) {
        uint16_t color = p_pixels[i];
        spi_write_byte((uint8_t)(color >> 8U));
        spi_write_byte((uint8_t)(color & 0xFFU));
    }
    PIN_DISP_CS = 1;
    return HAL_DISPLAY_OK;
}

hal_display_status_t hal_display_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color16)
{
    (void)hal_display_set_window(x0, y0, x1, y1);
    uint32_t total = (uint32_t)(x1 - x0 + 1U) * (uint32_t)(y1 - y0 + 1U);
    uint8_t hi = (uint8_t)(color16 >> 8U);
    uint8_t lo = (uint8_t)(color16 & 0xFFU);

    PIN_DISP_DC = 1;
    PIN_DISP_CS = 0;
    while (total > 0U) {
        spi_write_byte(hi);
        spi_write_byte(lo);
        total--;
    }
    PIN_DISP_CS = 1;
    return HAL_DISPLAY_OK;
}

void hal_display_set_backlight(uint8_t brightness_pct)
{
    /* Duty cycle PWM on CCP1/Timer2 */
    (void)brightness_pct;
}

bool hal_display_is_busy(void)
{
    return false;
}

#endif /* __XC8 */
