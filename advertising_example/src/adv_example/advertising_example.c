/**
 ****************************************************************************************
 *
 * @file ADV_TIMER_EXAMPLE_timer.c
 *
 * @brief Source file of advertising example
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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
#include "rwip_config.h"      
#include "gap.h"
#include "app_easy_timer.h"
#include "user_barebone.h"
#include "co_bt.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "wkupct_quadec.h"
#include "advertising_example.h"
#include "arch_console.h"


#ifdef ADV_EXAMPLE

bool long_press = 0;
advertising_state adv_state = UNDIRECT_ADVERTISING;
advertising_state adv_state_before_sleep;

// Button example function declarations
#ifdef ADV_BUTTON_EXAMPLE
static void user_button_nEdge_interrupt_cb(void);
static void user_wakeup_press_cb(void);
static void user_easy_timer_cb(void);
#endif //ADV_BUTTON_EXAMPLE

// Timer example function declarations
#ifdef ADV_TIMER_EXAMPLE
static void user_resume_from_sleep(void);
#endif //ADV_TIMER_EXAMPLE

/**
 ****************************************************************************************
 * @brief Send the device to sleep.
 * @return void
 ****************************************************************************************
*/                   
void user_activate_sleep(void)
{
		arch_set_sleep_mode(ARCH_EXT_SLEEP_ON);
    arch_set_extended_sleep(false);
		#ifdef ADV_TIMER_EXAMPLE
				app_easy_timer(SLEEP_DURATION, user_resume_from_sleep);
		#endif
}

/**
 ****************************************************************************************
 * @brief Function to set the advertising state
					[Optional] If CFG_PRINTF is defined the advertising state will be send to
					a UART serial terminal (i.e. Tera Term).
 * @return void
 ****************************************************************************************
*/
void user_change_adv_state(advertising_state state)
{
	switch (state)
	{
			case UNDIRECT_ADVERTISING:
					adv_state = UNDIRECT_ADVERTISING;
					#ifdef CFG_PRINTF
							arch_printf("\n\n\rAdvertising state changed to undirected advertising...");
					#endif
					break;
			case NONCON_ADVERTISING:
					adv_state = NONCON_ADVERTISING;
					#ifdef CFG_PRINTF
							arch_printf("\n\n\rAdvertising state changed to nonconnectable advertising...");
					#endif
					break;
			case SLEEP:
					adv_state = SLEEP;
					#ifdef CFG_PRINTF
							arch_printf("\n\n\rAdvertising turned off, system going to sleep...");
					#endif
					break;
			default :
					adv_state = NONCON_ADVERTISING;
					#ifdef CFG_PRINTF
							arch_printf("\n\n\rAdvertising state changed to undirected advertising...");
					#endif
	}	
} 

#ifdef ADV_BUTTON_EXAMPLE
/**
 ****************************************************************************************
 * @brief Setting the interrupt (wakeup) button to change advertising state aswell as 
					the negative edge interrupt for the same button (P1_1). 
 * @return void
 ****************************************************************************************
*/
void user_init_button_interrupt(void)
{
		// Enable wakeup interrupt for SW3 (P1_1)
		wkupct_enable_irq(WKUPCT_PIN_SELECT(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN), 						
									WKUPCT_PIN_POLARITY(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, 1),	
									1,																																																						
									30);																																																								

		
		// Register the proper wakeup callback function
		wkupct_register_callback(user_wakeup_press_cb);
	
		// Enable GPIO interrupt for SW3 (P1_1)
		GPIO_EnableIRQ(	GPIO_BUTTON_PORT, 
										GPIO_BUTTON_PIN, 
										GPIO0_IRQn, 
										true, 
										true, 
										30);
										
		// Register the proper GPIO callback function					
		GPIO_RegisterCallback(GPIO0_IRQn, user_button_nEdge_interrupt_cb);
}

/**
 ****************************************************************************************
 * @brief Interrupt callback to change advertising state. Triggered on negative egde of
					the button press.  
 * @return void
 ****************************************************************************************
*/
void user_button_nEdge_interrupt_cb(void)
{
		// Set interupt to be triggered on negative instead of positive edge
		if(GPIO_GetIRQInputLevel(GPIO0_IRQn) == GPIO_IRQ_INPUT_LEVEL_LOW)									
		{
				GPIO_SetIRQInputLevel(GPIO0_IRQn, GPIO_IRQ_INPUT_LEVEL_HIGH);
				return;
		}
		
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
void user_wakeup_press_cb(void)
{
		SetBits16(WKUP_CTRL_REG, WKUP_ENABLE_IRQ, 1); 							// Resets interupt
		arch_disable_sleep();																				// Wake up the system
	
		// If peripheral domain is down, reinitialize
    if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))										
    {
         periph_init();
    }
		
		//Re-enable gpio interrupt 
		GPIO_EnableIRQ(	GPIO_BUTTON_PORT, 
										GPIO_BUTTON_PIN, 
										GPIO0_IRQn, 
										true, 
										true, 
										30);
								
		long_press = 0;																							// Reset flag
		app_easy_timer(300, user_easy_timer_cb);										// Start 3 second timer
		wdg_freeze(); 																							// prevent watchdog to interfere with main loop as described in user_callback_config.h 
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
		long_press = 1;			// set flag to confirm long button press (toggle sleepstate)
}

#endif //ADV_BUTTON_EXAMPLE

#ifdef ADV_TIMER_EXAMPLE
/**
 ****************************************************************************************
 * @brief Callback when waking op the system to start advertising
 * @return void
 ****************************************************************************************
*/
void user_resume_from_sleep(void)
{
    arch_disable_sleep();
    if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))
    {
         periph_init();
    }
		user_change_adv_state(UNDIRECT_ADVERTISING);
		user_app_adv_start();
}
#endif //ADV_TIMER_EXAMPLE
#endif //ADV_EXAMPLE

