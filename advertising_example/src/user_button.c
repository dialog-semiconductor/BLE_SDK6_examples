/**
 ****************************************************************************************
 *
 * @file user_button.c
 *
 * @brief Source file of advertising example
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs 
 * included in this package ("Software") include confidential, proprietary information 
 * of Dialog Semiconductor. All Rights Reserved.
 * 
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’) AND MAY
 * ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND. THIS 
 * SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS. RECIPIENT 
 * SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG’S PRIOR 
 * WRITTEN PERMISSION.
 * 
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT 
 * TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT 
 * WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT 
 * LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, 
 * MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM 
 * COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS
 * AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY 
 * REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED 
 * SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, 
 * AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY 
 * OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED 
 * BY THE FCC OR OTHER LIKE AGENCIES.
 * 
 * IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 ****************************************************************************************
 */
#include "rwip_config.h"      
#include "gap.h"
#include "app_easy_timer.h"
#include "user_adv_example.h"
#include "co_bt.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "wkupct_quadec.h"
#include "user_button.h"
#include "arch_console.h"


#ifdef ADV_BUTTON_EXAMPLE

extern advertising_state adv_state;

static bool long_press = 0;
static advertising_state adv_state_before_sleep;

static void user_button_nEdge_interrupt_cb(void);
static void user_wakeup_press_cb(void);
static void user_easy_timer_cb(void);

/**
 ****************************************************************************************
 * @brief Setting the interrupt (wakeup) button to change advertising state aswell as 
					the negative edge interrupt for the same button (P1_1). 
 * @return void
 ****************************************************************************************
*/
void user_init_button_interrupt(GPIO_PORT gpio_port, GPIO_PIN gpio_pin)
{
		// Enable wakeup interrupt for SW3 (P1_1)
		wkupct_enable_irq(WKUPCT_PIN_SELECT(gpio_port, gpio_pin), 						
									WKUPCT_PIN_POLARITY(gpio_port, gpio_pin, 1),	
									1,																																																						
									30);																																																								

		
		// Register the proper wakeup callback function
		wkupct_register_callback(user_wakeup_press_cb);
	
		// Enable GPIO interrupt for SW3 (P1_1)
		GPIO_EnableIRQ(	gpio_port, 
										gpio_pin, 
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

