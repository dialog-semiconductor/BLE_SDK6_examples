/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Simple_Button example
 *
 * Copyright (c) 2012-2019 Dialog Semiconductor. All rights reserved.
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
#include "systick.h" 
#include "uart_utils.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "arch_system.h"
 
/****************************************************************************************/
/* User constants                                                                   		  	  				*/
/****************************************************************************************/
 const uint32_t 	BLINK_INTERVAL 				= 500000;			// In usec. 
 const uint32_t	LONG_PRESS_TIME 		= 3000000;  	// In usec	
 const uint8_t 		DEBOUNCE_MS 					= 30;			  		// In ms
 const bool 				SYSTICK_ENABLE_T   	= 1; 
 
// Interrupt initialization function declarations
void systick_ISR(void);
void negative_GPIO_ISR(void);
void GPIO_ISR(void);
void wakeup_ISR(void);
	
void interrupts_init(void);
void set_timer_interrupt(void) ;
void set_positive_GPIO_interrupt(void);
void set_negative_GPIO_interrupt(void);
	
// Flag for detecting long (3s=) or short press
bool three_second_push = 0;


void Led_blink(void);
 
/**
 ****************************************************************************************
 * @brief Main routine of the Simple_Button example
 * @return void
 ****************************************************************************************
 */
int main (void)
{
    system_init();
    periph_init();
		interrupts_init();


    for(;;);
}

/**
 ****************************************************************************************
 * @brief Led_blink function
 * @return void
 ****************************************************************************************
 */
void Led_blink(void)
{
		systick_stop();
		systick_register_callback(Led_blink);
		if (GPIO_GetPinStatus(LED_PORT, LED_PIN))
		{
				GPIO_SetInactive(LED_PORT, LED_PIN);
				printf_string(UART,"\n\r=> LED OFF <=");
		}
		else
		{
				GPIO_SetActive(LED_PORT, LED_PIN);
				printf_string(UART,"\n\r=> LED ON  <=");
		}
		systick_start(BLINK_INTERVAL, SYSTICK_ENABLE_T);
}

/**
 ****************************************************************************************
 * @brief systick ISR handler
 * @return void
 ****************************************************************************************
 */
void systick_ISR(void) 
{
		systick_stop();
		three_second_push = 1;
		printf_string(UART,"\n\n\rLong Press");
		Led_blink();
}

/**
 ****************************************************************************************
 * @brief negative_GPIO_ISR function
 * @return void
 ****************************************************************************************
 */
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
			printf_string(UART,"\n\n\rShort press");
				if (GPIO_GetPinStatus(LED_PORT, LED_PIN))
				{
						GPIO_SetInactive(LED_PORT, LED_PIN);
						printf_string(UART,"\n\r=> LED OFF <=");
				}
				else
				{
						GPIO_SetActive(LED_PORT, LED_PIN);
						printf_string(UART,"\n\r=> LED ON  <=");
				}
		}
}

/**
 ****************************************************************************************
 * @brief GPIO_ISR function generate timer, if LONG_PRESS_TIME_T time is passed an exception is generated
 * @return void
 ****************************************************************************************
 */
void GPIO_ISR(void)
{
		three_second_push = 0; 																													
		systick_stop(); 																																				
		systick_register_callback(systick_ISR);																			
		systick_start(LONG_PRESS_TIME, SYSTICK_ENABLE_T);		
}
	
/**
 ****************************************************************************************
 * @brief interrupts_init function
 * @return void
 ****************************************************************************************
 */
void interrupts_init(void)
{
		set_timer_interrupt();
		set_positive_GPIO_interrupt();
		set_negative_GPIO_interrupt();
}

/**
 ****************************************************************************************
 * @brief set_timer_interrupt function, configures interupt handler function
 * @return void
 ****************************************************************************************
 */
void set_timer_interrupt(void) 
{
		systick_register_callback(systick_ISR); 
}

/**
 ****************************************************************************************
 * @brief set_positive_GPIO_interrupt function
 * @return void
 ****************************************************************************************
 */
void set_positive_GPIO_interrupt(void)
{
		GPIO_EnableIRQ(GPIO_SW3_PORT, GPIO_SW3_PIN, GPIO0_IRQn, true, true, DEBOUNCE_MS);
		GPIO_SetIRQInputLevel(GPIO0_IRQn, GPIO_IRQ_INPUT_LEVEL_LOW);
		GPIO_RegisterCallback(GPIO0_IRQn, GPIO_ISR);
}

/**
 ****************************************************************************************
 * @brief set_negative_GPIO_interrupt function
 * @return void
 ****************************************************************************************
 */
void set_negative_GPIO_interrupt(void)
{
		GPIO_EnableIRQ(GPIO_SW3_PORT, GPIO_SW3_PIN, GPIO1_IRQn, true, true, DEBOUNCE_MS);
		GPIO_SetIRQInputLevel(GPIO1_IRQn, GPIO_IRQ_INPUT_LEVEL_LOW);
		GPIO_RegisterCallback(GPIO1_IRQn, negative_GPIO_ISR);
}

