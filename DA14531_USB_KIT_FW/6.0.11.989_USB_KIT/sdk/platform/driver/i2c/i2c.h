/**
 ****************************************************************************************
 *
 * @file i2c.h
 *
 * @brief Definition of API for the I2C Low Level Driver for DA14585, DA14586 and DA14531 devices.
 *
 * Copyright (c) 2018-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef _I2C_H_
#define _I2C_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include "datasheet.h"
#include "dma.h"

/*
 * DEFINITIONS
 *****************************************************************************************
 */

/// Flags passed to master transmit/receive operations
enum
{
    /// No special command for the operation
    I2C_F_NONE =            0x00000000,

    /// Operation will wait until stop condition occurs
    I2C_F_WAIT_FOR_STOP =   0x00000001,

#if defined (__DA14531__)
    /// Add stop condition after read or write
    I2C_F_ADD_STOP      =   0x00000002,
#endif
};


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// I2C registers (@ 0x50001300)
typedef struct
{
    /// I2C Control Register (@ 0x00000000)
    volatile uint16_t I2C_CON_REGF;
    uint16_t  RESERVED;

    /// I2C Target Address Register (@ 0x00000004)
    volatile uint16_t I2C_TAR_REGF;
    uint16_t  RESERVED1;

    /// I2C Slave Address Register (@ 0x00000008)
    volatile uint16_t I2C_SAR_REGF;
    uint16_t  RESERVED2[3];

    /// I2C Rx/Tx Data Buffer and Command Register (@ 0x00000010)
    volatile uint16_t I2C_DATA_CMD_REGF;
    uint16_t  RESERVED3;

    /// Standard Speed I2C Clock SCL High Count Register (@ 0x00000014)
    volatile uint16_t I2C_SS_SCL_HCNT_REGF;
    uint16_t  RESERVED4;

    /// Standard Speed I2C Clock SCL Low Count Register (@ 0x00000018)
    volatile uint16_t I2C_SS_SCL_LCNT_REGF;
    uint16_t  RESERVED5;

    /// Fast Speed I2C Clock SCL High Count Register (@ 0x0000001C)
    volatile uint16_t I2C_FS_SCL_HCNT_REGF;
    uint16_t  RESERVED6;

    /// Fast Speed I2C Clock SCL Low Count Register (@ 0x00000020)
    volatile uint16_t I2C_FS_SCL_LCNT_REGF;
    uint16_t  RESERVED7[5];

    /// I2C Interrupt Status Register (@ 0x0000002C)
    volatile uint16_t I2C_INTR_STAT_REGF;
    uint16_t  RESERVED8;

    /// I2C Interrupt Mask Register (@ 0x00000030)
    volatile uint16_t I2C_INTR_MASK_REGF;
    uint16_t  RESERVED9;

    /// I2C Raw Interrupt Status Register (@ 0x00000034)
    volatile uint16_t I2C_RAW_INTR_STAT_REGF;
    uint16_t  RESERVED10;

    /// I2C Receive FIFO Threshold Register (@ 0x00000038)
    volatile uint16_t I2C_RX_TL_REGF;
    uint16_t  RESERVED11;

    /// I2C Transmit FIFO Threshold Register (@ 0x0000003C)
    volatile uint16_t I2C_TX_TL_REGF;
    uint16_t  RESERVED12;

    /// Clear Combined and Individual Interrupt Register (@ 0x00000040)
    volatile uint16_t I2C_CLR_INTR_REGF;
    uint16_t  RESERVED13;

    /// Clear RX_UNDER Interrupt Register (@ 0x00000044)
    volatile uint16_t I2C_CLR_RX_UNDER_REGF;
    uint16_t  RESERVED14;

    /// Clear RX_OVER Interrupt Register (@ 0x00000048)
    volatile uint16_t I2C_CLR_RX_OVER_REGF;
    uint16_t  RESERVED15;

    /// Clear TX_OVER Interrupt Register (@ 0x0000004C)
    volatile uint16_t I2C_CLR_TX_OVER_REGF;
    uint16_t  RESERVED16;

    /// Clear RD_REQ Interrupt Register (@ 0x00000050)
    volatile uint16_t I2C_CLR_RD_REQ_REGF;
    uint16_t  RESERVED17;

    /// Clear TX_ABRT Interrupt Register (@ 0x00000054)
    volatile uint16_t I2C_CLR_TX_ABRT_REGF;
    uint16_t  RESERVED18;

    /// Clear RX_DONE Interrupt Register (@ 0x00000058)
    volatile uint16_t I2C_CLR_RX_DONE_REGF;
    uint16_t  RESERVED19;

    /// Clear ACTIVITY Interrupt Register (@ 0x0000005C)
    volatile uint16_t I2C_CLR_ACTIVITY_REGF;
    uint16_t  RESERVED20;

    /// Clear STOP_DET Interrupt Register (@ 0x00000060)
    volatile uint16_t I2C_CLR_STOP_DET_REGF;
    uint16_t  RESERVED21;

    /// Clear START_DET Interrupt Register (@ 0x00000064)
    volatile uint16_t I2C_CLR_START_DET_REGF;
    uint16_t  RESERVED22;

    /// Clear GEN_CALL Interrupt Register (@ 0x00000068)
    volatile uint16_t I2C_CLR_GEN_CALL_REGF;
    uint16_t  RESERVED23;

    /// I2C Enable Register (@ 0x0000006C)
    volatile uint16_t I2C_ENABLE_REGF;
    uint16_t  RESERVED24;

    /// I2C Status Register (@ 0x00000070)
    volatile uint16_t I2C_STATUS_REGF;
    uint16_t  RESERVED25;

    /// I2C Transmit FIFO Level Register (@ 0x00000074)
    volatile uint16_t I2C_TXFLR_REGF;
    uint16_t  RESERVED26;

    /// I2C Receive FIFO Level Register (@ 0x00000078)
    volatile uint16_t I2C_RXFLR_REGF;
    uint16_t  RESERVED27;

    /// I2C SDA Hold Time Length Register (@ 0x0000007C)
    volatile uint16_t I2C_SDA_HOLD_REGF;
    uint16_t  RESERVED28;

    /// I2C Transmit Abort Source Register (@ 0x00000080)
    volatile uint16_t I2C_TX_ABRT_SOURCE_REGF;
    uint16_t  RESERVED29[3];

    /// DMA Control Register (@ 0x00000088)
    volatile uint16_t I2C_DMA_CR_REGF;
    uint16_t  RESERVED30;

    /// DMA Transmit Data Level Register (@ 0x0000008C)
    volatile uint16_t I2C_DMA_TDLR_REGF;
    uint16_t  RESERVED31;

    /// I2C Receive Data Level Register (@ 0x00000090)
    volatile uint16_t I2C_DMA_RDLR_REGF;
    uint16_t  RESERVED32;

    /// I2C SDA Setup Register (@ 0x00000094)
    volatile uint16_t I2C_SDA_SETUP_REGF;
    uint16_t  RESERVED33;

    /// I2C ACK General Call Register (@ 0x00000098)
    volatile uint16_t I2C_ACK_GENERAL_CALL_REGF;
    uint16_t  RESERVED34;

    /// I2C Enable Status Register (@ 0x0000009C)
    volatile uint16_t I2C_ENABLE_STATUS_REGF;
    uint16_t  RESERVED35;

    /// I2C SS and FS spike suppression limit Size (@ 0x000000A0)
    volatile uint16_t I2C_IC_FS_SPKLEN_REGF;
    uint16_t  RESERVED36[41];

    /// Component Parameter Register (@ 0x000000F4)
    volatile uint16_t I2C_COMP_PARAM1_REGF;

    /// Component Parameter Register 2 (@ 0x000000F6)
    volatile uint16_t I2C_COMP_PARAM2_REGF;

    /// I2C Component Version Register (@ 0x000000F8)
    volatile uint16_t I2C_COMP_VERSION_REGF;

    /// I2C Component2 Version Register (@ 0x000000FA)
    volatile uint16_t I2C_COMP2_VERSIONF;

    /// I2C Component Type Register (@ 0x000000FC)
    volatile uint16_t I2C_COMP_TYPE_REGF;

    /// I2C Component2 Type Register (@ 0x000000FE)
    volatile uint16_t I2C_COMP_TYPE2_REGF;

} i2c_t;

/// I2C Instantiation
static i2c_t * const i2c = (i2c_t *) (I2C_CON_REG);

/// I2C clock status
typedef enum
{
    /// I2C clock disable
    I2C_CLOCK_DISABLE   = 0,

    /// I2C controller enable
    I2C_CLOCK_ENABLE    = 1,

} i2c_clock_t;

/// I2C controller status
typedef enum
{
    /// I2C controller disable
    I2C_CONTROLLER_DISABLE  = 0,

    /// I2C controller enable
    I2C_CONTROLLER_ENABLE   = 1,

} i2c_controller_t;

/// I2C role
typedef enum
{
    /// Slave role
    I2C_MODE_SLAVE  = 0,

    /// Master role
    I2C_MODE_MASTER = 1,

} i2c_mode_t;

/// Restart condition
typedef enum
{
    // Restart condition disabled
    I2C_RESTART_DISABLE = 0,

    // Restart condition enabled
    I2C_RESTART_ENABLE  = 1,

} i2c_restart_t;

/// I2C addressing mode
typedef enum {
    /// 7-bit addressing
    I2C_ADDRESSING_7B   = 0,

    /// 10-bit addressing
    I2C_ADDRESSING_10B  = 1,

} i2c_addressing_t;

/// I2C interface speed
typedef enum
{
    /// 100 kb/s
    I2C_SPEED_STANDARD = 1,

    /// 400 kb/s
    I2C_SPEED_FAST     = 0,

} i2c_speed_t;

/// I2C interrupt source
enum
{
    /// Attempt to read from empty RX FIFO has been made.
    I2C_INT_RX_UNDERFLOW = RX_UNDER,

    /// RX FIFO is full but new data are incoming and being discarded.
    I2C_INT_RX_OVERFLOW = RX_OVER,

    /// RX FIFO level is equal or above threshold.
    I2C_INT_RX_FULL = RX_FULL,

    /// Attempt to write to TX FIFO which is already full.
    I2C_INT_TX_OVERFLOW = TX_OVER,

    /// TX FIFO level is  equal or below threshold.
    I2C_INT_TX_EMPTY = TX_EMPTY,

    /// I2C master attempts to read data (slave only).
    I2C_INT_READ_REQUEST = RD_REQ,

    /// TX cannot be completed.
    I2C_INT_TX_ABORT = TX_ABRT,

    /// I2C master did not acknowledge transmitted byte (slave only).
    I2C_INT_RX_DONE = RX_DONE,

    /// Any I2C activity occurred.
    I2C_INT_ACTIVITY = ACTIVITY,

    /// STOP condition occurred.
    I2C_INT_STOP_DETECTED = STOP_DET,

    /// START/RESTART condition occurred.
    I2C_INT_START_DETECTED = START_DET,

    /// General Call address received (slave only).
    I2C_INT_GENERAL_CALL = GEN_CALL

};

/// I2C abort source
typedef enum
{
    /// No abort occurred.
    I2C_ABORT_NONE                   = 0x00,

    /// Address byte of 7-bit address was not acknowledged by any slave.
    I2C_ABORT_7B_ADDR_NO_ACK         = ABRT_7B_ADDR_NOACK,

    /// 1st address byte of the 10-bit address was not acknowledged by any slave.
    I2C_ABORT_10B_ADDR1_NO_ACK       = ABRT_10ADDR1_NOACK ,

    /// 2nd address byte of the 10-bit address was not acknowledged by any slave.
    I2C_ABORT_10B_ADDR2_NO_ACK       = ABRT_10ADDR2_NOACK,

    /// Data were not acknowledged by slave(master mode).
    I2C_ABORT_TX_DATA_NO_ACK         = ABRT_TXDATA_NOACK,

    /// General Call sent but no slave acknowledged(master mode).
    I2C_ABORT_GENERAL_CALL_NO_ACK    = ABRT_GCALL_NOACK,

    /// Trying to read from bus after General Call(master mode).
    I2C_ABORT_GENERAL_CALL_READ      = ABRT_GCALL_READ,

    /// Master is in High Speed mode and the High Speed Master code was acknowledged (wrong behavior).
    I2C_ABORT_HS_ACKDET              = ABRT_HS_ACKDET,

    ///  Master has sent a START Byte and the START Byte was acknowledged (wrong behavior).
    I2C_ABORT_SBYTE_ACKDET           = ABRT_SBYTE_ACKDET,

    /// High Speed mode with restart disabled.
    I2C_ABORT_HS_NORSTRT             = ABRT_HS_NORSTRT,

    /// Read command in 10-bit addressing mode with RESTART disabled.
    I2C_ABORT_10B_READ_NO_RESTART    = ABRT_10B_RD_NORSTRT,

    /// Master operation initiated with master mode disabled.
    I2C_ABORT_MASTER_DISABLED        = ABRT_MASTER_DIS,

    /// Bus arbitration lost.
    I2C_ABORT_ARBITRATION_LOST       = ARB_LOST,

    /// Request for data with data already in TX FIFO - used to flush data in TX FIFO(slave mode).
    I2C_ABORT_SLAVE_FLUSH_TX_FIFO = ABRT_SLVFLUSH_TXFIFO,

    /// Bus lost when transmitting to master(slave mode).
    I2C_ABORT_SLAVE_ARBITRATION_LOST = ABRT_SLV_ARBLOST,

    /// Request for data replied with read request(slave mode).
    I2C_ABORT_SLAVE_IN_TX            = ABRT_SLVRD_INTX,

    /// Abort due to software error
    I2C_ABORT_SW_ERROR

} i2c_abort_t;

#if defined (CFG_I2C_DMA_SUPPORT)
/// DMA transfer type
typedef enum
{
    /// Master or Slave write transfer
    I2C_DMA_TRANSFER_WRITE          = 0,

    /// Master read transfer
    I2C_DMA_TRANSFER_MASTER_READ    = 1,

    /// Slave read transfer
    I2C_DMA_TRANSFER_SLAVE_READ     = 2,

} i2c_dma_transfer_t;

/// DMA channel-pairs for I2C
typedef enum
{
    I2C_DMA_CHANNEL_PAIR_1 = 0,
    I2C_DMA_CHANNEL_PAIR_2 = 2,
} i2c_dma_channel_pair_t;
#endif

/// Callback called on interrupt from I2C controller
typedef void (*i2c_interrupt_cb_t) (uint16_t mask);

 /// Callback called upon completion of read or write in non-blocking mode (FIFO or DMA)
typedef void (*i2c_complete_cb_t) (void *cb_data, uint16_t len, bool success);

/// I2C configuration
typedef struct
{
    /// I2C clock (SCL) settings, refer to datasheet for details.
    struct
    {
        /// Standard speed I2C clock (SCL) high count
        uint16_t ss_hcnt;

        /// Standard speed I2C clock (SCL) low count
        uint16_t ss_lcnt;

        /// Fast speed I2C clock (SCL) high count
        uint16_t fs_hcnt;

        /// Fast speed I2C clock (SCL) low count
        uint16_t fs_lcnt;

    } clock_cfg;

    /// Restart enable (only valid in master mode). When the RESTART is disabled master is
    /// incapable of performing a read operation with a 10-bit address.
    i2c_restart_t   restart_en;

    /// Bus speed
    i2c_speed_t             speed;

    /// Mode of operation
    i2c_mode_t              mode;

    /// Addressing mode
    i2c_addressing_t        addr_mode;

    /// Target slave address in master mode or controller address in slave mode
    uint16_t                address;

    /// Transmit FIFO level configuration (valid values 1-32)
    uint8_t                 tx_fifo_level;

    /// Receive FIFO level configuration (valid values 1-32)
    uint8_t                 rx_fifo_level;

} i2c_cfg_t;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Set I2C controller status
 *
 * @param[in] Enable or Disable I2C controller
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_controller_status(i2c_controller_t status)
{
    SetBits16(&i2c->I2C_ENABLE_REGF, CTRL_ENABLE, status);
}

/**
 ****************************************************************************************
 * @brief Get I2C Controller Enable status
 * @return The contents of the I2C Controller enable status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE i2c_controller_t i2c_get_controller_status(void)
{
    return (((GetWord16(&i2c->I2C_ENABLE_REGF) & CTRL_ENABLE) == 0) ? I2C_CONTROLLER_DISABLE : I2C_CONTROLLER_ENABLE);
}

/**
 ****************************************************************************************
 * @brief Set I2C clock status
 *
 * @param[in] status for clock
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_clock_status(i2c_clock_t status)
{
    SetBits16(CLK_PER_REG, I2C_ENABLE, status);
}

/**
 ****************************************************************************************
 * @brief Get I2C clock status
 * @return clock status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE i2c_clock_t i2c_get_clock_status(void)
{
    return ((GetBits16(CLK_PER_REG, I2C_ENABLE) == 0) ? I2C_CLOCK_DISABLE : I2C_CLOCK_ENABLE);
}

/**
*****************************************************************************************
 * @brief Set I2C controller mode
 *
 * Can be only set when controller is disabled.
 * @param[in] mode mode to be set
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_mode(i2c_mode_t mode)
{
    // default to master mode, if incorrect value specified
    SetBits16(&i2c->I2C_CON_REGF, I2C_MASTER_MODE, (mode == I2C_MODE_SLAVE) ? I2C_MODE_SLAVE : I2C_MODE_MASTER);
    SetBits16(&i2c->I2C_CON_REGF, I2C_SLAVE_DISABLE, (mode == I2C_MODE_SLAVE) ? I2C_MODE_SLAVE : I2C_MODE_MASTER);
}

/**
*****************************************************************************************
 * @brief Get I2C controller mode
 *
 * @return Mode of I2C controller
 ****************************************************************************************
 */
__STATIC_FORCEINLINE i2c_mode_t i2c_get_mode(void)
{
    return (((GetBits16(&i2c->I2C_CON_REGF, I2C_MASTER_MODE) == 0) & (GetBits16(&i2c->I2C_CON_REGF, I2C_SLAVE_DISABLE) == 0)) ? I2C_MODE_SLAVE : I2C_MODE_MASTER);
}

/**
 ****************************************************************************************
 * @brief Set I2C interface bus speed
 *
 * Can be only set when controller is disabled.
 * @param[in] speed speed to be set
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_speed(i2c_speed_t speed)
{
    // default to standard mode (100kbit/s), if incorrect value specified
    SetBits16(&i2c->I2C_CON_REGF, I2C_SPEED, (speed == I2C_SPEED_FAST) ? I2C_SPEED_FAST : I2C_SPEED_STANDARD);
}

/**
 ****************************************************************************************
 * @brief Get I2C interface bus speed
 *
 * @return I2C bus speed
 ****************************************************************************************
 */
__STATIC_FORCEINLINE i2c_speed_t i2c_get_speed(void)
{
    return ((GetBits16(&i2c->I2C_CON_REGF, I2C_SPEED) == I2C_SPEED_STANDARD) ? I2C_SPEED_STANDARD : I2C_SPEED_FAST);
}

/**
 ****************************************************************************************
 * @brief Set whether RESTART conditions may be sent when acting as master
 *
 * Can be only set when controller is disabled.
 * When the RESTART is disabled master is incapable of performing a read operation with a 10-bit address.
 * @param[in] enabled RESTART status to be set
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_restart_status(i2c_restart_t restart)
{
    SetBits16(&i2c->I2C_CON_REGF, I2C_RESTART_EN, restart);
}

/**
*****************************************************************************************
 * @brief Set target slave addressing mode in master mode
 *
 * Can be only set when controller is disabled.
 * @param[in] addr_mode mode of addressing
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_target_addressing_mode(i2c_addressing_t addr_mode)
{
    SetBits16(&i2c->I2C_CON_REGF, I2C_10BITADDR_MASTER, addr_mode);
}

/**
 ****************************************************************************************
 * @brief Set target slave address in master mode
 *
 * Can be only set when controller is disabled.
 * @param[in] address target(slave) address
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_target_address(uint16_t address)
{
    SetBits16(&i2c->I2C_TAR_REGF, IC_TAR, address);
}

/**
 ****************************************************************************************
 * @brief Get target address
 *
 * @return target address
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t i2c_get_target_address(void)
{
    return GetBits16(&i2c->I2C_TAR_REGF, IC_TAR);
}

/**
 ****************************************************************************************
 * @brief Setup controller for operation in master mode
 *
 * Shortcut for calling i2c_set_mode(), i2c_set_target_addressing_mode() and
 * i2c_set_target_address().
 *
 * Can be only set when controller is disabled.
 * @param[in] restart enable restart condition
 * @param[in] addr_mode mode of addressing
 * @param[in] address target slave address
 * @return void
 ****************************************************************************************
 */
void i2c_setup_master(i2c_restart_t restart, i2c_addressing_t addr_mode, uint16_t target_addr);

/**
*****************************************************************************************
 * @brief Set slave addressing mode in slave mode
 *
 * Can be only set when controller is disabled.
 * @param[in] addr_mode mode of addressing
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_slave_addressing_mode(i2c_addressing_t addr_mode)
{
    SetBits16(&i2c->I2C_CON_REGF, I2C_10BITADDR_SLAVE, addr_mode);
}

/**
*****************************************************************************************
 * @brief Set slave address in slave mode
 *
 * Can be only set when controller is disabled.
 * @param[in] address slave address
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_slave_address(uint16_t address)
{
    SetBits16(&i2c->I2C_SAR_REGF, IC_SAR, address);
}
/**
*****************************************************************************************
 * @brief Set whether General Call should be used to address slaves
 *
 * Can be only set when controller is disabled.
 * @param[in] enabled General Call status to be set
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_general_call_enabled(bool enabled)
{
    SetBits16(&i2c->I2C_TAR_REGF, SPECIAL, !!enabled);
    SetBits16(&i2c->I2C_TAR_REGF, GC_OR_START, !enabled);
}

/**
*****************************************************************************************
 * @brief Set support for general call acknowledgment
 *
 * When enabled, controller will send ACK for general call address.
 * This applies only to controller working in slave mode.
 *
 * @param[in] ack acknowledgment status
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_general_call_ack_enabled(bool ack)
{
    SetBits16(&i2c->I2C_ACK_GENERAL_CALL_REGF, ACK_GEN_CALL, !!ack);
}

/**
*****************************************************************************************
 * @brief Setup controller for operation in slave mode
 *
 * Shortcut for calling i2c_set_mode(), i2c_set_slave_addressing_mode() and
 * i2c_set_slave_address().
 *
 * @param[in] addr_mode mode of addressing
 * @param[in] address slave address
 * @return void
 *
 * @return void
 ****************************************************************************************
 */
void i2c_setup_slave(i2c_addressing_t addr_mode, uint16_t addr);

/**
*****************************************************************************************
 * @brief Initialize I2C controller
 *
 * I2C controller is disabled, clock and interrupt for I2C component are enabled, all interrupts are
 * masked though.
 * @param[in] cfg configuration
 * @return void
 ****************************************************************************************
 */
void i2c_init(const i2c_cfg_t *cfg);

/**
 ****************************************************************************************
 * @brief Disable I2C controller and clock.
 *
 * @return void
 ****************************************************************************************
 */
void i2c_release(void);

/**
 ****************************************************************************************
 * @brief Register interrupt handler
 *
 * @param[in] cb callback function
 * @param[in] mask initial bitmask of requested interrupt events
 * @return void
 ****************************************************************************************
 */
void i2c_register_int(i2c_interrupt_cb_t cb, uint16_t mask);

/**
 ****************************************************************************************
 * @brief Unregister interrupt handler
 *
 * This function disables all I2C interrupts by masking them. In addition
 * it clears any pending ones on the ARM core. The status of RAW_INTR_STAT_REG
 * remains unchanged.
 * @return void
 ****************************************************************************************
 */
void i2c_unregister_int(void);

/**
 ****************************************************************************************
 * @brief Set I2C interrupt mask
 *
 * This register is active low; a value of 0 masks the interrupt, whereas a value of 1
 * unmasks the interrupt.
 *
 * @param[in] mask bitmask of requested interrupt events
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_int_mask(uint16_t mask)
{
    SetWord16(&i2c->I2C_INTR_MASK_REGF, mask);
}

/**
 ****************************************************************************************
 * @brief Get I2C interrupt mask
 *
 * @return current bitmask
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t i2c_get_int_mask(void)
{
    return GetWord16(&i2c->I2C_INTR_MASK_REGF);
}

/**
 ****************************************************************************************
 * @brief Get interrupt state
 *
 * Interrupt state returned includes only interrupts which are not masked. For raw interrupt status
 * use i2c_get_raw_int_state().
 *
 * @return current interrupt state bitmask
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t i2c_get_int_state(void)
{
    return GetWord16(&i2c->I2C_INTR_STAT_REGF);
}

/**
 ****************************************************************************************
 * @brief Get raw interrupt state
 *
 * @return current interrupt state bitmask
 ****************************************************************************************
 */
__STATIC_INLINE uint16_t i2c_get_raw_int_state(void)
{
    return GetWord16(&i2c->I2C_RAW_INTR_STAT_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset all interrupt state
 *
 * This does reset all interrupts which can be reset by software and TX_ABORT status.
 *
 * @warning Although this also clears TX_ABORT it does not reset flushed state on TX FIFO. This has
 * to be cleared manually using i2c_reset_abort_source().
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_all(void)
{
    GetWord16(&i2c->I2C_CLR_INTR_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset RX_UNDERFLOW interrupt state
 *
 * Should be used to reset RX_UNDERFLOW interrupt.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_rx_underflow(void)
{
    GetWord16(&i2c->I2C_CLR_RX_UNDER_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset RX_OVERFLOW interrupt state
 *
 * Should be used to reset RX_OVERFLOW interrupt.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_rx_overflow(void)
{
    GetWord16(&i2c->I2C_CLR_RX_OVER_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset TX_OVERFLOW interrupt state
 *
 * Should be used to reset TX_OVERFLOW interrupt.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_tx_overflow(void)
{
    GetWord16(&i2c->I2C_CLR_TX_OVER_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset READ_REQUEST interrupt state
 *
 * Should be used to reset READ_REQUEST interrupt.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_read_request(void)
{
    GetWord16(&i2c->I2C_CLR_RD_REQ_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset TX_ABORT interrupt state
 *
 * Should be used to reset TX_ABORT interrupt.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_tx_abort(void)
{
    GetWord16(&i2c->I2C_CLR_TX_ABRT_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset RX_DONE interrupt state
 *
 * Should be used to reset RX_DONE interrupt.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_rx_done(void)
{
    GetWord16(&i2c->I2C_CLR_RX_DONE_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset ACTIVITY interrupt state
 *
 * Should be used to reset ACTIVITY interrupt.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_activity(void)
{
    GetWord16(&i2c->I2C_CLR_ACTIVITY_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset START_DETECTED interrupt state
 *
 * Should be used to reset START_DETECTED interrupt.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_start_detected(void)
{
    GetWord16(&i2c->I2C_CLR_START_DET_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset STOP_DETECTED interrupt state
 *
 * Should be used to reset STOP_DETECTED interrupt.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_stop_detected(void)
{
    GetWord16(&i2c->I2C_CLR_STOP_DET_REGF);
}

/**
 ****************************************************************************************
 * @brief Reset GENERAL_CALL interrupt state
 *
 * Should be used to reset GENERAL_CALL interrupt.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_reset_int_gen_call(void)
{
    GetWord16(&i2c->I2C_CLR_GEN_CALL_REGF);
}

/**
 ****************************************************************************************
 * @brief Set threshold level on TX FIFO
 *
 * An interrupt will be generated once number of entries in TX FIFO is less or equal to @p level.
 * The valid range is 0-31.
 *
 * @param[in] level     fifo level threshold
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_tx_fifo_threshold(uint8_t level)
{
    SetBits16(&i2c->I2C_TX_TL_REGF, RX_TL, level);
}

/**
 ****************************************************************************************
 * @brief Get threshold level on TX FIFO
 *
 * @return TX FIFO threshold
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t i2c_get_tx_fifo_threshold(void)
{
    return GetBits16(&i2c->I2C_TX_TL_REGF, RX_TL);
}

/**
 ****************************************************************************************
 * @brief Check if TX FIFO queue is empty
 *
 * This function should be used to check if all data written to TX FIFO were transmitted.
 * @return TX FIFO empty status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool i2c_is_tx_fifo_empty(void)
{
    return (GetWord16(&i2c->I2C_STATUS_REGF) & TFE);
}

/**
 ****************************************************************************************
 * @brief Check if TX FIFO is not full
 *
 * This function should be used to check if data can be written to TX FIFO.
 * @return TX FIFO not full status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool i2c_is_tx_fifo_not_full(void)
{
    return (GetWord16(&i2c->I2C_STATUS_REGF) & TFNF);
}

/**
 ****************************************************************************************
 * @brief Get number of bytes in TX FIFO
 *
 * @return number of bytes
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t i2c_get_tx_fifo_level(void)
{
    return GetBits16(&i2c->I2C_TXFLR_REGF, TXFLR);
}

/**
 ****************************************************************************************
 * @brief Set threshold level on RX FIFO
 *
 * An interrupt will be generated once number of entries in RX FIFO is greater than @p level.
 * The valid range is 0-31.
 *
 * @param[in] level     fifo level threshold
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_set_rx_fifo_threshold(uint8_t level)
{
    SetBits16(&i2c->I2C_RX_TL_REGF, RX_TL, level);
}

/**
 ****************************************************************************************
 * @brief Get threshold level on TX FIFO
 *
 * @return RX FIFO threshold
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t i2c_get_rx_fifo_threshold(void)
{
    return GetBits16(&i2c->I2C_RX_TL_REGF, RX_TL);
}

/**
 ****************************************************************************************
 * @brief Check if RX FIFO queue is full
 *
 * This function should be used to check if RX FIFO is filled, i.e. subsequent data read will be
 * discarded.
 * @return RX FIFO full status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool i2c_is_rx_fifo_full(void)
{
    return (GetWord16(&i2c->I2C_STATUS_REGF) & RFF);
}

/**
 ****************************************************************************************
 * @brief Check if RX FIFO is not empty
 *
 * This function should be used to check if there are any data received in RX FIFO.
 * @return RX FIFO not empty status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool i2c_is_rx_fifo_not_empty(void)
{
    return (GetWord16(&i2c->I2C_STATUS_REGF) & RFNE);
}

/**
 ****************************************************************************************
 * @brief Get number of bytes in RX FIFO
 *
 * @return number of bytes
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t i2c_get_rx_fifo_level(void)
{
    return GetBits16(&i2c->I2C_RXFLR_REGF, RXFLR);
}

/**
 ****************************************************************************************
 * @brief Get abort source
 *
 * This can be used to retrieve source of TX_ABORT interrupt. TX FIFO is flushed and
 * remains in this state until cleared using i2c_reset_int_tx_abort().
 * @return abort source bitmask
 *
 * @sa i2c_reset_abort_source
 ****************************************************************************************
 */
__STATIC_FORCEINLINE i2c_abort_t i2c_get_abort_source(void)
{
    return (i2c_abort_t) (GetWord16(&i2c->I2C_TX_ABRT_SOURCE_REGF) & 0x1FFFF);

}

/**
 ****************************************************************************************
 * @brief Reset abort source
 *
 * This clears TX_ABORT interrupt status and unlocks TX FIFO.
 *
 * @note this is an alias for i2c_reset_int_tx_abort()
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_INLINE void i2c_reset_abort_source(void)
{
    i2c_reset_int_tx_abort();
}

/**
 ****************************************************************************************
 * @brief Check if controller is busy when operating in slave mode
 *
 * @return busy status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool i2c_is_slave_busy(void)
{
    return (GetWord16(&i2c->I2C_STATUS_REGF) & SLV_ACTIVITY);
}

/**
 ****************************************************************************************
 * @brief Check if controller is busy when operating in master mode
 *
 * @return busy status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool i2c_is_master_busy(void)
{
    return (GetWord16(&i2c->I2C_STATUS_REGF) & MST_ACTIVITY);
}

/**
 ****************************************************************************************
 * @brief Check controller activity
 *
 * @return busy status
 ****************************************************************************************
 */
__STATIC_FORCEINLINE bool i2c_controler_is_busy(void)
{
    return (GetWord16(I2C_STATUS_REG) & I2C_ACTIVITY);
}

/**
 ****************************************************************************************
 * @brief Write single byte into TX FIFO
 *
 * It is caller's responsibility to ensure there is free space in TX FIFO before calling this
 * function - either i2c_is_tx_fifo_not_full() or i2c_get_tx_fifo_level() can be used for this
 * purpose.
 *
 * This function can be used in both master and slave modes.
 *
 * @param[in] byte data to write
 *
 * @warning This function does not check for errors during transmission. Use i2c_master_transmit_buffer_sync()
 *          or i2c_master_transmit_buffer_async() instead
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_write_byte(uint16_t data)
{
    SetWord16(&i2c->I2C_DATA_CMD_REGF, data);
}

/**
 ****************************************************************************************
 * @brief Read single byte from RX FIFO
 *
 * It is caller's responsibility to ensure there is data to read in RX FIFO before calling this
 * function by checking either i2c_get_rx_fifo_level() or i2c_rx_fifo_not_empty().
 *
 * This function can be used in both master and slave modes.
 *
 * @return read byte
 *
 * @sa i2c_get_rx_fifo_level
 * @sa i2c_rx_fifo_not_empty
 *
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t i2c_read_byte(void)
{
#if defined (__DA14531__)
    return GetBits16(&i2c->I2C_DATA_CMD_REGF, DAT);
#else
    return GetBits16(&i2c->I2C_DATA_CMD_REGF, I2C_DAT);
#endif
}

/**
 ****************************************************************************************
 * @brief Write multiple bytes to I2C slave synchronously
 *
 * Writes array of data to a slave device on the I2C bus via the TX FIFO. This call blocks until
 * the operation completes. In case of failure the function stores the Tx error code to
 * the abrt_code parameter -if provided- and clears the Tx Abort register.
 *
 * @param[in]     data         Address of the buffer containing the data to write
 * @param[in]     len          Length of data buffer
 * @param[in,out] abrt_code    If not NULL, the status of the operation
 * @param[in]     flags        Possible values for flags are:
 * @parblock
 *      ::I2C_F_NONE<br>
 *      The function will return as soon as the last byte of data is written to the Tx FIFO.
 *      Possible Tx failure after this point must be checked by the caller. This flag can be used
 *      for consecutive calls where the transaction can resume without stop/start conditions in
 *      between. In case of DA14585 STOP condition is automatically generated at the end of the transaction.
 *
 *      ::I2C_F_WAIT_FOR_STOP<br>
 *      The function will return only when all bytes have been transmitted
 *      and a STOP condition has been generated.
 *
 *      ::I2C_F_ADD_STOP<br>
 *      STOP condition will be generated at the end of the transaction. This flag has no effect in
 *      DA14585/6 since STOP condition is automatically generated at the end of the transaction.
 *      The function will return as soon as the last byte of data is written to the Tx FIFO.
 *      Possible Tx failure after this point must be checked by the caller.
 * @endparblock
 *
 * This can be used to retrieve source of TX_ABORT interrupt. TX FIFO is flushed and
 * remains in this state until cleared using i2c_reset_int_tx_abort().
 * @return The number of bytes written to the Tx FIFO. It is strongly recommended to check
 *         the value of abrt_code to make sure that the number of bytes returned were actually
 *         transmitted to the I2C bus.
 *
 * @note    This function should be used only when operating in master mode.
 *
 ****************************************************************************************
 */
uint16_t i2c_master_transmit_buffer_sync(const uint8_t *data, uint16_t len, i2c_abort_t *abrt_code, uint32_t flags);

/**
 ****************************************************************************************
 * @brief Read multiple bytes from I2C slave synchronously
 *
 * This function will read multiple bytes from bus taking care of RX FIFO control. This call blocks
 * until the operation completes. The operation completes when the complete buffer is filled, or in
 * case of a failure. Failures are cleared by the function before returning.
 *
 * @param[in,out]  data         Address of the buffer where data are stored
 * @param[in]      len          Length of the data buffer
 * @param[in,out]  abrt_code    If not NULL, the status of the operation
 * @param[in]      flags        Possible values for flags are:
 * @parblock
 *      ::I2C_F_NONE<br>
 *      This flag can be used for consecutive calls where the transaction can resume without
 *      stop/start conditions in between. In case of DA14585 STOP condition is automatically
 *      generated at the end of the transaction.
 *
 *      ::I2C_F_ADD_STOP<br>
 *      STOP condition will be generated at the end of the transaction. This flag has no effect in
 *      DA14585/6 since STOP condition is automatically generated at the end of the transaction.
 * @endparblock
 *
 * @return The number of bytes read
 *
 * @note This function should be used only when operating in master mode.
 *
 ****************************************************************************************
 */
uint16_t i2c_master_receive_buffer_sync(uint8_t *data, uint16_t len, i2c_abort_t *abrt_code, uint32_t flags);

/**
 ****************************************************************************************
 * @brief Write multiple bytes to I2C slave asynchronously
 *
 * Writes array of data to a slave device on the I2C bus via the TX FIFO. This call is non-blocking
 * and specified callback is called upon completion, with the operation status. In case of failure
 * the failure cause must be read from the cb by calling i2c_get_abort_source().
 *
 * @param[in] data     Address of the buffer containing the data to write
 * @param[in] len      Length of the data buffer
 * @param[in] cb       Callback to be called upon completion
 * @param[in] cb_data  Data to pass to cb
 * @param[in] flags    Possible values for flags are:
 * @parblock
 *      ::I2C_F_NONE<br>
 *      The callback will be called as soon as the last byte of data is written to the Tx FIFO. Possible
 *      Tx failure after this point must be checked by the caller. This flag can be used for
 *      consecutive calls where the transaction can resume without stop/start conditions in
 *      between. In case of DA14585 STOP condition is automatically generated at the end of the transaction.
 *
 *      ::I2C_F_WAIT_FOR_STOP<br>
 *      The callback will be called when all bytes have been transmitted and a STOP condition has
 *      been generated.
 *
 *      ::I2C_F_ADD_STOP<br>
 *      STOP condition will be generated at the end of the transaction. This flag has no effect in
 *      DA14585/6 since STOP condition is automatically generated at the end of the transaction.
 *      The callback will be called as soon as the last byte of data is written to the Tx FIFO. Possible
 *      Tx failure after this point must be checked by the caller.
 * @endparblock
 *
 * @return void
 *
 * @note    This function should be used only when operating in master mode.
 *
 * @warning This function registers an internal interrupt handler, which overrides any previously
 *          installed handler.
 *
 ****************************************************************************************
 */
void i2c_master_transmit_buffer_async(const uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data, uint32_t flags);

/**
 ****************************************************************************************
 * @brief Read multiple bytes from I2C slave asynchronously
 *
 * This function will read multiple bytes from bus taking care of RX FIFO control. This call is
 * non-blocking and specified callback is called upon completion, with the operation status.
 * The operation completes when the complete buffer is filled, or in case of a failure.
 * In case of failure the failure cause must be read from the cb by calling
 * i2c_get_abort_source().
 *
 * @param[in,out] data  Address of the buffer where data are stored
 * @param[in] len       Length of the data buffer
 * @param[in] cb        Callback to be called upon completion
 * @param[in] cb_data   Data to pass to cb
 * @param[in] flags     Possible values for flags are:
 * @parblock
 *      ::I2C_F_NONE<br>
 *      This flag can be used for consecutive calls where the transaction can resume without
 *      stop/start conditions in between. In case of DA14585 STOP condition is automatically
 *      generated at the end of the transaction.
 *
 *      ::I2C_F_ADD_STOP<br>
 *      STOP condition will be generated at the end of the transaction. This flag has no effect in
 *      DA14585/6 since STOP condition is automatically generated at the end of the transaction.
 * @endparblock
 *
 * @return void
 *
 * @note    This function should be used only when operating in master mode.
 *
 * @warning This function registers an internal interrupt handler, which overrides any previously
 *          installed handler.
 *
 ****************************************************************************************
 */
void i2c_master_receive_buffer_async(uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data, uint32_t flags);

#if defined (CFG_I2C_DMA_SUPPORT)
/**
 ****************************************************************************************
 * @brief Transmit multiple bytes on I2C bus using DMA
 *
 * Shortcut for calling i2c_prepare_dma() and i2c_dma_start().
 *
 * @param[in] channel   DMA channel
 * @param[in] data      buffer to write data to
 * @param[in] len       length of the data buffer
 * @param[in] cb        callback for transmit completed
 * @param[in] cb_data   Data to pass to to cb
 * @param[in] flags     Possible values for flags are:
 * @parblock
 *      ::I2C_F_NONE<br>
 *      The cb will be called as soon as the last byte of data is written to the Tx FIFO. Possible
 *      Tx failure after this point must be checked by the caller. This flag can be used for
 *      consecutive calls where the transaction can resume without stop/start conditions in
 *      between. In case of DA14585 STOP condition is automatically generated at the end of the transaction.
 *
 *      ::I2C_F_WAIT_FOR_STOP<br>
 *      The callback will be called when all bytes have been transmitted and a STOP condition has
 *      been generated.
 *
 *      ::I2C_F_ADD_STOP<br>
 *      STOP condition will be generated at the end of the transaction. This flag has no effect in
 *      DA14585/6 since STOP condition is automatically generated at the end of the transaction.
 * @endparblock
 *
 * @return void
 *
 * @note This function should be used only when operating in master mode.
 *
 ****************************************************************************************
 */
void i2c_master_transmit_buffer_dma(i2c_dma_channel_pair_t dma_channel_pair, const uint16_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data, uint32_t flags);

/**
 * @brief Receive multiple bytes from I2C bus using DMA
 *
 * Shortcut for calling i2c_prepare_dma() and i2c_dma_start().
 *
 * @param [in] channel  DMA channel
 * @param [out] data    Buffer to put data read from I2C bus
 * @param [in] len      Length of the data buffer
 * @param [in] cb       Callback for receive completed
 * @param [in] cb_data  Data to pass to cb
 * @param [in] flags    Possible values for flags are:
 * @parblock
 *      ::I2C_F_NONE<br>
 *      This flag can be used for consecutive calls where the transaction can resume without
 *      stop/start conditions in between. In case of DA14585 STOP condition is automatically
 *      generated at the end of the transaction.
 *
 *      ::I2C_F_ADD_STOP<br>
 *      STOP condition will be generated at the end of the transaction. This flag has no effect in
 *      DA14585/6 since STOP condition is automatically generated at the end of the transaction.
 * @endparblock
 *
 * @return void
 *
 * @note This function should be used only when operating in master mode.
 *
 ****************************************************************************************
 */
void i2c_master_receive_buffer_dma(i2c_dma_channel_pair_t dma_channel_pair, uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data, uint32_t flags);
#endif

/**
 ****************************************************************************************
 * @brief Receive multiple bytes from I2C master synchronously
 *
 * This function will receive multiple bytes from master taking care of RX FIFO control. This call blocks
 * until the operation completes. The operation completes when the complete buffer is filled.
 *
 * @param[in,out] data         Address of the buffer where data are stored
 * @param[in]     len          Length of the data buffer
 *
 * @return The number of bytes read
 *
 * @note This function should be used only when operating in slave mode.
 *
 ****************************************************************************************
 */
uint16_t i2c_slave_receive_buffer_sync(uint8_t *data, uint16_t len);

/**
 ****************************************************************************************
 * @brief Transmit multiple bytes to I2C master synchronously
 *
 * Transmit an array of data to a slave device on the I2C bus via the TX FIFO. This call blocks until
 * the operation completes. In case of failure the function stores the Tx error code to
 * the abrt_code parameter -if provided- and clears the Tx Abort register.
 *
 * @param[in] data              Address of the buffer containing the data to write
 * @param[in] len               Length of the data buffer
 * @param[in,out] abrt_code     If not NULL, the status of the operation
 *
 * This can be used to retrieve source of TX_ABORT interrupt. TX FIFO is flushed and
 * remains in this state until cleared using i2c_reset_int_tx_abort().
 * @return The number of bytes written to the Tx FIFO. It is strongly recommended to check
 *         the value of abrt_code to make sure that the number of bytes returned were actually
 *         transmitted to the I2C bus.
 *
 * @note    This function should be used only when operating in slave mode.
 *
 ****************************************************************************************
 */
uint16_t i2c_slave_transmit_buffer_sync(const uint8_t *data, uint16_t len, i2c_abort_t *abrt_code);

/**
 ****************************************************************************************
 * @brief Receive multiple bytes from I2C master asynchronously
 *
 * This function will receive multiple bytes from master taking care of RX FIFO control. This call is
 * non-blocking and specified callback is called upon completion, with the operation status.
 * The operation completes when the complete buffer is filled, or in case of a failure.
 * In case of failure the failure cause must be read from the cb by calling
 * i2c_get_abort_source().
 *
 * @param[in,out] data  Address of the buffer where data are stored
 * @param[in] len       Length of the data buffer
 * @param[in] cb        Callback to be called upon completion
 * @param[in] cb_data   Data to pass to cb
 *
 * @return void
 *
 * @note This function should be used only when operating in slave mode.
 *
 ****************************************************************************************
 */
void i2c_slave_receive_buffer_async(uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data);

/**
 ****************************************************************************************
 * @brief Transmit multiple bytes to I2C master asynchronously
 *
 * Transmit an array of data to a master device on the I2C bus via the TX FIFO. This call is non-blocking
 * and specified callback is called upon completion, with the operation status. In case of failure
 * the failure cause must be read from the cb by calling i2c_get_abort_source().
 *
 * @param[in] data     Address of the buffer containing the data to write
 * @param[in] len      Length of the data buffer
 * @param[in] cb       Callback to be called upon completion
 * @param[in] cb_data  Data to pass to cb
 *
 * @return void
 *
 * @note    This function should be used only when operating in slave mode.
 *
 ****************************************************************************************
 */
void i2c_slave_transmit_buffer_async(const uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data);

#if defined (CFG_I2C_DMA_SUPPORT)
/**
 ****************************************************************************************
 * @brief Transmit multiple bytes on I2C bus using DMA
 *
 * Shortcut for calling i2c_prepare_dma() and i2c_dma_start().
 *
 * @param[in] channel   DMA channel
 * @param[in] data      Buffer to write data to
 * @param[in] len       Length of data buffer
 * @param[in] cb        Callback for transmit completed
 * @param[in] cb_data   Data to pass to cb
 *
 * @return void
 *
 * @note This function should be used only when operating in slave mode.
 *
 ****************************************************************************************
 */
void i2c_slave_transmit_buffer_dma(i2c_dma_channel_pair_t dma_channel_pair, const uint16_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data);

/**
 * @brief Receive multiple bytes from I2C bus using DMA
 *
 * Shortcut for calling i2c_prepare_dma() and i2c_dma_start().
 *
 * @param [in] channel  DMA channel
 * @param [out] data    Buffer to put data read from I2C bus
 * @param [in] len      Length of data buffer
 * @param [in] cb       Callback for receive completed
 * @param [in] cb_data  Data to pass to cb
 *
 * @return void
 *
 * @note This function should be used only when operating in slave mode.
 *
 ****************************************************************************************
 */
void i2c_slave_receive_buffer_dma(i2c_dma_channel_pair_t dma_channel_pair, uint8_t *data, uint16_t len, i2c_complete_cb_t cb, void *cb_data);

/**
 ****************************************************************************************
 * @brief Prepares I2C DMA for transfer
 *
 * This function prepares DMA for DMA transfer
 *
 * @p channel specifies either of channels in RX/TX pair used for transfer, i.e. channel=I2C_DMA_CHANNEL_PAIR_1
 * means channels 0 and 1 will be used, channel=I2C_DMA_CHANNEL_PAIR_2 means channels 2 and 3 will be used and so on.
 * Once DMA is prepared, no other application should make changes to either of channels.
 *
 * @p data buffer elements are 16-bit wide, this high byte should be 0 for writing and discarded
 * when reading.
 *
 * Callback is called once DMA transfer between buffer and RX/TX FIFOs is completed which means that
 * there could still be activity on I2C bus. Application can check when transfer is completed using
 * other means, i.e. STOP_DETECTED interrupt.
 *
 * @param [in] channel  DMA channel pair
 * @param [in] data     Buffer to read from or write
 * @param [in] len      Length of data buffer
 * @param [in] type     Type of transfer
 * @param [in] cb       Callback for transfer completed
 * @param [in] cb_data  Data to pass to cb
 * @param [in] flags    Possible values for flags are:
 * @parblock
 *      ::I2C_F_NONE<br>
 *      The callback will be called as soon as the last byte of data is written to the Tx FIFO. Possible
 *      Tx failure after this point must be checked by the caller. This flag can be used for
 *      consecutive calls where the transaction can resume without stop/start conditions in
 *      between. In case of DA14585 STOP condition is automatically generated at the end of the transaction.
 *
 *      ::I2C_F_WAIT_FOR_STOP<br>
 *      The callback will be called when all bytes have been transmitted and a STOP condition has
 *      been generated.
 *
 *      ::I2C_F_ADD_STOP<br>
 *      STOP condition will be generated at the end of the transaction. This flag has no effect in
 *      DA14585/6 since STOP condition is automatically generated at the end of the transaction.
 * @endparblock
 *
 * @return void
 ****************************************************************************************
 */
void i2c_prepare_dma(i2c_dma_channel_pair_t dma_channel_pair, void *data, uint16_t len, i2c_dma_transfer_t type, i2c_complete_cb_t cb, void *cb_data, uint32_t flags);

/**
 ****************************************************************************************
 * @brief Starts DMA transfer
 *
 * Should be called once I2C DMA is setup using i2c_prepare_dma(). Once started, DMA transfer
 * will only finish once previously specified number of bytes is read or written.
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_dma_start(void)
{
    SetWord16(&i2c->I2C_DMA_CR_REGF, TDMAE | RDMAE);
}

/**
 ****************************************************************************************
 * @brief Stop DMA transfer
 *
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void i2c_dma_stop(void)
{
    SetWord16(&i2c->I2C_DMA_CR_REGF, 0);
}
#endif

#endif // _I2C_H_

