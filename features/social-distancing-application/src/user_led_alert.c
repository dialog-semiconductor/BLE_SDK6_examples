/**
 ****************************************************************************************
 *
 * @file user_led_alert.c
 *
 * @brief Social distancing software example source code for LED blink.
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
    led_alert.alert_timer_interval_hnd = EASY_TIMER_INVALID_TIMER;
    
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
    
    led_alert.alert_timer_interval_hnd = app_easy_timer(led_alert.alert_type, timer_alert_cb);
    
}

void alert_user_init(void)
{
    led_alert.alert_active = false;
    led_alert.alert_timer_interval_hnd = EASY_TIMER_INVALID_TIMER;
    led_alert.alert_timer_end_hnd = EASY_TIMER_INVALID_TIMER;
    led_alert.cb = timer_alert_cb;
    led_alert.state = false;
}

void alert_user_stop(void)
{
    led_alert.alert_active = false;
    if (led_alert.alert_timer_interval_hnd != EASY_TIMER_INVALID_TIMER)
        app_easy_timer_cancel(led_alert.alert_timer_interval_hnd);
    
    led_alert.alert_timer_interval_hnd = EASY_TIMER_INVALID_TIMER;
    led_alert.alert_timer_end_hnd = EASY_TIMER_INVALID_TIMER;
    GPIO_SetInactive(ALERT_PORT, ALERT_PIN);
    led_alert.state = false;
    led_alert.alert_type = INVALID_ZONE;
}

static void timer_end_alert_cb(void)
{
    alert_user_stop();
    if(led_alert.cmp_cb!=NULL)
        led_alert.cmp_cb(led_alert.con_idx);
}

void alert_user_start(uint16_t danger_zone, alert_cmp_t cmp_cb, uint8_t con_idx)
{
    led_alert.alert_active = true;
    led_alert.alert_type = danger_zone;
    led_alert.con_idx = con_idx;
    led_alert.cmp_cb = cmp_cb;
    led_alert.cb();
    led_alert.alert_timer_end_hnd = app_easy_timer(ALERT_TIME, timer_end_alert_cb);
}
/// @} APP