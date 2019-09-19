/**
 ****************************************************************************************
 *
 * @file bmsc_task.c
 *
 * @brief C file - Bond Management Service Client Role Task Implementation.
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
 * @addtogroup BMSCTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_BMS_CLIENT)
#include <string.h>

#include "gap.h"
#include "bmsc.h"
#include "bmsc_task.h"
#include "gattc_task.h"
#include "prf_utils.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/// Used to retrieve Bond Management service characteristics information
const struct prf_char_def bmsc_bms_char[BMSC_CHAR_MAX] =
{
    /// Control point
    [BMSC_CHAR_CP]              = {ATT_CHAR_BM_CNTL_PT,
                                    ATT_MANDATORY,
                                    ATT_CHAR_PROP_WR},

    /// Feature
    [BMSC_CHAR_FEATURE]         = {ATT_CHAR_BM_FEAT,
                                    ATT_MANDATORY,
                                    ATT_CHAR_PROP_RD},
};

/// Used to retrieve Bond Management service characteristics descriptors information
const struct prf_char_desc_def bmsc_bms_char_desc[BMSC_DESC_MAX] =
{
    /// Reliable Write
    [BMSC_DESC_RELIABLE_WRITE]  = {ATT_DESC_CHAR_EXT_PROPERTIES,
                                    ATT_OPTIONAL,
                                    BMSC_CHAR_CP},
};

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_SDP_SVC_IND_HANDLER message.
 * The handler stores the found service details for service discovery.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_sdp_svc_ind_handler(ke_msg_id_t const msgid,
                                             struct gattc_sdp_svc_ind const *ind,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if(state == BMSC_DISCOVERING)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);

        struct bmsc_env_tag *bmsc_env = PRF_ENV_GET(BMSC, bmsc);

        ASSERT_INFO(bmsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(bmsc_env->env[conidx] != NULL, dest_id, src_id);

        if(bmsc_env->env[conidx]->nb_svc == 0)
        {
            // Retrieve BMS characteristics and descriptors
            prf_extract_svc_info(ind, BMSC_CHAR_MAX, &bmsc_bms_char[0], &bmsc_env->env[conidx]->bms.chars[0],
                                      BMSC_DESC_MAX, &bmsc_bms_char_desc[0], &bmsc_env->env[conidx]->bms.descs[0]);

            // Even if we get multiple responses we only store 1-st range
            bmsc_env->env[conidx]->bms.svc.shdl = ind->start_hdl;
            bmsc_env->env[conidx]->bms.svc.ehdl = ind->end_hdl;
        }

        bmsc_env->env[conidx]->nb_svc++;
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BMSC_ENABLE_REQ message.
 * The handler enables the Bond Management Profile Client Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bmsc_enable_req_handler(ke_msg_id_t const msgid,
                                   struct bmsc_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    // Status
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);

    uint8_t state = ke_state_get(dest_id);
    // Bond Management Profile Client Role Task Environment
    struct bmsc_env_tag *bmsc_env = PRF_ENV_GET(BMSC, bmsc);

    ASSERT_INFO(hrpc_env != NULL, dest_id, src_id);
    // If in IDLE state
    if((state == BMSC_IDLE) && (bmsc_env->env[conidx] == NULL))
    {
        // Allocate environment variable for task instance
        bmsc_env->env[conidx] = (struct bmsc_cnx_env*) ke_malloc(sizeof(struct bmsc_cnx_env), KE_MEM_ENV);
        memset(bmsc_env->env[conidx], 0, sizeof(struct bmsc_cnx_env));

        // Config connection, start discovering
        if(param->con_type == PRF_CON_DISCOVERY)
        {
            // Start discovering BMS on peer
            prf_disc_svc_send(&(bmsc_env->prf_env), conidx, ATT_SVC_BOND_MANAGEMENT);

            bmsc_env->env[conidx]->last_uuid_req = ATT_SVC_BOND_MANAGEMENT;

            // Go to DISCOVERING state
            ke_state_set(dest_id, BMSC_DISCOVERING);
        }
        // Normal connection, get saved att details
        else
        {
            bmsc_env->env[conidx]->bms = param->bms;
            // Send APP confirmation and stay in IDLE state
            bmsc_enable_rsp_send(bmsc_env, conidx, GAP_ERR_NO_ERROR);
        }

        status = GAP_ERR_NO_ERROR;
    }

    // Send an error if request fails
    if(status != GAP_ERR_NO_ERROR)
    {
        bmsc_enable_rsp_send(bmsc_env, conidx, status);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BMSC_WR_CNTL_POINT_REQ message.
 * When receiving this request, Bond Management client sends an operation code to
 * Bond Management server.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bmsc_wr_cntl_point_req_handler(ke_msg_id_t const msgid,
                                          struct bmsc_wr_cntl_point_req const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct bmsc_env_tag *bmsc_env = PRF_ENV_GET(BMSC, bmsc);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = GAP_ERR_NO_ERROR;

    // Message status
    uint8_t msg_status = KE_MSG_CONSUMED;
    uint8_t state = ke_state_get(dest_id);

    if(state == BMSC_IDLE)
    {
        // This is mandatory readable if it is included in the peer's DB
        if(bmsc_env->env[conidx] != NULL)
        {
            // This is mandatory readable if it is included in the peer's DB
            if (bmsc_env->env[conidx]->bms.chars[BMSC_CHAR_CP].char_hdl != ATT_INVALID_SEARCH_HANDLE)
            {
                if ((bmsc_env->env[conidx]->bms.chars[BMSC_CHAR_CP].prop & ATT_CHAR_PROP_WR) == ATT_CHAR_PROP_WR)
                {
                    bmsc_env->env[conidx]->last_char_code = BMSC_CHAR_CP;

                    uint8_t value[param->operation.operand_length + 1];
                    value[0] = param->operation.op_code;
                    memcpy(&value[1], &(param->operation.operand), param->operation.operand_length);

                    // Send GATT Write Request
                    prf_gatt_write(&bmsc_env->prf_env, conidx, bmsc_env->env[conidx]->bms.chars[BMSC_CHAR_CP].val_hdl,
                            (uint8_t *)value, param->operation.operand_length + 1, GATTC_WRITE);
                    // Go to the Busy state
                    ke_state_set(dest_id, BMSC_BUSY);
                }
                // Write not allowed, so no point in continuing
                else
                {
                    status = PRF_ERR_NOT_WRITABLE;
                }
            }
            // Send app error indication for inexistent handle for this characteristic
            else
            {
                status = PRF_ERR_INEXISTENT_HDL;
            }
        }
        // Send app profile error indication
        else
        {
            status = PRF_APP_ERROR;
        }
    }
    else if (state == BMSC_FREE)
    {
        status = GAP_ERR_DISCONNECTED;
    }
    else
    {
        // Another procedure is pending, keep the command for later
        msg_status = KE_MSG_SAVED;
        status = GAP_ERR_NO_ERROR;
    }

    if(status != GAP_ERR_NO_ERROR)
    {
        struct bmsc_wr_cntl_point_rsp *rsp = KE_MSG_ALLOC(BMSC_WR_CNTL_POINT_RSP, src_id, dest_id, bmsc_wr_cntl_point_rsp);
        rsp->status    = status;
        // Send the message
        ke_msg_send(rsp);
    }

    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BMSC_RD_FEATURES_REQ message.
 * Send by application task, it's used to retrieve Bond Management Service features.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bmsc_rd_features_req_handler(ke_msg_id_t const msgid,
                                          void const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Get the address of the environment
    struct bmsc_env_tag *bmsc_env = PRF_ENV_GET(BMSC, bmsc);

    if(state == BMSC_IDLE)
    {
        ASSERT_INFO(bmsc_env != NULL, dest_id, src_id);

        // Environment variable not ready
        if(bmsc_env->env[conidx] == NULL)
        {
            status = PRF_APP_ERROR;
        }
        else
        {
            status = bmsc_validate_request(bmsc_env, conidx, BMSC_CHAR_FEATURE);

            // Request can be performed
            if(status == GAP_ERR_NO_ERROR)
            {
                // Read Bond Management server feature
                prf_read_char_send(&(bmsc_env->prf_env), conidx,
                                     bmsc_env->env[conidx]->bms.svc.shdl,
                                     bmsc_env->env[conidx]->bms.svc.ehdl,
                                     bmsc_env->env[conidx]->bms.chars[BMSC_CHAR_FEATURE].val_hdl);

                ke_state_set(dest_id, BMSC_BUSY);
            }
        }
    }

            // request cannot be performed
    if(status != GAP_ERR_NO_ERROR)
    {
        struct bmsc_rd_features_rsp *rsp = KE_MSG_ALLOC(BMSC_RD_FEATURES_RSP,
                                                        prf_dst_task_get(&(bmsc_env->prf_env), conidx),
                                                        prf_src_task_get(&(bmsc_env->prf_env), conidx),
                                                        bmsc_rd_features_rsp);
        // Set error status
        rsp->status = status;
        ke_msg_send(rsp);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_CMP_EVT message.
 * This generic event is received for different requests, so need to keep track.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                                struct gattc_cmp_evt const *param,
                                                ke_task_id_t const dest_id,
                                                ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    struct bmsc_env_tag *bmsc_env = PRF_ENV_GET(BMSC, bmsc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = param->status;

    ASSERT_INFO(bmsc_env != NULL, dest_id, src_id);
    ASSERT_INFO(bmsc_env->env[conidx] != NULL, dest_id, src_id);

    if (state == BMSC_DISCOVERING)
    {

        if ((param->status == ATT_ERR_ATTRIBUTE_NOT_FOUND) || (param->status == ATT_ERR_NO_ERROR))
        {
            if(bmsc_env->env[conidx]->nb_svc == 1)
            {
                status = prf_check_svc_char_validity(BMSC_CHAR_MAX,
                                                     bmsc_env->env[conidx]->bms.chars,
                                                     bmsc_bms_char);
            }
            // Too many services
            else if (bmsc_env->env[conidx]->nb_svc > 1)
            {
                status = PRF_ERR_MULTIPLE_SVC;
            }
            // No services found
            else
            {
                status = PRF_ERR_STOP_DISC_CHAR_MISSING;
            }

            // If Characteristics are fine, check descriptor validity
            if (status == GAP_ERR_NO_ERROR)
            {
                status = prf_check_svc_char_desc_validity(BMSC_DESC_MAX,
                                                          bmsc_env->env[conidx]->bms.descs,
                                                          bmsc_bms_char_desc,
                                                          bmsc_env->env[conidx]->bms.chars);
            }
        }
        // Send response and go to IDLE state
        bmsc_enable_rsp_send(bmsc_env, conidx, status);

        ke_state_set(dest_id, BMSC_IDLE);
    }
    else if (state == BMSC_BUSY)
    {
        switch (param->operation)
        {
            case GATTC_WRITE:
            case GATTC_WRITE_NO_RESPONSE:
            {
                struct bmsc_wr_cntl_point_rsp *rsp = KE_MSG_ALLOC(BMSC_WR_CNTL_POINT_RSP,
                                            prf_dst_task_get(&(bmsc_env->prf_env), conidx),
                                            prf_src_task_get(&(bmsc_env->prf_env), conidx),
                                            bmsc_wr_cntl_point_rsp);

                rsp->status = param->status;
                ke_msg_send(rsp);
            } break;
            case GATTC_READ:
            {
                if(param->status != GAP_ERR_NO_ERROR)
                {
                    struct bmsc_rd_features_rsp *rsp = KE_MSG_ALLOC(BMSC_RD_FEATURES_RSP,
                                                                    prf_dst_task_get(&(bmsc_env->prf_env), conidx),
                                                                    prf_src_task_get(&(bmsc_env->prf_env), conidx),
                                                                    bmsc_rd_features_rsp);
                    // set error status
                    rsp->status = param->status;
                    rsp->length = 0;

                    ke_msg_send(rsp);
                }
            } break;
            default:
                break;
        }
        ke_state_set(dest_id, BMSC_IDLE);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_IND message.
 * Generic event received after every simple read command sent to peer server.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_ind_handler(ke_msg_id_t const msgid,
                                  struct gattc_read_ind const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if (state == BMSC_BUSY)
    {
        struct bmsc_env_tag *bmsc_env = PRF_ENV_GET(BMSC, bmsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        struct bmsc_rd_features_rsp *rsp = KE_MSG_ALLOC_DYN(BMSC_RD_FEATURES_RSP,
                                                            prf_dst_task_get(&(bmsc_env->prf_env), conidx),
                                                            prf_src_task_get(&(bmsc_env->prf_env), conidx),
                                                            bmsc_rd_features_rsp,
                                                            param->length);
        // set error status
        rsp->status = ATT_ERR_NO_ERROR;
        // value length
        rsp->length = param->length;
        // extract features
        memcpy(rsp->features, param->value, param->length);

        ke_msg_send(rsp);
    }

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler bmsc_default_state[] =
{
    {GATTC_SDP_SVC_IND,             (ke_msg_func_t)gattc_sdp_svc_ind_handler},
    {BMSC_ENABLE_REQ,               (ke_msg_func_t)bmsc_enable_req_handler},
    {BMSC_WR_CNTL_POINT_REQ,        (ke_msg_func_t)bmsc_wr_cntl_point_req_handler},
    {BMSC_RD_FEATURES_REQ,          (ke_msg_func_t)bmsc_rd_features_req_handler},
    {GATTC_CMP_EVT,                 (ke_msg_func_t)gattc_cmp_evt_handler},
    {GATTC_READ_IND,                (ke_msg_func_t)gattc_read_ind_handler}
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler bmsc_default_handler = KE_STATE_HANDLER(bmsc_default_state);

#endif // (BLE_BMS_CLIENT)

/// @} BMSCTASK
