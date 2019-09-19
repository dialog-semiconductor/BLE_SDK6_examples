/**
 ****************************************************************************************
 *
 * @file udss_task.c
 *
 * @brief C file - User Data Service Server Role Task Implementation.
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
 * @addtogroup UDSSTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_UDS_SERVER)

#include "gattc_task.h"
#include "udss_task.h"
#include "atts.h"

extern const struct attm_desc uds_att_db[UDS_IDX_NB];

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSS_DB_UPDATED_NTF_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udss_db_updated_ntf_req_handler(ke_msg_id_t const msgid,
                                           struct udss_db_updated_ntf_req const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
    uint8_t conidx = KE_IDX_GET(dest_id);

    ASSERT_ERROR(udss_env->att_tbl[UDS_USER_DB_CHANGE_INCR_CHAR_CODE]);

    // Notify peer that characteristic values were updated by Host
    if (udss_env->db_change_ccc_val[conidx] == GATT_CCC_START_NTF)
    {
        struct gattc_send_evt_cmd *ntf = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                                                          KE_BUILD_ID(TASK_GATTC, conidx),
                                                          prf_src_task_get(&(udss_env->prf_env), conidx),
                                                          gattc_send_evt_cmd, sizeof(uint32_t));

        ntf->operation = GATTC_NOTIFY;
        ntf->handle = udss_env->att_tbl[UDS_USER_DB_CHANGE_INCR_CHAR_CODE];

        co_write32(ntf->value, param->db_change_incr_val);
        ntf->length = sizeof(uint32_t);

        ke_msg_send(ntf);
    }

    return (KE_MSG_CONSUMED);
}

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

    if(state == UDSS_IDLE)
    {
        struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
        struct gattc_att_info_cfm * cfm = KE_MSG_ALLOC(GATTC_ATT_INFO_CFM, src_id,
                                                       dest_id, gattc_att_info_cfm);
        cfm->handle = param->handle;

        if ((param->handle == udss_env->att_tbl[UDS_IDX_USER_FIRST_NAME_VAL]) || \
            (param->handle == udss_env->att_tbl[UDS_IDX_USER_LAST_NAME_VAL]) || \
            (param->handle == udss_env->att_tbl[UDS_IDX_USER_EMAIL_ADDRESS_VAL]))
        {
            // Force length to zero to reject any write starting from something != 0
            cfm->length = 0;
            cfm->status = GAP_ERR_NO_ERROR;
        }
        else if ((param->handle == udss_env->att_tbl[UDS_IDX_USER_DB_CHANGE_INCR_CFG]) || \
                 (param->handle == udss_env->att_tbl[UDS_IDX_USER_CNTL_POINT_CFG]))
        {
            // Descriptor length is 2
            cfm->length = 2;
            cfm->status = GAP_ERR_NO_ERROR;
        }
        else
        {
            // Write Long not supported
            cfm->length = 0;
            cfm->status = ATT_ERR_REQUEST_NOT_SUPPORTED;
        }

        // Send write response
        ke_msg_send(cfm);
    }
    else
    {
        return (KE_MSG_SAVED);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the control point write request.
 *
 * @param[in] udss_env      Service environment variable.
 * @param[in] cfm           Write confirmation message to be filled with status and send.
 * @param[in] conidx        Connection Index
 * @param[in] param         Write request parameters.
 * @return void.
 ****************************************************************************************
 */
static void gattc_write_cntl_point_ind_handler(struct udss_env_tag* udss_env,
                                               struct gattc_write_cfm *cfm,
                                               uint8_t conidx,
                                               struct gattc_write_req_ind const *param)
{
    if (ke_state_get(prf_src_task_get(&(udss_env->prf_env), conidx)) == UDSS_UCP_OP_ONGOING)
    {
        // Return error when UCP operation is ongoing
        cfm->status = UDS_ERROR_PROC_IN_PROGRESS;
        ke_msg_send(cfm);
    }
    else
    {
        uint8_t reqstatus;
        struct uds_ucp_req ucp_req;
        struct udss_ucp_req_ind *req;

        if (udss_env->ucp_ccc_val[conidx] != PRF_CLI_START_IND)
        {
            cfm->status = UDS_ERROR_IMPROPERLY_CONFIGURED;
            ke_msg_send(cfm);
        }
        else
        {
            // Send write response before sending any ucp indications
            cfm->status = GAP_ERR_NO_ERROR;
            ke_msg_send(cfm);

            // UCP request can be handled now
            reqstatus = udss_unpack_ucp_req(param->value, param->length, &ucp_req);
            if (reqstatus != GAP_ERR_NO_ERROR)
            {
                struct uds_ucp_rsp ucp_rsp;

                ucp_rsp.op_code = UDS_REQ_RSP_CODE;
                ucp_rsp.req_op_code = ucp_req.op_code;
                ucp_rsp.rsp_val = reqstatus;

                // Send user control response indication about the invalid request
                udss_send_ucp_rsp(&ucp_rsp, conidx);

                // Switch to UCP operation ongoing state
                ke_state_set(prf_src_task_get(&(udss_env->prf_env), conidx),
                             UDSS_UCP_OP_ONGOING);
            }
            else
            {
                // Forward request operation to Host
                req = KE_MSG_ALLOC(UDSS_UCP_REQ_IND,
                                   prf_dst_task_get(&(udss_env->prf_env), conidx),
                                   prf_src_task_get(&(udss_env->prf_env), conidx),
                                   udss_ucp_req_ind);
                req->ucp_req = ucp_req;

                // Switch to UCP operation ongoing state
                ke_state_set(prf_src_task_get(&(udss_env->prf_env), conidx),
                             UDSS_UCP_OP_ONGOING);

                ke_msg_send(req);
            }
        }
    }
}

/**
 ****************************************************************************************
 * @brief Handles reception of characteristic write request.
 *
 * @param[in] udss_env      Service environment variable.
 * @param[in] conidx        Connection Index
 * @param[in] param         Write request parameters.
 * @return void.
 ****************************************************************************************
 */
static void gattc_write_char_ind_handler(struct udss_env_tag* udss_env,
                                         uint8_t conidx,
                                         struct gattc_write_req_ind const *param)
{
    struct udss_char_val *ind;
    uint8_t i;

    // Find charcode by handle
    for (i = UDS_IDX_SVC; i < UDS_IDX_NB; i++)
    {
        if (udss_env->att_tbl[i] == param->handle)
        {
            break;
        }
    }

    ASSERT_ERROR(i != (UDS_IDX_NB - 1));

    // Notify Host about the new value
    ind = KE_MSG_ALLOC_DYN(UDSS_SET_CHAR_VAL_IND,
                           prf_dst_task_get(&(udss_env->prf_env), conidx),
                           prf_src_task_get(&(udss_env->prf_env), conidx),
                           udss_char_val, param->length);
    ind->char_code = i;
    ind->length = param->length;
    memcpy(ind->value, param->value, param->length);

    ke_state_set(prf_src_task_get(&(udss_env->prf_env), conidx), UDSS_BUSY);
    ke_msg_send(ind);
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
    struct gattc_write_cfm *cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id,
                                               gattc_write_cfm);
    struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
    uint8_t conidx = KE_IDX_GET(src_id);

    cfm->handle = param->handle;

    // Only CCC descriptors and user index require no user consent on write.
    if (!udss_env->consent[conidx] &&
        (param->handle < udss_env->att_tbl[UDS_IDX_USER_DB_CHANGE_INCR_CFG]))
    {
        cfm->status = UDS_ERROR_DATA_ACCESS_NOT_PERMITTED;

        ke_msg_send(cfm);
    }
    else
    {
        if (param->handle == udss_env->att_tbl[UDS_IDX_USER_CNTL_POINT_VAL])
        {
            // Handle control point operations
            gattc_write_cntl_point_ind_handler(udss_env, cfm, conidx, param);
        }
        else if (param->handle <= udss_env->att_tbl[UDS_IDX_USER_DB_CHANGE_INCR_VAL])
        {
            // Confirm when app layer confirms
            KE_MSG_FREE(cfm);

            if(ke_state_get(dest_id) == UDSS_IDLE)
            {
                // Handle user characteristic value writes
                gattc_write_char_ind_handler(udss_env, conidx, param);
            }
            else
            {
                // Postpone until ongoing operation is done
                return KE_MSG_SAVED;
            }
        } // Handle CCC value writes
        else if ((param->handle == udss_env->att_tbl[UDS_IDX_USER_DB_CHANGE_INCR_CFG]) || \
                 (param->handle == udss_env->att_tbl[UDS_IDX_USER_CNTL_POINT_CFG]))
        {
            // Store CCC values for each connection
            if (param->handle == udss_env->att_tbl[UDS_IDX_USER_DB_CHANGE_INCR_CFG])
            {
                memcpy(&udss_env->db_change_ccc_val[conidx], param->value, sizeof(uint16_t));
            }
            else
            {
                memcpy(&udss_env->ucp_ccc_val[conidx], param->value, sizeof(uint16_t));
            }

            cfm->status = ATT_ERR_NO_ERROR;

            ke_msg_send(cfm);
        }
        else
        {
            cfm->status = ATT_ERR_WRITE_NOT_PERMITTED;

            ke_msg_send(cfm);
        }
    }

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the read request from peer device
 *
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
    struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
    struct udss_char_val_req *req;
    uint8_t conidx = KE_IDX_GET(src_id);

    if(ke_state_get(dest_id) != UDSS_IDLE)
    {
        return (KE_MSG_SAVED);
    }

    if (!udss_env->consent[conidx] &&
        (param->handle < udss_env->att_tbl[UDS_IDX_USER_DB_CHANGE_INCR_VAL]))
    {
        struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id,
                                                      gattc_read_cfm, 0);

        cfm->status = UDS_ERROR_DATA_ACCESS_NOT_PERMITTED;
        cfm->handle = param->handle;
        cfm->length = 0;

        ke_msg_send(cfm);
        return (KE_MSG_CONSUMED);
    }
    else if ((param->handle == udss_env->att_tbl[UDS_IDX_USER_DB_CHANGE_INCR_CFG]) || \
             (param->handle == udss_env->att_tbl[UDS_IDX_USER_CNTL_POINT_CFG]))
    {
        // Handle descriptor reads
        struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id,
                                                      gattc_read_cfm, sizeof(uint16_t));

        if (param->handle == udss_env->att_tbl[UDS_IDX_USER_DB_CHANGE_INCR_CFG])
        {
            memcpy(cfm->value, &udss_env->db_change_ccc_val[conidx], sizeof(uint16_t));
        }
        else
        {
            memcpy(cfm->value, &udss_env->ucp_ccc_val[conidx], sizeof(uint16_t));
        }

        cfm->length = sizeof(uint16_t);
        cfm->handle = param->handle;
        cfm->status = ATT_ERR_NO_ERROR;

        // Send value to peer device.
        ke_msg_send(cfm);
    }
    else
    {
        // Ask Host only if consent was already given
        if ((param->handle == udss_env->att_tbl[UDS_IDX_USER_INDEX_VAL]) && \
            (!udss_env->consent[conidx]))
        {
            struct gattc_read_cfm* cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id,
                                                          gattc_read_cfm,
                                                          sizeof(uint8_t));

            co_write8(cfm->value, 0xFF);
            cfm->length = sizeof(uint8_t);
            cfm->handle = param->handle;
            cfm->status = ATT_ERR_NO_ERROR;

            // Send value to peer device.
            ke_msg_send(cfm);
        }
        else
        {
            // Handle characteristic reads
            req = KE_MSG_ALLOC(UDSS_CHAR_VAL_REQ,
                               prf_dst_task_get(&(udss_env->prf_env), conidx),
                               prf_src_task_get(&(udss_env->prf_env), conidx),
                               udss_char_val_req);

            // Find charcode by handle
            for (req->char_code = UDS_IDX_SVC; req->char_code < UDS_IDX_NB; req->char_code++)
            {
                if (udss_env->att_tbl[req->char_code] == param->handle)
                {
                    break;
                }
            }

            ke_msg_send(req);
            ke_state_set(dest_id, UDSS_BUSY);
        }
    }

    return (KE_MSG_CONSUMED);
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
    struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
    uint8_t conidx = KE_IDX_GET(src_id);

    if(param->operation == GATTC_INDICATE)
    {
        ke_msg_send_basic(UDSS_UCP_CMP_EVT, prf_dst_task_get(&(udss_env->prf_env), conidx),
                          dest_id);
        ke_state_set(dest_id, UDSS_IDLE);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSS_UCP_REQ_CFM message.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udss_ucp_req_cfm_handler(ke_msg_id_t const msgid,
                                    struct udss_ucp_req_cfm const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == UDSS_UCP_OP_ONGOING)
    {
        struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
        uint8_t conidx = KE_IDX_GET(dest_id);

        udss_send_ucp_rsp(&param->ucp_rsp, conidx);

        if ((param->ucp_rsp.req_op_code == UDS_REQ_CONSENT) || (param->ucp_rsp.req_op_code == UDS_REQ_REG_NEW_USER))
        {
            udss_env->consent[conidx] = param->ucp_rsp.rsp_val == UDS_RSP_SUCCESS ? true : false;
        }
        else if ((param->ucp_rsp.req_op_code == UDS_REQ_DEL_USER_DATA))
        {
            if (udss_env->consent[conidx])
            {
                udss_env->consent[conidx] = param->ucp_rsp.rsp_val == UDS_RSP_SUCCESS ? false : true;
            }
        }
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSS_CHAR_VAL_RSP message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udss_char_val_rsp_handler(ke_msg_id_t const msgid,
                                     struct udss_char_val const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == UDSS_BUSY)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
        struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
        struct gattc_read_cfm* cfm;
        uint16_t length = param->length;

        if (length != 0)
        {
            // Provide default values required by the spec. when user sends empty data
            cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, KE_BUILD_ID(TASK_GATTC, conidx),
                                                               prf_src_task_get(&(udss_env->prf_env), conidx),
                                                               gattc_read_cfm, length);
            memcpy(cfm->value, param->value, length);
        }
        else
        {
            uint8_t default_gender = UDS_GENDER_UNSPECIFIED;

            if (param->char_code == UDS_USER_GENDER_CHAR_CODE)
            {
                length = sizeof(default_gender);

                cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, KE_BUILD_ID(TASK_GATTC, conidx),
                                       prf_src_task_get(&(udss_env->prf_env), conidx),
                                       gattc_read_cfm, length);
                memcpy(cfm->value, &default_gender, length);
            }
            else if (param->char_code <= UDS_USER_EMAIL_ADDRESS_CHAR_CODE)
            {
                // First Name, Last Name and Email address can be an empty string
                cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, KE_BUILD_ID(TASK_GATTC, conidx),
                                       prf_src_task_get(&(udss_env->prf_env), conidx),
                                       gattc_read_cfm, 0);
            }
            else
            {
                // For the rest of characteristics provide at least the right amount of empty bytes
                length = uds_att_db[param->char_code].max_size & (~(0x01 << 15));   // Mask-out the read indication bit
                cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, KE_BUILD_ID(TASK_GATTC, conidx),
                                       prf_src_task_get(&(udss_env->prf_env), conidx),
                                       gattc_read_cfm, length);
                memset(cfm->value, 0, length);
            }
        }

        cfm->length = length;
        cfm->handle = udss_env->att_tbl[param->char_code];
        cfm->status = ATT_ERR_NO_ERROR;

        // Send value to peer device and go to IDLE state
        ke_msg_send(cfm);
        ke_state_set(dest_id, UDSS_IDLE);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSS_CHAR_VAL_CFM message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udss_set_char_val_cfm_handler(ke_msg_id_t const msgid,
                                         struct udss_char_val_cfm const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == UDSS_BUSY)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
        struct udss_env_tag *udss_env = PRF_ENV_GET(UDSS, udss);
        struct gattc_write_cfm *cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM,
                                                   KE_BUILD_ID(TASK_GATTC, conidx), dest_id,
                                                   gattc_write_cfm);

        // Go back to IDLE to handle other characteristic writes
        ke_state_set(dest_id, UDSS_IDLE);

        // Get handle by charcode
        cfm->handle = udss_env->att_tbl[param->char_code];
        cfm->status = GAP_ERR_NO_ERROR;

        ke_msg_send(cfm);
    }

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler udss_default_state[] =
{
    {UDSS_DB_UPDATED_NTF_REQ,   (ke_msg_func_t) udss_db_updated_ntf_req_handler},
    {GATTC_ATT_INFO_REQ_IND,    (ke_msg_func_t) gattc_att_info_req_ind_handler},
    {GATTC_WRITE_REQ_IND,       (ke_msg_func_t) gattc_write_req_ind_handler},
    {GATTC_READ_REQ_IND,        (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_CMP_EVT,             (ke_msg_func_t) gattc_cmp_evt_handler},
    {UDSS_UCP_REQ_CFM,          (ke_msg_func_t) udss_ucp_req_cfm_handler},
    {UDSS_CHAR_VAL_RSP,         (ke_msg_func_t) udss_char_val_rsp_handler},
    {UDSS_SET_CHAR_VAL_CFM,     (ke_msg_func_t) udss_set_char_val_cfm_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler udss_default_handler = KE_STATE_HANDLER(udss_default_state);

#endif // BLE_UDS_SERVER

/// @} UDSSTASK
