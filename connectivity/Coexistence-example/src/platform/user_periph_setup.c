/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (C) 2015-2021 Dialog Semiconductor
# The MIT License (MIT)
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
# OR OTHER DEALINGS IN THE SOFTWARE.E.
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

#if (WLAN_COEX_ENABLED)
#include "wlan_coex.h"
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
 
 #if (WLAN_COEX_ENABLED)
// Configuration struct for WLAN coexistence
const wlan_coex_cfg_t wlan_coex_cfg = {
   .ext_24g_eip_port = WLAN_COEX_24G_EIP_PORT,
   .ext_24g_eip_pin = WLAN_COEX_24G_EIP_PIN,
   .ble_eip_port = WLAN_COEX_BLE_EIP_PORT,
   .ble_eip_pin = WLAN_COEX_BLE_EIP_PIN,
   .ble_prio_port = WLAN_COEX_BLE_PRIO_PORT,
   .ble_prio_pin = WLAN_COEX_BLE_PRIO_PIN,
#if defined (CFG_WLAN_COEX_DEBUG)
   .debug_a_port = WLAN_COEX_DEBUG_A_PORT,
   .debug_a_pin = WLAN_COEX_DEBUG_A_PIN,
   .debug_b_port = WLAN_COEX_DEBUG_B_PORT,
   .debug_b_pin = WLAN_COEX_DEBUG_B_PIN,
#endif
   .irq = WLAN_COEX_IRQ,
};
#endif

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/

#if (WLAN_COEX_ENABLED)
   RESERVE_GPIO(COEX_EIP, wlan_coex_cfg.ble_eip_port, wlan_coex_cfg.ble_eip_pin, PID_GPIO);
   RESERVE_GPIO(COEX_PRIO, wlan_coex_cfg.ble_prio_port, wlan_coex_cfg.ble_prio_pin, PID_GPIO);
   RESERVE_GPIO(COEX_REQ, wlan_coex_cfg.ext_24g_eip_port, wlan_coex_cfg.ext_24g_eip_pin, PID_GPIO);
#endif /*Extra Addition*/
#if defined (CFG_WLAN_COEX_DEBUG)
   RESERVE_GPIO(DEBUGPIN1, wlan_coex_cfg.debug_b_port, wlan_coex_cfg.debug_b_pin, PID_GPIO);
   RESERVE_GPIO(DEBUGPIN2, wlan_coex_cfg.debug_a_port, wlan_coex_cfg.debug_a_pin, PID_GPIO);
#endif
    
#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif
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
#endif

#if defined (CFG_PRINTF_UART2)
    // Configure UART2 TX Pad
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif

#if (WLAN_COEX_ENABLED)
   wlan_coex_gpio_cfg();
#endif
    
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
