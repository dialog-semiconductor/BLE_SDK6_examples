/**
 ****************************************************************************************
 *
 * @file periph_setup.c
 *
 * @brief Peripherals initialization functions
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

#include "user_periph_setup.h"
#include "datasheet.h"

static void set_pad_functions(void)
{
#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#endif

		GPIO_ConfigurePin(GPIO_SW_PORT, GPIO_SW_PIN, INPUT_PULLUP, PID_GPIO, false);
	
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);

    GPIO_ConfigurePin(LED_PORT, LED_PIN, OUTPUT, PID_GPIO, false);
}

// Configuration struct for UART2
static const uart_cfg_t uart_cfg = {
    .baud_rate = UART2_BAUDRATE,
    .data_bits = UART2_DATABITS,
    .parity = UART2_PARITY,
    .stop_bits = UART2_STOPBITS,
    .auto_flow_control = UART2_AFCE,
    .use_fifo = UART2_FIFO,
    .tx_fifo_tr_lvl = UART2_TX_FIFO_LEVEL,
    .rx_fifo_tr_lvl = UART2_RX_FIFO_LEVEL,
    .intr_priority = 2,
};

void periph_init(void)
{
#if !defined (__DA14531__)
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif
	
    uart_initialize(UART2, &uart_cfg);

    set_pad_functions();

    GPIO_set_pad_latch_en(true);
}