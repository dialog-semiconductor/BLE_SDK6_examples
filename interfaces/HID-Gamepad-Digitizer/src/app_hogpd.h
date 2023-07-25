/**
 ****************************************************************************************
 *
 * \file app_hogpd.h
 *
 * \brief Keyboard (HID) Application entry point header file.
 *
 * Copyright (C) 2020-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef _APP_HOGPD_H_
#define _APP_HOGPD_H_

/**
 * \addtogroup USER
 * \{
 * \addtogroup PROFILE
 * \{
 * \addtogroup APP_HOGPD
 *
 * \brief HOGPD Profile application implementation
 *
 * \{
 */

#if (BLE_HID_DEVICE)

#include "hogpd.h"
#include "hogpd_task.h"

/**
 ****************************************************************************************
 * \brief Enables the HOGPD profile 
 *
 * \param   conidx
 ****************************************************************************************
 */
void app_hogpd_enable(uint8_t conidx);


/**
 ****************************************************************************************
 * \brief Creates the HID over GATT database
 ****************************************************************************************
 */
void app_hogpd_create_db(void);

/**
 ****************************************************************************************
 * \brief    
 ****************************************************************************************
 */
bool app_hogpd_send_report(uint8_t report_idx, uint8_t *data, uint16_t length, enum hogpd_report_type type);

/**
 ****************************************************************************************
 * \brief   
 ****************************************************************************************
 */
uint8_t app_hogpd_get_protocol_mode(void);

uint16_t app_hogpd_report_handle(uint8_t report_nb);

#endif

/**
 * \}
 * \}
 * \}
 */

#endif // _APP_HOGPD_H_
