/******************************************************************************
 *
 * Copyright (c) 2018 mCube, Inc.  All rights reserved.
 *
 * This source is subject to the mCube Software License.
 * This software is protected by Copyright and the information and source code
 * contained herein is confidential. The software including the source code
 * may not be copied and the information contained herein may not be used or
 * disclosed except with the written permission of mCube Inc.
 *
 * All other rights reserved.
 *****************************************************************************/

/**
 * @file    m_drv_interface.h
 * @author  mCube
 * @date    22 June 2018
 * @brief   Header of system I2C/SPI interface for mCube drivers.
 * @see     http://www.mcubemems.com
 */

#ifndef __M_DRV_INTERFACE_H__
#define __M_DRV_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *** INCLUDE FILES
 *****************************************************************************/
/** Standard header. */
#include <stdint.h>
#include <stdbool.h>


/******************************************************************************
 *** CONSTANT / DEFINE
 *****************************************************************************/
/** Ambiq SDK using uint32_t for communication. Nordic SDK using uint8_t. */
#define uint_dev    uint8_t

#define m_drv_buffer(A)                                                       \
    union                                                                     \
    {                                                                         \
        uint32_t words[(A + 3) >> 2];                                         \
        uint8_t  bytes[A];                                                    \
    }

// Select I2C settings
#if defined (__DA14531__)
		
#define I2C_GPIO_PORT    GPIO_PORT_0
#define I2C_SCL_PIN      GPIO_PIN_7
#define I2C_SDA_PIN      GPIO_PIN_8
		
#else

#define I2C_GPIO_PORT     GPIO_PORT_2//GPIO_PORT_0
#define I2C_SCL_PIN       GPIO_PIN_7//GPIO_PIN_2
#define I2C_SDA_PIN       GPIO_PIN_8//GPIO_PIN_3

#endif


/******************************************************************************
 *** DATA TYPE / STRUCTURE DEFINITION / ENUM
 *****************************************************************************/
typedef enum
{
    /** SPI run under 2MHz when normal mode enable. */
    E_M_DRV_INTERFACE_SPIMODE_NORMAL = 0,
    /** SPI bus could over 2MHz after enable high speed mode. */
    E_M_DRV_INTERFACE_SPIMODE_HS,
    E_M_DRV_INTERFACE_SPIMODE_END,
}   e_m_drv_interface_spimode_t;

/* mCube ratcodes definition. */
typedef enum
{
    E_M_DRV_MCUBE_RATCODE_SUCCESS               =     (0),
    E_M_DRV_MCUBE_RATCODE_ERROR_BUS             =    (-1),
    E_M_DRV_MCUBE_RATCODE_ERROR_NULL_POINTER    =    (-2),
    E_M_DRV_MCUBE_RATCODE_ERROR_STATUS          =    (-3),
    E_M_DRV_MCUBE_RATCODE_ERROR_SETUP           =    (-4),
    E_M_DRV_MCUBE_RATCODE_ERROR_GET_DATA        =    (-5),
    E_M_DRV_MCUBE_RATCODE_ERROR_IDENTIFICATION  =    (-6),
    E_M_DRV_MCUBE_RATCODE_ERROR_NO_DATA         =    (-7),
    E_M_DRV_MCUBE_RATCODE_ERROR_WRONG_ARGUMENT  =    (-8),
}   e_m_drv_mcube_ratcode_t;

/******************************************************************************
 *** GLOBAL VARIABLE
 *****************************************************************************/
/** Platform dependent bus functions. */
extern int m_drv_i2c_init(void);
extern int m_drv_spi_init(e_m_drv_interface_spimode_t spi_hs_mode);

extern void mcube_delay_ms(unsigned int ms);
extern uint8_t mcube_write_regs(bool bSpi, uint8_t chip_select, uint8_t reg,\
                                uint8_t *value, uint8_t size);
extern uint8_t mcube_read_regs( bool bSpi, uint8_t chip_select, uint8_t reg,\
                                uint8_t *value, uint8_t size);
extern uint8_t mcube_burst_read(uint8_t reg, uint8_t *value, uint8_t size);
extern uint8_t mcube_burst_read_multi(uint8_t chip_select, uint8_t reg,     \
                                      uint8_t *value, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __M_DRV_INTERFACE_H__
