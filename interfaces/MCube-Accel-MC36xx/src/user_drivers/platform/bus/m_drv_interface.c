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
 * @file    m_drv_interface.c
 * @author  mCube
 * @date    22 June 2018
 * @brief   System I2C/SPI interface for mCube drivers.
 * @see     http://www.mcubemems.com
 */

/******************************************************************************
 *** INCLUDE FILES
 *****************************************************************************/
/** Please include interface driver from platform SDK */
#include "user_periph_setup.h"

/** mCube functions include */
#include "m_drv_interface.h"

/******************************************************************************
 *** CONFIGURATION
 *****************************************************************************/

/******************************************************************************
 *** CONSTANT / DEFINE
 *****************************************************************************/
#if 0
enum I2C_SPEED_MODES
{
    I2C_STANDARD = 1,
    I2C_FAST,           // fast mode (400 kbits/s)
};

enum I2C_ADDRESS_MODES
{
    I2C_7BIT_ADDR,
    I2C_10BIT_ADDR,
};
#endif
#define I2C_SLAVE_ADDRESS 0x4C            // Set slave device address

/******************************************************************************
 *** STATIC VARIABLE
 *****************************************************************************/

/******************************************************************************
 *** FUNCTION
 *****************************************************************************/
/** Delay required milliseconds */
void mcube_delay_ms(unsigned int ms)
{
    /** Function hook by customer. */
}

/** I2C init function */
int m_drv_i2c_init(void)
{
    /** Function hook by customer. */
	  i2c_cfg_t i2c_configuration;
    memset(&i2c_configuration, 0, sizeof(i2c_cfg_t));
    i2c_configuration.speed = I2C_SPEED_FAST;
    i2c_configuration.mode = I2C_MODE_MASTER;
    i2c_configuration.addr_mode = I2C_ADDRESSING_7B;
    i2c_configuration.address = I2C_SLAVE_ADDRESS;
	  i2c_configuration.restart_en = I2C_RESTART_ENABLE;
    i2c_configuration.clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET;
    i2c_configuration.clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET;
    i2c_configuration.clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET;
    i2c_configuration.clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET;
		i2c_configuration.tx_fifo_level     = 32;
		i2c_configuration.rx_fifo_level     = 32;
			
    //Initialisation	
	  i2c_init(&i2c_configuration);

    return 0;
}


/** SPI init function */
int m_drv_spi_init(e_m_drv_interface_spimode_t spi_hs_mode)
{
    /** Function hook by customer. */
    return 0;
}

/** I2C/SPI write function */
/** bSpi : I2C/SPI bus selection.        SPI: 0,       I2C: 1           */
/** chip_select : Chip selection.        SPI: CS pins, I2C: I2C address */
/** reg : Sensor registers. */
/** value : Write value.*/
/** size : data length */
uint8_t mcube_write_regs(bool bSpi, uint8_t chip_select, uint8_t reg,       \
                         uint8_t *value, uint8_t size)
{
    /** Please implement I2C/SPI write function from platform SDK */
    /** 0 = SPI, 1 = I2C */
    if(!bSpi) {
        /** SPI write function */
    } else {
        /** I2C write function */
        i2c_abort_t abrt_code;
        // Critical section
        //GLOBAL_INT_DISABLE();
			  i2c_master_transmit_buffer_sync(&reg, 1, &abrt_code, I2C_F_NONE);
			  i2c_master_transmit_buffer_sync(value, size, &abrt_code, I2C_F_WAIT_FOR_STOP);
        //GLOBAL_INT_RESTORE();
    } 

    return 0;
}

/** I2C/SPI write function */
/** bSpi : I2C/SPI bus selection.        SPI: 0,       I2C: 1           */
/** chip_select : Chip selection.        SPI: CS pins, I2C: I2C address */
/** reg : Sensor registers. */
/** value : read value.*/
/** size : data length */
unsigned char mcube_read_regs(bool bSpi, uint8_t chip_select, uint8_t reg,  \
                              uint8_t * value, uint8_t size)

{
    /** Please implement I2C/SPI read function from platform SDK */
    /** 0 = SPI, 1 = I2C */
    if(!bSpi) {
        /** SPI read function */
    } else {
        i2c_abort_t abrt_code;
			  i2c_master_transmit_buffer_sync(&reg, 1, &abrt_code, I2C_F_NONE);
#if defined (__DA14531__)
			  i2c_master_receive_buffer_sync(value, size, &abrt_code, I2C_F_ADD_STOP);
#else
			  i2c_master_receive_buffer_sync(value, size, &abrt_code, I2C_F_NONE);
#endif
		}
    return 0;
}

/*********************************************************************
 *** mcube_burst_read
 *********************************************************************/
uint8_t mcube_burst_read(uint8_t address, uint8_t *buf, uint8_t size)
{
    return mcube_read_regs(0, 0, address, buf, size);
}

