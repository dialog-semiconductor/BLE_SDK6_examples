/**
 ****************************************************************************************
 *
 * @file periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (C) 2012-2024 Renesas Electronics Corporation and/or its affiliates.
 * All rights reserved. Confidential Information.
 *
 * This software ("Software") is supplied by Renesas Electronics Corporation and/or its
 * affiliates ("Renesas"). Renesas grants you a personal, non-exclusive, non-transferable,
 * revocable, non-sub-licensable right and license to use the Software, solely if used in
 * or together with Renesas products. You may make copies of this Software, provided this
 * copyright notice and disclaimer ("Notice") is included in all such copies. Renesas
 * reserves the right to change or discontinue the Software at any time without notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS". RENESAS DISCLAIMS ALL WARRANTIES OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. TO THE
 * MAXIMUM EXTENT PERMITTED UNDER LAW, IN NO EVENT SHALL RENESAS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE, EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES. USE OF THIS SOFTWARE MAY BE SUBJECT TO TERMS AND CONDITIONS CONTAINED IN
 * AN ADDITIONAL AGREEMENT BETWEEN YOU AND RENESAS. IN CASE OF CONFLICT BETWEEN THE TERMS
 * OF THIS NOTICE AND ANY SUCH ADDITIONAL LICENSE AGREEMENT, THE TERMS OF THE AGREEMENT
 * SHALL TAKE PRECEDENCE. BY CONTINUING TO USE THIS SOFTWARE, YOU AGREE TO THE TERMS OF
 * THIS NOTICE.IF YOU DO NOT AGREE TO THESE TERMS, YOU ARE NOT PERMITTED TO USE THIS
 * SOFTWARE.
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
#include "fpga_helper.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
__USED gtl_pads_config_u gtl_pads_config __SECTION("fsp_gtl_pads") = {
    .config.gtl_tx_pad_config_tag =  {'T', 'X'},
    .config.tx_pad_port = UART1_TX_PORT,
    .config.tx_pad_pin = UART1_TX_PIN,
    .config.gtl_rx_pad_config_tag =  {'R', 'X'},
    .config.rx_pad_port = UART1_RX_PORT,
    .config.rx_pad_pin = UART1_RX_PIN,
    .config.gtl_rts_pad_config_tag = {'R', 'T', 'S'},
    .config.rts_pad_port = UART1_RTSN_PORT,
    .config.rts_pad_pin = UART1_RTSN_PIN,
    .config.gtl_cts_pad_config_tag = {'C', 'T', 'S'},
    .config.cts_pad_port = UART1_CTSN_PORT,
    .config.cts_pad_pin = UART1_CTSN_PIN,
    .config.por_pads_config_tag = {'P', 'O', 'R'},
    .config.por_pad_port = POR_RESET_PORT,
    .config.por_pad_pin = POR_RESET_PIN,
    .config.por_pad_polarity = GPIO_POR_PIN_POLARITY_HIGH,
};

const gtl_pads_config_t * pad_cfg = &gtl_pads_config.config;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
static bool _UART_POR_conflicts(void)
{
    if (((pad_cfg->rx_pad_port == pad_cfg->por_pad_port) && (pad_cfg->rx_pad_pin == pad_cfg->por_pad_pin)) || 
        ((pad_cfg->tx_pad_port == pad_cfg->por_pad_port) && (pad_cfg->tx_pad_pin == pad_cfg->por_pad_pin)) || 
        ((pad_cfg->rts_pad_port == pad_cfg->por_pad_port) && (pad_cfg->rts_pad_pin == pad_cfg->por_pad_pin)) || 
        ((pad_cfg->cts_pad_port == pad_cfg->por_pad_port) && (pad_cfg->cts_pad_pin == pad_cfg->por_pad_pin))) 
    {
        return true;
    }

    return false;
}

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/
#if defined (__DA14531__)
    if(GPIO_is_valid(pad_cfg->por_pad_port, pad_cfg->por_pad_pin) && !_UART_POR_conflicts()) {
        RESERVE_GPIO(RESET_PIN, pad_cfg->por_pad_port, pad_cfg->por_pad_pin, PID_GPIO);
    }
#endif

    RESERVE_GPIO(UART1_TX, pad_cfg->tx_pad_port, pad_cfg->tx_pad_pin, PID_UART1_TX);
    RESERVE_GPIO(UART1_RX, pad_cfg->rx_pad_port, pad_cfg->rx_pad_pin, PID_UART1_RX);
    RESERVE_GPIO(UART1_RTS, pad_cfg->rts_pad_port, pad_cfg->rts_pad_pin, PID_UART1_RTSN);
    RESERVE_GPIO(UART1_CTS, pad_cfg->cts_pad_port, pad_cfg->cts_pad_pin, PID_UART1_CTSN);

#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX,        UART2_TX_PORT,   UART2_TX_PIN,   PID_UART2_TX);
#endif

#if defined (CFG_WAKEUP_EXT_PROCESSOR)
    // external MCU wakeup GPIO
    RESERVE_GPIO(EXT_WAKEUP_GPIO, EXT_WAKEUP_PORT,      EXT_WAKEUP_PIN,      PID_GPIO);
#endif
}
#endif

void set_pad_functions(void)
{
/*
    i.e. to set P0_1 as Generic purpose Output:
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
*/
#if defined (__DA14531__)
    // Check if POR can be enabled (check if the port/pin is valid and if there is no conflict with the UART pins)
    if(GPIO_is_valid(pad_cfg->por_pad_port, pad_cfg->por_pad_pin) && !_UART_POR_conflicts()) {
        GPIO_Disable_HW_Reset();
        GPIO_ConfigurePin(pad_cfg->por_pad_port, pad_cfg->por_pad_pin, INPUT, PID_GPIO, false);
        GPIO_EnablePorPin(pad_cfg->por_pad_port, pad_cfg->por_pad_pin, 
                          pad_cfg->por_pad_polarity ? GPIO_POR_PIN_POLARITY_HIGH:GPIO_POR_PIN_POLARITY_LOW, 1);
    }
#endif
    
#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#endif

    GPIO_ConfigurePin(pad_cfg->tx_pad_port, pad_cfg->tx_pad_pin, OUTPUT, PID_UART1_TX, false);
    GPIO_ConfigurePin(pad_cfg->rx_pad_port, pad_cfg->rx_pad_pin, INPUT, PID_UART1_RX, false);
    GPIO_ConfigurePin(pad_cfg->rts_pad_port, pad_cfg->rts_pad_pin, OUTPUT, PID_UART1_RTSN, false);
    GPIO_ConfigurePin(pad_cfg->cts_pad_port, pad_cfg->cts_pad_pin, INPUT, PID_UART1_CTSN, false);

#if defined (CFG_PRINTF_UART2)
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif

#if defined (CFG_WAKEUP_EXT_PROCESSOR)
    // External MCU wakeup GPIO
    GPIO_ConfigurePin(EXT_WAKEUP_PORT, EXT_WAKEUP_PIN, OUTPUT, PID_GPIO, false);
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
    // Select FPGA GPIO_MAP 2 to work with the FPGA add-on board SPI memory flash
    // set debugger SWD to SW_CLK = P0[2], SW_DIO=P0[10] to enable RTS flow control on pin 5
    FPGA_HELPER(FPGA_GPIO_MAP_2, SWD_DATA_AT_P0_10);

    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    // Assumption: The connected external peripheral is powered by 3V
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#else
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    // ROM patch
    patch_func();

    // Initialize UART1 ROM driver
    uart_init(BAUD_RATE_DIV(UART1_BAUDRATE), BAUD_RATE_FRAC(UART1_BAUDRATE), UART1_DATABITS);

#if defined (CFG_PRINTF_UART2)
    // Initialize UART2
    uart_initialize(UART2, &uart_cfg);
#endif

    // Set pad functionality
    set_pad_functions();

    // Enable the pads
    GPIO_set_pad_latch_en(true);
}
