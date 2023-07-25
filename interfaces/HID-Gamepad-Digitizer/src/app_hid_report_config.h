/**
 ****************************************************************************************
 *
 * \file app_hid_report_config.h
 *
 * \brief HID report module configuration header file.
 *
 * Copyright (C) 2017-2021 Dialog Semiconductor
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
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
