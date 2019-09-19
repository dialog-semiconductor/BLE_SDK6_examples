/**
 ****************************************************************************************
 *
 * @file ctss_task.c
 *
 * @brief Current Time Service Task Implementation.
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
 * @addtogroup CTSSTASK
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_CTS_SERVER)
#include "gapc_task.h"
#include "gattc_task.h"
#include "atts.h"
#include "prf_utils.h"
#include "cts_common.h"
#include "ctss.h"
#include "ctss_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CTSS_UPD_LOCAL_TIME_INFO_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ctss_upd_local_time_info_req_handler(ke_msg_id_t const msgid,
                                                struct ctss_upd_local_time_info_req const *param,
                                                ke_task_id_t const dest_id,
                                                ke_task_id_t const src_id)
{
    ke_state_t state = ke_state_get(dest_id);
    int msg_status = KE_MSG_CONSUMED;

    if (state == CTSS_IDLE)
    {
        // Status
        uint8_t status;
        struct ctss_env_tag *ctss_env = PRF_ENV_GET(CTSS, ctss);
        uint16_t handle = ctss_get_att_handle(ctss_env, CTSS_IDX_LOCAL_TIME_INFO_VAL);
        struct ctss_upd_local_time_info_rsp *rsp;

        // Check if the Local Time Info Char. has been added in the database
        if (handle)
        {
            // Set the value in the database
            status = attmdb_att_set_value(handle, sizeof(struct cts_loc_time_info), 0,
                (uint8_t *) &(param->loc_time_info));
        }
        else
        {
            status = PRF_ERR_FEATURE_NOT_SUPPORTED;
        }

        // send response to application
        rsp = KE_MSG_ALLOC(CTSS_UPD_LOCAL_TIME_INFO_RSP, src_id, dest_id, ctss_upd_local_time_info_rsp);
        rsp->status = status;
        ke_msg_send(rsp);
    }
    else if (state == CTSS_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
}


/**
 ****************************************************************************************
 * @brief Handles @ref GATTC_CMP_EVT for GATTC_NOTIFY message meaning that Measurement
 * notification has been correctly sent to peer device (but not confirmed by peer device).
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,  struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == CTSS_BUSY)
    {
        if (param->operation == GATTC_NOTIFY)
        {
            // continue operation execution
            ctss_exe_operation();
        }
    }
    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the value confirmation from application
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ctss_read_rsp_handler(ke_msg_id_t msgid,
                                 const struct ctss_read_rsp *param,
                                 ke_task_id_t dest_id,
                                 ke_task_id_t src_id)
{
    ke_state_t state = ke_state_get(dest_id);

    if (ke_state_get(dest_id) == CTSS_BUSY)
    {
        struct ctss_env_tag *ctss_env = PRF_ENV_GET(CTSS, ctss);
        struct gattc_read_cfm *cfm;
        // retrieve value attribute
        uint16_t handle = ctss_get_att_handle(ctss_env, ctss_env->req_val);

        // chack if application provide correct value
        if (ctss_env->req_val == param->value)
        {
            // Send value to peer device.
            cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, KE_BUILD_ID(TASK_GATTC, ctss_env->req_conidx), dest_id, gattc_read_cfm, param->length);
            cfm->status = ATT_ERR_NO_ERROR;
            cfm->length = param->length;
            memcpy(cfm->value, param->data, param->length);
        }
        else
        {
            // application error, value provided by application is not the expected one
            cfm = KE_MSG_ALLOC(GATTC_READ_CFM, KE_BUILD_ID(TASK_GATTC, ctss_env->req_conidx), dest_id, gattc_read_cfm);
            cfm->status = ATT_ERR_APP_ERROR;
        }
        cfm->handle = handle;
        ke_msg_send(cfm);

        // return to idle state
        ke_state_set(dest_id, CTSS_IDLE);
    }

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CTSS_NTF_CURR_TIME_REQ message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ctss_ntf_curr_time_req_handler(ke_msg_id_t msgid,
                                          const struct ctss_ntf_curr_time_req *param,
                                          ke_task_id_t dest_id,
                                          ke_task_id_t src_id)
{
    ke_state_t state = ke_state_get(dest_id);
    int msg_status = KE_MSG_CONSUMED;

    if (state == CTSS_IDLE)
    {
        struct ctss_env_tag *ctss_env = PRF_ENV_GET(CTSS, ctss);
        // put task in a busy state
        ke_state_set(dest_id, CTSS_BUSY);
        ctss_env->operation = ke_param2msg(param);
        ctss_env->cursor = 0;
        ctss_pack_curr_time_value(&param->current_time, ctss_env->pckd_time);

        // trigger notification
        ctss_exe_operation();

        msg_status = KE_MSG_NO_FREE;
    }
    else if (state == CTSS_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_REQ_IND message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_req_ind_handler(ke_msg_id_t const msgid,
                                     const struct gattc_read_req_ind *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    int msg_status = KE_MSG_CONSUMED;
    uint8_t state = ke_state_get(dest_id);

    if (state == CTSS_IDLE)
    {
        struct ctss_read_req_ind *req_ind;
        struct gattc_read_cfm *cfm;
        uint8_t conidx = KE_IDX_GET(src_id);
        uint16_t ntf_cfg;

        struct ctss_env_tag *ctss_env = PRF_ENV_GET(CTSS, ctss);
        // retrieve value attribute
        uint8_t value = ctss_get_att_idx(ctss_env, param->handle);

        switch (value)
        {
        case CTSS_IDX_CURRENT_TIME_VAL:
        case CTSS_IDX_REF_TIME_INFO_VAL:
            // request value from application
            req_ind = KE_MSG_ALLOC(CTSS_READ_REQ_IND,
                    prf_dst_task_get(&(ctss_env->prf_env), KE_IDX_GET(src_id)),
                    dest_id, ctss_read_req_ind);
            req_ind->value = value;
            ctss_env->req_val    = value;
            ctss_env->req_conidx = KE_IDX_GET(src_id);
            ke_msg_send(req_ind);

            // Put Service in a busy state
            ke_state_set(dest_id, CTSS_BUSY);
            break;
        case CTSS_IDX_LOCAL_TIME_INFO_VAL:
            // Send data to peer device
            cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(struct cts_loc_time_info));
            cfm->length = sizeof(struct cts_loc_time_info);
            memcpy(cfm->value, &ctss_env->loc_time_info, sizeof(struct cts_loc_time_info));
            cfm->handle = param->handle;
            cfm->status = ATT_ERR_NO_ERROR;

            // Send value to peer device.
            ke_msg_send(cfm);
            break;
        case CTSS_IDX_CURRENT_TIME_CFG:
            // Fill data
            ntf_cfg = ctss_env->ntf_cfg[conidx] ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND;
            // Send data to peer device
            cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(uint16_t));
            cfm->length = sizeof(uint16_t);
            memcpy(cfm->value, &ntf_cfg, sizeof(uint16_t));
            cfm->handle = param->handle;
            cfm->status = ATT_ERR_NO_ERROR;

            // Send value to peer device.
            ke_msg_send(cfm);
            break;
        default:
            // Send data to peer device
            cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(uint16_t));
            cfm->length = 0;
            cfm->handle = param->handle;
            cfm->status = ATT_ERR_REQUEST_NOT_SUPPORTED;

            // Send value to peer device.
            ke_msg_send(cfm);
            break;
        }
    }
    else if (state == CTSS_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }
    return msg_status;
}

/**
 ****************************************************************************************
 * @brief Helper function to check it contents of \p cts_curr_time is valid
 *
 * @param[in] time     current time
 * return true if \p time is valid
 ****************************************************************************************
 */
static inline bool cts_is_current_time_valid(const struct cts_curr_time *time)
{
        return (((time->exact_time_256.day_date_time.date_time.year >= 1582) &&
                 (time->exact_time_256.day_date_time.date_time.year <= 9999)) ||
               (time->exact_time_256.day_date_time.date_time.year == 0)) &&
               (time->exact_time_256.day_date_time.date_time.month <= 12) &&
               (time->exact_time_256.day_date_time.date_time.day <= 31) &&
               (time->exact_time_256.day_date_time.date_time.hour <= 23) &&
               (time->exact_time_256.day_date_time.date_time.min <= 59) &&
               (time->exact_time_256.day_date_time.date_time.sec <= 59) &&
               (time->exact_time_256.day_date_time.day_of_week <= 7) &&
               (time->adjust_reason < 0xF);
}

/**
 ****************************************************************************************
 * @brief Helper function to check it contents of \p cts_local_time_info_t is valid
 *
 * @param [in] info     local time information
 *
 * return true if \p info is valid
 ****************************************************************************************
 */
static inline bool cts_is_local_time_info_valid(const struct cts_loc_time_info *info)
{
        return (((info->time_zone >= -48) && (info->time_zone <= 56)) || (info->time_zone == -128)) &&
               (info->dst_offset <= 8);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_WRITE_REQ_IND message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_write_req_ind_handler(ke_msg_id_t msgid,
                                       const struct gattc_write_req_ind *param,
                                       ke_task_id_t dest_id,
                                       ke_task_id_t src_id)
{
    int msg_status = KE_MSG_CONSUMED;
    uint8_t state = ke_state_get(dest_id);

    if (state == CTSS_IDLE)
    {
        uint8_t status = ATT_ERR_NO_ERROR;
        bool send_response_now = true;
        struct ctss_env_tag *ctss_env = PRF_ENV_GET(CTSS, ctss);
        uint8_t value = ctss_get_att_idx(ctss_env, param->handle);
        uint8_t conidx = KE_IDX_GET(src_id);

        switch (value)
        {
        case CTSS_IDX_CURRENT_TIME_VAL:
            if (ctss_env->features & CTSS_CURRENT_TIME_WRITE_SUP)
            {
                if (param->offset)
                {
                    status = ATT_ERR_ATTRIBUTE_NOT_LONG;
                    break;
                }
                if (param->length != CTSS_CURRENT_TIME_VAL_LEN)
                {
                    status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
                    break;
                }

                // CTSS_WRITE_REQ_IND
                // Inform APP about write request
                struct ctss_cur_time_write_ind *ind = KE_MSG_ALLOC(CTSS_WRITE_CUR_TIME_REQ_IND,
                        prf_dst_task_get(&ctss_env->prf_env, conidx), dest_id,
                        ctss_cur_time_write_ind);
                ctss_unpack_curr_time_value(param->value, &ind->current_time);
                if (!cts_is_current_time_valid(&ind->current_time))
                {
                    status = ATT_ERR_APP_ERROR;
                    ke_free(ind);
                    break;
                }
                ind->value = value;
                ctss_env->req_conidx = conidx;
                ctss_env->req_val = value;

                send_response_now = false;
                ke_state_set(dest_id, CTSS_BUSY);
                ke_msg_send(ind);
            }
            else
            {
                status = ATT_ERR_WRITE_NOT_PERMITTED;
            }
            break;
        case CTSS_IDX_LOCAL_TIME_INFO_VAL:
            if (ctss_env->features & CTSS_LOC_TIME_WRITE_SUP)
            {
                struct cts_loc_time_info lti;
                if (param->offset)
                {
                    status = ATT_ERR_ATTRIBUTE_NOT_LONG;
                    break;
                }
                if (param->length != 2)
                {
                    status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
                    break;
                }

                lti.time_zone = (cts_time_zone) param->value[0];
                lti.dst_offset = (cts_dst_offset) param->value[1];
                if (!cts_is_local_time_info_valid(&lti))
                {
                    status = ATT_ERR_APP_ERROR;
                    break;
                }

                // CTSS_WRITE_REQ_IND
                // Inform APP about write request
                struct ctss_loc_time_info_write_ind *ind = KE_MSG_ALLOC(CTSS_WRITE_LOC_TIME_REQ_IND,
                        prf_dst_task_get(&ctss_env->prf_env, conidx), dest_id,
                        ctss_loc_time_info_write_ind);
                ctss_env->loc_time_info = lti;
                ctss_env->operation = ke_param2msg(param);
                ctss_env->req_val = value;
                ctss_env->req_conidx = conidx;
                ind->local_time_info = ctss_env->loc_time_info;
                ind->value = value;

                ke_msg_send(ind);
            }
            else
            {
                status = ATT_ERR_WRITE_NOT_PERMITTED;
            }
            break;
        case CTSS_IDX_CURRENT_TIME_CFG:
            if (param->offset)
            {
                status = ATT_ERR_ATTRIBUTE_NOT_LONG;
                break;
            }
            if (param->length != 2)
            {
                status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
                break;
            }
            // check only bit 0
            ctss_env->ntf_cfg[conidx] = (param->value[0] & (1 << 0)) ? 1 : 0;
            break;
        default:
            status = ATT_ERR_WRITE_NOT_PERMITTED;
            break;
        }

        if (send_response_now)
        {
            struct gattc_write_cfm *cfm;
            //Send write response
            cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
            cfm->handle = param->handle;
            cfm->status = status;
            ke_msg_send(cfm);
        }
    }
    else if (state == CTSS_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CTSS_WRITE_RSP message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ctss_write_rsp_handler(ke_msg_id_t msgid,
                                  const struct ctss_write_rsp *param,
                                  ke_task_id_t dest_id,
                                  ke_task_id_t src_id)
{
    struct gattc_write_cfm *cfm;
    struct ctss_env_tag *ctss_env = PRF_ENV_GET(CTSS, ctss);

    if (ke_state_get(dest_id) == CTSS_BUSY)
    {
        if (param->value == ctss_env->req_val)
        {
            if (ctss_env->req_val == CTSS_IDX_LOCAL_TIME_INFO_VAL && param->status == 0)
            {
                struct ctss_loc_time_info_write_ind *ind = ke_msg2param(ctss_env->operation);
                ctss_env->loc_time_info = ind->local_time_info;
            }

            if (ctss_env->operation)
            {
                ke_free(ctss_env->operation);
                ctss_env->operation = NULL;
            }
            //Send write response
            cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, KE_BUILD_ID(TASK_GATTC, ctss_env->req_conidx), dest_id, gattc_write_cfm);
            cfm->handle = ctss_get_att_handle(ctss_env, param->value);
            cfm->status = param->status;
            ke_state_set(dest_id, CTSS_IDLE);
            ke_msg_send(cfm);
        }
    }

    return KE_MSG_CONSUMED;
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Connected State handler definition.
const struct ke_msg_handler ctss_default_state[] =
{
    {CTSS_NTF_CURR_TIME_REQ,        (ke_msg_func_t) ctss_ntf_curr_time_req_handler},
    {CTSS_UPD_LOCAL_TIME_INFO_REQ,  (ke_msg_func_t) ctss_upd_local_time_info_req_handler},
    {GATTC_READ_REQ_IND,            (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_WRITE_REQ_IND,           (ke_msg_func_t) gattc_write_req_ind_handler},
    {CTSS_WRITE_RSP,                (ke_msg_func_t) ctss_write_rsp_handler},
    {CTSS_READ_RSP,                 (ke_msg_func_t) ctss_read_rsp_handler},
    {GATTC_CMP_EVT,                 (ke_msg_func_t) gattc_cmp_evt_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler ctss_default_handler = KE_STATE_HANDLER(ctss_default_state);

#endif //BLE_CTS_SERVER

/// @} CTSSTASK
