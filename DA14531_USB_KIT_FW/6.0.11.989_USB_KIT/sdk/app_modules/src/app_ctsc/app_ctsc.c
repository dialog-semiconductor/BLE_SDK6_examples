/**
 ****************************************************************************************
 *
 * @file app_ctsc.c
 *
 * @brief Current Time Service Application entry point
 *
 * Copyright (c) 2017-2018 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_PRESENT)

#if (BLE_CTS_CLIENT)
#include "app_ctsc.h"                // Current Time Service Application Definitions
#include "app.h"                     // Application Definitions
#include "app_task.h"                // Application Task Definitions
#include "ctsc_task.h"               // Health Thermometer Functions
#include "app_prf_perm_types.h"
#include "user_profiles_config.h"

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_ctsc_init(void)
{
    // Nothing to do
}

void app_ctsc_create_task(void)
{
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC(GAPM_PROFILE_TASK_ADD_CMD,
                                                         TASK_GAPM,
                                                         TASK_APP,
                                                         gapm_profile_task_add_cmd);

    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_CTSC);
    req->prf_task_id = TASK_ID_CTSC;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Send the message
    ke_msg_send(req);
}

void app_ctsc_enable(uint8_t conidx)
{
    // Allocate the message
    struct ctsc_enable_req *req = KE_MSG_ALLOC(CTSC_ENABLE_REQ,
                                               prf_get_task_from_id(TASK_ID_CTSC),
                                               TASK_APP,
                                               ctsc_enable_req);

    // Fill in the parameter structure
    req->con_type = PRF_CON_DISCOVERY;

    // Send the message
    ke_msg_send(req);
}

void app_ctsc_read_ct(uint8_t conidx)
{
    // Allocate the message
    struct ctsc_read_req *req = KE_MSG_ALLOC(CTSC_READ_CT_REQ,
                                             prf_get_task_from_id(TASK_ID_CTSC),
                                             TASK_APP,
                                             ctsc_read_req);

    // Fill in the parameter structure
    req->char_code = CTSC_CHAR_CTS_CURRENT_TIME;

    // Send the message
    ke_msg_send(req);
}

void app_ctsc_read_lti(uint8_t conidx)
{
    // Allocate the message
    struct ctsc_read_req *req = KE_MSG_ALLOC(CTSC_READ_LTI_REQ,
                                             prf_get_task_from_id(TASK_ID_CTSC),
                                             TASK_APP,
                                             ctsc_read_req);

    // Fill in the parameter structure
    req->char_code = CTSC_CHAR_CTS_LOC_TIME_INFO;

    // Send the message
    ke_msg_send(req);
}

void app_ctsc_read_rti(uint8_t conidx)
{
    // Allocate the message
    struct ctsc_read_req *req = KE_MSG_ALLOC(CTSC_READ_RTI_REQ,
                                             prf_get_task_from_id(TASK_ID_CTSC),
                                             TASK_APP,
                                             ctsc_read_req);

    // Fill in the parameter structure
    req->char_code = CTSC_CHAR_CTS_REF_TIME_INFO;

    // Send the message
    ke_msg_send(req);
}

void app_ctsc_read_desc(uint8_t conidx)
{
    // Allocate the message
    struct ctsc_read_req *req = KE_MSG_ALLOC(CTSC_READ_DESC_REQ,
                                             prf_get_task_from_id(TASK_ID_CTSC),
                                             TASK_APP,
                                             ctsc_read_req);

    // Fill in the parameter structure
    req->char_code = CTSC_DESC_CTS_CT_CLI_CFG | CTSC_DESC_MASK;

    // Send the message
    ke_msg_send(req);
}

void app_ctsc_write_ct(uint8_t conidx, const struct cts_curr_time *ct)
{
    // Allocate the message
    struct ctsc_write_ct_req *req = KE_MSG_ALLOC(CTSC_WRITE_CT_REQ,
                                                 prf_get_task_from_id(TASK_ID_CTSC),
                                                 TASK_APP,
                                                 ctsc_write_ct_req);

    // Fill in the parameter structure
    req->chr.char_code = CTSC_CHAR_CTS_CURRENT_TIME;
    req->ct_val = *ct;

    // Send the message
    ke_msg_send(req);
}

void app_ctsc_write_lti(uint8_t conidx, const struct cts_loc_time_info *lti)
{
    // Allocate the message
    struct ctsc_write_lti_req *req = KE_MSG_ALLOC(CTSC_WRITE_LTI_REQ,
                                                  prf_get_task_from_id(TASK_ID_CTSC),
                                                  TASK_APP,
                                                  ctsc_write_lti_req);

    // Fill in the parameter structure
    req->chr.char_code = CTSC_CHAR_CTS_LOC_TIME_INFO;
    req->lti_val = *lti;

    // Send the message
    ke_msg_send(req);
}

void app_ctsc_write_desc(uint8_t conidx, uint16_t value)
{
    // Allocate the message
    struct ctsc_write_desc_req *req = KE_MSG_ALLOC(CTSC_WRITE_DESC_REQ,
                                                   prf_get_task_from_id(TASK_ID_CTSC),
                                                   TASK_APP,
                                                   ctsc_write_desc_req);

    // Fill in the parameter structure
    req->desc.char_code = CTSC_DESC_CTS_CT_CLI_CFG | CTSC_DESC_MASK;
    req->value = value;

    // Send the message
    ke_msg_send(req);
}

#endif // (BLE_CTS_CLIENT)

#endif // BLE_APP_PRESENT

/// @} APP
