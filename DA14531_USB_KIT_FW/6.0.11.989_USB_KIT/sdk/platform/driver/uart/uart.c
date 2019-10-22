/**
 ****************************************************************************************
 *
 * @file uart.c
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

#include <stdint.h>
#include <string.h>
#include "compiler.h"
#include "uart.h"
#include "ll.h"

#if defined (CFG_UART_DMA_SUPPORT)
#include "dma.h"
#endif

/*
 * UART Environment
 ****************************************************************************************
 */
/// @brief UART environment definition
typedef struct
{
    /// Pointer to the data to transmit
    const uint8_t           *tx_buffer;

    /// Length of data to transmit in Bytes
    uint16_t                tx_total_length;

    /// Current index of transmitted data
    uint16_t                tx_index;

    /// Pointer to the buffer where received data will be stored
    uint8_t                 *rx_buffer;

    /// Length of data to receive in Bytes
    uint16_t                rx_total_length;

    /// Current index of received data
    uint16_t                rx_index;

    /// UART error status callback
    uart_err_cb_t           err_cb;

    /// UART transmit callback
    uart_cb_t               tx_cb;

    /// UART receive callback
    uart_cb_t               rx_cb;

    /// UART interrupt priority
    uint8_t                 intr_priority;

    /// UART receive FIFO trigger level
    UART_RX_FIFO_LEVEL      rx_fifo_tr_lvl;

#if defined (CFG_UART_DMA_SUPPORT)
    /// UART DMA RX Channel
    DMA_ID                  uart_dma_rx_channel;

    /// UART DMA TX Channel
    DMA_ID                  uart_dma_tx_channel;
#endif

#if defined (CFG_UART_ONE_WIRE_SUPPORT)
    /// Enable/Disable One-Wire-UART functionality
    bool                    one_wire_enabled;

    /// Port used with One-Wire-UART
    GPIO_PORT               one_wire_port;

    /// Pin used with One-Wire-UART
    GPIO_PIN                one_wire_pin;
#endif
} uart_env_t;

/// UART environment, retained in retention memory area.
static uart_env_t uartn_env[2] __SECTION_ZERO("retention_mem_area0");

/*
 * STATIC FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if !defined(__EXCLUDE_UART2_HANDLER__) || defined(CFG_UART1_SDK) || defined(__NON_BLE_EXAMPLE__)
/**
 ****************************************************************************************
 * @brief Transmit interrupt handler, when DMA is not used
 * @param[in] uart_id      Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
static void uart_tx_isr(uart_t *uart_id)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // While there is available space, put data in thr_reg or FIFO
    while (uart_env->tx_index < uart_env->tx_total_length)
    {
        // If FIFO is enabled, exit if it is full
        if (uart_fifo_enabled_getf(uart_id))
        {
            if(!uart_txfifo_not_full_getf(uart_id))
            {
                return;
            }
        }
        // Else, exit if THR is full
        else if (!uart_thr_empty_getf(uart_id))
        {
            return;
        }
        // Send one byte
        uart_thr_setf(uart_id, uart_env->tx_buffer[uart_env->tx_index++]);
    }

    // Everything was sent
    // Disable TX interrupts
    uart_txdata_intr_setf(uart_id, UART_BIT_DIS);
    uart_thre_intr_setf(uart_id, UART_BIT_DIS);

    // Fire callback
    if (uart_env->tx_cb != NULL)
    {
        uart_env->tx_cb(uart_env->tx_total_length);
    }
}

/**
 ****************************************************************************************
 * @brief Receive interrupt handler, when DMA is not used
 * @param[in] uart_id      Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
static void uart_rx_isr(uart_t *uart_id)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Read the available data in RBR
    while (uart_env->rx_index < uart_env->rx_total_length)
    {
        // Read 1 byte
        if (uart_data_ready_getf(uart_id))
        {
            uart_env->rx_buffer[uart_env->rx_index++] = uart_read_rbr(uart_id);
        }
        // Return
        else
        {
            return;
        }
    }

    // All data have been read.
    // Disable RX interrupts
    uart_rxdata_intr_setf(uart_id, UART_BIT_DIS);

    // Fire callback
    if (uart_env->rx_cb != NULL)
    {
        uart_env->rx_cb(uart_env->rx_index);
    }
}

/**
 ****************************************************************************************
 * @brief Character Timeout Interrupt handler
 * @param[in] uart_id      Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
static void uart_rx_timeout_isr(uart_t *uart_id)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Read already received data
    uart_rx_isr(uart_id);

    // Disable RX interrupts
    uart_rxdata_intr_setf(uart_id, UART_BIT_DIS);

    // Fire callback
    if (uart_env->rx_cb != NULL)
    {
        uart_env->rx_cb(uart_env->rx_index);
    }
}

/**
 ****************************************************************************************
 * @brief Receiver line status error interrupt handler
 * @param[in] uart_id      Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
static void uart_rls_isr(uart_t *uart_id)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Fire error callback
    if (uart_env->err_cb != NULL)
    {
        uart_env->err_cb(uart_id, uart_rls_error_getf(uart_id));
    }
}

/**
 ****************************************************************************************
 * @brief Busy detect interrupt handler
 * @param[in] uart_id      Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
static void uart_busy_isr(uart_t *uart_id)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Fire error callback
    if (uart_env->err_cb != NULL)
    {
        uart_env->err_cb(uart_id, UART_ERR_BUSY_ERROR);
    }
}
#endif

#if defined (CFG_UART_DMA_SUPPORT)
static void uart_dma_init(uart_t *uart_id, const uart_cfg_t *uart_cfg)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Save channel configuration to UART environment
    uart_env->uart_dma_rx_channel = uart_cfg->uart_dma_channel == UART_DMA_CHANNEL_01 ? DMA_CHANNEL_0 : DMA_CHANNEL_2;

    // Configure DMA for UART Rx
    dma_cfg_t dma_uart_cfg = {
        .bus_width =    DMA_BW_BYTE,
        .irq_enable =   DMA_IRQ_STATE_ENABLED,
        .dreq_mode =    DMA_DREQ_TRIGGERED,
        .src_inc =      DMA_INC_FALSE,
        .dst_inc =      DMA_INC_TRUE,
        .circular =     DMA_MODE_NORMAL,
        .dma_prio =     uart_cfg->uart_dma_priority,
        .dma_idle =     DMA_IDLE_BLOCKING_MODE,
        .dma_init =     DMA_INIT_AX_BX_AY_BY,
        .dma_sense =    DMA_SENSE_LEVEL_SENSITIVE,
        .dma_req_mux =  UART_ID(uart_id) == 0 ? DMA_TRIG_UART_RXTX : DMA_TRIG_UART2_RXTX,
        .src_address =  (uint32_t) &uart_id->UART_RBR_THR_DLL_REGF,
        .irq_nr_of_trans = 0,
    };

    // Initialize DMA Rx Channel
    dma_initialize(uart_env->uart_dma_rx_channel, &dma_uart_cfg);

    // Save channel configuration to UART environment
    uart_env->uart_dma_tx_channel = uart_cfg->uart_dma_channel == UART_DMA_CHANNEL_01 ? DMA_CHANNEL_1 : DMA_CHANNEL_3;

    // Configure DMA for UART Tx
    dma_uart_cfg.src_inc = DMA_INC_TRUE;
    dma_uart_cfg.dst_inc = DMA_INC_FALSE;
    dma_uart_cfg.dma_sense = DMA_SENSE_LEVEL_SENSITIVE;
    dma_uart_cfg.dst_address = (uint32_t) &uart_id->UART_RBR_THR_DLL_REGF;

    // Initialize DMA Tx Channel
    dma_initialize(uart_env->uart_dma_tx_channel, &dma_uart_cfg);
}

/**
 ****************************************************************************************
 * @brief Tx DMA callback, called upon Tx DMA completion
 * @param[in] uart_id      Identifies which UART to use
 * @param[in] len           Data length of completed transaction
 * @return void
 ****************************************************************************************
 */
static void uart_tx_dma_callback(void *uart_id, uint16_t len)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Fire user callback
    if (uart_env->tx_cb != NULL)
    {
        uart_env->tx_cb(len);
    }
}

/**
 ****************************************************************************************
 * @brief Finish Receive. Reads the remaining bytes from RX FIFO.
 * @param[in] uart_id      Identifies which UART to use
 * @return uint8_t          Number of data read from RX FIFO
 ****************************************************************************************
 */
static uint8_t uart_finish_receive(uart_t *uart_id)
{
    uart_env_t *uart_env = UART_ENV(uart_id);
    uint8_t rx_count = 0;

    // Read the remaining bytes from RX FIFO.
    while (uart_env->rx_index < uart_env->rx_total_length)
    {
        if (uart_data_ready_getf(uart_id))
        {
            uart_env->rx_buffer[uart_env->rx_index++] = uart_read_rbr(uart_id);
            rx_count++;
        }
    }

    return rx_count;
}

/**
 ****************************************************************************************
 * @brief Rx DMA callback, called upon Rx DMA completion
 * @param[in] uart_id    Pointer to user data
 * @param[in] len           Data length of completed transaction
 * @return void
 ****************************************************************************************
 */
static void uart_rx_dma_callback(void *uart_id, uint16_t len)
{
    uart_env_t *uart_env = UART_ENV(uart_id);
    uint8_t rest_rcved_count = 0;

    /*  If UART_RX_FIFO_LEVEL is not UART_RX_FIFO_LEVEL_0, then DMA cannot read all data from FIFO, since it does not
        support programmable burst size. In that case, the actual dma transfer length depends on UART_RX_FIFO_LEVEL.
        CB should check if there are more data in FIFO and read them without DMA, to finish the transfer successfully.
    */
    if (len != uart_env->rx_total_length)
    {
        rest_rcved_count = uart_finish_receive(uart_id);
    }

    // Fire user callback
    if (uart_env->rx_cb != NULL)
    {
        uart_env->rx_cb(len + rest_rcved_count);
    }
}
#endif

#if !defined(__EXCLUDE_UART2_HANDLER__) || defined(CFG_UART1_SDK) || defined(__NON_BLE_EXAMPLE__)
/**
 ****************************************************************************************
 * @brief UART/UART2 interrupt handler
 * @param[in] uart_id      Identifies which UART to use
 * @return void
 ****************************************************************************************
 */
static void uart_intr_hndl(uart_t *uart_id)
{
    uint8_t int_id;
    for (;;)
    {
        int_id = uart_intr_id_getf(uart_id);
        switch (int_id)
        {
            // Timeout interrupt
            case UART_INT_TIMEOUT:
            {
                uart_rx_timeout_isr(uart_id);
                break;
            }
            // No other interrupt pending
            case UART_INT_NO_INT_PEND:
            {
                return;
            }
            // THR empty interrupt
            case UART_INT_THR_EMPTY:
            {
                uart_tx_isr(uart_id);
                break;
            }
            // Received data available interrupt
            case UART_INT_RECEIVED_AVAILABLE:
            {
                uart_rx_isr(uart_id);
                break;
            }
            // Receiver line status error interrupt
            case UART_INT_RECEIVE_LINE_STAT:
            {
                uart_rls_isr(uart_id);
                break;
            }
            // Busy detect interrupt
            case UART_INT_BUSY_DETECTED:
            {
                uart_busy_isr(uart_id);
                break;
            }
            default:
                break;
        }
    }
}
#endif

#if defined (CFG_UART1_SDK)
/**
 ****************************************************************************************
 * @brief UART interrupt handler. It is used only in BLE projects, when UART1 ROM driver
 * is overriden by SDK driver.
 * @return void
 ****************************************************************************************
 */
void UART_Handler_SDK_func(void)
{
    uart_intr_hndl(UART1);
}

#elif defined (__NON_BLE_EXAMPLE__)
/**
 ****************************************************************************************
 * @brief UART interrupt handler. It is used only in non BLE projects.
 * @return void
 ****************************************************************************************
 */
void UART_Handler(void)
{
    uart_intr_hndl(UART1);
}
#endif

#if !defined(__EXCLUDE_UART2_HANDLER__)
/**
 ****************************************************************************************
 * @brief UART2 interrupt handler.
 * @return void
 ****************************************************************************************
 */
void UART2_Handler(void)
{
    uart_intr_hndl(UART2);
}
#endif

/*
 * UART Configuration API
 ****************************************************************************************
 */

void uart_enable(uart_t *uart_id)
{
    // Disable interrupts
    GLOBAL_INT_DISABLE();

    // Enable clock for UART peripheral
    if (uart_id == UART1)
    {
        SetBits16(CLK_PER_REG, UART1_ENABLE, UART_BIT_EN);
    }
    else
    {
        SetBits16(CLK_PER_REG, UART2_ENABLE, UART_BIT_EN);
    }

    // Enable interrupts
    GLOBAL_INT_RESTORE();
}

void uart_disable(uart_t *uart_id)
{
    // Reset UART
    uart_rst_shd_setf(uart_id);

    // Disable interrupts
    GLOBAL_INT_DISABLE();

    // Disable clock for UART peripheral
    if (uart_id == UART1)
    {
        SetBits16(CLK_PER_REG, UART1_ENABLE, UART_BIT_DIS);
    }
    else
    {
        SetBits16(CLK_PER_REG, UART2_ENABLE, UART_BIT_DIS);
    }

    // Enable interrupts
    GLOBAL_INT_RESTORE();
}

void uart_baudrate_setf(uart_t *uart_id, UART_BAUDRATE baud_rate)
{
    // Wait as long as UART is in busy state
    while(uart_is_busy_getf(uart_id));

    // Set Divisor Latch Access Bit in LCR register to access DLL & DLH registers
    uart_dlab_setf(uart_id, UART_BIT_EN);

    // Set fraction byte of baud rate
    SetWord16(&uart_id->UART_DLF_REGF, 0xFF & baud_rate);
    // Set low byte of baud rate
    SetWord16(&uart_id->UART_RBR_THR_DLL_REGF, 0xFF & baud_rate >> 8);
    // Set high byte of baud rare
    SetWord16(&uart_id->UART_IER_DLH_REGF, 0xFF & baud_rate >> 16);

    // Reset Divisor Latch Access Bit in LCR register
    uart_dlab_setf(uart_id, UART_BIT_DIS);
}

void uart_register_tx_cb(uart_t *uart_id, uart_cb_t cb)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Save callback to environment
    uart_env->tx_cb = cb;
}

void uart_register_rx_cb(uart_t *uart_id, uart_cb_t cb)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Save callback to environment
    uart_env->rx_cb = cb;
}

void uart_register_err_cb(uart_t *uart_id, uart_err_cb_t cb)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Save callback to environment
    uart_env->err_cb = cb;
}

void uart_initialize(uart_t *uart_id, const uart_cfg_t *uart_cfg)
{
    // Initialize Environment
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Enable UART clock
    uart_enable(uart_id);

#if defined (CFG_UART_LOOPBACK_MODE)
    /* If enabled, loopback mode shall be set before activating the pads or
       before setting the Baud Rate to ensure that UART module is not busy.
    */
    uart_loopback_setf(uart_id, UART_BIT_EN);
#endif

    // Read UART_USR_REG to clear any pending busy interrupt
    uart_txfifo_is_empty_getf(uart_id);

    // Set baud rate
    uart_baudrate_setf(uart_id, uart_cfg->baud_rate);

    // Set Parity, Stop and Data bits
    uart_lcr_setf(uart_id,  (uart_cfg->parity << 3) |
                            (uart_cfg->stop_bits << 2) |
                            (uart_cfg->data_bits << 0));

    // Set FIFO configuration
    uart_fifo_enable_shd_setf(uart_id, uart_cfg->use_fifo);
    if (uart_cfg->use_fifo)
    {
        // Set Auto flow control. AFC is supported only when FIFOs are enabled
#if defined (__DA14531__)
        if (uart_id == UART1)
#endif
        {
            uart_afce_setf(uart_id, uart_cfg->auto_flow_control);
            uart_rtsn_setf(uart_id, uart_cfg->auto_flow_control);
        }

        // Fifo set RCVR trigger level
        uart_fifo_rcvr_trg_shd_setf(uart_id, uart_cfg->rx_fifo_tr_lvl);
        // Fifo set Tx empty trigger level
        uart_fifo_txempty_trg_shd_setf(uart_id, uart_cfg->tx_fifo_tr_lvl);
    }

    // Set DMA configuration
#if defined (CFG_UART_DMA_SUPPORT)
    uart_dma_init(uart_id, uart_cfg);
#endif

    // Save configuration to environment
    uart_env->intr_priority = uart_cfg->intr_priority;
    uart_env->err_cb = uart_cfg->uart_err_cb;
    uart_env->tx_cb = uart_cfg->uart_tx_cb;
    uart_env->rx_cb = uart_cfg->uart_rx_cb;
    uart_env->rx_fifo_tr_lvl = uart_cfg->rx_fifo_tr_lvl;
}


/*
 * UART Send API
 ****************************************************************************************
 */

void uart_write_byte(uart_t *uart_id, uint8_t data)
{
    // Wait if Transmit Holding Register or Tx FIFO is full
    while (!uart_thr_empty_getf(uart_id));

    // Write data to THR
    uart_thr_setf(uart_id, data);
}

void uart_write_buffer(uart_t *uart_id, const uint8_t *data, uint16_t len)
{
    while (len > 0)
    {
        uart_write_byte(uart_id, *data++);
        len--;
    }
}

void uart_send(uart_t *uart_id, const uint8_t *data, uint16_t len, UART_OP_CFG op)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Initialize UART environment
    uart_env->tx_index = 0;
    uart_env->tx_total_length = len;

    // Blocking transmit procedure
    if (op == UART_OP_BLOCKING)
    {
        // Save starting address of data in environment
        uart_write_buffer(uart_id, data, len);
    }
    else if (op == UART_OP_INTR)
    {
        uart_env->tx_buffer = data;
        // Disable NVIC interrupts
        NVIC_DisableIRQ(UART_INTR(uart_id));

        // Enable UART transmit interrupts
        uart_txdata_intr_setf(uart_id, UART_BIT_EN);
        uart_thre_intr_setf(uart_id, UART_BIT_EN);

        // Enable interrupt priority
        NVIC_SetPriority(UART_INTR(uart_id), uart_env->intr_priority);
        // Enable NVIC interrupts
        NVIC_EnableIRQ(UART_INTR(uart_id));
    }
#if defined (CFG_UART_DMA_SUPPORT)
    else
    {
        dma_set_src(uart_env->uart_dma_tx_channel, (uint32_t) data);
        // Initiate the DMA transfer and return
        uart_dmasa_setf(uart_id, UART_BIT_EN);
        // Update Tx DMA INT
        dma_set_int(uart_env->uart_dma_tx_channel, len);
        // Update Tx DMA length
        dma_set_len(uart_env->uart_dma_tx_channel, len);
        // Update DMA callback
        dma_register_callback(uart_env->uart_dma_tx_channel, uart_tx_dma_callback, uart_id);
        // Start DMA
        dma_channel_start(uart_env->uart_dma_tx_channel, DMA_IRQ_STATE_ENABLED);
    }
#endif
}

void uart_wait_tx_finish(uart_t *uart_id)
{
    while (!uart_tx_empty_getf(uart_id));
}


/*
 * UART Receive API
 ****************************************************************************************
 */

uint8_t uart_read_byte(uart_t *uart_id)
{
    // Wait until received data are available
    while (!uart_data_ready_getf(uart_id));

    // Read element from the receive FIFO
    return uart_read_rbr(uart_id);
}

void uart_read_buffer(uart_t *uart_id, uint8_t *data, uint16_t len)
{
    while (len > 0)
    {
        *data++ = uart_read_byte(uart_id);
        len--;
    }
}

void uart_receive(uart_t *uart_id, uint8_t *data, uint16_t len, UART_OP_CFG op)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Initialize UART environment
    uart_env->rx_total_length = len;
    uart_env->rx_index = 0;
    // Save starting address of data in environment
    uart_env->rx_buffer = data;

    // Blocking receive procedure
    if (op == UART_OP_BLOCKING)
    {
        uart_read_buffer(uart_id, data, len);
    }
    else if (op == UART_OP_INTR)
    {
        // Disable NVIC interrupts
        NVIC_DisableIRQ(UART_INTR(uart_id));

        // Enable receive interrupts
        uart_rxdata_intr_setf(uart_id, UART_BIT_EN);
        uart_rls_intr_setf(uart_id, UART_BIT_EN);

        // Enable interrupt priority
        NVIC_SetPriority(UART_INTR(uart_id), uart_env->intr_priority);
        // Enable NVIC interrupts
        NVIC_EnableIRQ(UART_INTR(uart_id));
    }
#if defined (CFG_UART_DMA_SUPPORT)
    else
    {
        /*  If UART_RX_FIFO_LEVEL is not UART_RX_FIFO_LEVEL_0, then DMA cannot read all data from FIFO, since it does not
        support programmable burst size. In that case, decrease the dma transfer length (depending on UART_RX_FIFO_LEVEL)
        to ensure that DMA finishes. Then read the rest of the data without DMA. RX Length must be greater than
        FIFO_SIZE (= 16) - Trigger Level.
        */
        uart_env->rx_index = (
                            uart_env->rx_fifo_tr_lvl == UART_RX_FIFO_LEVEL_0 ? len :
                            uart_env->rx_fifo_tr_lvl == UART_RX_FIFO_LEVEL_1 ? len - 4 :
                            uart_env->rx_fifo_tr_lvl == UART_RX_FIFO_LEVEL_2 ? len - 8 :
                            len - 14);
        dma_set_dst(uart_env->uart_dma_rx_channel, (uint32_t) data);
        // Initiate the DMA transfer and return
        uart_dmasa_setf(uart_id, UART_BIT_EN);
        // Update Tx DMA INT
        dma_set_int(uart_env->uart_dma_rx_channel, uart_env->rx_index);
        // Update Tx DMA length
        dma_set_len(uart_env->uart_dma_rx_channel, uart_env->rx_index);
        // Update DMA callback
        dma_register_callback(uart_env->uart_dma_rx_channel, uart_rx_dma_callback, uart_id);
        // Start DMA
        dma_channel_start(uart_env->uart_dma_rx_channel, DMA_IRQ_STATE_ENABLED);
    }
#endif
}

void uart_enable_flow_control(uart_t *uart_id)
{
    uart_afce_setf(uart_id, UART_AFCE_EN);
    uart_rtsn_setf(uart_id, UART_AFCE_EN);
}

bool uart_disable_flow_control(uart_t *uart_id, uint32_t retries)
{
    // First check if no transmission is ongoing
    while ((uart_tx_empty_getf(uart_id) == 0) || (uart_thr_empty_getf(uart_id) == 0))
    {
        if (retries-- == 0)
        {
            // Failed to disable flow control
            return false;
        }
    }

    // Disable flow control
    uart_afce_setf(uart_id, UART_AFCE_DIS);
    uart_rtsn_setf(uart_id, UART_AFCE_DIS);

    // Wait for 1 character duration to ensure host has not started a transmission at the
    // same time
    for (volatile uint16_t i = 0; i < 350; i++);

    // Check if data has been received during wait time
    if (uart_data_ready_getf(uart_id))
    {
        // Re-enable UART flow control
        uart_enable_flow_control(uart_id);

        // Failed to disable flow control
        return false;
    }

    return true;
}

#if defined (CFG_UART_ONE_WIRE_SUPPORT)

void uart_one_wire_enable(uart_t *uart_id, GPIO_PORT port, GPIO_PIN pin)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    // Store settings to environment
    uart_env->one_wire_enabled = true;
    uart_env->one_wire_port = port;
    uart_env->one_wire_pin = pin;

    GPIO_ConfigurePin(uart_env->one_wire_port, uart_env->one_wire_pin, INPUT, 
                      (uart_id == UART1) ? PID_UART1_RX : PID_UART2_RX, false);
}

void uart_one_wire_disable(uart_t *uart_id)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    uart_env->one_wire_enabled = false;
}

void uart_one_wire_tx_en(uart_t *uart_id)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    if (uart_env->one_wire_enabled)
    {
        GPIO_ConfigurePin(uart_env->one_wire_port, uart_env->one_wire_pin, OUTPUT,
                          (uart_id == UART1) ? PID_UART1_TX : PID_UART2_TX, false);
    }
}

void uart_one_wire_rx_en(uart_t *uart_id)
{
    uart_env_t *uart_env = UART_ENV(uart_id);

    if (uart_env->one_wire_enabled)
    {
        uart_wait_tx_finish(uart_id);

        GPIO_ConfigurePin(uart_env->one_wire_port, uart_env->one_wire_pin, INPUT,
                          (uart_id == UART1) ? PID_UART1_RX : PID_UART2_RX, false);
    }
}

#endif
