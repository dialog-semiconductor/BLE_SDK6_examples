/**
 ****************************************************************************************
 *
 * @file app_bass.c
 *
 * @brief Battery server application.
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

#include "rwip_config.h"

#if (BLE_APP_PRESENT)

#if (BLE_BATT_SERVER)

#include "bass_task.h"
#include "app_bass.h"
#include "gpio.h"
#include "battery.h"
#include "user_periph_setup.h"
#include "app_prf_perm_types.h"
#include "app.h"
#include "prf.h"
#include "user_profiles_config.h"

/*
 * VARIABLES DEFINITION
 ****************************************************************************************
 */

uint16_t bat_poll_timeout     __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t cur_batt_level        __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t batt_alert_en         __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t bat_led_state         __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t bat_lvl_alert_used    __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
GPIO_PORT bat_led_port        __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
GPIO_PIN bat_led_pin          __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_bass_create_db(void)
{
    struct bass_db_cfg* db_cfg;

    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd,
                                                             sizeof(struct bass_db_cfg));

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_BASS);
    req->prf_task_id = TASK_ID_BASS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct bass_db_cfg* ) req->param;
    db_cfg->bas_nb = 1;
    db_cfg->features[0] = BAS_BATT_LVL_NTF_SUP;
    db_cfg->batt_level_pres_format[0].unit = 0x27AD;
    db_cfg->batt_level_pres_format[0].description = 0;
    db_cfg->batt_level_pres_format[0].format = 4;
    db_cfg->batt_level_pres_format[0].exponent = 0;
    db_cfg->batt_level_pres_format[0].name_space = 1;
#ifndef USE_ONE_BAS_INSTANCE
    db_cfg->features[1] = BAS_BATT_LVL_NTF_NOT_SUP;
#endif //USE_ONE_BAS_INSTANCE

    // Send the message
    ke_msg_send(req);
}

void app_batt_init(void)
{
    cur_batt_level = battery_get_lvl(BATT_CR2032);
}

void app_batt_config(uint8_t batt_lvl, uint8_t lvl_alert_used, GPIO_PORT led_port, GPIO_PIN led_pin)
{
    bat_lvl_alert_used  = lvl_alert_used;
    bat_led_port        = led_port;
    bat_led_pin         = led_pin;
    cur_batt_level      = batt_lvl;
}

void app_bass_enable(uint8_t conidx)
{
    // Allocate the message
    struct bass_enable_req * req = KE_MSG_ALLOC(BASS_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_BASS),
                                                TASK_APP,
                                                bass_enable_req);

    // Fill in the parameter structure
    req->conidx = conidx;
    req->ntf_cfg = 0;        // Notification OFF by default.
    req->old_batt_lvl[0] = cur_batt_level;
#ifndef USE_ONE_BAS_INSTANCE
    req->old_batt_lvl[1] = 0;
#endif //USE_ONE_BAS_INSTANCE

    // Send the message
    ke_msg_send(req);

    app_batt_config(0, USE_BAT_LEVEL_ALERT, GPIO_BAT_LED_PORT, GPIO_BAT_LED_PIN);
    app_batt_poll_start(APP_BASS_POLL_INTERVAL);
}

void app_batt_lvl(void)
{
    uint8_t batt_lvl;

    batt_lvl = battery_get_lvl(BATT_CR2032);

    if (batt_lvl != cur_batt_level)
    {
        app_batt_set_level(batt_lvl);
    }

    //update old_batt_lvl for the next use
    cur_batt_level = batt_lvl;

#if defined(BATTERY_ALERT_AT_PERCENTAGE_LEFT)
    if(bat_lvl_alert_used)
    {
      if (batt_lvl < BATTERY_ALERT_AT_PERCENTAGE_LEFT)
      {
          app_batt_alert_start(); // Start battery level alert
      }

      if (batt_lvl > BATTERY_ALERT_AT_PERCENTAGE_LEFT + 5)
      {
          app_batt_alert_stop();  // Stop battery level alert
      }
    }
#else
    //if battery level is below 5% then start a battery alert to notify user
    if(bat_lvl_alert_used && batt_lvl < 5)
    {
        app_batt_alert_start();
    }
#endif //defined(BATTERY_ALERT_AT_PERCENTAGE_LEFT)
}

void app_batt_set_level(uint8_t batt_lvl)
{
    // Allocate the message
    struct bass_batt_level_upd_req * req = KE_MSG_ALLOC(BASS_BATT_LEVEL_UPD_REQ,
                                                        prf_get_task_from_id(TASK_ID_BASS),
                                                        TASK_APP,
                                                        bass_batt_level_upd_req);

    // Fill in the parameter structure
    req->bas_instance = 0;
    req->batt_level = batt_lvl;

    // Send the message
    ke_msg_send(req);
}

void app_batt_poll_start(uint16_t poll_timeout)
{
    bat_poll_timeout = poll_timeout;

    app_timer_set(APP_BASS_TIMER, TASK_APP, 10);    //first poll in 100 ms
}

void app_batt_poll_stop(void)
{
    ke_timer_clear(APP_BASS_TIMER, TASK_APP);
}

void app_batt_alert_start(void)
{
    batt_alert_en = 1;

#ifndef CUSTOM_BATTERY_LEVEL_ALERT_LED_HANDLING
    if (bat_lvl_alert_used)
    {
        GPIO_SetActive( bat_led_port, bat_led_pin);
    }
    bat_led_state = 1;
    app_timer_set(APP_BASS_ALERT_TIMER, TASK_APP, 200);
#else
    if (bat_lvl_alert_used)
    {
        leds_set_battery_low_warning();
    }
#endif //CUSTOM_BATTERY_LEVEL_ALERT_LED_HANDLING
}

void app_batt_alert_stop(void)
{
    batt_alert_en = 0;

#ifndef CUSTOM_BATTERY_LEVEL_ALERT_LED_HANDLING
    if (bat_lvl_alert_used)
    {
        GPIO_SetInactive(bat_led_port, bat_led_pin);
    }
    ke_timer_clear(APP_BASS_ALERT_TIMER, TASK_APP);
    bat_led_state = 0;
#else
    if (bat_lvl_alert_used)
    {
        leds_clear_battery_low_warning();
    }
#endif //CUSTOM_BATTERY_LEVEL_ALERT_LED_HANDLING
}

void app_batt_port_reinit(void)
{
#ifndef CUSTOM_BATTERY_LEVEL_ALERT_LED_HANDLING
    if (bat_lvl_alert_used)
    {
        if(bat_led_state == 1)
        {
            GPIO_SetActive(bat_led_port, bat_led_pin);
        }
        else
        {
            GPIO_SetInactive(bat_led_port, bat_led_pin);
        }
    }
#endif //CUSTOM_BATTERY_LEVEL_ALERT_LED_HANDLING
}

#endif //BLE_BATT_SERVER

#endif // BLE_APP_PRESENT

/// @} APP
