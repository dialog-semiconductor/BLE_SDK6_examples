/**
 ****************************************************************************************
 *
 * @file user_button.c
 *
 * @brief uder wakeup source code.
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

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#if defined(ADV_BUTTON_EXAMPLE)

#include "wkupct_quadec.h" 
#include "user_button.h"
#include "arch.h"
#include "user_periph_setup.h"
#include "uart.h"
#include "uart_utils.h"
#include "user_barebone.h"
#include "systick.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
extern advertising_state adv_state;

static bool long_press __SECTION_ZERO("retention_mem_area0");
static advertising_state adv_state_before_sleep __SECTION_ZERO("retention_mem_area0");

static void user_button_release_cb(void);
static void user_button_press_cb(void);
static void user_easy_timer_cb(void);

#if defined(__DA14531__)
/**
 ****************************************************************************************
 * @brief reset function for the event counter of the DA14531
 * @return void
 ****************************************************************************************
 */
static void reset_event_counter(void){

   SetWord16(WKUP_IRQ_STATUS_REG, WKUP_CNTR_RST);

}
#endif


/**
 ****************************************************************************************
 * @brief Setting the interrupt (wakeup) button to change advertising state aswell as 
					the negative edge interrupt for the same button (P1_1). 
 * @return void
 ****************************************************************************************
*/
void user_init_button_interrupt(GPIO_PORT gpio_port, GPIO_PIN gpio_pin)
{
		adv_state_before_sleep = UNDIRECT_ADVERTISING;
		long_press = 0;

		wkupct_enable_irq(WKUPCT_PIN_SELECT(gpio_port, gpio_pin), 						
									WKUPCT_PIN_POLARITY(gpio_port, gpio_pin, 1),	
									1,																																																						
									30);																																																								

		
		wkupct_register_callback(user_button_press_cb);
	
		GPIO_EnableIRQ(	gpio_port, 
										gpio_pin, 
										GPIO0_IRQn, 
										true, 
										true, 
										30);
										
		// Register the proper GPIO callback function					
		GPIO_RegisterCallback(GPIO0_IRQn, user_button_release_cb);
//		GPIO_RegisterCallback(GPIO1_IRQn, user_button_press_cb);
}

/**
 ****************************************************************************************
 * @brief Interrupt callback to change advertising state. Triggered on negative egde of
					the button press.  
 * @return void
 ****************************************************************************************
*/
void user_button_release_cb(void)
{
		// Set interupt to be triggered on negative instead of positive edge
		if(GPIO_GetIRQInputLevel(GPIO0_IRQn) == GPIO_IRQ_INPUT_LEVEL_LOW)									
		{
				GPIO_SetIRQInputLevel(GPIO0_IRQn, GPIO_IRQ_INPUT_LEVEL_HIGH);
				return;
		}
		
//		printf_string(UART2,"\n\n\ruser_button_nEdge_interrupt_cb");
		// terminate the three second timer (to detect long  button press) if its still running. 
		app_easy_timer_cancel_all();																
		wdg_resume();
		
		// Change to non-connectible advertising
		if (adv_state == UNDIRECT_ADVERTISING && !long_press)				
		{
				user_change_adv_state(NONCON_ADVERTISING);
				app_easy_gap_advertise_stop();
		}
		
		// Change to undirected advertising
		else if (adv_state == NONCON_ADVERTISING && !long_press)
		{
				user_change_adv_state(UNDIRECT_ADVERTISING);
				app_easy_gap_advertise_stop();
		}
		
		// Go to sleep
		else if (adv_state != SLEEP && long_press)
		{
				// save current adv_state to return to after sleep
				adv_state_before_sleep = adv_state;											
				user_change_adv_state(SLEEP);
				app_easy_gap_advertise_stop();
		}
		
		// Wake up from sleep
		else if (adv_state == SLEEP && long_press)
		{
				user_change_adv_state(adv_state_before_sleep);
//				arch_ble_ext_wakeup_off();
				// wakeup and start advertising	
				user_app_adv_start();																		
		}
		
		// Remain in sleep 
		// Short button press won't alter state while sleeping
		else if (adv_state == SLEEP && !long_press)
		{
				user_change_adv_state(SLEEP);
				user_app_adv_start();
		}
}

/**
 ****************************************************************************************
 * @brief Wakeup interrupt callback triggered on positive edge of button press. 
					Here, a timer is started to detect a long or short button press. If the system 
					is in sleep state it will (temporarily) wake up the system to detect a long or short 
					button press. 
 * @return void
 ****************************************************************************************
*/
void user_button_press_cb(void)
{

			SetBits16(WKUP_CTRL_REG, WKUP_ENABLE_IRQ, 1); 							// Resets interupt
			if (adv_state == SLEEP){
				arch_disable_sleep();																				// Wake up the system
			
				// If peripheral domain is down, reinitialize
				#if !defined(__DA14531__)
				if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))										
				{
						 periph_init();
				}
				#endif
				#if defined(__DA14531__)
					reset_event_counter(); // When callback is triggerd the event counter is not set 0 for the 531, that is why this function is called. 
				#endif								 // For the 585 the event counter is set to 0 even before reaching this (app_wakeup_press_cb) callback funtion.

				//Re-enable gpio interrupt 
				GPIO_EnableIRQ(	GPIO_SW_PORT, 
												GPIO_SW_PIN, 
												GPIO0_IRQn, 
												true, 
												true, 
												30);
			}								
				
		long_press = 0;																							// Reset flag
		app_easy_timer(300, user_easy_timer_cb);										// Start 3 second timer
		wdg_freeze(); 																							// prevent watchdog to interfere with main loop as described in user_callback_config.h 
		
		#if defined(__DA14531__)
			reset_event_counter();
		#endif
}

/**
 ****************************************************************************************
 * @brief 	Easy timer interrupt callback. If the timer reaches zero a long press will be
						deteced by setting the long_press flag.
 * @return 	void
 ****************************************************************************************
*/
void user_easy_timer_cb(void)
{
//		printf_string(UART2,"\n\n\ruser_easy_timer_cb");
		long_press = 1;			// set flag to confirm long button press (toggle sleepstate)
}

#endif //ADV_BUTTON_EXAMPLE
 
/// @} APP
