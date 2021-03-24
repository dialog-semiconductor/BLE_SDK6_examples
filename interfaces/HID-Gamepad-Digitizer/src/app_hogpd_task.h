/**
 ****************************************************************************************
 *
 * \file app_hogpd_task.h
 *
 * \brief HID Keyboard handlers header file.
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information  
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com>
 *
 ****************************************************************************************
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
