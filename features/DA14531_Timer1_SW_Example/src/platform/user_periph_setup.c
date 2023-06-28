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
#if (ENABLE_TMR_COUNTING)
extern bool led_state;
#endif

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/
#if (ENABLE_PULSE_MEASURING)
	RESERVE_GPIO(SIGNAL, GPIO_SIGNAL_PORT, GPIO_SIGNAL_PIN, PID_GPIO);
#endif
	
#if (ENABLE_FREQ_COUNTING && PWM_TIMER0_ENABLE)	
    RESERVE_GPIO(SIGNAL, GPIO_SIGNAL_PORT, GPIO_SIGNAL_PIN, PID_PWM0);
#endif
    
#if (ENABLE_TMR_COUNTING)
    RESERVE_GPIO(LED, GPIO_LED_PORT, GPIO_LED_PIN, PID_GPIO);
#endif
	
#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif
    //RESERVE_GPIO(SYS_CLK, GPIO_PORT_0, GPIO_PIN_8, PID_GPIO);
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

#if (ENABLE_PULSE_MEASURING)
	GPIO_ConfigurePin(GPIO_SIGNAL_PORT, GPIO_SIGNAL_PIN, INPUT_PULLUP, PID_GPIO, false);
#endif
	
#if (ENABLE_FREQ_COUNTING && PWM_TIMER0_ENABLE)
    GPIO_ConfigurePin(GPIO_SIGNAL_PORT, GPIO_SIGNAL_PIN, OUTPUT, PID_PWM0, false);
#endif
    
#if (ENABLE_TMR_COUNTING)
    GPIO_ConfigurePin(GPIO_LED_PORT, GPIO_LED_PIN, OUTPUT, PID_GPIO, led_state);
#endif
	
#if defined (CFG_PRINTF_UART2)
    // Configure UART2 TX Pad
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
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