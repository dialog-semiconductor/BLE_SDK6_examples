/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (c) 2015-2021 Dialog Semiconductor
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_periph_setup.h"
#include "datasheet.h"
#include "system_library.h"
#include "rwip_config.h"
#include "gpio.h"
#include "uart.h"
#include "syscntl.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

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
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/

#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif

    //RESERVE_GPIO(LED, GPIO_LED_PORT, GPIO_LED_PIN, PID_GPIO);

#if !defined (__DA14586__) && !defined(__DA14531__)
    RESERVE_GPIO(SPI_EN, SPI_EN_PORT, SPI_EN_PIN, PID_SPI_EN);
#endif
		RESERVE_GPIO(GAMEPAD_LS_X, GAMEPAD_LS_X_PORT, GAMEPAD_LS_X_PIN, PID_ADC);
		RESERVE_GPIO(GAMEPAD_LS_Y, GAMEPAD_LS_Y_PORT, GAMEPAD_LS_Y_PIN, PID_ADC);
#ifndef DEBUGGING
    RESERVE_GPIO(GAMEPAD_RS_X, GAMEPAD_RS_X_PORT, GAMEPAD_RS_X_PIN, PID_ADC);
#endif
		RESERVE_GPIO(GAMEPAD_RS_Y, GAMEPAD_RS_Y_PORT, GAMEPAD_RS_Y_PIN, PID_ADC);
		
		RESERVE_GPIO(GAMEPAD_BUTTON_A, GAMEPAD_BUTTON_A_PORT, GAMEPAD_BUTTON_A_PIN, PID_GPIO);
		RESERVE_GPIO(GAMEPAD_BUTTON_B, GAMEPAD_BUTTON_B_PORT, GAMEPAD_BUTTON_B_PIN, PID_GPIO);
		RESERVE_GPIO(GAMEPAD_BUTTON_STA, GAMEPAD_BUTTON_STA_PORT, GAMEPAD_BUTTON_STA_PIN, PID_GPIO);
		RESERVE_GPIO(GAMEPAD_BUTTON_SEL, GAMEPAD_BUTTON_SEL_PORT, GAMEPAD_BUTTON_SEL_PIN, PID_GPIO);

}

#endif
void set_pad_functions(void)
{
/*
    i.e. to set P0_1 as Generic purpose Output:
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
*/

#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#elif !defined(__DA14531__)
    // Disallow spontaneous SPI Flash wake-up
    GPIO_ConfigurePin(SPI_EN_PORT, SPI_EN_PIN, OUTPUT, PID_SPI_EN, true);
#endif

#if defined (CFG_PRINTF_UART2)
    // Configure UART2 TX Pad
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif

    GPIO_ConfigurePin(GPIO_LED_PORT, GPIO_LED_PIN, OUTPUT, PID_GPIO, false);
	
		GPIO_ConfigurePin(GAMEPAD_LS_X_PORT, GAMEPAD_LS_X_PIN, INPUT, PID_ADC, false);
		GPIO_ConfigurePin(GAMEPAD_LS_Y_PORT, GAMEPAD_LS_Y_PIN, INPUT, PID_ADC, false);
		GPIO_ConfigurePin(GAMEPAD_RS_X_PORT, GAMEPAD_RS_X_PIN, INPUT, PID_ADC, false);
		GPIO_ConfigurePin(GAMEPAD_RS_Y_PORT, GAMEPAD_RS_Y_PIN, INPUT, PID_ADC, false);
		
		GPIO_ConfigurePin(GAMEPAD_BUTTON_A_PORT, GAMEPAD_BUTTON_A_PIN, INPUT, PID_GPIO, false);
		GPIO_ConfigurePin(GAMEPAD_BUTTON_B_PORT, GAMEPAD_BUTTON_B_PIN, INPUT, PID_GPIO, false);
		GPIO_ConfigurePin(GAMEPAD_BUTTON_STA_PORT, GAMEPAD_BUTTON_STA_PIN, INPUT_PULLUP, PID_GPIO, false);
		GPIO_ConfigurePin(GAMEPAD_BUTTON_SEL_PORT, GAMEPAD_BUTTON_SEL_PIN, INPUT_PULLUP, PID_GPIO, false);
}

#if defined (CFG_PRINTF_UART2)
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
#endif

void periph_init(void)
{
#if defined (__DA14531__)
    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#else
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    // ROM patch
    patch_func();

    // Initialize peripherals
#if defined (CFG_PRINTF_UART2)
    // Initialize UART2
    uart_initialize(UART2, &uart_cfg);
#endif

    // Set pad functionality
    set_pad_functions();

    // Enable the pads
    GPIO_set_pad_latch_en(true);
}
