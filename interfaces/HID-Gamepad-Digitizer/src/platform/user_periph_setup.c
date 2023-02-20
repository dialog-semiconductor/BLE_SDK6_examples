/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (c) 2015-2019 Dialog Semiconductor. All rights reserved.
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
