/**
 ****************************************************************************************
 *
 * @file wsss_task.c
 *
 * @brief Weight Scale Service Server Task implementation.
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
 * @addtogroup WSSSTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_WSS_SERVER)
#include "prf_utils.h"
#include "wsss.h"
#include "wsss_task.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref WSSS_ENABLE_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int wsss_enable_req_handler(ke_msg_id_t const msgid,
                                   struct wsss_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    uint8_t conidx = KE_IDX_GET(src_id);

    if (ke_state_get(dest_id) == WSSS_IDLE)
    {
        if(conidx < BLE_CONNECTION_MAX)
        {
            struct wsss_env_tag* wsss_env = PRF_ENV_GET(WSSS, wsss);

            wsss_env->ind_cfg[conidx] = param->wt_meas_ind_en;
            status = GAP_ERR_NO_ERROR;
        }
    }

    // send response
    struct wsss_enable_cfm *rsp = KE_MSG_ALLOC(WSSS_ENABLE_CFM, src_id, dest_id, wsss_enable_cfm);
    rsp->status = status;

    ke_msg_send(rsp);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref WSSS_MEAS_SEND_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int wsss_meas_send_req_handler(ke_msg_id_t const msgid,
                                      struct wsss_meas_send_req const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    // Request status
    uint8_t status = GAP_ERR_NO_ERROR;
    // Message status
    int msg_status = KE_MSG_SAVED;
    // Packed value and its size
    uint8_t pval_size;

    struct wsss_env_tag *wsss_env = PRF_ENV_GET(WSSS, wsss);
    uint8_t conidx = KE_IDX_GET(src_id);

    if (ke_state_get(dest_id) == WSSS_IDLE)
    {
        if (WSSS_IS_ENABLED(WSSS_WT_MEAS_IND_CFG))
        {
            // Calculate measurement value size
            pval_size = wsss_calc_meas_value_size(param->meas_val.flags);
            // Allocate memory space for the measurement list element

            // Trigger indication
            struct gattc_send_evt_cmd *evt_cmd = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                KE_BUILD_ID(TASK_GATTC, conidx), prf_src_task_get(&(wsss_env->prf_env),0),
                gattc_send_evt_cmd, pval_size);

            // Fill in the parameter structure
            evt_cmd->operation = GATTC_INDICATE;
            evt_cmd->handle = wsss_env->att_tbl[WSS_IDX_WT_MEAS_VAL];
            evt_cmd->length = pval_size;
            // Pack the Wt Measurement value
            wsss_pack_meas_value(evt_cmd->value, &param->meas_val);

            ke_state_set(dest_id, WSSS_BUSY);

            // send notification to peer device
            ke_msg_send(evt_cmd);
        }
        else
        {
            status = PRF_ERR_IND_DISABLED;
        }
        wsss_meas_send_cfm_send(conidx, status);
        msg_status = KE_MSG_CONSUMED;
    }

    return msg_status;
}


/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_WRITE_REQ_IND message.
 * The handler compares the new values with current ones and notifies them if they changed.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_write_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    struct gattc_write_cfm * cfm;
    uint8_t status = PRF_APP_ERROR;
    int msg_status = KE_MSG_SAVED;

    if(ke_state_get(dest_id) == WSSS_IDLE)
    {
        uint8_t conidx = KE_IDX_GET(src_id);
        struct wsss_env_tag *wsss_env = PRF_ENV_GET(WSSS, wsss);
        uint16_t ind_cfg = co_read16p(&param->value[0]);

        if ((param->handle == wsss_env->att_tbl[WSS_IDX_WT_MEAS_IND_CFG])
            && ((ind_cfg == PRF_CLI_STOP_NTFIND) || (ind_cfg == PRF_CLI_START_IND)))
        {

            if (ind_cfg == PRF_CLI_STOP_NTFIND)
                wsss_env->ind_cfg[conidx] &= ~WSSS_WT_MEAS_IND_CFG;
            else if (ind_cfg == PRF_CLI_START_IND)
                wsss_env->ind_cfg[conidx] |= WSSS_WT_MEAS_IND_CFG;

            status = GAP_ERR_NO_ERROR;
        }
        else
        {
            status = PRF_APP_ERROR;
        }

        if (status == GAP_ERR_NO_ERROR)
        {
            //Inform APP of configuration change
            struct wsss_cfg_indntf_ind *ind = KE_MSG_ALLOC(WSSS_CFG_INDNTF_IND,
                                                           wsss_env->prf_env.app_task,
                                                           TASK_ID_WSSS,
                                                           wsss_cfg_indntf_ind);

            ind->cfg_val = ind_cfg;

            ke_msg_send(ind);
        }
        // Allocate write confirmation message.
        cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
        cfm->handle = param->handle;
        cfm->status = status;
        ke_msg_send(cfm);
        msg_status = KE_MSG_CONSUMED;
    }

    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_REQ_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_req_ind_handler(ke_msg_id_t const msgid,
                                      struct gattc_read_req_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{

    struct wsss_env_tag *wsss_env = PRF_ENV_GET(WSSS, wsss);
    uint8_t conidx = KE_IDX_GET(src_id);
    uint8_t status = ATT_ERR_INVALID_HANDLE;
    int msg_status = KE_MSG_SAVED;
    struct gattc_read_cfm* cfm = NULL;
    att_size_t length = 0;

    if(ke_state_get(dest_id) == WSSS_IDLE)
    {
        if (param->handle == wsss_env->att_tbl[WSS_IDX_WT_MEAS_IND_CFG])
        {
            length = sizeof(uint16_t);
        }
        else if (param->handle == wsss_env->att_tbl[WSS_IDX_FEATURE_VAL])
        {
            length = sizeof(uint32_t);
        }

        cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, length);
        cfm->handle = param->handle;
        cfm->status = status;
        cfm->length = length;

        if (param->handle == wsss_env->att_tbl[WSS_IDX_WT_MEAS_IND_CFG])
        {
            uint16_t ind_cfg = (wsss_env->ind_cfg[conidx] & WSSS_WT_MEAS_IND_CFG) ?
                        PRF_CLI_START_IND : PRF_CLI_STOP_NTFIND;

            co_write16p(cfm->value, ind_cfg);
            status = ATT_ERR_NO_ERROR;
        }
        else if (param->handle == wsss_env->att_tbl[WSS_IDX_FEATURE_VAL])
        {
            co_write32p(cfm->value, wsss_env->wss_features);
            status = ATT_ERR_NO_ERROR;
        }

        cfm->status = status;
        ke_msg_send(cfm);
        msg_status = KE_MSG_CONSUMED;
    }

    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Completion indication from GATT
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                 struct gattc_cmp_evt const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);

    if(param->operation == GATTC_INDICATE)
    {
        wsss_meas_send_cfm_send(conidx, param->status);
    }

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler wsss_default_state[] =
{
    {WSSS_ENABLE_REQ,       (ke_msg_func_t)wsss_enable_req_handler},
    {WSSS_MEAS_SEND_REQ,    (ke_msg_func_t)wsss_meas_send_req_handler},
    {GATTC_WRITE_REQ_IND,   (ke_msg_func_t)gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,    (ke_msg_func_t)gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,         (ke_msg_func_t)gattc_cmp_evt_handler},
};

///Specifies the message handlers that are common to all states.
const struct ke_state_handler wsss_default_handler = KE_STATE_HANDLER(wsss_default_state);

#endif //BLE_WSS_SERVER

/// @} WSSSTASK
