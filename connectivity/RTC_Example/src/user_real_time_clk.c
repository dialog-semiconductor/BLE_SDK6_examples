/**
 ****************************************************************************************
 *
 * @file user_real_time_clk.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (C) 2012-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "user_real_time_clk.h"

#include "rtc.h"

#include "arch_system.h"
#include "arch_console.h"
#include "user_custs1_def.h"
#include "custs1_task.h"
#include "attm_db.h"
#include "custom_common.h"
#include "user_rtc_util.h"
#include "user_custs1_impl.h"
#include "user_periph_setup.h"

#if BLE_CTS_SERVER
#include "ctss.h"
#include "user_ctss.h"
#include "user_profiles_config.h"
#endif

// Manufacturer Specific Data ADV structure type
struct __PACKED date_specific_data_ad_structure
{
    uint8_t     ad_struct_size;
    uint8_t     ad_struct_type;
    uint16_t    year;
    uint8_t     month;
    uint8_t     mday;
    uint8_t     hour;
    uint8_t     minute;
    uint8_t     second;
};

const char *days_of_week[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

struct date_specific_data_ad_structure adv_date         __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t date_data_index                                 __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_adv_data_len                             __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                        __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]                   __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN]         __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

uint8_t app_connection_idx                              __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY  
timer_hnd app_param_update_request_timer_used           __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/* Alert timer variables */
timer_hnd app_alert_timer_used                          __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint16_t app_alert_timeout                              __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
bool led_state                                          __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
#if (BLE_CUSTOM1_SERVER)
static void alarm_stop(void)
{
    app_alert_timeout = 0;
    app_alert_timer_used = EASY_TIMER_INVALID_TIMER;
    led_state = false;
    GPIO_ConfigurePin(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, OUTPUT, PID_GPIO, led_state);
}

static void toggle_led(void)
{
    led_state = !led_state;
    GPIO_ConfigurePin(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, OUTPUT, PID_GPIO, led_state);
    app_alert_timeout++;
    
    if(app_alert_timeout == ALERT_TIMEOUT/ALERT_INTERVAL)
        alarm_stop();
    else
        app_alert_timer_used = app_easy_timer(ALERT_INTERVAL, toggle_led);
}

static void alarm_start(void)
{
    app_alert_timer_used = app_easy_timer(ALERT_INTERVAL, toggle_led);
    GPIO_SetActive(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN);
    led_state = true;
}
#endif // BLE_CUSTOM1_SERVER

/**
 ****************************************************************************************
 * @brief Parameter update request timer callback function.
 * @return void
 ****************************************************************************************
*/
static void param_update_request_timer_cb()
{
    app_easy_gap_param_update_start(app_connection_idx);
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
}

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;    

        // Check if the parameters of the established connection are the preferred ones.
        // If not then schedule a connection parameter update request.
        if ((param->con_interval < user_connection_param_conf.intv_min) ||
            (param->con_interval > user_connection_param_conf.intv_max) ||
            (param->con_latency != user_connection_param_conf.latency) ||
            (param->sup_to != user_connection_param_conf.time_out))
        {
            // Connection params are not these that we expect
            app_param_update_request_timer_used = app_easy_timer(APP_PARAM_UPDATE_REQUEST_TO, param_update_request_timer_cb);
        }
    }
    else
    {
        // No connection has been established, restart advertising
        user_app_adv_start();
    }

    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}

static void adv_data_date_init(void)
{
    rtc_time_t current_time;
    rtc_calendar_t current_date;
    rtc_get_time_clndr(&current_time, &current_date);
    adv_date.ad_struct_size = sizeof(struct date_specific_data_ad_structure ) - sizeof(uint8_t); // minus the size of the ad_structure_size field
    adv_date.ad_struct_type = GAP_AD_TYPE_MANU_SPECIFIC_DATA;
    adv_date.year = current_date.year;
    adv_date.month = current_date.month;
    adv_date.mday = current_date.mday;
    adv_date.hour = current_time.hour;
    adv_date.minute = current_time.minute;
    adv_date.second = current_time.sec;
    
    // Initialize Advertising and Scan Response Data
    memcpy(stored_adv_data, USER_ADVERTISE_DATA, USER_ADVERTISE_DATA_LEN);
    stored_adv_data_len = USER_ADVERTISE_DATA_LEN;
    memcpy(stored_scan_rsp_data, USER_ADVERTISE_SCAN_RESPONSE_DATA, USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN);
    stored_scan_rsp_data_len = USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN;
}

static void app_add_ad_date(struct gapm_start_advertise_cmd *cmd, void *date_data_ad_structure, uint8_t ad_struct_len)
{   
    if ((ADV_DATA_LEN - cmd->info.host.adv_data_len) >= ad_struct_len)
    {
        // Append manufacturer data to advertising data
        memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len], date_data_ad_structure, ad_struct_len);

        // Update Advertising Data Length
        cmd->info.host.adv_data_len += ad_struct_len;
        
        // Store index of manufacturer data which are included in the advertising data
        date_data_index = cmd->info.host.adv_data_len - sizeof(struct date_specific_data_ad_structure);
    }
    else if ((SCAN_RSP_DATA_LEN - cmd->info.host.scan_rsp_data_len) >= ad_struct_len)
    {
        // Append manufacturer data to scan response data
        memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len], date_data_ad_structure, ad_struct_len);

        // Update Scan Response Data Length
        cmd->info.host.scan_rsp_data_len += ad_struct_len;
        
        // Store index of manufacturer data which are included in the scan response data
        date_data_index = cmd->info.host.scan_rsp_data_len - sizeof(struct date_specific_data_ad_structure);
        // Mark that manufacturer data is in scan response and not advertising data
        date_data_index |= 0x80;
    }
    else
    {
        // Manufacturer Specific Data do not fit in either Advertising Data or Scan Response Data
        ASSERT_WARNING(0);
    }
    // Store advertising data length
    stored_adv_data_len = cmd->info.host.adv_data_len;
    // Store advertising data
    memcpy(stored_adv_data, cmd->info.host.adv_data, stored_adv_data_len);
    // Store scan response data length
    stored_scan_rsp_data_len = cmd->info.host.scan_rsp_data_len;
    // Store scan_response data
    memcpy(stored_scan_rsp_data, cmd->info.host.scan_rsp_data, stored_scan_rsp_data_len);
}

/**
 ****************************************************************************************
 * @brief Update the date data in the advertising string
 * @return void
 ****************************************************************************************
 */
static void date_data_update(rtc_time_t *time, rtc_calendar_t *clndr)
{
    adv_date.year = clndr->year;
    adv_date.month = clndr->month;
    adv_date.mday = clndr->mday;
    adv_date.hour = time->hour;
    adv_date.minute = time->minute;
    adv_date.second = time->sec;
}

/**
 ****************************************************************************************
 * @brief Print the provided date and time
 * @return void
 ****************************************************************************************
 */
static void print_date_time(rtc_time_t *time, rtc_calendar_t *clndr)
{
    uint32_t msec = rtc_convert_time_to_msec(time);
    /* Print the current date */
    arch_printf("%s, %02d/%02d/%04d ", days_of_week[clndr->wday - 1], clndr->mday, clndr->month, clndr->year );
    /* Print the current time */
    arch_printf("%02d:%02d:%02d:%02d %s ", time->hour, time->minute, time->sec, time->hsec,
                                           (time->hour_mode) ? ((time->pm_flag) ? "PM" : "AM") : "24H");
    /* Print the current time in ms */
    arch_printf("%04d.%03d ms \n\r", (uint32_t)msec/1000, (uint32_t)msec%1000);
}

void rtc_event_wakeup_handler(void)
{
    rtc_time_t current_time;
    rtc_calendar_t current_date;
    // Read the time from the RTC
    rtc_get_time_clndr(&current_time, &current_date);
    
    if (ke_state_get(TASK_APP) == APP_CONNECTABLE)
    {
        // Serve the pending update
        uint8_t *mnf_data_storage = (date_data_index & 0x80) ? stored_scan_rsp_data : stored_adv_data;
        // Update the advertising time data structure
        date_data_update(&current_time, &current_date);
        // Update the selected fields of the advertising data (manufacturer data)
        memcpy(mnf_data_storage + (date_data_index & 0x7F), &adv_date, sizeof(struct date_specific_data_ad_structure));
        // Update advertising data on the fly
        app_easy_gap_update_adv_data(stored_adv_data, stored_adv_data_len, stored_scan_rsp_data, stored_scan_rsp_data_len);
    }
    
#ifdef PRINT_DATE_TIME_DATA
    print_date_time(&current_time, &current_date);
#endif  //PRINT_TIME_DATA
}

#if (BLE_CUSTOM1_SERVER) 
void rtc_alarm_wakeup_handler(void)
{
    if(!is_alarm_recursive())
    {
        rtc_alarm_enable(0x0);
        user_rtc_unregister_intr(RTC_INTR_ALRM);
    }
    
    alarm_start();
    
    // Add a blinking led for an amount of time
#ifdef PRINT_DATE_TIME_DATA
    arch_printf("Alarm Triggered \n\r");
#endif
}
#endif // BLE_CUSTOM1_SERVER

/**
 ****************************************************************************************
 * @brief Wake up rtc handler.
 * @param[in] event which trigger the interrupt
 * @return void
 ****************************************************************************************
*/
void rtc_wakeup_event(uint8_t event)
{   
    arch_ble_force_wakeup();
    
    if (event & ADVERTISE_UPDATE)
        rtc_event_wakeup_handler();
#if (BLE_CUSTOM1_SERVER)    
    if (event & RTC_EVENT_ALRM)
        rtc_alarm_wakeup_handler();
#endif  // BLE_CUSTOM1_SERVER
#if (BLE_CTS_SERVER)    
    if (event & RTC_EVENT_HOUR)
        update_ref_time_info();
#endif
}

void user_app_adv_start(void)
{
    struct gapm_start_advertise_cmd* cmd;
    
    cmd = app_easy_gap_undirected_advertise_get_active();
    
    app_add_ad_date(cmd, &adv_date, sizeof(struct date_specific_data_ad_structure));

    app_easy_gap_undirected_advertise_start();
    // Start the update timer using the RTC events mechanism
    user_rtc_register_intr(rtc_wakeup_event, ADVERTISE_UPDATE);
}

void user_app_on_init(void)
{
#if BLE_DIS_SERVER
    app_dis_init();
#endif
    
#if BLE_CTS_SERVER
    app_ctss_init();
#endif

    rtc_time_t      time;
    rtc_calendar_t  calendar;
    rtc_config_t    config;
    
    // Set up the RTC mode in 24hrs
    config.hour_clk_mode = user_rtc_conf.hour_clk_mode;
    // The registers of the RTC should be retained
    config.keep_rtc = user_rtc_conf.keep_rtc;
    
    // This should be accessed by the user to set the starting time perhaps from user_config.h file
    time.hour_mode  = user_time_conf.hour_mode;
    time.pm_flag    = user_time_conf.pm_flag;
    time.hour       = user_time_conf.hour;
    time.minute     = user_time_conf.minute;
    time.sec        = user_time_conf.sec;
    time.hsec       = user_time_conf.hsec;
    
    calendar.year   = user_calendar_conf.year;
    calendar.month  = user_calendar_conf.month;
    calendar.mday   = user_calendar_conf.mday;
    calendar.wday   = user_calendar_conf.wday;
    // Configure RTC time, calendar
    rtc_status_code_t status = rtc_configure(&time, &calendar, &config);
    ASSERT_ERROR(status == RTC_STATUS_CODE_VALID_ENTRY);
    
#if BLE_CTS_SERVER
    user_ctss_init();
/// If the Reference Time Feature is enabled activate the RTC interrupt on hour event
    if(APP_CTS_FEATURES & CTSS_REF_TIME_INFO_SUP)
        user_rtc_register_intr(rtc_wakeup_event, RTC_HOUR_INT_EN);
#endif

    adv_data_date_init();
    
    // Start RTC
    rtc_time_start();
    
    // Initialize service access write permissions for all the included profiles
    prf_init_srv_perm();

    // Set sleep mode
    arch_set_sleep_mode(app_default_sleep_mode);
}

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {
#if BLE_CUSTOM1_SERVER
        case CUSTS1_VAL_WRITE_IND:
        {
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

            switch (msg_param->handle)
            {
                case SVC1_IDX_SET_CURRENT_TIME_VAL:
                    // Update the Current Time Characteristic with value from master
                    user_svc1_current_time_wr_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                case SVC1_IDX_CURRENT_TIME_NTF_CFG:
                    user_svc1_current_time_cfg_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                case SVC1_IDX_ALARM_VAL:
                    user_svc1_alarm_wr_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                default:
                    break;
            }
        } break;

        case CUSTS1_VAL_NTF_CFM:
        {
            struct custs1_val_ntf_cfm const *msg_param = (struct custs1_val_ntf_cfm const *)(param);

            switch (msg_param->handle)
            {
                case SVC1_IDX_CURRENT_TIME_VAL:
                    break;

                default:
                    break;
            }
        } break;
        
        case CUSTS1_VALUE_REQ_IND:
        {
            struct custs1_value_req_ind const *msg_param = (struct custs1_value_req_ind const *)param;
            
            switch (msg_param->att_idx)
            {
                case SVC1_IDX_CURRENT_TIME_VAL:
                    user_svc1_current_time_read_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;
                
                default:
                {
                    struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC(CUSTS1_VALUE_REQ_RSP,
                                                                    src_id,
                                                                    dest_id,
                                                                    custs1_value_req_rsp);

                    // Provide the connection index.
                    rsp->conidx  = app_env[msg_param->conidx].conidx;
                    // Provide the attribute index.
                    rsp->att_idx = msg_param->att_idx;
                    // Force current length to zero.
                    rsp->length = 0;
                    // Set Error status
                    rsp->status  = ATT_ERR_APP_ERROR;
                    // Send message
                    ke_msg_send(rsp);
                }break;
            }
        } break;
#endif // BLE_CUSTOM1_SERVER
        case GAPC_PARAM_UPDATED_IND:
        {
            // Cast the "param" pointer to the appropriate message structure
            struct gapc_param_updated_ind const *msg_param = (struct gapc_param_updated_ind const *)(param);

            // Check if updated Conn Params filled to preferred ones
            if ((msg_param->con_interval >= user_connection_param_conf.intv_min) &&
                (msg_param->con_interval <= user_connection_param_conf.intv_max) &&
                (msg_param->con_latency == user_connection_param_conf.latency) &&
                (msg_param->sup_to == user_connection_param_conf.time_out))
            {
            }
        } break;

        default:
            break;
    }
}

/// @} APP
