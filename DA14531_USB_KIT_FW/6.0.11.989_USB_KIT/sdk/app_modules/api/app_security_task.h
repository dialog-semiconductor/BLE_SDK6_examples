/**
 ****************************************************************************************
 *
 * @file app_security_task.h
 *
 * @brief Application Security Task Header file
 *
 * Copyright (C) RivieraWaves 2009-2013
 * Copyright (C) 2017 Modified by Dialog Semiconductor
 *
 ****************************************************************************************
 */

#ifndef _APP_SECURITY_TASK_H_
#define _APP_SECURITY_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APPSECTASK
 * @ingroup
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

#include "rwip_config.h"

#if (BLE_APP_SEC)

#include "ke_msg.h"         // Kernel Message

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Proccess security related messages.
 * @param[in] msgid                 Id of the message received
 * @param[in] param                 Pointer to the parameters of the message
 * @param[in] dest_id               ID of the receiving task instance
 * @param[in] src_id                ID of the sending task instance
 * @param[in|out] ke_msg_status_tag Pointer to message status value
 * @return If the message was consumed or not
 ****************************************************************************************
 */
enum process_event_response app_sec_process_handler(ke_msg_id_t const msgid,
                                                    void const *param,
                                                    ke_task_id_t const dest_id,
                                                    ke_task_id_t const src_id,
                                                    enum ke_msg_status_tag *msg_ret);

/// @} APPSECTASK

#endif // (BLE_APP_SEC)

#endif // _APP_SECURITY_TASK_H_
