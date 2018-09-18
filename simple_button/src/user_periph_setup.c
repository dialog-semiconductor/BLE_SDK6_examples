/**
 ****************************************************************************************
 *
 * @file periph_setup.c
 *
 * @brief Peripherals initialization functions
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
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

#include "uart.h"
#include "gpio.h"
#include "user_periph_setup.h"

 /**
 ****************************************************************************************
 * @brief Enable pad and peripheral clocks assuming that peripheral power domain
 *        is down. The UART and SPI clocks are set.
 ****************************************************************************************
 */
void periph_init(void)
{
    // system init
    SetWord16(CLK_AMBA_REG, 0x00);                 // set clocks (hclk and pclk ) 16MHz
    SetWord16(SET_FREEZE_REG,FRZ_WDOG);            // stop watch dog
    SetBits16(SYS_CTRL_REG,PAD_LATCH_EN,1);        // open pads
    SetBits16(SYS_CTRL_REG,DEBUGGER_ENABLE,1);     // open debugger
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP,0);       // exit peripheral power down

    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));

    //Init pads
    GPIO_ConfigurePin(UART2_TX_GPIO_PORT, UART2_TX_GPIO_PIN, OUTPUT, PID_UART2_TX, false);
    GPIO_ConfigurePin(UART2_RX_GPIO_PORT, UART2_RX_GPIO_PIN, INPUT, PID_UART2_RX, false);
	
		//Init SW3
		GPIO_ConfigurePin(GPIO_SW3_PORT, GPIO_SW3_PIN, INPUT_PULLUP, PID_GPIO, false);
		
    // Init LED
    GPIO_ConfigurePin(LED_PORT, LED_PIN, OUTPUT, PID_GPIO, false);
    SetBits16(CLK_PER_REG, UART2_ENABLE, 1);                      // enable  clock for UART2
    // Initialize UART component
    uart2_init(UART2_BAUDRATE, UART2_FRAC_BAUDRATE, UART2_DATALENGTH);
}
