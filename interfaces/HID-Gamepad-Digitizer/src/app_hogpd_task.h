/**
 ****************************************************************************************
 *
 * \file app_hogpd_task.h
 *
 * \brief HID Keyboard handlers header file.
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

#ifndef _APP_HOGPD_TASK_H_
#define _APP_HOGPD_TASK_H_

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

#include "ke_msg.h"
#include "hogpd.h"
#include "gattc_task.h"
#include "app_hogpd_defs.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \brief   Called when the Service Changed indication has been successfully received by the Host
 *          
 * \param[in] msgid
 * \param[in] param
 * \param[in] dest_id
 * \param[in] src_id
 *
 * \return enum process_event_response
 *****************************************************************************************
 */
enum process_event_response app_hogpd_process_handler(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id);

/**
 * \}
 * \}
 * \}
 */

#endif // _APP_HOGPD_TASK_H_