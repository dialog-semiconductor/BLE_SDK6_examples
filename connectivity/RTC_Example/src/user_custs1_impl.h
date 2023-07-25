/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.h
 *
 * @brief User implementation for custom profile.
 *
 * Copyright (c) 2012-2021 Renesas Electronics Corporation and/or its affiliates
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
 * OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */

#ifndef _USER_CUSTS1_IMPL_H_
#define _USER_CUSTS1_IMPL_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief 
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "custs1_task.h" 

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

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
    uint8_t             recursive_alarm;
};

/*
 * DEFINES
 ****************************************************************************************
 */

#define APP_NTF_RTC_UPDATE                  (200)    // 200*10ms = 2 sec

#define ALARM_IGNORE_TIME_VALUE             (0xFF)
#define ALARM_IGNORE_DATE_VALUE             (0x00)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Get if the alarm occured is recursive or not
 * @param[in] void
 * @return bool true if recursive false otherwise
 ****************************************************************************************
*/
bool is_alarm_recursive(void);

/**
 ****************************************************************************************
 * @brief Current Time value write indication handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_svc1_current_time_wr_ind_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id);
                                            
/**
 ****************************************************************************************
 * @brief Current Time configuration indication handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/
void user_svc1_current_time_cfg_ind_handler(ke_msg_id_t const msgid,
                                            struct custs1_val_write_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Alarm value write indication handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/                                            
void user_svc1_alarm_wr_ind_handler(ke_msg_id_t const msgid,
                                    struct custs1_val_write_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Current time attribute read request handler.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return void
 ****************************************************************************************
*/                                    
void user_svc1_current_time_read_ind_handler(ke_msg_id_t const msgid,
                                                struct custs1_value_req_ind const *param,
                                                ke_task_id_t const dest_id,
                                                ke_task_id_t const src_id);
                                                

#endif // BLE_CUSTOM1_SERVER
                                                
#endif // _USER_CUSTS1_IMPL_H_
