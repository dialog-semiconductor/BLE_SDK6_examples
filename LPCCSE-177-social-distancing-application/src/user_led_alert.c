/**
 ****************************************************************************************
 *
 * @file user_led_alert.c
 *
 * @brief Social distancing software example source code for LED blink.
 *
 * Copyright (c) 2015-2020 Dialog Semiconductor. All rights reserved.
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
 #include "user_led_alert.h"
 #include "user_periph_setup.h"
 
 /*
 * VARIABLES
 ****************************************************************************************
 */
 alert_led_t led_alert                           __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 
 
 /*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

void timer_alert_cb(void)
{
    
    if(led_alert.state)
    {
        GPIO_SetInactive(ALERT_PORT, ALERT_PIN);
        led_alert.state = false;
    }
    else
    {
        GPIO_SetActive(ALERT_PORT, ALERT_PIN);
        led_alert.state = true;        
    }
    
    led_alert.alert_type--;
    
    if (led_alert.alert_type == 0)
    {
        alert_user_stop();
        if(led_alert.cmp_cb!=NULL)
            led_alert.cmp_cb(led_alert.con_idx);
    }
    else
        led_alert.alert_timer_hnd = app_easy_timer(ALERT_LED_ON, timer_alert_cb);
    
}

void alert_user_init(void)
{
    led_alert.alert_active = false;
    led_alert.alert_timer_hnd = EASY_TIMER_INVALID_TIMER;
    led_alert.cb = timer_alert_cb;
    led_alert.state = false;
}

void alert_user_stop(void)
{
    led_alert.alert_active = false;
    led_alert.alert_timer_hnd = EASY_TIMER_INVALID_TIMER;
    GPIO_SetInactive(ALERT_PORT, ALERT_PIN);
    led_alert.state = false;
    led_alert.alert_type = INVALID_ZONE;
}

void alert_user_start(uint16_t danger_zone, alert_cmp_t cmp_cb, uint8_t con_idx)
{
    led_alert.alert_active = true;
    led_alert.alert_type = danger_zone;
    led_alert.con_idx = con_idx;
    led_alert.cmp_cb = cmp_cb;
    led_alert.cb();
}
/// @} APP
