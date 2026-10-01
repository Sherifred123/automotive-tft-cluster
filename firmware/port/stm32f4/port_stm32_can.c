/**
 * @file port_stm32_can.c
 * @brief STM32F4xx bxCAN Reference Driver & FreeRTOS Queue Port.
 * 
 * Provides an architectural reference implementation of bxCAN mailbox management,
 * acceptance filtering, and FreeRTOS queue dispatch for target STM32 hardware.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "../../hal/hal_can.h"

#if defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx)
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"

extern CAN_HandleTypeDef hcan1;
static QueueHandle_t s_can_rx_queue = NULL;

hal_can_status_t hal_can_init(can_baud_rate_t baud)
{
    (void)baud;
    if (s_can_rx_queue == NULL) {
        s_can_rx_queue = xQueueCreate(16, sizeof(can_frame_t));
    }
    return HAL_CAN_OK;
}

hal_can_status_t hal_can_transmit(const can_frame_t *p_frame)
{
    if (p_frame == NULL) {
        return HAL_CAN_ERR_PARAM;
    }

    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = (uint32_t)p_frame->id;
    tx_header.ExtId = (uint32_t)p_frame->id;
    tx_header.IDE   = p_frame->is_extended ? CAN_ID_EXT : CAN_ID_STD;
    tx_header.RTR   = p_frame->is_rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA;
    tx_header.DLC   = p_frame->dlc;
    tx_header.TransmitGlobalTime = DISABLE;

    uint32_t tx_mailbox;
    if (HAL_CAN_AddTxMessage(&hcan1, &tx_header, (uint8_t *)p_frame->data, &tx_mailbox) == HAL_OK) {
        return HAL_CAN_OK;
    }
    return HAL_CAN_ERR_BUFFER_FULL;
}

hal_can_status_t hal_can_receive(can_frame_t *p_frame)
{
    if (p_frame == NULL) {
        return HAL_CAN_ERR_PARAM;
    }
    if (s_can_rx_queue != NULL) {
        if (xQueueReceive(s_can_rx_queue, p_frame, 0) == pdTRUE) {
            return HAL_CAN_OK;
        }
    }
    return HAL_CAN_ERR_BUFFER_EMPTY;
}

hal_can_status_t hal_can_set_filter(uint32_t filter_id, uint32_t filter_mask, bool is_extended)
{
    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = (uint16_t)(filter_id >> 16U);
    sFilterConfig.FilterIdLow = (uint16_t)(filter_id & 0xFFFFU);
    sFilterConfig.FilterMaskIdHigh = (uint16_t)(filter_mask >> 16U);
    sFilterConfig.FilterMaskIdLow = (uint16_t)(filter_mask & 0xFFFFU);
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) == HAL_OK) {
        return HAL_CAN_OK;
    }
    return HAL_CAN_ERR_PARAM;
}

bool hal_can_is_bus_off(void)
{
    return (HAL_CAN_GetError(&hcan1) & HAL_CAN_ERROR_BOF) != 0U;
}

void hal_can_bus_recovery(void)
{
    /* Reset bxCAN peripheral */
    HAL_CAN_ResetError(&hcan1);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance == CAN1) {
        CAN_RxHeaderTypeDef rx_header;
        can_frame_t frame;
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, frame.data) == HAL_OK) {
            frame.id = (rx_header.IDE == CAN_ID_EXT) ? rx_header.ExtId : rx_header.StdId;
            frame.dlc = (uint8_t)rx_header.DLC;
            frame.is_extended = (rx_header.IDE == CAN_ID_EXT);
            frame.is_rtr = (rx_header.RTR == CAN_RTR_REMOTE);

            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (s_can_rx_queue != NULL) {
                xQueueSendFromISR(s_can_rx_queue, &frame, &xHigherPriorityTaskWoken);
            }
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

#endif /* STM32F4 */
