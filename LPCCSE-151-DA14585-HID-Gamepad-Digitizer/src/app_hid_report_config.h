/**
 ****************************************************************************************
 *
 * \file app_hid_report_config.h
 *
 * \brief HID report module configuration header file.
 *
 * Copyright (C) 2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information  
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com>
 *
 ****************************************************************************************
 */

#ifndef _APP_HID_REPORT_CONFIG_H_
#define _APP_HID_REPORT_CONFIG_H_

/**
 ****************************************************************************************
 * \addtogroup CONFIGURATION
 * \{
 * \addtogroup MODULE_CONFIG
 * \{
 * \addtogroup HID_REPORT_CFG
 *
 * \brief HID FIFO module configuration
 * \{
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \brief Number of HID key reports that can be stored in the report FIFO
 ****************************************************************************************
 */
#define HID_REPORT_FIFO_SIZE 8

/**
 ****************************************************************************************
 * \brief Define HID_REPORT_LIST_FULL_WARNING to have an assertion in DEVELOPMENT_DEBUG 
 * mode when the report FIFO is full
 ****************************************************************************************
 */
#undef HID_REPORT_FULL_WARNING

/**
 ****************************************************************************************
 * \brief Maximum HID report size that can be stored in the report FIFO
 ****************************************************************************************
 */
#define HID_REPORT_MAX_REPORT_SIZE 8

/**
 ****************************************************************************************
 * \brief Set the size of the rollover buffer. It must be greater or equal to the number
 * of keys that can be detected as pressed at the same time.
 ****************************************************************************************
 */
#define HID_REPORT_ROLL_OVER_BUF_SIZE 7

/**
 ****************************************************************************************
 * Enable reporting of any events happened while disconnected                        
 ****************************************************************************************
 */
#define HID_REPORT_HISTORY


//#if defined(CFG_PRF_HOGPD) && (defined(HAS_KBD) || defined(HAS_TOUCHPAD_SLIDER))
#if 0
    #define HID_REPORT_NORMAL_REPORT_IDX    HID_KBD_NORMAL_REPORT_IDX
    #define HID_REPORT_NORMAL_REPORT_SIZE   HID_KBD_NORMAL_REPORT_SIZE
    #define HID_REPORT_EXTENDED_REPORT_IDX  HID_KBD_EXTENDED_REPORT_IDX
    #define HID_REPORT_EXTENDED_REPORT_SIZE HID_KBD_EXTENDED_REPORT_SIZE
#else
    #define HID_REPORT_NORMAL_REPORT_IDX    0
    #define HID_REPORT_NORMAL_REPORT_SIZE   8
    #define HID_REPORT_EXTENDED_REPORT_IDX  2
    #define HID_REPORT_EXTENDED_REPORT_SIZE 3
#endif

#if 0
#if defined(HAS_AUDIO) && (defined(HAS_KBD) || defined(HAS_TOUCHPAD_SLIDER))
    #include "user_rcu_audio.h"

/**
 ****************************************************************************************
 * \brief This callback will be called to notify the application that a report has been
 *        sent.
 ****************************************************************************************
 */
    #define APP_HID_REPORT_SEND_CB user_audio_hid_send_report_cb

#endif
#endif

/**
 * \}
 * \}
 * \}
 */

#endif // _APP_HID_REPORT_CONFIG_H_
