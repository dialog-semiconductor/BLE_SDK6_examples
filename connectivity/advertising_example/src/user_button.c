/**
 ****************************************************************************************
 *
 * @file user_button.c
 *
 * @brief uder wakeup source code.
 *
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
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
advertising_state adv_state             __SECTION_ZERO("retention_mem_area0");
timer_hnd button_press_timer_handler    __SECTION_ZERO("retention_mem_area0");

#ifdef ADV_BUTTON_EXAMPLE
/**
 ****************************************************************************************
 * @brief Debounce timer handler callback.
 ****************************************************************************************
*/
static void button_press_timer_handler_cb(void)
{
    button_press_timer_handler = EASY_TIMER_INVALID_TIMER;
    // If the status of the pin is still set to low the button is still pressed start advertising
    if(!GPIO_GetPinStatus(GPIO_SW_PORT, GPIO_SW_PIN))
    {
        if(adv_state == PERMANENT_SLEEP)
            user_change_adv_state(UNDIRECT_ADVERTISING);
        else
            user_change_adv_state(PERMANENT_SLEEP);
    }
}

/**
 ****************************************************************************************
 * @brief Application wakeup callback function. Registered in API message utility.
 ****************************************************************************************
*/
static void app_wakeup_cb(void)
{
    // Start the timer to perform the 3 sec button check
    // Check if there is a timer already running in that case cancel the running timer and set up a new one
    if(button_press_timer_handler != EASY_TIMER_INVALID_TIMER)     
        app_easy_timer_cancel(button_press_timer_handler);
    button_press_timer_handler = app_easy_timer(GOTO_SLEEP_TIME, button_press_timer_handler_cb);
    
    if (adv_state == PERMANENT_SLEEP)
    {
        user_change_adv_state(UNDIRECT_ADVERTISING);
        user_app_adv_start();
    }
    else if (adv_state == UNDIRECT_ADVERTISING)
        user_change_adv_state(NONCON_ADVERTISING);
    else if (adv_state == NONCON_ADVERTISING)
        user_change_adv_state(UNDIRECT_ADVERTISING);
}

static void user_button_press_cb(void)
{
#if !defined (__DA14531__)
    if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))
    {
        periph_init();
    }
#endif
    
    if (arch_ble_ext_wakeup_get())
    {
        arch_set_sleep_mode(app_default_sleep_mode);
        arch_ble_force_wakeup();
        arch_ble_ext_wakeup_off();
    }

    app_easy_wakeup();
    
    // Enable the wakeup interrupt for the next button press
    user_button_enable(GPIO_SW_PORT, GPIO_SW_PIN);
}

/**
 ****************************************************************************************
 * @brief Setting the interrupt (wakeup) button to change advertising state as well as 
					the negative edge interrupt for the same button (P1_1). 
 * @return void
 ****************************************************************************************
*/
void user_button_enable(GPIO_PORT gpio_port, GPIO_PIN gpio_pin)
{
    app_easy_wakeup_set(app_wakeup_cb);
    wkupct_enable_irq(WKUPCT_PIN_SELECT(gpio_port, gpio_pin), 						
                      WKUPCT_PIN_POLARITY(gpio_port, gpio_pin, 1),	
                      1,																																																						
                      30);																																																								

    wkupct_register_callback(user_button_press_cb);
}

#endif //ADV_BUTTON_EXAMPLE
 
/// @} APP