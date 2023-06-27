/******************************************************************************
 *
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
 */

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

/** mCube functions include */
#include "m_drv_console.h"

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
#define I2C_GPIO_PORT     GPIO_PORT_1//GPIO_PORT_0
#define I2C_SCL_PIN       GPIO_PIN_0//GPIO_PIN_2
#define I2C_SDA_PIN       GPIO_PIN_2//GPIO_PIN_3

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

extern void mcube_delay_ms(uint32_t ms);
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