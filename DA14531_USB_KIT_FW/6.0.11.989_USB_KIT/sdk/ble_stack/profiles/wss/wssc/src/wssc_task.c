/**
 ****************************************************************************************
 *
 * @file wssc_task.c
 *
 * @brief C file - Weight Scale Service Client Role Task Implementation.
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
 * @addtogroup WSSCTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_WSS_COLLECTOR)
#include <string.h>

#include "gap.h"
#include "wssc.h"
#include "wssc_task.h"
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

const struct prf_char_def wssc_wss_char[WSSC_CHAR_MAX] =
{
    // Weight Scale Feature
    [WSSC_CHAR_WSS_WS_FEAT]        = {ATT_CHAR_WS_FEAT,
                                      ATT_MANDATORY,
                                      ATT_CHAR_PROP_RD},
    // Weight Measurement
    [WSSC_CHAR_WSS_WEIGHT_MEAS]    = {ATT_CHAR_WS_MEAS,
                                      ATT_MANDATORY,
                                      ATT_CHAR_PROP_IND},
};

const struct prf_char_desc_def wssc_wss_char_desc[WSSC_DESC_MAX] =
{
    [WSSC_DESC_WSS_WM_CLI_CFG]     = {ATT_DESC_CLIENT_CHAR_CFG,
                                      ATT_MANDATORY,
                                      WSSC_CHAR_WSS_WEIGHT_MEAS},
};

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref WSSC_ENABLE_REQ message.
 * The handler enables the Weight Scale Service Client Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int wssc_enable_req_handler(ke_msg_id_t const msgid,
                                   struct wssc_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    struct wssc_env_tag *wssc_env = PRF_ENV_GET(WSSC, wssc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t state = ke_state_get(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

    ASSERT_ERROR(wssc_env != NULL);

    if ((state == WSSC_IDLE) && (wssc_env->env[conidx] == NULL))
    {
        wssc_env->env[conidx] = (struct wssc_cnx_env*) ke_malloc(sizeof(struct wssc_cnx_env), KE_MEM_ENV);
        memset(wssc_env->env[conidx], 0, sizeof(struct wssc_cnx_env));

        // Config connection, start discovering
        if (param->con_type == PRF_CON_DISCOVERY)
        {
            // Start discovering WSS on peer
            prf_disc_svc_send(&(wssc_env->prf_env), conidx, ATT_SVC_WEIGHT_SCALE);

            wssc_env->env[conidx]->last_uuid_req = ATT_SVC_WEIGHT_SCALE;

            // Go to DISCOVERING state
            ke_state_set(dest_id, WSSC_DISCOVERING);
        }
        // Normal connection, get saved att details
        else
        {
            wssc_env->env[conidx]->wss = param->wss;
            // Send APP confirmation and stay in IDLE state
            wssc_enable_rsp_send(wssc_env, conidx, GAP_ERR_NO_ERROR);
        }
        status = GAP_ERR_NO_ERROR;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        wssc_enable_rsp_send(wssc_env, conidx, status);
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
    if (ke_state_get(dest_id) == WSSC_DISCOVERING)
    {
        struct wssc_env_tag *wssc_env = PRF_ENV_GET(WSSC, wssc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_ERROR(wssc_env != NULL);
        ASSERT_ERROR(wssc_env->env[conidx] != NULL);

        if(wssc_env->env[conidx]->nb_svc == 0)
        {
            // Retrieve WSS characteristics and descriptors
            prf_extract_svc_info(ind,
                        WSSC_CHAR_MAX, wssc_wss_char, wssc_env->env[conidx]->wss.chars,
                        WSSC_DESC_MAX, wssc_wss_char_desc, wssc_env->env[conidx]->wss.descs);

            // Even if we get multiple responses we only store 1-st range
            wssc_env->env[conidx]->wss.svc.shdl = ind->start_hdl;
            wssc_env->env[conidx]->wss.svc.ehdl = ind->end_hdl;
        }

        wssc_env->env[conidx]->nb_svc++;
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

    if (state == WSSC_DISCOVERING)
    {
        struct wssc_env_tag *wssc_env = PRF_ENV_GET(WSSC, wssc);
        uint8_t status = param->status;;
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_ERROR(wssc_env != NULL);
        ASSERT_ERROR(wssc_env->env[conidx] != NULL);

        if ((status == ATT_ERR_ATTRIBUTE_NOT_FOUND)||
            (status == ATT_ERR_NO_ERROR))
        {
            if(wssc_env->env[conidx]->nb_svc == 1)
            {
                status = prf_check_svc_char_validity(WSSC_CHAR_MAX,
                                                     wssc_env->env[conidx]->wss.chars,
                                                     wssc_wss_char);
            }
            // too much services
            else if (wssc_env->env[conidx]->nb_svc > 1)
            {
                status = PRF_ERR_MULTIPLE_SVC;
            }
            // no services found
            else
            {
                status = PRF_ERR_STOP_DISC_CHAR_MISSING;
            }

            // If Characteristics are fine check descriptor validity
            if (status == GAP_ERR_NO_ERROR)
            {
                status = prf_check_svc_char_desc_validity(WSSC_DESC_MAX,
                                                          wssc_env->env[conidx]->wss.descs,
                                                          wssc_wss_char_desc,
                                                          wssc_env->env[conidx]->wss.chars);
            }
        }

        // Send response
        wssc_enable_rsp_send(wssc_env, conidx, status);
    }
    else if (state == WSSC_BUSY)
    {
        struct wssc_env_tag *wssc_env = PRF_ENV_GET(WSSC, wssc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_ERROR(wssc_env != NULL);
        ASSERT_ERROR(wssc_env->env[conidx] != NULL);

        switch (param->operation)
        {
            case GATTC_WRITE:
            case GATTC_WRITE_NO_RESPONSE:
            {
                struct wssc_cfg_indntf_rsp*rsp = KE_MSG_ALLOC(WSSC_CFG_INDNTF_RSP,
                                                              prf_dst_task_get(&(wssc_env->prf_env), conidx),
                                                              prf_src_task_get(&(wssc_env->prf_env), conidx),
                                                              wssc_cfg_indntf_rsp);

                rsp->status = param->status;
                ke_msg_send(rsp);
            } break;

            case GATTC_READ:
            {
                if(param->status != GAP_ERR_NO_ERROR)
                {
                    struct wssc_rd_char_rsp *rsp = KE_MSG_ALLOC(WSSC_RD_CHAR_RSP,
                                                                prf_dst_task_get(&(wssc_env->prf_env), conidx),
                                                                prf_src_task_get(&(wssc_env->prf_env), conidx),
                                                                wssc_rd_char_rsp);
                    // set error status
                    rsp->info.status = param->status;
                    rsp->info.length = 0;

                    ke_msg_send(rsp);
                }
            } break;

            default:
                break;
        }
    }

    // Go to IDLE state
    ke_state_set(dest_id, WSSC_IDLE);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref WSSC_RD_CHAR_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int wssc_rd_char_req_handler(ke_msg_id_t const msgid,
                                          struct wssc_rd_char_req const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    int msg_status = KE_MSG_CONSUMED;

    if (state == WSSC_IDLE)
    {
        struct wssc_env_tag *wssc_env = PRF_ENV_GET(WSSC, wssc);
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint16_t search_hdl = ATT_INVALID_SEARCH_HANDLE;

        ASSERT_ERROR(wssc_env != NULL);
        ASSERT_ERROR(wssc_env->env[conidx] != NULL);

        if (param->char_code == WSSC_RD_WSS_WS_FEAT)
        {
            search_hdl = wssc_env->env[conidx]->wss.chars[WSSC_CHAR_WSS_WS_FEAT].val_hdl;
        }
        else if (param->char_code == WSSC_RD_WSS_WM_CLI_CFG)
        {
            search_hdl = wssc_env->env[conidx]->wss.descs[WSSC_DESC_WSS_WM_CLI_CFG].desc_hdl;
        }

        if (search_hdl != ATT_INVALID_SEARCH_HANDLE)
        {
            prf_read_char_send(&(wssc_env->prf_env), conidx,
                    wssc_env->env[conidx]->wss.svc.shdl,
                    wssc_env->env[conidx]->wss.svc.ehdl,
                    search_hdl);

            ke_state_set(dest_id, WSSC_BUSY);
        }
        else
        {
            struct wssc_rd_char_rsp *rsp = KE_MSG_ALLOC(WSSC_RD_CHAR_RSP,
                                                        prf_dst_task_get(&(wssc_env->prf_env), conidx),
                                                        prf_src_task_get(&(wssc_env->prf_env), conidx),
                                                        wssc_rd_char_rsp);

            rsp->info.length = 0;
            rsp->info.status = GAP_ERR_INVALID_PARAM;

            ke_msg_send(rsp);
        }
    }
    else if (state == WSSC_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref WSSC_CFG_INDNTF_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int wssc_cfg_indntf_req_handler(ke_msg_id_t const msgid,
                                            struct wssc_cfg_indntf_req const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    int msg_status = KE_MSG_CONSUMED;

    if (state == WSSC_IDLE)
    {
        struct wssc_env_tag *wssc_env = PRF_ENV_GET(WSSC, wssc);
        uint16_t cfg_hdl = ATT_INVALID_SEARCH_HANDLE;
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t status = PRF_ERR_REQ_DISALLOWED;

        if ((param->cfg_val == PRF_CLI_STOP_NTFIND)||
            (param->cfg_val == PRF_CLI_START_IND))
        {
            cfg_hdl = wssc_env->env[conidx]->wss.descs[WSSC_DESC_WSS_WM_CLI_CFG].desc_hdl;

            if(cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
            {
                prf_gatt_write_ntf_ind(&wssc_env->prf_env, conidx, cfg_hdl, param->cfg_val);
                ke_state_set(dest_id, WSSC_BUSY);
                status = GAP_ERR_NO_ERROR;
            }
            else
            {
                status = PRF_ERR_INEXISTENT_HDL;
            }
        }

        if (status != GAP_ERR_NO_ERROR)
        {
            wssc_err_ind_send(wssc_env, conidx, status);
        }
    }
    else if (state == WSSC_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
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
    if (ke_state_get(dest_id) == WSSC_BUSY)
    {
        struct wssc_env_tag *wssc_env = PRF_ENV_GET(WSSC, wssc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        prf_client_att_info_rsp(&wssc_env->prf_env, conidx, WSSC_RD_CHAR_RSP,
                                GAP_ERR_NO_ERROR, param);
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
    struct wssc_env_tag *wssc_env = PRF_ENV_GET(WSSC, wssc);

    if (param->handle == wssc_env->env[conidx]->wss.chars[WSSC_CHAR_WSS_WEIGHT_MEAS].val_hdl &&
        param->type == GATTC_INDICATE)
    {
        wssc_unpack_meas_value(wssc_env, conidx, (uint8_t*)&(param->value), param->length);
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
const struct ke_msg_handler wssc_default_state[] =
{
    {WSSC_ENABLE_REQ,               (ke_msg_func_t)wssc_enable_req_handler},
    {GATTC_SDP_SVC_IND,             (ke_msg_func_t)gattc_sdp_svc_ind_handler},
    {GATTC_CMP_EVT,                 (ke_msg_func_t)gattc_cmp_evt_handler},
    {WSSC_RD_CHAR_REQ,              (ke_msg_func_t)wssc_rd_char_req_handler},
    {WSSC_CFG_INDNTF_REQ,           (ke_msg_func_t)wssc_cfg_indntf_req_handler},
    {GATTC_READ_IND,                (ke_msg_func_t)gattc_read_ind_handler},
    {GATTC_EVENT_IND,               (ke_msg_func_t)gattc_event_ind_handler},
    {GATTC_EVENT_REQ_IND,           (ke_msg_func_t)gattc_event_ind_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler wssc_default_handler = KE_STATE_HANDLER(wssc_default_state);

#endif // (BLE_WSS_COLLECTOR)

/// @} WSSCTASK
