/**
 * @file port_pic18_eeprom.c
 * @brief Microchip PIC18F46K22 Internal EEPROM Driver (XC8 Compiler).
 * 
 * Hardware register-level Data EEPROM read/write with atomic unlock sequences.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, Microchip XC8 C99
 */

#include "../../hal/hal_nvm.h"

#ifdef __XC8
#include <xc.h>

hal_nvm_status_t hal_nvm_init(void)
{
    return HAL_NVM_OK;
}

hal_nvm_status_t hal_nvm_read(uint16_t offset, void *p_dest, size_t length)
{
    if ((p_dest == (void *)0) || ((offset + length) > HAL_NVM_TOTAL_SIZE_BYTES)) {
        return HAL_NVM_ERR_OUT_OF_BOUNDS;
    }

    uint8_t *p_byte = (uint8_t *)p_dest;
    for (size_t i = 0; i < length; i++) {
        uint16_t addr = offset + (uint16_t)i;
        EEADRH = (uint8_t)(addr >> 8U);
        EEADR  = (uint8_t)(addr & 0xFFU);
        EECON1bits.EEPGD = 0; /* Data EEPROM */
        EECON1bits.CFGS  = 0;
        EECON1bits.RD    = 1; /* Initiate Read */
        p_byte[i] = EEDATA;
    }
    return HAL_NVM_OK;
}

hal_nvm_status_t hal_nvm_write(uint16_t offset, const void *p_src, size_t length)
{
    if ((p_src == (void *)0) || ((offset + length) > HAL_NVM_TOTAL_SIZE_BYTES)) {
        return HAL_NVM_ERR_OUT_OF_BOUNDS;
    }

    const uint8_t *p_byte = (const uint8_t *)p_src;
    for (size_t i = 0; i < length; i++) {
        uint16_t addr = offset + (uint16_t)i;
        EEADRH = (uint8_t)(addr >> 8U);
        EEADR  = (uint8_t)(addr & 0xFFU);
        EEDATA = p_byte[i];

        EECON1bits.EEPGD = 0;
        EECON1bits.CFGS  = 0;
        EECON1bits.WREN  = 1; /* Enable Writes */

        uint8_t gie_status = INTCONbits.GIE;
        INTCONbits.GIE = 0;   /* Disable Interrupts for Unlock Sequence */

        /* Required Microchip Unlock Sequence */
        EECON2 = 0x55;
        EECON2 = 0xAA;
        EECON1bits.WR = 1;    /* Initiate Write */

        INTCONbits.GIE = gie_status; /* Restore Interrupts */

        while (EECON1bits.WR) {
            /* Wait for internal EEPROM write cycle (~4ms) */
        }
        EECON1bits.WREN = 0;  /* Disable Writes */
    }
    return HAL_NVM_OK;
}

#endif /* __XC8 */
