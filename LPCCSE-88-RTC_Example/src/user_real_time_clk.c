/**
 ****************************************************************************************
 *
 * @file user_real_time_clk.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (C) 2012-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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

#if BLE_CUSTOM1_SERVER
struct __PACKED current_time_char_structure
{
    uint16_t            year;
    uint8_t             month;
    uint8_t             mday;
    uint8_t             wday;
    uint8_t             hour;
    uint8_t             minute;
    uint8_t             second;
    uint8_t             hsecond;
    rtc_hour_mode_t     hour_mode;
    uint8_t             pm_flag;
};

struct __PACKED alarm_char_structure
{
    uint8_t             month;
    uint8_t             mday;
    uint8_t             hour;
    uint8_t             minute;
    uint8_t             second;
    uint8_t             hsecond;
    uint8_t             pm_flag;
};
#endif // BLE_CUSTOM1_SERVER

#if BLE_CTS_SERVER
struct cts_curr_time cts_current_time                   __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
struct cts_ref_time_info ref_time                       __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
#endif

struct date_specific_data_ad_structure adv_date         __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t date_data_index                                 __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_adv_data_len                             __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                        __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]                   __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN]         __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

uint8_t app_connection_idx                              __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY  
timer_hnd app_param_update_request_timer_used           __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY 
timer_hnd ntf_update_tmr_hnd                            __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

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

//        // Stop the interrupts occuring from RTC for updating the advertising string
//        rtc_unregister_intr();    

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
    arch_printf("%02d, %02d/%02d/%04d ", clndr->wday, clndr->mday, clndr->month, clndr->year );
    /* Print the current time */
    arch_printf("%02d:%02d:%02d:%02d %s ", time->hour, time->minute, time->sec, time->hsec,
                                           (time->hour_mode) ? ((time->pm_flag) ? "PM" : "AM") : "24H");
    /* Prin the current time in ms */
    arch_printf("%04d.%03d ms \n\r", (uint32_t)msec/1000, (uint32_t)msec%1000);
}

void rtc_wakeup_handler(void)
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

/**
 ****************************************************************************************
 * @brief Wake up rtc handler.
 * @param[in] event which trigger the interrupt
 * @return void
 ****************************************************************************************
*/
static void rtc_wakeup_event(uint8_t event)
{
    if ((event & RTC_EVENT_ALRM) != RTC_EVENT_ALRM)
    {
        arch_ble_force_wakeup();        // Force the BLE to wake up to process the if not the device will stay awake until the next event.
        app_easy_wakeup_set(rtc_wakeup_handler);
        app_easy_wakeup();
    }
    else
        arch_printf("Alarm triggered \n\r");
}

void user_app_adv_start(void)
{
    struct gapm_start_advertise_cmd* cmd;
    
#ifdef NON_CONNECTABLE_ADVERTISING
    cmd = app_easy_gap_non_connectable_advertise_get_active();
#else
    cmd = app_easy_gap_undirected_advertise_get_active();
#endif
    
    app_add_ad_date(cmd, &adv_date, sizeof(struct date_specific_data_ad_structure));

#ifdef NON_CONNECTABLE_ADVERTISING
    app_easy_gap_non_connectable_advertise_start();
#else
    app_easy_gap_undirected_advertise_start();
#endif
    
    app_easy_wakeup_set(rtc_wakeup_handler);
    // Start the update timer using the RTC events mechanism
    rtc_register_intr(rtc_wakeup_event, ADVERTISE_UPDATE);
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

    adv_data_date_init();
    
    // Start RTC
    rtc_time_start();
    
    // Initialize service access write permissions for all the included profiles
    prf_init_srv_perm();

    // Set sleep mode
    arch_set_sleep_mode(app_default_sleep_mode);
}

#if BLE_CTS_SERVER

void user_ctss_init(void)
{
    struct prf_date_time date_time = {
        user_calendar_conf.year, 
        user_calendar_conf.month, 
        user_calendar_conf.mday, 
        user_time_conf.hour, 
        user_time_conf.minute, 
        user_time_conf.sec
    };
    
    cts_current_time.adjust_reason = CTSS_REASON_FLAG_MAN_TIME_UPDATE;
    cts_current_time.exact_time_256.day_date_time.date_time = date_time;
    cts_current_time.exact_time_256.day_date_time.day_of_week = user_calendar_conf.wday;
    cts_current_time.exact_time_256.fraction_256 = 0;

    ref_time.time_source = 4;
    ref_time.time_accuracy = 0;
    ref_time.days_update = 0;
    ref_time.hours_update = 0;
}


void user_on_current_time_read(struct cts_curr_time *ct)
{
    rtc_time_t rtc_current_time;
    rtc_calendar_t rtc_current_date;    
    rtc_get_time_clndr(&rtc_current_time, &rtc_current_date);
    
    // Update the struct for the CTS profile with the data from the RTC
    cts_current_time.exact_time_256.day_date_time.day_of_week       = rtc_current_date.wday;
    cts_current_time.exact_time_256.day_date_time.date_time.year    = rtc_current_date.year;
    cts_current_time.exact_time_256.day_date_time.date_time.month   = rtc_current_date.month;
    cts_current_time.exact_time_256.day_date_time.date_time.day     = rtc_current_date.mday;
    cts_current_time.exact_time_256.day_date_time.date_time.hour    = rtc_current_time.hour;
    cts_current_time.exact_time_256.day_date_time.date_time.min     = rtc_current_time.minute;
    cts_current_time.exact_time_256.day_date_time.date_time.sec     = rtc_current_time.sec;
    cts_current_time.exact_time_256.fraction_256 = (rtc_current_time.hsec << 8) / 100;
    
    *ct = cts_current_time;
}

void user_on_ref_time_read(struct cts_ref_time_info *rt)
{
    *rt = ref_time;
}

uint8_t user_on_cur_time_write_req(const struct cts_curr_time *ct)
{
    // Check the alarms when the time is updated by the accumulation value
     
    rtc_time_t rtc_current_time;
    rtc_calendar_t rtc_current_date;
    
    rtc_current_date.wday   = ct->exact_time_256.day_date_time.day_of_week;
    rtc_current_date.year   = ct->exact_time_256.day_date_time.date_time.year;
    rtc_current_date.month  = ct->exact_time_256.day_date_time.date_time.month;
    rtc_current_date.mday   = ct->exact_time_256.day_date_time.date_time.day;
    rtc_current_time.hour   = ct->exact_time_256.day_date_time.date_time.hour;
    rtc_current_time.minute = ct->exact_time_256.day_date_time.date_time.min;
    rtc_current_time.sec    = ct->exact_time_256.day_date_time.date_time.sec;
    // First have the RTC driver to check if the date/time entry is valid
    rtc_status_code_t status = rtc_set_time_clndr(&rtc_current_time, &rtc_current_date);
    
    if(status != RTC_STATUS_CODE_VALID_ENTRY)
    {
        return ATT_ERR_APP_ERROR;
    }
    else
    {
        cts_current_time = *ct;     // Update the current time of the profile
        return ATT_ERR_NO_ERROR;
    }
}

#endif // BLE_CTS_SERVER

/***************************************************************************************/
/****************************Custom Profile Handlers************************************/
/***************************************************************************************/

#if BLE_CUSTOM1_SERVER

static void rtc_error_ntf_send(uint16_t handle, uint8_t err_code)
{
    uint8_t error_code[5] = "ERR ";
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                      prf_get_task_from_id(TASK_ID_CUSTS1),
                                                      TASK_APP,
                                                      custs1_val_ntf_ind_req,
                                                      sizeof(error_code));
    
    error_code[4] = err_code;
    req->handle = handle;
    req->length = sizeof(error_code);
    req->notification = true;
    memcpy(req->value, error_code, sizeof(error_code));

    ke_msg_send(req);
}

static void rtc_alarm_handler(uint8_t event)
{
    if ((event & RTC_EVENT_ALRM) == RTC_EVENT_ALRM)
        arch_printf("Alarm triggered \n\r");
}

void user_svc1_current_time_wr_ind_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{    
    rtc_time_t rtc_current_time;
    rtc_calendar_t rtc_current_date;
    
    struct current_time_char_structure current_timestamp;
    memset(&current_timestamp, 0, sizeof(struct current_time_char_structure));
    memcpy(&current_timestamp, &param->value[0], param->length);
    
    rtc_current_date.wday       = current_timestamp.wday;
    rtc_current_date.year       = current_timestamp.year;
    rtc_current_date.month      = current_timestamp.month;
    rtc_current_date.mday       = current_timestamp.mday;
    
    rtc_current_time.hour       = current_timestamp.hour;
    rtc_current_time.minute     = current_timestamp.minute;
    rtc_current_time.sec        = current_timestamp.second;
    rtc_current_time.hsec       = current_timestamp.hsecond;
    rtc_current_time.hour_mode  = current_timestamp.hour_mode;
    
    if (current_timestamp.hour_mode == RTC_HOUR_MODE_12H)
        rtc_current_time.pm_flag    = current_timestamp.pm_flag;

    // First have the RTC driver to check if the date/time entry is valid
    rtc_status_code_t status = rtc_set_time_clndr(&rtc_current_time, &rtc_current_date);
    
    if(status != RTC_STATUS_CODE_VALID_ENTRY)
    {
        rtc_error_ntf_send(param->handle, status);
    }
}

static void app_ntf_rtc_update_handler(void)
{
    struct current_time_char_structure updated_timestamp;
    rtc_time_t rtc_current_time;
    rtc_calendar_t rtc_current_date;    
    rtc_get_time_clndr(&rtc_current_time, &rtc_current_date);
    
    updated_timestamp.wday      = rtc_current_date.wday;
    updated_timestamp.year      = rtc_current_date.year;
    updated_timestamp.month     = rtc_current_date.month;
    updated_timestamp.mday      = rtc_current_date.mday;
    updated_timestamp.hour      = rtc_current_time.hour;
    updated_timestamp.minute    = rtc_current_time.minute;
    updated_timestamp.second    = rtc_current_time.sec;
    updated_timestamp.hsecond   = rtc_current_time.hsec;
    updated_timestamp.hour_mode = rtc_current_time.hour_mode;
    updated_timestamp.pm_flag   = rtc_current_time.pm_flag;
    
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                      prf_get_task_from_id(TASK_ID_CUSTS1),
                                                      TASK_APP,
                                                      custs1_val_ntf_ind_req,
                                                      DEF_SVC1_CURRENT_TIME_CHAR_LEN);
    
    req->handle = SVC1_IDX_CURRENT_TIME_VAL;
    req->length = DEF_SVC1_CURRENT_TIME_CHAR_LEN;
    req->notification = true;
    memcpy(req->value, &updated_timestamp, DEF_SVC1_CURRENT_TIME_CHAR_LEN);

    ke_msg_send(req);
    
    if (ke_state_get(TASK_APP) == APP_CONNECTED)
    {
        // Set it once again until Stop command is received in Control Characteristic
        ntf_update_tmr_hnd = app_easy_timer(APP_NTF_RTC_UPDATE, app_ntf_rtc_update_handler);
    }
}

void user_svc1_current_time_cfg_ind_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    uint16_t ntf_en = 0;
    memcpy (&ntf_en, param->value, param->length);
    if (ntf_en == PRF_CLI_START_NTF)
    {
        ntf_update_tmr_hnd = app_easy_timer(APP_NTF_RTC_UPDATE, app_ntf_rtc_update_handler);
    }
    else
    {
        if (ntf_update_tmr_hnd != EASY_TIMER_INVALID_TIMER)
        {
            app_easy_timer_cancel(ntf_update_tmr_hnd);
            ntf_update_tmr_hnd = EASY_TIMER_INVALID_TIMER;
        }
    }
}

void user_svc1_alarm_wr_ind_handler(ke_msg_id_t const msgid,
                                    struct custs1_val_write_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    uint8_t temp = 0x00;
    rtc_status_code_t status;
    rtc_time_t time;
    rtc_alarm_calendar_t calendar;
    rtc_alarm_calendar_t *p_calendar = &calendar;
    struct alarm_char_structure alarm_time;
    
    memset(&alarm_time, 0, sizeof(struct current_time_char_structure));
    memcpy(&alarm_time, &param->value[0], sizeof(struct current_time_char_structure));
    
    if (!alarm_time.month || !alarm_time.mday)
        p_calendar = NULL;
    else
    {
        calendar.month  = alarm_time.month;
        (alarm_time.month != 0) ? (temp |= RTC_ALARM_EN_MONTH) : (temp |= 0x00);
        calendar.mday   = alarm_time.mday;
        (alarm_time.mday != 0) ? (temp |= RTC_ALARM_EN_MDAY) : (temp |= 0x00);
    }
    
    time.hour       = alarm_time.hour;
    (alarm_time.hour != 0) ? (temp |= RTC_ALARM_EN_HOUR) : (temp |= 0x00);
    time.minute     = alarm_time.minute;
    (alarm_time.minute != 0) ? (temp |= RTC_ALARM_EN_MIN) : (temp |= 0x00);
    time.sec        = alarm_time.second;
    (alarm_time.second != 0) ? (temp |= RTC_ALARM_EN_SEC) : (temp |= 0x00);
    time.hsec       = alarm_time.hsecond;
    (alarm_time.hsecond != 0) ? (temp |= RTC_ALARM_EN_HSEC) : (temp |= 0x00);
    
    time.hour_mode  = rtc_get_hour_clk_mode();
    
    if (time.hour_mode == RTC_HOUR_MODE_12H)
        time.pm_flag    = alarm_time.pm_flag;
    
    rtc_register_intr(rtc_alarm_handler, RTC_INTR_ALRM | ADVERTISE_UPDATE);
    status = rtc_set_alarm(&time, p_calendar, temp);
    
    if (status != RTC_STATUS_CODE_VALID_ENTRY)
    {
        rtc_error_ntf_send(param->handle, status);
#ifdef PRINT_DATE_TIME_DATA
        arch_printf("Invalid Alarm time, RTC return status %02x \n\r", status);
    }
    else
    {
        arch_printf("Alarm was set for %02d/%02d %02d:%02d:%02d:%02d \n\r", 
                                                                    p_calendar->mday,
                                                                    p_calendar->month,
                                                                    time.hour,
                                                                    time.minute,
                                                                    time.sec,
                                                                    time.hsec);
#endif
    }
}

void user_svc1_current_time_read_ind_handler(ke_msg_id_t const msgid,
                                                struct custs1_value_req_ind const *param,
                                                ke_task_id_t const dest_id,
                                                ke_task_id_t const src_id)
{
    struct current_time_char_structure updated_timestamp;
    rtc_time_t rtc_current_time;
    rtc_calendar_t rtc_current_date;    
    rtc_get_time_clndr(&rtc_current_time, &rtc_current_date);
    
    updated_timestamp.wday      = rtc_current_date.wday;
    updated_timestamp.year      = rtc_current_date.year;
    updated_timestamp.month     = rtc_current_date.month;
    updated_timestamp.mday      = rtc_current_date.mday;
    updated_timestamp.hour      = rtc_current_time.hour;
    updated_timestamp.minute    = rtc_current_time.minute;
    updated_timestamp.second    = rtc_current_time.sec;
    updated_timestamp.hsecond   = rtc_current_time.hsec;
    updated_timestamp.hour_mode = rtc_current_time.hour_mode;
    updated_timestamp.pm_flag   = rtc_current_time.pm_flag;
   
    
    struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC_DYN(CUSTS1_VALUE_REQ_RSP,
                                               prf_get_task_from_id(TASK_ID_CUSTS1),
                                               TASK_APP,
                                               custs1_value_req_rsp,
                                               DEF_SVC1_CURRENT_TIME_CHAR_LEN);
    
    // Provide the connection index.
    rsp->conidx  = app_env[param->conidx].conidx;
    // Provide the attribute index.
    rsp->att_idx = param->att_idx;
    // Force current length to zero.
    rsp->length  = sizeof(struct current_time_char_structure);
    // Provide the ATT error code.
    rsp->status  = ATT_ERR_NO_ERROR;
    // Copy value
    memcpy(&rsp->value, &updated_timestamp, rsp->length);
    // Send message
    ke_msg_send(rsp);
}

#endif // BLE_CUSTOM1_SERVER

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
