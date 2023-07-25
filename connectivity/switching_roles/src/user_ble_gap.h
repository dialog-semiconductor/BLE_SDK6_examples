/**
 ****************************************************************************************
 *
 * @file user_ble_gap.h
 *
 * @brief Header for ble tools for scanning and formatting ble data
 *
 * Copyright (C) 2012-2022 Renesas Electronics Corporation and/or its affiliates
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
 
#ifndef _USER_BLE_GAP_H_
#define _USER_BLE_GAP_H_

#include "app_easy_timer.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef enum{
		BLE_GAP_ERROR_NO_ERROR,
		BLE_GAP_ERROR_INVALID_PARAMS,	
}ble_gap_error_t;

/*
 * DEFINES
 ****************************************************************************************
 */

/*Min and Max are not limitations of the stack but these windows provide better functionality*/
#define BLE_MIN_SCAN_INTVL_MS		(uint16_t)(10)
#if defined (__DA14531__)
//531 has an LDO refresh period where we need to give it time to refresh
#define BLE_MAX_SCAN_INTVL_MS		(uint16_t)(50)
#else
#define BLE_MAX_SCAN_INTVL_MS		(uint16_t)(250)
#endif

#define MAX_ADV_PAYLOAD_LEN			(uint16_t)(40)
#define NO_NAME_FOUND						"N/A"
#define NO_NAME_FOUND_LEN				(size_t)sizeof(NO_NAME_FOUND)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
const char *format_local_name(const uint8_t *, const uint8_t);
const char *format_bd_address(const struct bd_addr *);
timer_hnd set_routine_timer(void);
ble_gap_error_t user_ble_gap_start_scan(bool, uint16_t, uint16_t, bool);

#endif // _USER_BLE_GAP_H_
