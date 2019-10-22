/**
 ****************************************************************************************
 *
 * @file ancc_task.c
 *
 * @brief Generic Attribute Profile Service Client Task implementation.
 *
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
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
 * @addtogroup GATTCLIENT
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_GATT_CLIENT)

#include <string.h>

#include "gap.h"
#include "gatt_client.h"
#include "gatt_client_task.h"
#include "gattc_task.h"
#include "prf_utils.h"

const struct prf_char_def gattc_gatt_char[GATTC_CHAR_MAX] =
{
    /// Service Changed Characteristic
    [GATTC_CHAR_SVC_CHANGED]        = {ATT_CHAR_SERVICE_CHANGED,
                                       ATT_OPTIONAL,
                                       ATT_CHAR_PROP_IND},
};

const struct prf_char_desc_def gattc_gatt_char_desc[GATTC_DESC_MAX] =
{
    /// Service Changed Client Characteristic Configuration Descriptor
    [GATTC_DESC_SVC_CHANGED_CCC] = {ATT_DESC_CLIENT_CHAR_CFG,
                                       ATT_MANDATORY,
                                       GATTC_CHAR_SVC_CHANGED},
};

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATT_CLIENT_ENABLE_REQ message.
 * The handler enables the GATT Profile Service Client Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gatt_client_enable_req_handler(ke_msg_id_t const msgid,
                                          struct gatt_client_enable_req const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t state = ke_state_get(dest_id);

    // GATT Client Role Task Environment
    struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);

    ASSERT_ERROR(gatt_env != NULL);
    if ((state == GATT_CLIENT_IDLE) && (gatt_env->env[conidx] == NULL))
    {
        gatt_env->env[conidx] = (struct gatt_client_cnx_env*) ke_malloc(sizeof(struct gatt_client_cnx_env),
                                                                               KE_MEM_ENV);
        memset(gatt_env->env[conidx], 0, sizeof(struct gatt_client_cnx_env));

        if (param->con_type == PRF_CON_DISCOVERY)
        {
            prf_disc_svc_send(&(gatt_env->prf_env), conidx,  ATT_SVC_GENERIC_ATTRIBUTE);
            ke_state_set(dest_id, GATT_CLIENT_DISCOVERING);
        }
        else
        {
            gatt_env->env[conidx]->gatt = param->gatt;
            gatt_client_enable_rsp_send(gatt_env, conidx, GAP_ERR_NO_ERROR);
        }

        status = GAP_ERR_NO_ERROR;
    }

    // send an error if request fails
    if (status != GAP_ERR_NO_ERROR)
    {
        gatt_client_enable_rsp_send(gatt_env, conidx, status);
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
    uint8_t state = ke_state_get(dest_id);

    if (state == GATT_CLIENT_DISCOVERING)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);

        struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);

        ASSERT_ERROR(gatt_env != NULL);
        ASSERT_ERROR(gatt_env->env[conidx] != NULL);

        if (gatt_env->env[conidx]->nb_svc == 0)
        {
            // Retrieve GATT characteristic and descriptor
            prf_extract_svc_info(ind, GATTC_CHAR_MAX, &gattc_gatt_char[0],  &gatt_env->env[conidx]->gatt.chars[0],
                                 GATTC_DESC_MAX, &gattc_gatt_char_desc[0], &gatt_env->env[conidx]->gatt.descs[0]);

            //Even if we get multiple responses we only store 1 range
            gatt_env->env[conidx]->gatt.svc.shdl = ind->start_hdl;
            gatt_env->env[conidx]->gatt.svc.ehdl = ind->end_hdl;
        }

        gatt_env->env[conidx]->nb_svc++;
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

    if (state == GATT_CLIENT_DISCOVERING)
    {
        struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t status = param->status;

        ASSERT_ERROR(gatt_env != NULL);
        ASSERT_ERROR(gatt_env->env[conidx] != NULL);

        if ((param->status == ATT_ERR_ATTRIBUTE_NOT_FOUND)||
            (param->status == ATT_ERR_NO_ERROR))
        {
            if (gatt_env->env[conidx]->nb_svc == 1)
            {
                status = prf_check_svc_char_validity(GATTC_CHAR_MAX,
                                                     gatt_env->env[conidx]->gatt.chars,
                                                     gattc_gatt_char);
            }
            // too many services
            else if (gatt_env->env[conidx]->nb_svc > 1)
            {
                status = PRF_ERR_MULTIPLE_SVC;
            }
            // no services found
            else
            {
                status = PRF_ERR_STOP_DISC_CHAR_MISSING;
            }

            // If characteristic is fine check descriptor validity
            if (status == GAP_ERR_NO_ERROR)
            {
                status = prf_check_svc_char_desc_validity(GATTC_DESC_MAX,
                                                          gatt_env->env[conidx]->gatt.descs,
                                                          gattc_gatt_char_desc,
                                                          gatt_env->env[conidx]->gatt.chars);
            }

        }
        gatt_client_enable_rsp_send(gatt_env, conidx, status);
    }
    else if (state == GATT_CLIENT_BUSY)
    {
        struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_ERROR(gatt_env != NULL);
        ASSERT_ERROR(gatt_env->env[conidx] != NULL);

        switch (param->operation)
        {
            case GATTC_READ:
            {
                if (param->status != GAP_ERR_NO_ERROR)
                {
                    gatt_client_rd_cfg_ind_rsp_send(gatt_env, conidx, 0, param->status);
                }
            } break;

            case GATTC_WRITE:
            case GATTC_WRITE_NO_RESPONSE:
            {
                gatt_client_wr_cfg_ind_rsp_send(gatt_env, conidx, param->status);
            } break;

            default:
                break;
        }

    }
    ke_state_set(dest_id, GATT_CLIENT_IDLE);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATT_CLIENT_WR_CFG_IND_REQ message.
 * This generic event is received for different requests, so need to keep track.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gatt_client_wr_cfg_ind_req_handler(ke_msg_id_t const msgid,
                                              struct gatt_client_wr_cfg_ind_req const *param,
                                              ke_task_id_t const dest_id,
                                              ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    int status = KE_MSG_CONSUMED;

    if (state == GATT_CLIENT_IDLE)
    {
        struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint16_t cfg_hdl = ATT_INVALID_SEARCH_HANDLE;

        ASSERT_ERROR(gatt_env != NULL);
        ASSERT_ERROR(gatt_env->env[conidx] != NULL);

        cfg_hdl = gatt_env->env[conidx]->gatt.descs[GATTC_DESC_SVC_CHANGED_CCC].desc_hdl;
        if (cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
        {
            prf_gatt_write_ntf_ind(&gatt_env->prf_env, conidx, cfg_hdl, param->cfg_val);
            ke_state_set(dest_id, GATT_CLIENT_BUSY);
        }
        else
        {
            gatt_client_wr_cfg_ind_rsp_send(gatt_env, conidx, PRF_ERR_INEXISTENT_HDL);
        }
    }
    else if (state == GATT_CLIENT_BUSY)
    {
        status = KE_MSG_SAVED;
    }

    return status;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATT_CLIENT_WR_CFG_IND_REQ message.
 * This generic event is received for different requests, so need to keep track.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gatt_client_rd_cfg_ind_req_handler(ke_msg_id_t const msgid,
                                              struct gatt_client_rd_cfg_ind_req const *param,
                                              ke_task_id_t const dest_id,
                                              ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);
    int status = KE_MSG_CONSUMED;

    if (state == GATT_CLIENT_IDLE)
    {
        struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint16_t cfg_hdl = ATT_INVALID_SEARCH_HANDLE;

        ASSERT_ERROR(gatt_env != NULL);
        ASSERT_ERROR(gatt_env->env[conidx] != NULL);

        cfg_hdl = gatt_env->env[conidx]->gatt.descs[GATTC_DESC_SVC_CHANGED_CCC].desc_hdl;

        if (cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
        {
            prf_read_char_send(&(gatt_env->prf_env), conidx,
                               gatt_env->env[conidx]->gatt.svc.shdl,
                               gatt_env->env[conidx]->gatt.svc.ehdl,
                               cfg_hdl);

            ke_state_set(dest_id, GATT_CLIENT_BUSY);
        }
        else
        {
            gatt_client_rd_cfg_ind_rsp_send(gatt_env, conidx, 0, PRF_ERR_INEXISTENT_HDL);
        }

    }
    else if (state == GATT_CLIENT_BUSY)
    {
        status = KE_MSG_SAVED;
    }

    return status;
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
    // Get the address of the environment
    struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);
    // Get connection index
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint16_t cfg_val = 0;

    cfg_val = co_read16p(param->value);

    gatt_client_rd_cfg_ind_rsp_send(gatt_env, conidx, cfg_val, GAP_ERR_NO_ERROR);

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
    struct gatt_client_env_tag *gatt_env = PRF_ENV_GET(GATT_CLIENT, gatt_client);

    if (param->handle == gatt_env->env[conidx]->gatt.chars[GATTC_CHAR_SVC_CHANGED].val_hdl &&
        param->type == GATTC_INDICATE)
    {
        struct gatt_client_svc_changed_ind *ind = KE_MSG_ALLOC(GATT_CLIENT_SVC_CHANGED_IND,
                                                               prf_dst_task_get(&(gatt_env->prf_env), conidx),
                                                               prf_src_task_get(&(gatt_env->prf_env), conidx),
                                                               gatt_client_svc_changed_ind);

        ind->val.shdl = co_read16p(param->value);
        ind->val.ehdl = co_read16p(param->value + 2);
        ke_msg_send(ind);

        // confirm that indication has been correctly received
        struct gattc_event_cfm * cfm = KE_MSG_ALLOC(GATTC_EVENT_CFM, src_id, dest_id, gattc_event_cfm);
        cfm->handle = param->handle;
        ke_msg_send(cfm);
    }

    return KE_MSG_CONSUMED;
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Specifies the default handler structure for every state.
const struct ke_msg_handler gatt_client_default_state[] =
{
    {GATT_CLIENT_ENABLE_REQ,        (ke_msg_func_t)gatt_client_enable_req_handler},
    {GATTC_SDP_SVC_IND,             (ke_msg_func_t)gattc_sdp_svc_ind_handler},
    {GATTC_CMP_EVT,                 (ke_msg_func_t)gattc_cmp_evt_handler},
    {GATT_CLIENT_WR_CFG_IND_REQ,    (ke_msg_func_t)gatt_client_wr_cfg_ind_req_handler},
    {GATT_CLIENT_RD_CFG_IND_REQ,    (ke_msg_func_t)gatt_client_rd_cfg_ind_req_handler},
    {GATTC_READ_IND,                (ke_msg_func_t)gattc_read_ind_handler},
    {GATTC_EVENT_REQ_IND,           (ke_msg_func_t)gattc_event_ind_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler gatt_client_default_handler = KE_STATE_HANDLER(gatt_client_default_state);

#endif // BLE_GAT_CLIENT
