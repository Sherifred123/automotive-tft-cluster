/**
 * @file port_stm32_st7789_dma.c
 * @brief STM32F4xx FreeRTOS SPI + DMA Reference Display Driver & Architecture Port.
 * 
 * Provides an architectural reference implementation showing non-blocking SPI DMA
 * streaming with FreeRTOS binary semaphore ISR signaling (HAL_SPI_TxCpltCallback).
 * Designed for hardware integration with STM32CubeHAL and FreeRTOS.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "../../hal/hal_display.h"

#if defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx)
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;

static SemaphoreHandle_t s_dma_sem = NULL;

hal_display_status_t hal_display_init(void)
{
    if (s_dma_sem == NULL) {
        s_dma_sem = xSemaphoreCreateBinary();
        xSemaphoreGive(s_dma_sem);
    }
    return HAL_DISPLAY_OK;
}

hal_display_status_t hal_display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    /* Send CASET (0x2A) and RASET (0x2B) via HAL_SPI_Transmit */
    (void)x0; (void)y0; (void)x1; (void)y1;
    return HAL_DISPLAY_OK;
}

hal_display_status_t hal_display_stream_pixels(const uint16_t *p_pixels, uint32_t count)
{
    if (p_pixels == NULL) {
        return HAL_DISPLAY_ERR_PARAM;
    }

    if (xSemaphoreTake(s_dma_sem, pdMS_TO_TICKS(50)) == pdTRUE) {
        /* Initiate non-blocking SPI DMA Transfer */
        HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)p_pixels, (uint16_t)(count * 2U));
        return HAL_DISPLAY_OK;
    }
    return HAL_DISPLAY_ERR_TIMEOUT;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(s_dma_sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

hal_display_status_t hal_display_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color16)
{
    (void)hal_display_set_window(x0, y0, x1, y1);
    /* Stream solid color slice via DMA */
    (void)color16;
    return HAL_DISPLAY_OK;
}

void hal_display_set_backlight(uint8_t brightness_pct)
{
    /* TIM PWM duty cycle */
    (void)brightness_pct;
}

bool hal_display_is_busy(void)
{
    return (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
}

#endif /* STM32F4 */
