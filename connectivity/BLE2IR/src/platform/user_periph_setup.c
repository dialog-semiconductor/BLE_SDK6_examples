/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration
#include "user_periph_setup.h"       // peripheral configuration
#include "gpio.h"
#include "uart.h"                    // UART initialization
#include "system_library.h"
#include "user_modules.h"

/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 *
 * @return void
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
#ifdef CFG_PRINTF_UART2
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
    RESERVE_GPIO(UART2_RX, UART2_RX_PORT, UART2_RX_PIN, PID_UART2_RX);
#endif

    RESERVE_GPIO(LED, GPIO_LED_PORT, GPIO_LED_PIN, PID_GPIO);
}
#endif // CFG_DEVELOPMENT_DEBUG

void set_pad_functions(void)        // set gpio port function mode
{
#ifdef __DA14586__
    // disallow spontaneous flash wake-up
    GPIO_ConfigurePin(SPI_EN_GPIO_PORT, SPI_EN_GPIO_PIN, OUTPUT, PID_GPIO, true);
#endif

#ifdef CFG_PRINTF_UART2
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
    GPIO_ConfigurePin(UART2_RX_PORT, UART2_RX_PIN, INPUT, PID_UART2_RX, false);
#endif

#ifdef LED_INDICATION
    GPIO_ConfigurePin(GPIO_LED_PORT, GPIO_LED_PIN, OUTPUT, PID_GPIO, false);
#endif
}

// Configuration struct for UART
static const uart_cfg_t uart_cfg = {
    // Set Baud Rate
    .baud_rate = UART_BAUDRATE_115200,
    // Set data bits
    .data_bits = UART_DATABITS_8,
    // Set parity
    .parity = UART_PARITY_NONE,
    // Set stop bits
    .stop_bits = UART_STOPBITS_1,
    // Set flow control
    .auto_flow_control = UART_AFCE_DIS,
    // Set FIFO enable
    .use_fifo = UART_FIFO_EN,
    // Set Tx FIFO trigger level
    .tx_fifo_tr_lvl = UART_TX_FIFO_LEVEL_0,
    // Set Rx FIFO trigger level
    .rx_fifo_tr_lvl = UART_RX_FIFO_LEVEL_0,
    // Set interrupt priority
    .intr_priority = 2,
#if defined (CFG_UART_DMA_SUPPORT)
    // Set UART DMA Channel Pair Configuration
    .uart_dma_channel = UART_DMA_CHANNEL_01,
    // Set UART DMA Priority
    .uart_dma_priority = DMA_PRIO_0,
#endif
};

void uart_periph_init(uart_t *uart)
{
    // Initialize UART
    uart_initialize(uart, &uart_cfg);

    if (uart == UART1)
    {
        GPIO_ConfigurePin(UART1_TX_PORT, UART1_TX_PIN, OUTPUT, PID_UART1_TX, false);
        GPIO_ConfigurePin(UART1_RX_PORT, UART1_RX_PIN, INPUT, PID_UART1_RX, false);
    }
    else
    {
        GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
        GPIO_ConfigurePin(UART2_RX_PORT, UART2_RX_PIN, INPUT, PID_UART2_RX, false);
    }

    // Enable the pads
    GPIO_set_pad_latch_en(true);
}


void periph_init(void)
{
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));

    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);

    //rom patch
    patch_func();

    //Init pads
    set_pad_functions();
	
		// Perform gpio initialization for the modules. Set the corresponding 
		// module functions in user_module_config structure.
    user_modules_init_gpios();

    // (Re)Initialize peripherals
    // i.e.
    // uart_init(UART_BAUDRATE_115K2, UART_FRAC_BAUDRATE_115K2, UART_CHARFORMAT_8);

#ifdef CFG_PRINTF_UART2
    SetBits16(CLK_PER_REG, UART2_ENABLE, 1);
		uart_periph_init(UART2);
#endif

   // Enable the pads
    SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 1);
}