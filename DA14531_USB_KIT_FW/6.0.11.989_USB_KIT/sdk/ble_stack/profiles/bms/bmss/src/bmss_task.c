/**
 ****************************************************************************************
 *
 * @file bmss_task.c
 *
 * @brief C file - Bond Management Service Server Role Task Implementation.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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
 * @addtogroup BMSSTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_BMS_SERVER)

#include "gattc_task.h"
#include "bmss_task.h"
#include "atts.h"

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the attribute info request message.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
                                            struct gattc_att_info_req_ind *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    uint8_t state  = ke_state_get(dest_id);

    if(state == BMSS_IDLE)
    {
        struct bmss_env_tag *bmss_env = PRF_ENV_GET(BMSS, bmss);
        struct gattc_att_info_cfm * cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id,
                                                        dest_id,
                                                        gattc_att_info_cfm);
        cfm->handle = param->handle;

        if (param->handle != bmss_env->shdl + BMS_IDX_BM_CNTL_PT_VAL) {
            // force length to zero to reject any write starting from something != 0
            cfm->length = 0;
            cfm->status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        } else {
            cfm->length = 0;
            cfm->status = GAP_ERR_NO_ERROR;
        }

        //Send write response
        ke_msg_send(cfm);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_WRITE_CMD_IND message.
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
    struct bmss_env_tag *bmss_env = PRF_ENV_GET(BMSS, bmss);
    int msg_status = KE_MSG_CONSUMED;
    uint8_t status = ATT_ERR_NO_ERROR;

    if(ke_state_get(dest_id) == BMSS_IDLE)
    {
        struct gattc_write_cfm *cfm = NULL;

        // Check if the handle is invalid
        if ((param->handle != (bmss_env->shdl + BMS_IDX_BM_CNTL_PT_VAL)))
        {
            status = ATT_ERR_INVALID_HANDLE;
        }
        // Check if the length is invalid //// the check is already in attmdb_att_set_value??????
        else if (param->offset + param->length > BMS_CNTL_PT_MAX_LEN)
        {
            status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
        }
        else
        {
            // Update value in database
            status = attmdb_att_set_value(param->handle, param->length, param->offset,
                                                (uint8_t *) param->value);
            if (status == ATT_ERR_NO_ERROR)
            {
                switch (param->value[0])
                {
                    case BMS_OPCODE_DEL_BOND:
                        // Check if BMS_FEAT_DEL_BOND feature is supported
                        if (!(bmss_env->features & BMS_FEAT_DEL_BOND_SUPPORTED))
                        {
                            status = BMS_ATT_OPCODE_NOT_SUPPORTED;
                        }
                        // Check if authorization is required and authorization code has not been received
                        else if ((param->length == 1) &&
                                 (!(bmss_env->features & BMS_FEAT_DEL_BOND) &&
                                 (bmss_env->features & BMS_FEAT_DEL_BOND_AUTHZ)))
                        {
                            status = ATT_ERR_INSUFF_AUTHOR;
                        }
                        break;
                    case BMS_OPCODE_DEL_ALL_BOND:
                        // Check if BMS_FEAT_DEL_ALL_BOND feature is supported
                        if (!(bmss_env->features & BMS_FEAT_DEL_ALL_BOND_SUPPORTED))
                        {
                            status = BMS_ATT_OPCODE_NOT_SUPPORTED;
                        }
                        // Check if authorization is required and authorization code has not been received
                        else if ((param->length == 1)  &&
                                 (!(bmss_env->features & BMS_FEAT_DEL_ALL_BOND) &&
                                 (bmss_env->features & BMS_FEAT_DEL_ALL_BOND_AUTHZ)))
                        {
                            status = ATT_ERR_INSUFF_AUTHOR;
                        }
                        break;
                    case BMS_OPCODE_DEL_ALL_BOND_BUT_PEER:
                        // Check if BMS_FEAT_DEL_ALL_BOND_BUT_PEER feature is supported
                        if (!(bmss_env->features & BMS_FEAT_DEL_ALL_BOND_BUT_PEER_SUPPORTED))
                        {
                            status = BMS_ATT_OPCODE_NOT_SUPPORTED;
                        }
                        // Check if authorization is required and authorization code has not been received
                        else if ((param->length == 1)  &&
                                 (!(bmss_env->features & BMS_FEAT_DEL_ALL_BOND_BUT_PEER) &&
                                 (bmss_env->features & BMS_FEAT_DEL_ALL_BOND_BUT_PEER_AUTHZ)))
                        {
                            status = ATT_ERR_INSUFF_AUTHOR;
                        }
                        break;
                    default:
                        status = BMS_ATT_OPCODE_NOT_SUPPORTED;
                        break;
                }
            }

            if (status == ATT_ERR_NO_ERROR)
            {
            // Send request to the app
                uint8_t conidx = KE_IDX_GET(src_id);
                struct bmss_del_bond_req_ind *ind = KE_MSG_ALLOC_DYN(BMSS_DEL_BOND_REQ_IND,
                                                                     prf_dst_task_get(&(bmss_env->prf_env), conidx),
                                                                     prf_src_task_get(&(bmss_env->prf_env), conidx),
                                                                     bmss_del_bond_req_ind,
                                                                     param->length - 1);

                // Extract OpCode
                ind->operation.op_code = param->value[0];
                ind->operation.operand_length = param->length - 1;

                // Extract Authorization Code
                if (ind->operation.operand_length)
                {
                    memcpy(&(ind->operation.operand), &(param->value[1]),
                            ind->operation.operand_length);
                }

                ke_state_set(dest_id, BMSS_BUSY);
                ke_msg_send(ind);

                return (KE_MSG_CONSUMED);
            }
            else
            {

                // Allocate write confirmation message.
                cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);

                cfm->handle = param->handle;
                cfm->status = status;
                ke_msg_send(cfm);
            }
        }
    }
    else if(ke_state_get(dest_id) == BMSS_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BMSS_DEL_BOND_CFM message.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bmss_del_bond_cfm_handler(ke_msg_id_t const msgid,
                                     struct bmss_del_bond_cfm const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    struct bmss_env_tag *bmss_env = PRF_ENV_GET(BMSS, bmss);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = param->status;

    struct gattc_write_cfm *cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, \
                                                KE_BUILD_ID(TASK_GATTC, conidx), \
                                                dest_id, \
                                                gattc_write_cfm);

    // Accept only success or certain error codes from the spec.
    if ((status != ATT_ERR_NO_ERROR) && (status != ATT_ERR_INSUFF_AUTHOR))
        status = BMS_ATT_OPERATION_FAILED;

    // Confirm previous write
    cfm->handle = bmss_env->shdl + BMS_IDX_BM_CNTL_PT_VAL;
    cfm->status = status;
    ke_msg_send(cfm);

    ke_state_set(dest_id, BMSS_IDLE);

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

 /// Default State handlers definition
const struct ke_msg_handler bmss_default_state[] =
{
    {GATTC_ATT_INFO_REQ_IND,    (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,       (ke_msg_func_t) gattc_write_req_ind_handler},
};

const struct ke_msg_handler bmss_busy_state[] =
{
    {BMSS_DEL_BOND_CFM,         (ke_msg_func_t) bmss_del_bond_cfm_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler bmss_default_handler = KE_STATE_HANDLER(bmss_default_state);

/// Specifies state specific message handlers.
const struct ke_state_handler bmss_state_handler[BMSS_STATE_MAX] = {
    [BMSS_IDLE] = KE_STATE_HANDLER_NONE,
    [BMSS_BUSY] = KE_STATE_HANDLER(bmss_busy_state),
};

#endif // BLE_BMS_SERVER

/// @} BMSSTASK
