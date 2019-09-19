/**
 ****************************************************************************************
 *
 * @file uart.h
 *
 * @brief UART Low Level Driver for DA14585, DA14586 and DA14531 devices.
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

#ifndef _UART_H_
#define _UART_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>       // integer definition
#include <stdbool.h>      // boolean definition
#include "datasheet.h"
#include "dma.h"

/*
 * DEFINES
 *****************************************************************************************
 */

#if defined (CFG_UART_ONE_WIRE_SUPPORT)
#include "gpio.h"
#endif

/// UART registers
typedef struct                                      /*!< (@ 0x50001x00) UART Structure                                              */
{
    volatile uint16_t  UART_RBR_THR_DLL_REGF;       /*!< (@ 0x00000000) Receive Buffer Register                                     */
    uint16_t  RESERVED;
    volatile uint16_t  UART_IER_DLH_REGF;           /*!< (@ 0x00000004) Interrupt Enable Register                                   */
    uint16_t  RESERVED1;
    volatile uint16_t  UART_IIR_FCR_REGF;           /*!< (@ 0x00000008) Interrupt Identification Register/FIFO Control Register     */
    uint16_t  RESERVED2;
    volatile uint16_t  UART_LCR_REGF;               /*!< (@ 0x0000000C) Line Control Register                                       */
    uint16_t  RESERVED3;
    volatile uint16_t  UART_MCR_REGF;               /*!< (@ 0x00000010) Modem Control Register                                      */
    uint16_t  RESERVED4;
    volatile uint16_t  UART_LSR_REGF;               /*!< (@ 0x00000014) Line Status Register                                        */
    uint16_t  RESERVED5;
    volatile uint16_t  UART_MSR_REGF;               /*!< (@ 0x00000018) Modem Status Register                                       */
    uint16_t  RESERVED6;
    volatile uint16_t  UART_SCR_REGF;               /*!< (@ 0x0000001C) Scratchpad Register                                         */
    uint16_t  RESERVED7[9];
    volatile uint16_t  UART_SRBR_STHR0_REGF;        /*!< (@ 0x00000030) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED8;
    volatile uint16_t  UART_SRBR_STHR1_REGF;        /*!< (@ 0x00000034) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED9;
    volatile uint16_t  UART_SRBR_STHR2_REGF;        /*!< (@ 0x00000038) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED10;
    volatile uint16_t  UART_SRBR_STHR3_REGF;        /*!< (@ 0x0000003C) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED11;
    volatile uint16_t  UART_SRBR_STHR4_REGF;        /*!< (@ 0x00000040) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED12;
    volatile uint16_t  UART_SRBR_STHR5_REGF;        /*!< (@ 0x00000044) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED13;
    volatile uint16_t  UART_SRBR_STHR6_REGF;        /*!< (@ 0x00000048) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED14;
    volatile uint16_t  UART_SRBR_STHR7_REGF;        /*!< (@ 0x0000004C) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED15;
    volatile uint16_t  UART_SRBR_STHR8_REGF;        /*!< (@ 0x00000050) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED16;
    volatile uint16_t  UART_SRBR_STHR9_REGF;        /*!< (@ 0x00000054) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED17;
    volatile uint16_t  UART_SRBR_STHR10_REGF;       /*!< (@ 0x00000058) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED18;
    volatile uint16_t  UART_SRBR_STHR11_REGF;       /*!< (@ 0x0000005C) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED19;
    volatile uint16_t  UART_SRBR_STHR12_REGF;       /*!< (@ 0x00000060) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED20;
    volatile uint16_t  UART_SRBR_STHR13_REGF;       /*!< (@ 0x00000064) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED21;
    volatile uint16_t  UART_SRBR_STHR14_REGF;       /*!< (@ 0x00000068) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED22;
    volatile uint16_t  UART_SRBR_STHR15_REGF;       /*!< (@ 0x0000006C) Shadow Receive/Transmit Buffer Register                     */
    uint16_t  RESERVED23;
    volatile uint16_t  UART_FAR_REGF;               /*!< (@ 0x00000070) FIFO Access Register                                        */
    uint16_t  RESERVED24[5];
    volatile uint16_t  UART_USR_REGF;               /*!< (@ 0x0000007C) UART Status register.                                       */
    uint16_t  RESERVED25;
    volatile uint16_t  UART_TFL_REGF;               /*!< (@ 0x00000080) Transmit FIFO Level                                         */
    uint16_t  RESERVED26;
    volatile uint16_t  UART_RFL_REGF;               /*!< (@ 0x00000084) Receive FIFO Level.                                         */
    uint16_t  RESERVED27;
    volatile uint16_t  UART_SRR_REGF;               /*!< (@ 0x00000088) Software Reset Register.                                    */
    uint16_t  RESERVED28;
    volatile uint16_t  UART_SRTS_REGF;              /*!< (@ 0x0000008C) Shadow Request to Send                                      */
    uint16_t  RESERVED29;
    volatile uint16_t  UART_SBCR_REGF;              /*!< (@ 0x00000090) Shadow Break Control Register                               */
    uint16_t  RESERVED30;
    volatile uint16_t  UART_SDMAM_REGF;             /*!< (@ 0x00000094) Shadow DMA Mode                                             */
    uint16_t  RESERVED31;
    volatile uint16_t  UART_SFE_REGF;               /*!< (@ 0x00000098) Shadow FIFO Enable                                          */
    uint16_t  RESERVED32;
    volatile uint16_t  UART_SRT_REGF;               /*!< (@ 0x0000009C) Shadow RCVR Trigger                                         */
    uint16_t  RESERVED33;
    volatile uint16_t  UART_STET_REGF;              /*!< (@ 0x000000A0) Shadow TX Empty Trigger                                     */
    uint16_t  RESERVED34;
    volatile uint16_t  UART_HTX_REGF;               /*!< (@ 0x000000A4) Halt TX                                                     */
    uint16_t  RESERVED35;
    volatile uint16_t  UART_DMASA_REGF;             /*!< (@ 0x000000A8) DMA Software Acknowledge                                    */
    uint16_t  RESERVED36[11];
    volatile uint16_t  UART_DLF_REGF;               /*!< (@ 0x000000C0) Divisor Latch Fraction Register                             */
#if defined (__DA14531__)
    uint16_t  RESERVED37[27];
    volatile uint16_t  UART_UCV_REGF;               /*!< (@ 0x500010F8) Component Version                                           */
    volatile uint16_t  UART_UCV_HIGH_REGF;          /*!< (@ 0x500010FA) Component Version                                           */
    volatile uint16_t  UART_CTR_REGF;               /*!< (@ 0x500010FC) Component Type Register                                     */
    volatile uint16_t  UART_CTR_HIGH_REGF;          /*!< (@ 0x500010FE) Component Type Register                                     */
#else
    uint16_t  RESERVED37[25];
    volatile uint16_t  UART_CPR_REGF;               /*!< (@ 0x000000F4) Component Parameter Register                                */
    uint16_t  RESERVED38;
    volatile uint16_t  UART_UCV_REGF;               /*!< (@ 0x000000F8) Component Version                                           */
    uint16_t  RESERVED39;
    volatile uint16_t  UART_CTR_REGF;               /*!< (@ 0x000000FC) Component Type Register                                     */
#endif // __DA14531__
} uart_t;                                           /*!< Size = 254 (0xfe)                                                          */


/// UART Base Address
#define UART1   (uart_t *)  UART_RBR_THR_DLL_REG
/// UART2 Base Address
#define UART2   (uart_t *)  UART2_RBR_THR_DLL_REG

/// Macro to get the ID from UART Base Address
#define UART_ID(id) ((id) == UART1 ? 0 : 1)
/// Macro to get the environment from UART ID
#define UART_ENV(id)        (&uartn_env[((id) == UART1 ? 0 : 1)])
/// Macro to get the UART IRQ from UART ID
#define UART_INTR(id)       ((id) == UART1  ? (UART_IRQn) : (UART2_IRQn))

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */

/// @brief Generic enable/disable enum for UART driver
typedef enum {
    /// Disable
    UART_BIT_DIS                    = 0,

    /// Enable
    UART_BIT_EN                     = 1,
}UART_BIT_CFG;

/// @brief Status enum for UART driver
typedef enum {
    /// No error
    UART_ERR_NO_ERROR               = 0,

    /// Busy Error
    UART_ERR_BUSY_ERROR             = 1,

    /// Overrun error bit
    UART_ERR_OVERRUN_ERROR          = UART_OE,

    /// Parity Error
    UART_ERR_PARITY_ERROR           = UART_PE,

    /// Framing Error
    UART_ERR_FRAMING_ERROR          = UART_FE,

    /// Break Interrupt
    UART_ERR_BREAK_ERROR            = UART_BI,

    /// Receiver FIFO Error
    UART_ERR_RX_FIFO_ERROR          = UART_RFE,
} UART_STATUS;

/// @brief Interrupt Identification codes
typedef enum {
    /// No interrupt pending
    UART_INT_NO_INT_PEND            = 1,

    /// THR empty interrupt
    UART_INT_THR_EMPTY              = 2,

    /// Received data available interrupt
    UART_INT_RECEIVED_AVAILABLE     = 4,

    /// Receiver line status interrupt
    UART_INT_RECEIVE_LINE_STAT      = 6,

    /// Busy detect interrupt
    UART_INT_BUSY_DETECTED          = 7,

    /// Timeout interrupt
    UART_INT_TIMEOUT                = 12,
} UART_INT;


/// @brief Baud rates dividers
/// The defined values comprise the values of 3 registers: DLH, DLL, DLF.
//  The encoding of the values for each register is:
//  +--------+--------+--------+--------+
//  | unused |   DLH  |   DLL  |   DLF  |
//  +--------+--------+--------+--------+
typedef enum {
    /// 1M Baud Rate
    UART_BAUDRATE_1000000   = 0x000100,

    /// 921600 Baud Rate
    UART_BAUDRATE_921600    = 0x000101,

    /// 500000 Baud Rate
    UART_BAUDRATE_500000    = 0x000200,

    /// 460800 Baud Rate
    UART_BAUDRATE_460800    = 0x000203,

    /// 230400 Baud Rate
    UART_BAUDRATE_230400    = 0x000405,

    /// 115200 Baud Rate
    UART_BAUDRATE_115200    = 0x00080b,

    /// 57600 Baud Rate
    UART_BAUDRATE_57600     = 0x001106,

    /// 38400 Baud Rate
    UART_BAUDRATE_38400     = 0x001a01,

    /// 28800 Baud Rate
    UART_BAUDRATE_28800     = 0x00220c,

    /// 19200 Baud Rate
    UART_BAUDRATE_19200     = 0x003401,

    /// 14400 Baud Rate
    UART_BAUDRATE_14400     = 0x004507,

    /// 9600 Baud Rate
    UART_BAUDRATE_9600      = 0x006803,

    /// 4800 Baud Rate
    UART_BAUDRATE_4800      = 0x00d005,

    /// 2400 Baud Rate
    UART_BAUDRATE_2400      = 0x01a00b,
} UART_BAUDRATE;

/// Calculate Baud Rate Divisor
#define BAUD_RATE_DIV(x)    ((x >> 8) & 0xFFFF)

/// Calculate Baud Rate Fractional part
#define BAUD_RATE_FRAC(x)   (x & 0xFF)


/// @brief Character format
typedef enum {
    /// 5 Data bits format
    UART_DATABITS_5         = 0,

    /// 6 Data bits format
    UART_DATABITS_6         = 1,

    /// 7 Data bits format
    UART_DATABITS_7         = 2,

    /// 8 Data bits format
    UART_DATABITS_8         = 3,
} UART_DATABITS;

/// @brief Parity
typedef enum {
    /// No parity
    UART_PARITY_NONE        = 0,

    /// Odd parity
    UART_PARITY_ODD         = 1,

    /// Even parity
    UART_PARITY_EVEN        = 3,
} UART_PARITY;


/// @brief Stop bits
typedef enum {
    /// 1 stop bit
    UART_STOPBITS_1         = 0,

    /// 1.5 stop bits when DLS is 5 bits, else 2 stop bits
    UART_STOPBITS_2         = 1,
} UART_STOPBITS;

/// @brief Auto flow control
typedef enum {
    /// Disable Autoflow control
    UART_AFCE_DIS           = 0,

    /// Enable Autoflow control
    UART_AFCE_EN            = 1,
} UART_AFCE_CFG;

/// @brief Use FIFO
typedef enum {
    /// Disable FIFO
    UART_FIFO_DIS           = 0,

    /// Enable FIFO
    UART_FIFO_EN            = 1,
} UART_FIFO_CFG;

/// @brief UART RX FIFO level. Select the trigger level in the receiver FIFO
/// at which the Received Data Available Interrupt will be generated
typedef enum {
    /// 1 character in FIFO
    UART_RX_FIFO_LEVEL_0    = 0,

    /// FIFO 1/4 full
    UART_RX_FIFO_LEVEL_1    = 1,

    /// FIFO 1/2 full
    UART_RX_FIFO_LEVEL_2    = 2,

    /// FIFO 2 less than full
    UART_RX_FIFO_LEVEL_3    = 3,
} UART_RX_FIFO_LEVEL;

/// @brief UART TX FIFO level. Select the empty threshold level
/// at which the THRE Interrupts will be generated
typedef enum {
    /// FIFO empty
    UART_TX_FIFO_LEVEL_0    = 0,

    /// 2 characters in FIFO
    UART_TX_FIFO_LEVEL_1    = 1,

    /// FIFO 1/4 full
    UART_TX_FIFO_LEVEL_2    = 2,

    /// FIFO 1/2 full
    UART_TX_FIFO_LEVEL_3    = 3,
} UART_TX_FIFO_LEVEL;

#if defined (CFG_UART_DMA_SUPPORT)
/// @brief UART DMA support.
typedef enum {
    /// UART does not support DMA transactions.
    UART_DMA_DISABLE        = 0,

    /// UART supports DMA transactions.
    UART_DMA_ENABLE         = 1,
} UART_DMA_CFG;

/// @brief UART DMA channel configuration.
typedef enum {
    /// Rx = Channel 0, Tx = Channel 1
    UART_DMA_CHANNEL_01     = 0,

    /// Rx = Channel 2, Tx = Channel 3
    UART_DMA_CHANNEL_23     = 1,
} UART_DMA_CHANNEL_CFG;
#endif

/// @brief Mode of operation
typedef enum {
    /// Blocking operation (no interrupts - no DMA)
    UART_OP_BLOCKING,

    /// Interrupt driven operation
    UART_OP_INTR,

#if defined (CFG_UART_DMA_SUPPORT)
    /// DMA driven operation
    UART_OP_DMA,
#endif
} UART_OP_CFG;


/*
 * TYPE DEFINITIONS
 *****************************************************************************************
 */
/// Transmit callback type definition
typedef void (*uart_cb_t) (uint16_t data_cnt);

/// Error callback type definition
typedef void (*uart_err_cb_t) (uart_t *uart, uint8_t uart_err_status);

/// @brief UART configuration structure definition
typedef struct
{
    /// Baud Rate configuration
    UART_BAUDRATE           baud_rate:24;

    /// Data bits format configuration
    UART_DATABITS           data_bits:3;

    /// Parity configuration
    UART_PARITY             parity:2;

    /// Stop bits configuration
    UART_STOPBITS           stop_bits:1;

    /// Hardware flow control configuration
    UART_AFCE_CFG           auto_flow_control:1;

    /// Fifo usage configuration
    UART_FIFO_CFG           use_fifo:1;

    /// Transmit FIFO level configuration
    UART_TX_FIFO_LEVEL      tx_fifo_tr_lvl:2;

    /// Receive FIFO level configuration
    UART_RX_FIFO_LEVEL      rx_fifo_tr_lvl:2;

    /// Interrupt Priority
    uint32_t                intr_priority;

    /// UART error status callback
    uart_err_cb_t           uart_err_cb;

    /// UART transmit callback
    uart_cb_t               uart_tx_cb;

    /// UART receive callback
    uart_cb_t               uart_rx_cb;

#if defined (CFG_UART_DMA_SUPPORT)
    /// UART DMA Channel Pair Configuration
    UART_DMA_CHANNEL_CFG    uart_dma_channel:1;

    /// UART DMA Priority Configuration
    DMA_PRIO_CFG            uart_dma_priority;
#endif
} uart_cfg_t;


/*
 * Low Level Ragister Access Functions
 *****************************************************************************************
 */

/**** UART_RBR_THR_DLL_REG ****/

/**
 ****************************************************************************************
 * @brief Read a byte from Receive Buffer Register.
 *
 * @details If FIFOs are enabled (FCR0: set to one), read the head of the receive FIFO.
 *  Contains the data byte received on the serial input port (sin) in UART mode.
 *  The data in this register is valid only if the Data Ready (DR) bit in the Line
 *  status Register (LSR) is set. If FIFOs are disabled (FCR[0] set to zero),
 *  the data in the RBR must be read before the next data arrives, otherwise it will
 *  be overwritten, resulting in an overrun error. If FIFOs are enabled (FCR[0] set
 *  to one), this register accesses the head of the receive FIFO. If the receive FIFO
 *  is full and this register is not read before the next data character arrives,
 *  then the data already in the FIFO will be preserved but any incoming data will
 *  be lost. An overrun error will also occur.
 *
 * @param[in] uart_id   Identifies which UART to use
 * @return uint8_t      Value read from register
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t uart_read_rbr(uart_t *uart_id)
{
    return (uint8_t) GetWord16(&uart_id->UART_RBR_THR_DLL_REGF);
}

/**
 ****************************************************************************************
 * @brief Write byte to Transmit Holding Register.
 *
 * @details If FIFOs are enabled (FCR0: set to one), write byte to the transmit FIFO.
 *  This register contains data to be transmitted on the serial output port (sout)
 *  in UART mode. Data should only be written to the THR when the THR Empty (THRE)
 *  bit (LSR[5]) is set. If FIFO's are disabled (FCR[0] set to zero) and THRE is set,
 *  writing a single character to the THR clears the THRE. Any additional writes to
 *  the THR before the THRE is set again causes the THR data to be overwritten.
 *  If FIFO's are enabled (FCR[0] set to one) and THRE is set, x number of characters
 *  of data may be written to the THR before the FIFO is full. The number x (default=16)
 *  is determined by the value of FIFO Depth that you set during configuration. Any
 *  attempt to write data when the FIFO is full results in the write data being lost.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] data          Byte to write
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_thr_setf(uart_t *uart_id, uint8_t data)
{
    SetWord16(&uart_id->UART_RBR_THR_DLL_REGF, data);
}

/**
 ****************************************************************************************
 * @brief Write byte to Divisor Latch (Low) Register.
 *
 * @details Writes the lower 8-bits of the 16-bit divisor for the UART. This register may
 *  only be accessed when the DLAB bit (LCR[7]) is set.
 *  This register makes up the lower 8-bits of a 16-bit, read/write, Divisor Latch
 *  register that contains the baud rate divisor for the UART. This register may only be
 *  accessed when the DLAB bit (LCR[7]) is set. The output baud rate is equal to the serial
 *  clock (sclk) frequency divided by sixteen times the value of the baud rate divisor,
 *  as follows:
 *  baud rate = (serial clock freq) / (16 * divisor)
 *  Note that with the Divisor Latch Registers (DLL and DLH) set to zero, the baud clock is
 *  disabled and no serial communications will occur. Also, once the DLL is set, at least 8
 *  clock cycles of the slowest uart clock should be allowed to pass before transmitting
 *  or receiving data.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] data          DLL value
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_dll_setf(uart_t *uart_id, uint8_t data)
{
    SetBits16(&uart_id->UART_LCR_REGF, UART_DLAB, UART_BIT_EN);
    SetWord16(&uart_id->UART_RBR_THR_DLL_REGF, data);
    SetBits16(&uart_id->UART_LCR_REGF, UART_DLAB, UART_BIT_DIS);
}

/**
 ****************************************************************************************
 * @brief Write byte to Divisor Latch (High) Register.
 *
 * @details Writes the upper 8-bits of the 16-bit divisor for the UART.
 *  This register may only be accessed when the DLAB bit (LCR[7]) is set.
 *  Upper 8-bits of a 16-bit, read/write, Divisor Latch register that contains the
 *  baud rate divisor for the UART. This register may be accessed only when the DLAB bit
 *  (LCR[7]) is set. The output baud rate is equal to the serial clock frequency divided
 *  by sixteen times the value of the baud rate divisor, as follows:
 *  baud rate = (serial clock freq) / (16 * divisor)
 *  Note that with the Divisor Latch Registers (DLL and DLH) set to zero, the baud clock is
 *  disabled and no serial communications will occur. Also, once the DLL is set, at least 8
 *  clock cycles of the slowest uart clock should be allowed to pass before transmitting
 *  or receiving data.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] data          DLH value
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_dlh_setf(uart_t *uart_id, uint8_t data)
{
    SetBits16(&uart_id->UART_LCR_REGF, UART_DLAB, UART_BIT_EN);
    SetWord16(&uart_id->UART_IER_DLH_REGF, data);
    SetBits16(&uart_id->UART_LCR_REGF, UART_DLAB, UART_BIT_DIS);
}


/**** UART_IER_DLH_REG ****/

/**
 ****************************************************************************************
 * @brief Enable/Disable Received Data Available Interrupt
 *
 * @details This is used to enable/disable the generation of Received Data Available Interrupt
 *    and the Character Timeout Interrupt (if in FIFO mode and FIFO's enabled). These are
 *    the second highest priority interrupts.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] erbfi_en      UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_rxdata_intr_setf(uart_t *uart_id, UART_BIT_CFG erbfi_en)
{
    SetBits16(&uart_id->UART_IER_DLH_REGF, ERBFI_dlh0, erbfi_en);
}

/**
 ****************************************************************************************
 * @brief Enable/Disable Transmit Holding Register Empty Interrupt
 *
 * @details This is used to enable/disable the generation of Transmitter Holding Register
 *    Empty Interrupt. This is the third highest priority interrupt.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] etbei_en      UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_txdata_intr_setf(uart_t *uart_id, UART_BIT_CFG etbei_en)
{
    SetBits16(&uart_id->UART_IER_DLH_REGF, ETBEI_dlh1, etbei_en);
}

/**
 ****************************************************************************************
 * @brief Enable/Disable Receiver Line Status Interrupt
 *  This is used to enable/disable the generation of Receiver Line Status Interrupt.
 *  This is the highest priority interrupt.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] elsi_en       UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_rls_intr_setf(uart_t *uart_id, UART_BIT_CFG elsi_en)
{
    SetBits16(&uart_id->UART_IER_DLH_REGF, ELSI_dhl2, elsi_en);
}

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Enable/Disable Modem Status Interrupt
 *
 * @details This is used to enable/disable the generation of Modem Status Interrupt.
 *  This is the fourth highest priority interrupt.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] elsi_en       UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_ms_intr_setf(uart_t *uart_id, UART_BIT_CFG edssi_en)
{
    SetBits16(&uart_id->UART_IER_DLH_REGF, EDSSI_dlh3, edssi_en);
}
#endif

/**
 ****************************************************************************************
 * @brief Get interrupt configuration of UART
 *
 * @param[in] uart_id       Identifies which UART to use
 * @return uint8_t          Interrupt configuration
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t uart_intr_config_getf(uart_t *uart_id)
{
    return (uint8_t) (GetWord16(&uart_id->UART_IER_DLH_REGF) & 0x8F);
}

/**
 ****************************************************************************************
 * @brief Enable/Disable Programmable THRE Interrupt Mode
 *
 * @details This is used to enable/disable the generation of THRE Interrupt.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] ptime_en      UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_thre_intr_setf(uart_t *uart_id, UART_BIT_CFG ptime_en)
{
    SetBits16(&uart_id->UART_IER_DLH_REGF, PTIME_dlh7, ptime_en);
}

/**** UART_IIR_FCR_REG ****/
#if defined (__DA14531__)
    #define IIR_FCR     (UART_IID0_FIFOE | UART_IID1_RFIFOE | UART_IID2_XFIFOR | UART_IID3_DMAM)
#else
    #define UART_IID0_FIFOE         (0x0001)
    #define UART_IID1_RFIFOE        (0x0002)
    #define UART_IID2_XFIFOR        (0x0004)
    #define UART_IID3_DMAM          (0x0008)
    #define UART_TET                (0x0030)
    #define UART_FIFOSE_RT          (0x00C0)
#endif

/**
 ****************************************************************************************
 * @brief Read Interrupt ID
 * @param[in] uart_id       Identifies which UART to use
 * @return uint8_t          Interrupt ID
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t uart_intr_id_getf(uart_t *uart_id)
{
    return (uint8_t) (GetWord16(&uart_id->UART_IIR_FCR_REGF) & 0xF);
}

/**
 ****************************************************************************************
 * @brief Indicate whether the FIFO's are enabled or disabled
 * @param[in] uart_id       Identifies which UART to use
 * @return uint8_t          UART_FIFO_CFG
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint8_t uart_fifo_enabled_getf(uart_t *uart_id)
{
    return ((uint8_t) (GetWord16(&uart_id->UART_IIR_FCR_REGF) & UART_FIFOSE_RT) == 0xC0
            ? UART_FIFO_EN : UART_FIFO_DIS);
}

/**
 ****************************************************************************************
 * @brief Enable/Disable UART FIFOs
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] fifo_en       UART_FIFO_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_fifo_enable_setf(uart_t *uart_id, UART_FIFO_CFG fifo_en)
{
    SetBits16(&uart_id->UART_IIR_FCR_REGF, UART_IID0_FIFOE, fifo_en);
}

/**
 ****************************************************************************************
 * @brief Reset the control portion of the receive  FIFO and treats the FIFO as empty.
 *
 * @note This bit is 'self-clearing' and it is not necessary to clear it.
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_rxfifo_flush(uart_t *uart_id)
{
    SetBits16(&uart_id->UART_IIR_FCR_REGF, UART_IID1_RFIFOE, 0x01);
}

/**
 ****************************************************************************************
 * @brief Reset the control portion of the transmit FIFO and treats the FIFO as empty.
 *
 * @note This bit is 'self-clearing' and it is not necessary to clear it.
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_txfifo_flush(uart_t *uart_id)
{
    SetBits16(&uart_id->UART_IIR_FCR_REGF, UART_IID2_XFIFOR, 0x01);
}

/**
 ****************************************************************************************
 * @brief Set the empty threshold level at which the THRE Interrupts will be generated when
 *  the mode is active.
 *
 * @details It also determines when the dma_tx_req_n signal will be asserted
 *  when in certain modes of operation. The following trigger levels are supported:
 *    [00] = FIFO empty
 *    [01] = 2 characters in the FIFO
 *    [10] = FIFO 1/4 full
 *    [11] = FIFO 1/2 full
 * @param[in] uart_id           Identifies which UART to use
 * @param[in] txempty_trigger   UART_TX_FIFO_LEVEL
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_fifo_txempty_trg_setf(uart_t *uart_id,
                                               UART_TX_FIFO_LEVEL txempty_trigger)
{
    SetBits16(&uart_id->UART_IIR_FCR_REGF, UART_TET, txempty_trigger);
}

/**
 ****************************************************************************************
 * @brief Set the trigger level in the receiver FIFO at which the Received Data Available
 *  Interrupt will be generated.
 * @details In auto flow control mode it is used to determine when the rts_n signal will
 * be de-asserted. It also determines when the dma_rx_req_n signal will be asserted when
 * in certain modes of operation. The following trigger levels are supported:
 *    [00] = 1 character in the FIFO
 *    [01] = FIFO 1/4 full
 *    [10] = FIFO 1/2 full
 *    [11] = FIFO 2 less than full
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] rcvr_trigger  UART_RX_FIFO_LEVEL
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_fifo_rcvr_trg_setf(uart_t *uart_id,
                                            UART_RX_FIFO_LEVEL rcvr_trigger)
{
    SetBits16(&uart_id->UART_IIR_FCR_REGF, UART_FIFOSE_RT, rcvr_trigger);
}


/**** UART_LCR_REG ****/
/**
 ****************************************************************************************
 * @brief Write a value to LCR
 *
 * @param[in] uart_id      Identifies which UART to use
 * @param[in] lcr_val      Value to write to LCR
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_lcr_setf(uart_t *uart_id, uint16_t lcr_val)
{
    SetWord16(&uart_id->UART_LCR_REGF, lcr_val);
}

/**
 ****************************************************************************************
 * @brief Enable/Disable Divisor Latch Access Bit.
 *
 * @details This bit is used to enable reading and writing of the Divisor Latch register
 * (DLL and DLH) to set the baud rate of the UART. This bit must be cleared after initial
 * baud rate setup in order to access other registers.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] dlab          UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_dlab_setf(uart_t *uart_id, UART_BIT_CFG dlab)
{
    SetBits16(&uart_id->UART_LCR_REGF, UART_DLAB, dlab);
}

/**
 ****************************************************************************************
 * @brief Enable/Disable Break Control Bit.
 *
 * @details This is used to cause a break condition to be transmitted to the receiving device.
 *  If set to one the serial output is forced to the spacing (logic 0) state. When
 *  not in Loopback Mode, as determined by MCR[4], the sout line is forced low until
 *  the Break bit is cleared. If active (MCR[6] set to one) the sir_out_n line is
 *  continuously pulsed. When in Loopback Mode, the break condition is internally
 *  looped back to the receiver and the sir_out_n line is forced low.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] bc            UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_bc_setf(uart_t *uart_id, UART_BIT_CFG bc)
{
    SetBits16(&uart_id->UART_LCR_REGF, UART_BC, bc);
}

/**
 ****************************************************************************************
 * @brief Set Parity.
 *
 * @details Writeable only when UART is not busy (USR0: is zero). It is used to enable
 *  and disable parity generation and detection in transmitted and received serial
 *  character respectively.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] parity        UART_PARITY
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_parity_setf(uart_t *uart_id, UART_PARITY parity)
{
    SetBits16(&uart_id->UART_LCR_REGF, UART_PEN | UART_EPS, parity);
}

/**
 ****************************************************************************************
 * @brief Get Parity configuration of UART.
 * @param[in] uart_id       Identifies which UART to use
 * @return UART_PARITY      Parity configuration
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_parity_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_LCR_REGF, UART_PEN | UART_EPS);
}

/**
 ****************************************************************************************
 * @brief Set number of stop bits.
 *
 * @details This is used to select the number of stop bits per character that the peripheral
 *  transmits and receives.
 *  If set to zero, one stop bit is transmitted in the serial data.
 *  If set to one and the data bits are set to 5 (LCR[1:0] set to zero) one and a half
 *  stop bits is transmitted.
 *  Otherwise, two stop bits are transmitted.
 *  Note that regardless of the number of stop bits selected, the receiver checks only
 *  the first stop bit.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] stop_bits     UART_STOPBITS
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_stop_bits_setf(uart_t *uart_id, UART_STOPBITS stop_bits)
{
    SetBits16(&uart_id->UART_LCR_REGF, UART_STOP, stop_bits);
}

/**
 ****************************************************************************************
 * @brief Get stop bits configuration of UART.
 * @param[in] uart_id       Identifies which UART to use
 * @return UART_STOPBITS    Stop bits configuration
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_stop_bits_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_LCR_REGF, UART_STOP);
}

/**
 ****************************************************************************************
 * @brief Set Data Length Select.
 *
 * @details This is used to select the number of data bits per character that the peripheral
 *  transmits and receives. The number of bit that may be selected areas follows:
 * [00] = 5 bits, [01] = 6 bits, [10] = 7 bits, [11] = 8 bits
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] dls           UART_DATABITS
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_dls_setf(uart_t *uart_id, UART_DATABITS dls)
{
    SetBits16(&uart_id->UART_LCR_REGF, UART_DLS, dls);
}

/**
 ****************************************************************************************
 * @brief Get Data Length Select configuration of UART.
 * @param[in] uart_id       Identifies which UART to use
 * @return UART_DATABITS    Data bits configuration
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_dls_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_LCR_REGF, UART_DLS);
}

/**** UART_MCR_REG ****/

#if !defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Enable/Disable SIR Mode.
 *
 * @details This is used to enable/disable the IrDA SIR Mode features as described in
 *  IrDA 1.0 SIR Protocol.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] sire: 0: IrDA SIR Mode disabled, 1: IrDA SIR Mode enabled
 * @return void
 * @note SIR Mode is not supported in DA14531
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_sire_setf(uart_t *uart_id, UART_BIT_CFG sire)
{
    SetBits16(&uart_id->UART_MCR_REGF, UART_SIRE, sire);
}
#endif

/**
 ****************************************************************************************
 * @brief Enable/Disable Auto Flow Control.
 *
 * @details Writeable only when AFCE_MODE == Enabled, always readable. When FIFOs are enabled
 *  and the Auto Flow Control Enable (AFCE) bit is set, Auto Flow Control features
 *  are enabled.
 *
 * @note HW flowcontrol is not supported in UART2 of DA14531
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] afce          UART_AFCE_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_afce_setf(uart_t *uart_id, UART_AFCE_CFG afce)
{
#if defined (__DA14531__)
    if (uart_id == UART1)
#endif
    {
        SetBits16(&uart_id->UART_MCR_REGF, UART_AFCE, afce);
    }
}

/**
 ****************************************************************************************
 * @brief Get Auto Flow Control configuration of UART.
 *
 * @note HW flowcontrol is not supported in UART2 of DA14531
 *
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         UART_AFCE_CFG
 *
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_afce_getf(uart_t *uart_id)
{
#if defined (__DA14531__)
    if (uart_id == UART2)
    {
        return UART_AFCE_DIS;
    }
#endif
    return GetBits16(&uart_id->UART_MCR_REGF, UART_AFCE);
}

/**
 ****************************************************************************************
 * @brief Enable/Disable Request to Send.
 *
 * @details This is used to directly control the Request to Send (rts_n) output.
 *
 * @note HW flowcontrol is not supported in UART2 of DA14531
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] rts_n: 0: disabled, 1: enabled
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_rtsn_setf(uart_t *uart_id, UART_AFCE_CFG rts_n)
{
#if defined (__DA14531__)
    if (uart_id == UART1)
#endif
    {
        SetBits16(&uart_id->UART_MCR_REGF, UART_RTS, rts_n);
    }
}

/**
 ****************************************************************************************
 * @brief Enable/Disable LoopBack.
 *
 * @details Loopback mode shall be set before activating the pads or before setting the
 *          Baud Rate to ensure that UART module is not busy.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] lb_en         UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_loopback_setf(uart_t *uart_id, UART_BIT_CFG lb_en)
{
    SetBits16(&uart_id->UART_MCR_REGF, UART_LB, lb_en);
}


/**** UART_LSR_REG ****/
/**
 ****************************************************************************************
 * @brief Transmitter Empty bit
 *
 * @details If FIFOs enabled (FCR0: =1), this bit is set whenever the Transmitter Shift
 *  Register and the FIFO are both empty.
 *  If FIFOs are disabled, this bit is set whenever the Transmitter Holding Register
 *  and the Transmitter Shift Register are both empty.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         0 = not empty, 1 = empty
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_tx_empty_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_LSR_REGF, UART_TEMT);
}

/**
 ****************************************************************************************
 * @brief Transmit Holding Register Empty bit
 *
 * @details If THRE mode is disabled (IER[7] =0) and regardless of FIFO's being enabled or
 *  not, this bit indicates that the THR or TX FIFO is empty.
 *  This bit is set whenever data is transferred from the THR or TX FIFO to the
 *  transmitter shift register and no new data has been written to the THR or TX FIFO.
 *  This also causes a THRE Interrupt to occur, if the THRE Interrupt is enabled.
 *  If both modes are active (IER[7] =1 and FCR0: =1 respectively), the functionality
 *  is switched to indicate the transmitter FIFO is full, and no longer controls THRE
 *  interrupts, which are then controlled by the FCR[5:4] threshold setting.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         0 = not empty / not full, 1 = empty / full
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_thr_empty_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_LSR_REGF, UART_THRE);
}

/**
 ****************************************************************************************
 * @brief Data Ready bit.
 *
 * @details This is used to indicate that the receiver contains at least one character in
 *  the RBR or the receiver FIFO.  *  This bit is cleared when the RBR is read in non-FIFO
 *  mode, or when the receiver FIFO is empty, in FIFO mode.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         0 = no data ready, 1 = data ready
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_data_ready_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_LSR_REGF, UART_DR);
}


/**
 ****************************************************************************************
 * @brief Get RLS errors
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         No error or error type
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_rls_error_getf(uart_t *uart_id)
{
    return GetWord16(&uart_id->UART_LSR_REGF) & (UART_OE | UART_PE | UART_FE |
                                                 UART_BI | UART_RFE);
}

/**** UART_MSR_REG ****/

/**
 ****************************************************************************************
 * @brief Clear to Send.
 *
 * @details This is used to indicate the current state of the modem control line cts_n.
 *  This bit is the complement of cts_n. When the Clear to Send input (cts_n) is asserted
 *  it is an indication that the modem or data set is ready to exchange data with the UART Ctrl.
 *  In Loopback Mode (MCR[4] = 1), CTS is the same as MCR1: (RTS).
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         0 = cts_n input is de-asserted (logic 1),
 *                          1 = cts_n input is asserted (logic 0)
 * @note HW flowcontrol is not supported in UART2 of DA14531
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_cts_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_MSR_REGF, UART_CTS);
}

#if !defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Delta Clear to Send.
 *
 * @details This is used to indicate that the modem control line cts_n has changed since
 *  the last time the MSR was read.
 *  Reading the MSR clears the DCTS bit. In Loopback Mode (MCR[4] = 1), DCTS reflects
 *  changes on MCR1: (RTS).
 * @note If the DCTS bit is not set and the cts_n signal is asserted (low) and a reset occurs (software or otherwise),
 *  then the DCTS bit is set when the reset is removed if the cts_n signal remains asserted.
 * @note DCTS is not supported in DA14531
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         0 = no change on cts_n since last read of MSR,
 *                          1 = change on cts_n since last read of MSR
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_dcts_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_MSR_REGF, UART_DCTS);
}
#endif

/**** UART_SCR_REG ****/

/**
 ****************************************************************************************
 * @brief This register is for programmers to use as a temporary storage space.
 *
 * @note It has no defined purpose in the UART CTRL.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         read byte
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_scr_reg_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_SCR_REGF, UART_SCRATCH_PAD);
}

/**
 ****************************************************************************************
 * @brief This register is for programmers to use as a temporary storage space.
 *
 * @note It has no defined purpose in the UART CTRL.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] data          Byte to write
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_scr_reg_setf(uart_t *uart_id, uint8_t data)
{
    SetBits16(&uart_id->UART_SCR_REGF, UART_SCRATCH_PAD, data);
}


/**** UART_USR_REG ****/

/**
 ****************************************************************************************
 * @brief Receive FIFO Full.
 *
 * @details This is used to indicate that the receive FIFO is completely full.
 *  This bit is cleared when the RX FIFO is no longer full.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         0 = Receive FIFO not full, 1 = Receive FIFO Full
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_rxfifo_is_full_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_USR_REGF, UART_RFF);
}

/**
 ****************************************************************************************
 * @brief Receive FIFO Not Empty.
 *
 * @details This is used to indicate that the receive FIFO contains one or more entries.
 *  This bit is cleared when the RX FIFO is empty.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         0 = Receive FIFO is empty, 1 = Receive FIFO is not empty
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_rxfifo_not_empty_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_USR_REGF, UART_RFNE);
}

/**
 ****************************************************************************************
 * @brief Transmit FIFO Empty.
 *
 * @details This is used to indicate that the transmit FIFO is completely empty.
 *  This bit is cleared when the TX FIFO is no longer empty.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         False = Transmit FIFO is not empty, True = Transmit FIFO is empty
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_txfifo_is_empty_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_USR_REGF, UART_TFE);
}

/**
 ****************************************************************************************
 * @brief Transmit FIFO Not Full.
 *
 * @details This is used to indicate that the transmit FIFO in not full.
 *  This bit is cleared when the TX FIFO is full.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         0: Transmit FIFO is full, 1: Transmit FIFO is not full
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_txfifo_not_full_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_USR_REGF, UART_TFNF);
}

/**
 ****************************************************************************************
 * @brief UART Busy.
 *
 * @details This indicates that a serial transfer is in progress, when cleared indicates
 *  that uart is idle or inactive.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         False = idle, True = busy
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_is_busy_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_USR_REGF, UART_BUSY);
}

/**** UART_TFL_REG ****/

/**
 ****************************************************************************************
 * @brief Transmit FIFO Level.
 *
 * @details This is indicates the number of data entries in the transmit FIFO.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         Data entries
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_txfifo_level_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_TFL_REGF, UART_TRANSMIT_FIFO_LEVEL);
}


/**** UART_RFL_REG ****/

/**
 ****************************************************************************************
 * @brief Receive FIFO Level.
 *
 * @details This is indicates the number of data entries in the receive FIFO.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         Data entries
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_rxfifo_level_getf(uart_t *uart_id)
{
    return GetBits16(&uart_id->UART_RFL_REGF, UART_RECEIVE_FIFO_LEVEL);
}


/**** UART_SHADOW_REGS ****/

/**
 ****************************************************************************************
 * @brief XMIT FIFO Reset.
 *
 * @details This is a shadow register for the XMIT FIFO Reset bit (FCR[2]). This can be
 *  used to remove the burden on software having to store previously written FCR
 *  values (which are pretty static) just to reset the transmit FIFO. This resets
 *  the control portion of the transmit FIFO and treats the FIFO as empty.
 *  This bit is 'self-clearing'.
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_txfifo_flush_shd(uart_t *uart_id)
{
    SetWord16(&uart_id->UART_SRR_REGF, UART_XFR);
}

/**
 ****************************************************************************************
 * @brief RCVR FIFO Reset.
 *
 * @details This is a shadow register for the RCVR FIFO Reset bit (FCR1:). This can be used
 *  to remove the burden on software having to store previously written FCR values
 *  (which are pretty static) just to reset the receive FIFO This resets the control
 *  portion of the receive FIFO and treats the FIFO as empty.
 *  This bit is 'self-clearing'.
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_rxfifo_flush_shd(uart_t *uart_id)
{
    SetWord16(&uart_id->UART_SRR_REGF, UART_RFR);
}

/**
 ****************************************************************************************
 * @brief UART Reset.
 *
 * @details This asynchronously resets the UART Ctrl and synchronously removes the reset
 *  assertion.
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_rst_shd_setf(uart_t *uart_id)
{
    SetWord16(&uart_id->UART_SRR_REGF, UART_UR);
}

/**
 ****************************************************************************************
 * @brief Shadow Request to Send
 *
 * @details This is a shadow register for the RTS bit (MCR[1]), this can be used to remove
 *  the burden of having to performing a read-modify-write on the MCR. This is used
 *  to directly control the Request to Send (rts_n) output. The Request To Send (rts_n)
 *  output is used to inform the modem or data set that the UART Ctrl is ready to
 *  exchange data.
 *  When Auto RTS Flow Control is not enabled (MCR[5] = 0), the rts_n signal is set
 *  low by programming MCR[1] (RTS) to a high.
 *  In Auto Flow Control, AFCE_MODE == Enabled and active (MCR[5] = 1) and FIFOs enable
 *  (FCR[0] = 1), the rts_n output is controlled in the same way, but is also gated with
 *  the receiver FIFO threshold trigger (rts_n is inactive high when above the threshold).
 *  Note that in Loopback mode (MCR[4] = 1), the rts_n output is held inactive-high while
 *  the value of this location is internally looped back to an input.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] uart_rts      UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_rts_shd_setf(uart_t *uart_id, UART_BIT_CFG uart_rts)
{
#if defined (__DA14531__)
    if (uart_id == UART1)
#endif
    {
        SetWord16(&uart_id->UART_SRTS_REGF, uart_rts);
    }
}

/**
 ****************************************************************************************
 * @brief Shadow Break Control Bit
 * @param[in] uart_id           Identifies which UART to use
 * @param[in] uart_break_ctrl   UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_break_ctrl_shd_setf(uart_t *uart_id, UART_BIT_CFG uart_break_ctrl)
{
    SetWord16(&uart_id->UART_SBCR_REGF, uart_break_ctrl);
}

#if defined (CFG_UART_DMA_SUPPORT)
/**
 ****************************************************************************************
 * @brief Shadow DMA Mode
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] dma_mode
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_dma_mode_shd_setf(uart_t *uart_id, uint8_t dma_mode)
{
    SetWord16(&uart_id->UART_SDMAM_REGF, dma_mode);
}
#endif

/**
 ****************************************************************************************
 * @brief Shadow FIFO Enable
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] fifo_en
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_fifo_enable_shd_setf(uart_t *uart_id, UART_FIFO_CFG fifo_en)
{
    SetWord16(&uart_id->UART_SFE_REGF, fifo_en);
}

/**
 ****************************************************************************************
 * @brief Shadow RCVR Trigger
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] rcvr_trigger
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_fifo_rcvr_trg_shd_setf(uart_t *uart_id,
                                                UART_RX_FIFO_LEVEL rcvr_trigger)
{
    SetWord16(&uart_id->UART_SRT_REGF, rcvr_trigger);
}

/**
 ****************************************************************************************
 * @brief Shadow TX Empty Trigger
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] txempty_trigger
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_fifo_txempty_trg_shd_setf(uart_t *uart_id,
                                                   UART_TX_FIFO_LEVEL txempty_trigger)
{
    SetWord16(&uart_id->UART_STET_REGF, txempty_trigger);
}


/**** UART_HTX_REG ****/

/**
 ****************************************************************************************
 * @brief Read Halt Transmit configuration
 *
 * @details This register is use to halt transmissions for testing, so that the transmit
 *  FIFO can be filled by the master when FIFOs are implemented and enabled.
 *  Note, if FIFOs are not enabled, the setting of the halt TX register has no
 *  effect on operation.
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         0 = Halt TX disabled, 1 = Halt TX enabled
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_halt_getf(uart_t *uart_id)
{
    return GetWord16(&uart_id->UART_HTX_REGF);
}

/**
 ****************************************************************************************
 * @brief Halt Transmit
 *
 * @details This register is use to halt transmissions for testing, so that the transmit
 *  FIFO can be filled by the master when FIFOs are implemented and enabled.
 *  Note, if FIFOs are not enabled, the setting of the halt TX register has no
 *  effect on operation.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] halt_en       UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_halt_setf(uart_t *uart_id, UART_BIT_CFG halt_en)
{
    SetWord16(&uart_id->UART_HTX_REGF, halt_en);
}


/**** UART_DMASA_REG ****/

/**
 ****************************************************************************************
 * @brief This register is use to perform DMA software acknowledge if a transfer needs
 *  to be terminated due to an error condition.
 *
 * @details For example, if the DMA disables the channel, then uart should clear its request.
 *  This will cause the TX request, TX single, RX request and RX single signals to
 *  de-assert.
 * @note This bit is 'self-clearing'
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] dmasa_en      UART_BIT_CFG
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_dmasa_setf(uart_t *uart_id, UART_BIT_CFG dmasa_en)
{
    SetWord16(&uart_id->UART_DMASA_REGF, dmasa_en);
}


/**** UART_DLF_REG ****/
/**
 ****************************************************************************************
 * @brief The fractional value is added to integer value set by DLH, DLL.
 *
 * @details Fractional value is determined by (Divisor Fraction value)/(2^DLF_SIZE).
 * @param[in] uart_id       Identifies which UART to use
 * @return uint16_t         dlf value
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t uart_dlf_getf(uart_t *uart_id)
{
    return (GetWord16(&uart_id->UART_DLF_REGF)) & 0x0F;
}

/**
 ****************************************************************************************
 * @brief The fractional value is added to integer value set by DLH, DLL.
 *
 * @details Fractional value is determined by (Divisor Fraction value)/(2^DLF_SIZE).
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] dlf           uint8_t
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void uart_dlf_setf(uart_t *uart_id, uint8_t dlf)
{
    SetWord16(&uart_id->UART_DLF_REGF, dlf);
}


/*
 * UART Configuration API
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Enable clock for UART peripheral
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
void uart_enable(uart_t *uart_id);

/**
 ****************************************************************************************
 * @brief Reset UART and disable clock for UART peripheral
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
void uart_disable(uart_t *uart_id);

/**
 ****************************************************************************************
 * @brief Set Baud rate.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] baud_rate     UART_BAUDRATE
 * @return void
 ****************************************************************************************
 */
void uart_baudrate_setf(uart_t *uart_id, UART_BAUDRATE baud_rate);

/**
 ****************************************************************************************
 * @brief Register UART transmit callback (on-the-fly). Callback to return in interrupt/DMA-driven
 * transmitting operations.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] cb            pointer to callback of uart_cb_t type
 * @return void
 ****************************************************************************************
 */
void uart_register_tx_cb(uart_t *uart_id, uart_cb_t cb);

/**
 ****************************************************************************************
 * @brief Register UART receive callback (on-the-fly). Callback to return in interrupt/DMA-driven
 * receiving operations.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] cb            pointer to callback of uart_cb_t type
 * @return void
 ****************************************************************************************
 */
void uart_register_rx_cb(uart_t *uart_id, uart_cb_t cb);

/**
 ****************************************************************************************
 * @brief Register UART ERROR/STATYS callback (on-the-fly).
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] cb            pointer to callback of uart_cb_t type
 * @return void
 ****************************************************************************************
 */
void uart_register_err_cb(uart_t *uart_id, uart_err_cb_t cb);

/**
 ****************************************************************************************
 * @brief Initialize UART peripheral with custom settings
 *
 * @details This function initializes UART registers with given configuration.
 *  It also initializes all internal software variables for buffered transmissions.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] uart_cfg      UART configuration structure
 * @return void
 ****************************************************************************************
 */
void uart_initialize(uart_t *uart_id, const uart_cfg_t *uart_cfg);

/*
 * UART Send API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Write one byte to UART
 *
 * @details Writes one byte in Tx FIFO.
 *  If FIFO is full, it will wait until it has free space, according to the configured threshold level.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] data          Byte to write
 * @return void
 ****************************************************************************************
 */
void uart_write_byte(uart_t *uart_id, uint8_t data);

/**
 ****************************************************************************************
 * @brief Writes a number of bytes to UART
 *
 * @details This function is blocking, it finishes when all data has been written in FIFO.
 *  It does not wait for data to be transmitted out from UART.
 *  This function does not use interrupts or DMA.
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] data          Pointer to data buffer
 * @param[in] len           Length (bytes) of data buffer
 * @return void
 ****************************************************************************************
 */
void uart_write_buffer(uart_t *uart_id, const uint8_t *data, uint16_t len);

/**
 ****************************************************************************************
 * @brief Start sending data to UART transmitter.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] data          Pointer to data buffer
 * @param[in] len           Length (bytes) of data buffer
 * @param[in] op            Blocking, interrupt-driven or DMA-driven operation.
 * @return void
 ****************************************************************************************
 */
void uart_send(uart_t *uart_id, const uint8_t *data, uint16_t len, UART_OP_CFG op);

/**
 ****************************************************************************************
 * @brief Wait until UART has finished all transfers.
 *
 * @details If FIFOs are enabled, the function waits until the Transmitter Shift Register
 * and the FIFO are both empty. If FIFOs are disabled, it waits until the Transmitter
 * Holding Register and the Transmitter Shift Register are both empty.
 * This function must be called after the transmit callback is fired to ensure that all
 * data have been sent, i.e. before going to sleep.
 *
 * @note This function is blocking and must not be called inside the driver's tx isr or
 * the user tx callback in full-duplex UART transfers, to avoid missing reception interrupts.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
void uart_wait_tx_finish(uart_t *uart_id);


/*
 * UART Receive API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Read one byte from UART. If FIFO is empty, it polls until received data are available.
 * @param[in] uart_id       Identifies which UART to use
 * @return read byte
 ****************************************************************************************
 */
uint8_t uart_read_byte(uart_t *uart_id);

/**
 ****************************************************************************************
 * @brief Read number of bytes from UART
 *
 * @details This function is blocking, it waits to receive \p len bytes from UART.
 *  This function finishes when all data has been read from FIFO.
 *  This function does not use interrupts or DMA.
 * @param[in] uart_id   Identifies which UART to use
 * @param[out] data     Pointer to the data buffer for incoming data
 * @param[in] len       Number of bytes to read
 * @return void
 ****************************************************************************************
 */
void uart_read_buffer(uart_t *uart_id, uint8_t *data, uint16_t len);

/**
 ****************************************************************************************
 * @brief Start receiving data from UART receiver.
 *
 * @param[in] uart_id       Identifies which UART to use
 * @param[out] data         Data buffer to store received data
 * @param[in] len           Number of bytes to receive
 * @param[in] op            Blocking, interrupt-driven or DMA-driven operation.
 * @return void
 * @note If a timeout error occurs during reception, it will fire the registerd error callback.
 ****************************************************************************************
 */
void uart_receive(uart_t *uart_id, uint8_t *data, uint16_t len, UART_OP_CFG op);

/**
 ****************************************************************************************
 * @brief Enable UART flow control
 *
 * @param[in] uart_id       Identifies which UART to use.
 * @return void
 ****************************************************************************************
 */
void uart_enable_flow_control(uart_t *uart_id);

/**
 ****************************************************************************************
 * @brief Disable UART flow control, if UART is inactive
 * @param[in] uart_id       Identifies which UART to use.
 * @param[in] retries       Number of times to retry disabling flow control, if UART
 *                          transmitter is active.
 * @return True if flow control has been disabled, else false.
 ****************************************************************************************
 */
bool uart_disable_flow_control(uart_t *uart_id, uint32_t retries);

/*
 * One-Wire-UART API
 ****************************************************************************************
 */

#if defined (CFG_UART_ONE_WIRE_SUPPORT)
/**
 ****************************************************************************************
 * @brief Enable One-Wire-UART functionality
 * @param[in] uart_id       Identifies which UART to use
 * @param[in] port          Select which port will be used as One-Wire-UART
 * @param[in] port          Select which pin will be used as One-Wire-UART.
 * @return void
 *****************************************************************************************
 */
void uart_one_wire_enable(uart_t *uart_id, GPIO_PORT port, GPIO_PIN pin);

/**
 ****************************************************************************************
 * @brief Disable One-Wire-UART functionality
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 *****************************************************************************************
 */
void uart_one_wire_disable(uart_t *uart_id);

/**
 ****************************************************************************************
 * @brief Enable One-Wire-UART Transmit path
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 *****************************************************************************************
 */
void uart_one_wire_tx_en(uart_t *uart_id);

/**
 ****************************************************************************************
 * @brief Enable One-Wire-UART Receive path
 * @param[in] uart_id       Identifies which UART to use
 * @return void
 *****************************************************************************************
 */
void uart_one_wire_rx_en(uart_t *uart_id);
#endif

/**
 * The following section exposes the API of the UART1 driver implemented in ROM.
 * When the ROM UART1 driver is selected, it cannot be combined with the SDK API for UART1.
 */

/**
 ****************************************************************************************
 * @brief Initialize UART1.
 * @note This function is implemented in ROM and cannot be combined with the SDK API for
 * UART1.
 * @param[in] baudr         Baudrate Divisor Value
 * @param[in] dlf_value     Baudrate Fractional Value
 * @param[in] mode          Data bits mode
 * @return void
 *****************************************************************************************
 */
void uart_init(uint16_t baudr, uint8_t dlf_value, uint8_t mode);

/**
 ****************************************************************************************
 * @brief Enable UART1 flow control.
 * @note This function is implemented in ROM and cannot be combined with the SDK API for
 * UART1.
 * @return void
 *****************************************************************************************
 */
void uart_flow_on(void);

/**
 ****************************************************************************************
 * @brief Disable UART1 flow control.
 * @note This function is implemented in ROM and cannot be combined with the SDK API for
 * UART1.
 * @return bool True if flow control has been successfully disabled.
 *****************************************************************************************
 */
bool uart_flow_off(void);

/**
 ****************************************************************************************
 * @brief Finish current UART1 transfers.
 * @note This function is implemented in ROM and cannot be combined with the SDK API for
 * UART1.
 * @return void
 *****************************************************************************************
 */
void uart_finish_transfers(void);

/**
 ****************************************************************************************
 * @brief Start data reception.
 * @note This function is implemented in ROM and cannot be combined with the SDK API for
 * UART1.
 * @param[out] bufptr       Pointer to the RX buffer
 * @param[in] size          Size of the expected reception
 * @param[in] callback      Callback to return upon completion
 * @return void
 *****************************************************************************************
 */
void uart_read(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));

/**
 ****************************************************************************************
 * @brief Start data transmission.
 * @note This function is implemented in ROM and cannot be combined with the SDK API for
 * UART1.
 * @param[in] bufptr        Pointer to the TX buffer
 * @param[in] size          Size of the transmission
 * @param[in] callback      Callback to return upon completion
 * @return void
 *****************************************************************************************
 */
void uart_write(uint8_t *bufptr, uint32_t size, void (*callback) (uint8_t));

/**
 ****************************************************************************************
 * @brief Set the limit of retries to disable flow control
 * @note This function is implemented in ROM and cannot be combined with the SDK API for
 * UART1.
 * @param[in]  limit         Maximum number of retries
 * @return void
 *****************************************************************************************
 */
void uart_set_flow_off_retries_limit(uint32_t limit);

#endif /* _UART_H_ */
