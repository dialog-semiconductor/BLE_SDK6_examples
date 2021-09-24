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

#define SEND_I2C_COMMAND(X)                SetWord16(I2C_DATA_CMD_REG, (X))
#define WAIT_WHILE_I2C_FIFO_IS_FULL()      while((GetWord16(I2C_STATUS_REG) & TFNF) == 0)
#define WAIT_UNTIL_I2C_FIFO_IS_EMPTY()     while((GetWord16(I2C_STATUS_REG) & TFE) == 0)
#define WAIT_UNTIL_NO_MASTER_ACTIVITY()    while((GetWord16(I2C_STATUS_REG) & MST_ACTIVITY) !=0)
#define WAIT_FOR_RECEIVED_BYTE()           while(GetWord16(I2C_RXFLR_REG) == 0)

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
    SetBits16(CLK_PER_REG, I2C_ENABLE, 1);                                        // enable  clock for I2C
    SetWord16(I2C_ENABLE_REG, 0x0);                                               // Disable the I2C controller
    SetWord16(I2C_CON_REG, I2C_MASTER_MODE | I2C_SLAVE_DISABLE | I2C_RESTART_EN); // Slave is disabled
    SetBits16(I2C_CON_REG, I2C_SPEED, 2);                                         // Set I2C to 400k Hz speed
    SetBits16(I2C_CON_REG, I2C_10BITADDR_MASTER, 0);                              // Set 7bit addressing mode
    SetWord16(I2C_TAR_REG, I2C_SLAVE_ADDRESS & 0x3FF);                            // Set Slave device address
    SetWord16(I2C_ENABLE_REG, 0x1);                                               // Enable the I2C controller
    WAIT_UNTIL_NO_MASTER_ACTIVITY();                                              // Wait for I2C master FSM to become IDLE

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
        
        // Critical section
        GLOBAL_INT_DISABLE();

        SEND_I2C_COMMAND(reg & 0xFF);                    // Set address LSB, write access

        SEND_I2C_COMMAND(value[0] & 0xFF);                  // Send write byte

        // End of critical section
        GLOBAL_INT_RESTORE();

        WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
        WAIT_UNTIL_NO_MASTER_ACTIVITY();                // Wait until no master activity
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
        /** I2C read function */
        int j;

        // Critical section
        GLOBAL_INT_DISABLE();

        SEND_I2C_COMMAND(reg & 0xFF);                    // Set address LSB, write access

        for (j = 0; j < size; j++)
        {
            WAIT_WHILE_I2C_FIFO_IS_FULL();              // Wait if Tx FIFO is full
            SEND_I2C_COMMAND(0x0100);                   // Set read access for <size> times
        }

        // End of critical section
        GLOBAL_INT_RESTORE();
        
        // Get the received data
        for (j = 0; j < size; j++)
        {
            WAIT_FOR_RECEIVED_BYTE();                   // Wait for received data
            *value =(0xFF & GetWord16(I2C_DATA_CMD_REG));  // Get the received byte
            (value)++;
        }

        WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
        WAIT_UNTIL_NO_MASTER_ACTIVITY();                // wait until no master activity
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

