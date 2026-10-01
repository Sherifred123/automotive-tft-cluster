/**
 * @file hal_nvm.h
 * @brief Hardware Abstraction Layer for Non-Volatile Memory (Internal EEPROM / Flash Emulation).
 * 
 * Provides byte and block level read/write operations with boundary protection
 * for automotive odometer persistence and configuration parameters.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef HAL_NVM_H
#define HAL_NVM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Total NVM Allocated Space for Cluster Data */
#define HAL_NVM_TOTAL_SIZE_BYTES  (1024U)

typedef enum {
    HAL_NVM_OK = 0,
    HAL_NVM_ERR_OUT_OF_BOUNDS,
    HAL_NVM_ERR_WRITE_FAILED,
    HAL_NVM_ERR_PARAM
} hal_nvm_status_t;

/**
 * @brief Initialize NVM peripheral (internal EEPROM or Flash sector emulation).
 * @return HAL_NVM_OK on success.
 */
hal_nvm_status_t hal_nvm_init(void);

/**
 * @brief Read a block of bytes from non-volatile storage.
 * @param offset Starting byte address within NVM partition (0 to HAL_NVM_TOTAL_SIZE_BYTES - 1).
 * @param p_dest Destination RAM buffer.
 * @param length Number of bytes to read.
 * @return HAL_NVM_OK on success.
 */
hal_nvm_status_t hal_nvm_read(uint16_t offset, void *p_dest, size_t length);

/**
 * @brief Write a block of bytes to non-volatile storage.
 * @param offset Starting byte address within NVM partition.
 * @param p_src Source RAM buffer.
 * @param length Number of bytes to write.
 * @return HAL_NVM_OK on success.
 */
hal_nvm_status_t hal_nvm_write(uint16_t offset, const void *p_src, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* HAL_NVM_H */
