/**
 ****************************************************************************************
 *
 * @file bcss_task.c
 *
 * @brief Body Composition Service Server Task implementation.
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
 * @addtogroup BCSSTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (BLE_BCS_SERVER)
#include "prf_utils.h"
#include "bcss.h"
#include "bcss_task.h"

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BCSS_ENABLE_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bcss_enable_req_handler(ke_msg_id_t const msgid,
                                   struct bcss_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    uint8_t conidx = KE_IDX_GET(src_id);

    if(conidx < BLE_CONNECTION_MAX)
    {
        if(ke_state_get(dest_id) == BCSS_IDLE)
        {
            struct bcss_env_tag* bcss_env = PRF_ENV_GET(BCSS, bcss);

            bcss_env->ind_cfg[conidx] = param->meas_ind_en;
            status = GAP_ERR_NO_ERROR;
        }
    }

    // Send response
    struct bcss_enable_rsp *rsp = KE_MSG_ALLOC(BCSS_ENABLE_RSP, src_id, dest_id, bcss_enable_rsp);
    rsp->init_cfg_ind = param->meas_ind_en;
    rsp->status = status;

    ke_msg_send(rsp);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BCSS_MEAS_VAL_IND_REQ message.
 * The handler triggers the Measurement Value indication.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bcss_meas_val_ind_req_handler(ke_msg_id_t const msgid,
                                         struct bcss_meas_val_ind_req const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
    int msg_status = KE_MSG_SAVED;

    if (ke_state_get(dest_id) != BCSS_BUSY)
    {
        uint8_t conidx = KE_IDX_GET(src_id);
        struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);

        msg_status = KE_MSG_CONSUMED;

        if (bcss_env->meas)
        {
            bcss_ind_rsp_send(conidx, PRF_ERR_REQ_DISALLOWED);

            return msg_status;
        }

        if (bcss_env->ind_cfg[conidx] == PRF_CLI_START_IND)
        {
            ke_state_set(dest_id, BCSS_BUSY);
            bcss_indicate(conidx, bcss_env->bc_feat, &param->meas);
        }
        else
        {
            bcss_ind_rsp_send(conidx, PRF_ERR_IND_DISABLED);
        }
    }

    return msg_status;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GL2C_CODE_ATT_WR_CMD_IND message.
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
    int msg_status = KE_MSG_SAVED;

    if (ke_state_get(dest_id) != BCSS_BUSY)
    {
        struct gattc_write_cfm * cfm;
        uint8_t status = PRF_APP_ERROR;

        uint8_t conidx = KE_IDX_GET(src_id);
        struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);
        uint16_t ind_cfg = co_read16p(&param->value[0]);

        msg_status = KE_MSG_CONSUMED;

        if ((param->handle == (bcss_env->shdl + BCS_IDX_MEAS_IND_CFG))
            && ((ind_cfg == PRF_CLI_STOP_NTFIND) || (ind_cfg == PRF_CLI_START_IND)))
        {
            bcss_env->ind_cfg[conidx] = ind_cfg;
            status = GAP_ERR_NO_ERROR;
        }
        else
        {
            status = PRF_APP_ERROR;
        }

        if (status == GAP_ERR_NO_ERROR)
        {
            // Inform APP of configuration change
            struct bcss_meas_val_ind_cfg_ind *ind = KE_MSG_ALLOC(BCSS_MEAS_VAL_IND_CFG_IND,
                                                                 prf_dst_task_get(&bcss_env->prf_env, conidx),
                                                                 prf_src_task_get(&bcss_env->prf_env, conidx),
                                                                 bcss_meas_val_ind_cfg_ind);
            if (ind == NULL)
                return msg_status;

            ind->ind_cfg = ind_cfg;

            ke_msg_send(ind);
        }

        // Allocate write confirmation message.
        cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
        cfm->handle = param->handle;
        cfm->status = status;
        ke_msg_send(cfm);
    }

    return msg_status;
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
    int msg_status = KE_MSG_SAVED;

    if (ke_state_get(dest_id) != BCSS_BUSY)
    {
        struct gattc_read_cfm* cfm = NULL;

        struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);
        uint8_t conidx = KE_IDX_GET(src_id);
        uint8_t status = ATT_ERR_NO_ERROR;

        msg_status = KE_MSG_CONSUMED;

        if (param->handle == bcss_env->shdl + BCS_IDX_FEAT_VAL)
        {
            cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(uint32_t));
            cfm->length = sizeof(uint32_t);

            co_write32p(cfm->value, bcss_env->bc_feat);
        }
        else if (param->handle == bcss_env->shdl + BCS_IDX_MEAS_IND_CFG)
        {
            cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, sizeof(uint16_t));
            cfm->length = sizeof(uint16_t);

            co_write16p(cfm->value, bcss_env->ind_cfg[conidx]);
        }
        else
        {
            status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }

        if (status != ATT_ERR_NO_ERROR)
        {
            cfm = KE_MSG_ALLOC(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm);
            cfm->length = 0;
        }

        cfm->handle = param->handle;
        cfm->status = status;
        ke_msg_send(cfm);
    }

    return msg_status;
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
    if (ke_state_get(dest_id) == BCSS_BUSY)
    {
        struct bcss_env_tag *bcss_env = PRF_ENV_GET(BCSS, bcss);
        uint8_t conidx = KE_IDX_GET(src_id);

        switch (param->operation)
        {
            case GATTC_NOTIFY:
            case GATTC_INDICATE:
                // Continue indication
                if (bcss_env->ind_cont_feat)
                {
                    bcss_indicate(conidx, bcss_env->ind_cont_feat, bcss_env->meas);
                }
                else
                {
                    if (bcss_env->meas)
                    {
                        ke_free(bcss_env->meas);
                        bcss_env->meas = NULL;
                    }
                    bcss_ind_rsp_send(conidx ,param->status);
                }
            default:
                break;
        }
    }

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler bcss_default_state[] =
{
    {BCSS_ENABLE_REQ,       (ke_msg_func_t)bcss_enable_req_handler},
    {BCSS_MEAS_VAL_IND_REQ, (ke_msg_func_t)bcss_meas_val_ind_req_handler},
    {GATTC_WRITE_REQ_IND,   (ke_msg_func_t)gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,    (ke_msg_func_t)gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,         (ke_msg_func_t)gattc_cmp_evt_handler},

};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler bcss_default_handler = KE_STATE_HANDLER(bcss_default_state);

#endif // BLE_BCS_SERVER

/// @} BCSSTASK
