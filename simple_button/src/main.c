/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Blinky example
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
#include <stdio.h>
#include "common_uart.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "systick.h"

#define LED_OFF_THRESHOLD 10000
#define LED_ON_THRESHOLD  400000

// Interrupt initialization function declarations
void systick_ISR(void);
void negative_GPIO_ISR(void);
void GPIO_ISR(void);
void wakeup_ISR(void);

// Interrupt initialization function declarations
void interrupts_init(void);
void set_timer_interrupt(void) ;
void set_positive_GPIO_interrupt(void);
void set_negative_GPIO_interrupt(void);

// Flag for detecting long (3s) or short press
bool three_second_push = 0;

// System initialization function declaration
void system_init(void);

void Led_blink(void);
/**
 ****************************************************************************************
 * @brief Main routine of the Blinky example
 * 
 ****************************************************************************************
 */
int main (void)
{
    system_init();
    periph_init();
		interrupts_init();
    while(1);
}

/**
 ****************************************************************************************
 * @brief System Initialization
 *
 ****************************************************************************************
 */
void system_init(void)
{
    SetWord16(CLK_AMBA_REG, 0x00);                 // set clocks (hclk and pclk ) 16MHz
    SetWord16(SET_FREEZE_REG,FRZ_WDOG);            // stop watch dog
    SetBits16(SYS_CTRL_REG,PAD_LATCH_EN,1);        // open pads
    SetBits16(SYS_CTRL_REG,DEBUGGER_ENABLE,1);     // open debugger
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP,0);       // exit peripheral power down
}

/**
 ****************************************************************************************
 * @brief Blinky test function
 *
 ****************************************************************************************
 */
void Led_blink(void)
{
		systick_stop();
		systick_register_callback(Led_blink);
		if (GPIO_GetPinStatus(LED_PORT, LED_PIN))
		{
				GPIO_SetInactive(LED_PORT, LED_PIN);
				printf_string("\n\r=> LED OFF <=");
		}
		else
		{
				GPIO_SetActive(LED_PORT, LED_PIN);
				printf_string("\n\r=> LED ON  <=");
		}
		systick_start(500000, 1);
}

void systick_ISR(void) // systick ISR handler
{
		systick_stop();
		three_second_push = 1;
		printf_string("\n\n\rLong Press");
		//GPIO_SetActive(LED_PORT, LED_PIN); //test
		Led_blink();
}

void negative_GPIO_ISR(void)
{
		// Prevents interrupt from triggering at startup
		if (GPIO_GetIRQInputLevel(GPIO1_IRQn) == GPIO_IRQ_INPUT_LEVEL_LOW)
		{
				GPIO_SetIRQInputLevel(GPIO1_IRQn, GPIO_IRQ_INPUT_LEVEL_HIGH);
				return;
		}

		if (!three_second_push)
		{
				systick_stop();
			printf_string("\n\n\rShort press");
				if (GPIO_GetPinStatus(LED_PORT, LED_PIN))
				{
						GPIO_SetInactive(LED_PORT, LED_PIN);
						printf_string("\n\r=> LED OFF <=");
				}
				else
				{
						GPIO_SetActive(LED_PORT, LED_PIN);
						printf_string("\n\r=> LED ON  <=");
				}
		}
}

void GPIO_ISR(void)
{
		three_second_push = 0; 										// reset flag
		systick_stop(); 													// stop and reset systick if it's currently running
		systick_register_callback(systick_ISR);		// 
		systick_start(3000000, 1);  							// start timer generating exeption at 3 seconds
}

void interrupts_init(void)
{
		set_timer_interrupt();
		set_positive_GPIO_interrupt();
		set_negative_GPIO_interrupt();
}

void set_timer_interrupt(void) 
{
		systick_register_callback(systick_ISR); // configure interupt handler function
}

void set_positive_GPIO_interrupt(void)
{
		GPIO_EnableIRQ(GPIO_SW3_PORT, GPIO_SW3_PIN, GPIO0_IRQn, true, true, 30);
		GPIO_SetIRQInputLevel(GPIO0_IRQn, GPIO_IRQ_INPUT_LEVEL_LOW);
		GPIO_RegisterCallback(GPIO0_IRQn, GPIO_ISR);
}

void set_negative_GPIO_interrupt(void)
{
		GPIO_EnableIRQ(GPIO_SW3_PORT, GPIO_SW3_PIN, GPIO1_IRQn, true, true, 30);
		GPIO_SetIRQInputLevel(GPIO1_IRQn, GPIO_IRQ_INPUT_LEVEL_LOW);
		GPIO_RegisterCallback(GPIO1_IRQn, negative_GPIO_ISR);
}
