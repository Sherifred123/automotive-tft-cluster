/**
 * @file hal_can.h
 * @brief Hardware Abstraction Layer for CAN 2.0B Controller.
 * 
 * Standardized interface for CAN frame transmission, reception, and filter configuration.
 * Supports standard (11-bit) and extended (29-bit) identifiers (ISO 11898-1 / SAE J1939).
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef HAL_CAN_H
#define HAL_CAN_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Standard CAN Baud Rates */
typedef enum {
    CAN_BAUD_125K = 0,
    CAN_BAUD_250K,
    CAN_BAUD_500K,   /* Standard Automotive Powertrain */
    CAN_BAUD_1000K
} can_baud_rate_t;

/* CAN Frame Structure */
typedef struct {
    uint32_t id;         /**< Standard (11-bit) or Extended (29-bit) CAN ID */
    uint8_t  dlc;        /**< Data Length Code (0 - 8 bytes) */
    bool     is_extended;/**< True for 29-bit CAN ID (J1939), False for 11-bit */
    bool     is_rtr;     /**< True for Remote Transmission Request */
    uint8_t  data[8];    /**< Payload bytes */
} can_frame_t;

/* CAN Interface Status */
typedef enum {
    HAL_CAN_OK = 0,
    HAL_CAN_ERR_BUFFER_FULL,
    HAL_CAN_ERR_BUFFER_EMPTY,
    HAL_CAN_ERR_BUS_OFF,
    HAL_CAN_ERR_PARAM,
    HAL_CAN_ERR_TIMEOUT
} hal_can_status_t;

/**
 * @brief Initialize CAN hardware controller and set baud rate.
 * @param baud Target baud rate (e.g. 500 kbps for automotive).
 * @return HAL_CAN_OK on success.
 */
hal_can_status_t hal_can_init(can_baud_rate_t baud);

/**
 * @brief Transmit a CAN frame via hardware mailbox / FIFO.
 * @param p_frame Pointer to CAN frame to transmit.
 * @return HAL_CAN_OK on success, HAL_CAN_ERR_BUFFER_FULL if tx queue full.
 */
hal_can_status_t hal_can_transmit(const can_frame_t *p_frame);

/**
 * @brief Receive a CAN frame from the hardware Rx FIFO / Ring buffer.
 * @param p_frame Pointer to store received CAN frame.
 * @return HAL_CAN_OK on success, HAL_CAN_ERR_BUFFER_EMPTY if no frames waiting.
 */
hal_can_status_t hal_can_receive(can_frame_t *p_frame);

/**
 * @brief Configure acceptance filter mask for hardware message filtering.
 * @param filter_id Match ID.
 * @param filter_mask Acceptance mask (1 = match bit, 0 = ignore bit).
 * @param is_extended True for 29-bit extended IDs.
 * @return HAL_CAN_OK on success.
 */
hal_can_status_t hal_can_set_filter(uint32_t filter_id, uint32_t filter_mask, bool is_extended);

/**
 * @brief Check if CAN controller is in Bus-Off or Error Passive state.
 * @return True if bus error active, false if operating normally.
 */
bool hal_can_is_bus_off(void);

/**
 * @brief Perform automatic bus-off recovery sequence.
 */
void hal_can_bus_recovery(void);

#ifdef __cplusplus
}
#endif

#endif /* HAL_CAN_H */
