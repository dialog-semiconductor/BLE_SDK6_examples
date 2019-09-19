/**
 ****************************************************************************************
 *
 * @file bcsc_task.c
 *
 * @brief Header file - Body Composition Service Client Task Implementation.
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
 * @addtogroup BCSCTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_BCS_CLIENT)
#include <string.h>

#include "gap.h"
#include "bcsc.h"
#include "bcsc_task.h"
#include "gattc_task.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define BCS_FEATURE_MASK     0x0003FFFF

const struct prf_char_def bcsc_bcs_char[BCSC_CHAR_MAX] =
{
    /// Body Composition Feature
    [BCSC_CHAR_BC_FEATURE]           = {ATT_CHAR_BC_FEAT,
                                        ATT_MANDATORY,
                                        ATT_CHAR_PROP_RD},
    /// Body Composition Measurement
    [BCSC_CHAR_BC_MEAS]              = {ATT_CHAR_BC_MEAS,
                                        ATT_MANDATORY,
                                        ATT_CHAR_PROP_IND},
};

const struct prf_char_desc_def bcsc_bcs_char_desc[BCSC_DESC_MAX] =
{
    /// Body Composition Measurement client config
    [BCSC_DESC_MEAS_CLI_CFG] = {ATT_DESC_CLIENT_CHAR_CFG,
                                ATT_MANDATORY,
                                BCSC_CHAR_BC_MEAS},
};

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BCSC_ENABLE_REQ message.
 * The handler enables the Weight Scale Service Client Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bcsc_enable_req_handler(ke_msg_id_t const msgid,
                                   struct bcsc_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == BCSC_IDLE)
    {
        struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t state = ke_state_get(dest_id);

        ASSERT_INFO(bcsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(bcsc_env->env[conidx] != NULL, dest_id, src_id);

        bcsc_env->env[conidx] = (struct bcsc_cnx_env*) ke_malloc(sizeof(struct bcsc_cnx_env), KE_MEM_ENV);
        memset(bcsc_env->env[conidx], 0, sizeof(struct bcsc_cnx_env));

        // Config connection, start discovering
        if (param->con_type == PRF_CON_DISCOVERY)
        {
            if (param->bcs.is_secondary)
            {
                struct prf_svc svc = param->bcs.svc;
                if (svc.shdl == 0)
                    svc.shdl = 1;
                if (svc.ehdl <= svc.shdl)
                    svc.ehdl = 0xFFFF;

                prf_disc_incl_svc_send(&(bcsc_env->prf_env), conidx, &svc);

                bcsc_env->env[conidx]->bcs.is_secondary = true;
             }
            // Start discovering primary service on peer
            else
            {
                prf_disc_svc_send(&(bcsc_env->prf_env), conidx,
                                  ATT_SVC_BODY_COMPOSITION);
            }
            bcsc_env->env[conidx]->last_uuid_req = ATT_SVC_BODY_COMPOSITION;
            // Go to DISCOVERING state
            ke_state_set(dest_id, BCSC_DISCOVERING);
        }
        // Normal connection, get saved att details
        else
        {
            bcsc_env->env[conidx]->bcs = param->bcs;

            // Send APP confirmation and stay in IDLE state
            bcsc_enable_rsp_send(bcsc_env, conidx, GAP_ERR_NO_ERROR);
        }
    }

    return (KE_MSG_CONSUMED);
}

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
    if (ke_state_get(dest_id) == BCSC_DISCOVERING)
    {
        struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(bcsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(bcsc_env->env[conidx] != NULL, dest_id, src_id);

        if(bcsc_env->env[conidx]->nb_svc == 0)
        {
            // Retrieve BCS characteristics and descriptors
            prf_extract_svc_info(ind,
                                BCSC_CHAR_MAX, bcsc_bcs_char, bcsc_env->env[conidx]->bcs.chars,
                                BCSC_DESC_MAX, bcsc_bcs_char_desc, bcsc_env->env[conidx]->bcs.descs);

            // Even if we get multiple responses we only store 1-st range
            bcsc_env->env[conidx]->bcs.svc.shdl = ind->start_hdl;
            bcsc_env->env[conidx]->bcs.svc.ehdl = ind->end_hdl;
        }

        bcsc_env->env[conidx]->nb_svc++;
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_DISC_SVC_INCL_IND message.
 * The handler stores the found service details for service discovery.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_disc_svc_incl_ind_handler(ke_msg_id_t const msgid,
                                           struct gattc_disc_svc_incl_ind const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == BCSC_DISCOVERING)
    {
        // Get the address of the environment
        struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        // If included service search is performed
        if((bcsc_env->env[conidx]->bcs.is_secondary) && (param->uuid_len == ATT_UUID_16_LEN))
        {
            // Even if we get multiple responses we only store 1 range
            if (!memcmp(&param->uuid[0], &bcsc_env->env[conidx]->last_uuid_req, ATT_UUID_16_LEN))
            {
                bcsc_env->env[conidx]->bcs.svc.shdl = param->start_hdl;
                bcsc_env->env[conidx]->bcs.svc.ehdl = param->end_hdl;
                bcsc_env->env[conidx]->nb_svc++;
            }
        }
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_DISC_CHAR_IND message.
 * Characteristics for the currently desired service handle range are obtained and kept.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_disc_char_ind_handler(ke_msg_id_t const msgid,
                                       struct gattc_disc_char_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == BCSC_DISCOVERING)
    {
        // Get the address of the environment
        struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        // Retrieve BCS characteristics
        prf_search_chars(bcsc_env->env[conidx]->bcs.svc.ehdl, BCSC_CHAR_MAX,
                        &bcsc_env->env[conidx]->bcs.chars[0], bcsc_bcs_char,
                        param, &bcsc_env->env[conidx]->last_char_code);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_DISC_CHAR_DESC_IND message.
 * Descriptors for the currently desired characteristic handle range are obtained and kept.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_disc_char_desc_ind_handler(ke_msg_id_t const msgid,
                                            struct gattc_disc_char_desc_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == BCSC_DISCOVERING)
    {
        // Get the address of the environment
        struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        // Retrieve BCS descriptors
        prf_search_descs(BCSC_DESC_MAX, &bcsc_env->env[conidx]->bcs.descs[0],
                        &bcsc_bcs_char_desc[0], param, bcsc_env->env[conidx]->last_char_code);
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

    if (state == BCSC_DISCOVERING)
    {
        struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t status = param->status;

        ASSERT_INFO(bcsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(bcsc_env->env[conidx] != NULL, dest_id, src_id);

        if ((param->status == ATT_ERR_ATTRIBUTE_NOT_FOUND)||
            (param->status == ATT_ERR_NO_ERROR))
        {
            // Check if service handles are not ok
            if (bcsc_env->env[conidx]->last_uuid_req == ATT_SVC_BODY_COMPOSITION)
            {
                if(bcsc_env->env[conidx]->bcs.svc.shdl == ATT_INVALID_HANDLE)
                {
                    // Try looking for include in case no primary service is found
                    if (!bcsc_env->env[conidx]->bcs.is_secondary)
                    {
                        struct prf_svc svc;

                        bcsc_env->env[conidx]->bcs.is_secondary = true;
                        svc.shdl = 0x0001;
                        svc.ehdl = 0xFFFF;

                        prf_disc_incl_svc_send(&(bcsc_env->prf_env), conidx, &svc);
                    }
                    else
                    {
                        // Stop discovery procedure.
                        bcsc_enable_rsp_send(bcsc_env, conidx,
                                            PRF_ERR_STOP_DISC_CHAR_MISSING);
                    }
                }
                // Too many services found only one such service should exist
                else if(bcsc_env->env[conidx]->nb_svc != 1)
                {
                    // Stop discovery procedure.
                    bcsc_enable_rsp_send(bcsc_env, conidx, PRF_ERR_MULTIPLE_SVC);
                }
                else
                {
                    if (param->operation == GATTC_SDP_DISC_SVC)
                    {
                        // Check if complete service was disovered
                        status = prf_check_svc_char_validity(BCSC_CHAR_MAX,
                                                bcsc_env->env[conidx]->bcs.chars,
                                                bcsc_bcs_char);
                        if (status == GAP_ERR_NO_ERROR)
                        {
                            status = prf_check_svc_char_desc_validity(BCSC_DESC_MAX,
                                                        bcsc_env->env[conidx]->bcs.descs,
                                                        bcsc_bcs_char_desc,
                                                        bcsc_env->env[conidx]->bcs.chars);
                        }
                        // Stop the discovery procedure
                        bcsc_enable_rsp_send(bcsc_env, conidx, status);
                    }
                    else
                    {
                        // Included service found, discover all BCS characteristics
                        prf_disc_char_all_send(&(bcsc_env->prf_env), conidx,
                                            &(bcsc_env->env[conidx]->bcs.svc));
                        bcsc_env->env[conidx]->last_uuid_req = ATT_DECL_CHARACTERISTIC;
                    }
                }
            }
            else if (bcsc_env->env[conidx]->last_uuid_req == ATT_DECL_CHARACTERISTIC)
            {
                status = prf_check_svc_char_validity(BCSC_CHAR_MAX, bcsc_env->env[conidx]->bcs.chars,
                                                    bcsc_bcs_char);

                if (status == GAP_ERR_NO_ERROR)
                {
                    bcsc_env->env[conidx]->last_uuid_req = ATT_INVALID_HANDLE;
                    bcsc_env->env[conidx]->last_char_code = bcsc_bcs_char_desc[BCSC_DESC_MEAS_CLI_CFG].char_code;

                    // Discover Body Composition Measurement Char. Descriptor - Mandatory
                    prf_disc_char_desc_send(&(bcsc_env->prf_env), conidx,
                                    &(bcsc_env->env[conidx]->bcs.chars[bcsc_env->env[conidx]->last_char_code]));
                }
                else
                {
                    // Stop the discovery procedure
                    bcsc_enable_rsp_send(bcsc_env, conidx, status);
                }
            }
            else
            {
                status = prf_check_svc_char_desc_validity(BCSC_DESC_MAX,
                                                        bcsc_env->env[conidx]->bcs.descs,
                                                        bcsc_bcs_char_desc,
                                                        bcsc_env->env[conidx]->bcs.chars);

                bcsc_enable_rsp_send(bcsc_env, conidx, status);
            }
        }
        else
        {
            bcsc_enable_rsp_send(bcsc_env, conidx, status);
        }
    }
    else if (state == BCSC_BUSY)
    {
        struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(bcsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(bcsc_env->env[conidx] != NULL, dest_id, src_id);

        switch (param->operation)
        {
            case GATTC_WRITE:
            case GATTC_WRITE_NO_RESPONSE:
            {
                bcsc_bc_meas_cfg_ind_rsp_send(bcsc_env, conidx, param->status);
            } break;

            case GATTC_READ:
            {
                if(param->status != GAP_ERR_NO_ERROR)
                {
                    struct bcsc_rd_char_rsp *rsp = KE_MSG_ALLOC(BCSC_RD_CHAR_RSP,
                                                                prf_dst_task_get(&(bcsc_env->prf_env), conidx),
                                                                prf_src_task_get(&(bcsc_env->prf_env), conidx),
                                                                bcsc_rd_char_rsp);

                    rsp->char_code = bcsc_env->env[conidx]->last_char_code;
                    rsp->status = param->status;
                    rsp->lenght = 0;

                    ke_msg_send(rsp);
                }
            } break;

            default:
                break;
        }

        ke_state_set(dest_id, BCSC_IDLE);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BCSC_RD_CHAR_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bcsc_rd_char_req_handler(ke_msg_id_t const msgid,
                                    struct bcsc_rd_char_req const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == BCSC_IDLE)
    {
        struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint16_t search_hdl = ATT_INVALID_SEARCH_HANDLE;

        ASSERT_INFO(bcsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(bcsc_env->env[conidx] != NULL, dest_id, src_id);

        if (param->char_code == BCSC_RD_BC_FEAT)
        {
            search_hdl = bcsc_env->env[conidx]->bcs.chars[BCSC_CHAR_BC_FEATURE].val_hdl;
        }
        else if (param->char_code == BCSC_RD_BC_MEAS_CLI_CFG)
        {
            search_hdl = bcsc_env->env[conidx]->bcs.descs[BCSC_DESC_MEAS_CLI_CFG].desc_hdl;
        }

        if (search_hdl != ATT_INVALID_SEARCH_HANDLE)
        {
            prf_read_char_send(&(bcsc_env->prf_env), conidx,
                            bcsc_env->env[conidx]->bcs.svc.shdl,
                            bcsc_env->env[conidx]->bcs.svc.ehdl,
                            search_hdl);

            bcsc_env->env[conidx]->last_char_code = param->char_code;

            ke_state_set(dest_id, BCSC_BUSY);
        }
        else
        {
            struct bcsc_rd_char_rsp *rsp = KE_MSG_ALLOC(BCSC_RD_CHAR_RSP,
                                                        prf_dst_task_get(&(bcsc_env->prf_env), conidx),
                                                        prf_src_task_get(&(bcsc_env->prf_env), conidx),
                                                        bcsc_rd_char_rsp);

            rsp->lenght = 0;
            rsp->char_code = param->char_code;
            rsp->status = GAP_ERR_INVALID_PARAM;

            ke_msg_send(rsp);
        }
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
    if (ke_state_get(dest_id) == BCSC_BUSY)
    {
        struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        struct bcsc_rd_char_rsp *rsp = KE_MSG_ALLOC_DYN(BCSC_RD_CHAR_RSP,
                                                        prf_dst_task_get(&(bcsc_env->prf_env), conidx),
                                                        prf_src_task_get(&(bcsc_env->prf_env), conidx),
                                                        bcsc_rd_char_rsp,
                                                        param->length);

        rsp->char_code = bcsc_env->env[conidx]->last_char_code;
        rsp->status = GAP_ERR_NO_ERROR;
        rsp->lenght = param->length;
        memcpy(rsp->value, param->value, param->length);

        ke_msg_send(rsp);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BCSC_BC_MEAS_CFG_IND_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int bcsc_bc_meas_cfg_ind_req_handler(ke_msg_id_t const msgid,
                                            struct bcsc_bc_meas_cfg_ind_req const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    if (ke_state_get(dest_id) == BCSC_IDLE)
    {
        uint16_t cfg_hdl = ATT_INVALID_SEARCH_HANDLE;
        struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t status = PRF_ERR_REQ_DISALLOWED;

        if ((param->cfg_val == PRF_CLI_STOP_NTFIND)||
            (param->cfg_val == PRF_CLI_START_IND))
        {
            cfg_hdl = bcsc_env->env[conidx]->bcs.descs[BCSC_DESC_MEAS_CLI_CFG].desc_hdl;

            if(cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
            {
                prf_gatt_write_ntf_ind(&bcsc_env->prf_env, conidx, cfg_hdl, param->cfg_val);
                ke_state_set(dest_id, BCSC_BUSY);
                status = GAP_ERR_NO_ERROR;
            }
            else
            {
                status = PRF_ERR_INEXISTENT_HDL;
            }
        }

        if (status != GAP_ERR_NO_ERROR)
        {
            bcsc_bc_meas_cfg_ind_rsp_send(bcsc_env, conidx, status);
        }
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_EVENT_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_event_ind_handler(ke_msg_id_t const msgid,
                                   struct gattc_event_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(dest_id);
    struct bcsc_env_tag *bcsc_env = PRF_ENV_GET(BCSC, bcsc);

    if (param->handle == bcsc_env->env[conidx]->bcs.chars[BCSC_CHAR_BC_MEAS].val_hdl &&
        param->type == GATTC_INDICATE)
    {
        struct bcsc_bc_meas_ind *ind = KE_MSG_ALLOC(BCSC_BC_MEAS_IND,
                                                    prf_dst_task_get(&(bcsc_env->prf_env), conidx),
                                                    prf_src_task_get(&(bcsc_env->prf_env), conidx),
                                                    bcsc_bc_meas_ind);

        bcsc_unpack_meas_value(&ind->meas_val, &ind->flags, (uint8_t*)param->value);
        ke_msg_send(ind);

        // confirm that indication has been correctly received
        struct gattc_event_cfm * cfm = KE_MSG_ALLOC(GATTC_EVENT_CFM, src_id, dest_id, gattc_event_cfm);
        cfm->handle = param->handle;
        ke_msg_send(cfm);
    }

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Specifies the default handler structure for every state.
const struct ke_msg_handler bcsc_default_state[] =
{
    {GATTC_EVENT_IND,               (ke_msg_func_t) gattc_event_ind_handler},
    {GATTC_EVENT_REQ_IND,           (ke_msg_func_t) gattc_event_ind_handler},
    {BCSC_ENABLE_REQ,               (ke_msg_func_t) bcsc_enable_req_handler},
    {BCSC_RD_CHAR_REQ,              (ke_msg_func_t) bcsc_rd_char_req_handler},
    {BCSC_BC_MEAS_CFG_IND_REQ,      (ke_msg_func_t) bcsc_bc_meas_cfg_ind_req_handler},
    {GATTC_DISC_SVC_INCL_IND,       (ke_msg_func_t) gattc_disc_svc_incl_ind_handler},
    {GATTC_DISC_CHAR_IND,           (ke_msg_func_t) gattc_disc_char_ind_handler},
    {GATTC_DISC_CHAR_DESC_IND,      (ke_msg_func_t) gattc_disc_char_desc_ind_handler},
    {GATTC_SDP_SVC_IND,             (ke_msg_func_t) gattc_sdp_svc_ind_handler},
    {GATTC_CMP_EVT,                 (ke_msg_func_t) gattc_cmp_evt_handler},
    {GATTC_READ_IND,                (ke_msg_func_t) gattc_read_ind_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler bcsc_default_handler = KE_STATE_HANDLER(bcsc_default_state);

#endif // (BLE_BCS_CLIENT)

/// @} BCSCTASK
