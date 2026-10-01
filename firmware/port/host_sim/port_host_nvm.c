/**
 * @file port_host_nvm.c
 * @brief Host PC In-Memory EEPROM / NVM Driver.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "../../hal/hal_nvm.h"
#include <string.h>

static uint8_t s_host_nvm_storage[HAL_NVM_TOTAL_SIZE_BYTES];

hal_nvm_status_t hal_nvm_init(void)
{
    return HAL_NVM_OK;
}

hal_nvm_status_t hal_nvm_read(uint16_t offset, void *p_dest, size_t length)
{
    if ((p_dest == (void *)0) || ((offset + length) > HAL_NVM_TOTAL_SIZE_BYTES)) {
        return HAL_NVM_ERR_OUT_OF_BOUNDS;
    }
    (void)memcpy(p_dest, &s_host_nvm_storage[offset], length);
    return HAL_NVM_OK;
}

hal_nvm_status_t hal_nvm_write(uint16_t offset, const void *p_src, size_t length)
{
    if ((p_src == (void *)0) || ((offset + length) > HAL_NVM_TOTAL_SIZE_BYTES)) {
        return HAL_NVM_ERR_OUT_OF_BOUNDS;
    }
    (void)memcpy(&s_host_nvm_storage[offset], p_src, length);
    return HAL_NVM_OK;
}

/* Host Test Helper: Reset NVM memory */
void port_host_nvm_reset(void)
{
    (void)memset(s_host_nvm_storage, 0xFF, sizeof(s_host_nvm_storage));
}
