/**
 ****************************************************************************************
 *
 * @file ctsc_task.c
 *
 * @brief Current Time Service Client Task implementation.
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
 * @addtogroup CTSC Task
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_CTS_CLIENT)
#include <string.h>

#include "ctsc.h"
#include "ctsc_task.h"
#include "prf_types.h"
#include "prf_utils.h"

/*
 * STRUCTURES
 ****************************************************************************************
 */

const struct prf_char_def ctsc_cts_char[CTSC_CHAR_MAX] =
{
    // Current Time
    [CTSC_CHAR_CTS_CURRENT_TIME] = {
        ATT_CHAR_CT_TIME,
        ATT_MANDATORY,
        ATT_CHAR_PROP_RD | ATT_CHAR_PROP_NTF
    },
    // Local Time Information
    [CTSC_CHAR_CTS_LOC_TIME_INFO] = {
        ATT_CHAR_LOCAL_TIME_INFO,
        ATT_OPTIONAL,
        ATT_CHAR_PROP_RD
    },
    // Reference Time Information
    [CTSC_CHAR_CTS_REF_TIME_INFO] = {
        ATT_CHAR_REFERENCE_TIME_INFO,
        ATT_OPTIONAL,
        ATT_CHAR_PROP_RD
    },
};

const struct prf_char_desc_def ctsc_cts_char_desc[CTSC_DESC_MAX] =
{
    [CTSC_DESC_CTS_CT_CLI_CFG] = {
        ATT_DESC_CLIENT_CHAR_CFG,
        ATT_MANDATORY,
        CTSC_CHAR_CTS_CURRENT_TIME
    },
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref CTSC_ENABLE_REQ message.
 * The handler enables the Current Time Service Client Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ctsc_enable_req_handler(const ke_msg_id_t msgid,
                                   const struct ctsc_enable_req *param,
                                   const ke_task_id_t dest_id,
                                   const ke_task_id_t src_id)
{
    uint8_t status = GAP_ERR_NO_ERROR;
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t state = ke_state_get(dest_id);
    struct ctsc_env_tag *ctsc_env = PRF_ENV_GET(CTSC, ctsc);

    ASSERT_INFO(ctsc_env != NULL, dest_id, src_id);

    if ((state == CTSC_IDLE) && (ctsc_env->env[conidx] == NULL))
    {
        // allocate environment variable for task instance
        ctsc_env->env[conidx] = (struct ctsc_cnx_env *) ke_malloc(sizeof(struct ctsc_cnx_env), KE_MEM_ATT_DB);
        memset(ctsc_env->env[conidx], 0, sizeof(struct ctsc_cnx_env));

        // config connection, start discovering
        if (param->con_type == PRF_CON_DISCOVERY)
        {
            // start discovering CTS on peer
            prf_disc_svc_send(&ctsc_env->prf_env, conidx, ATT_SVC_CURRENT_TIME);

            // Go to DISCOVERING state
            ke_state_set(dest_id, CTSC_DISCOVERING);
        }
        else // normal connection, get saved att details
        {
            ctsc_env->env[conidx]->cts = param->cts;

            // send APP confirmation that can start normal connection to CTS
            ctsc_enable_rsp_send(ctsc_env, conidx, GAP_ERR_NO_ERROR);
        }
    }
    else if (state != CTSC_FREE)
    {
        status = PRF_ERR_REQ_DISALLOWED;
    }

    // send an error if request fails
    if (status != GAP_ERR_NO_ERROR)
    {
        ctsc_enable_rsp_send(ctsc_env, conidx, status);
    }

    return KE_MSG_CONSUMED;
};

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
static int gattc_sdp_svc_ind_handler(const ke_msg_id_t msgid,
                                     const struct gattc_sdp_svc_ind *ind,
                                     const ke_task_id_t dest_id,
                                     const ke_task_id_t src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if (state == CTSC_DISCOVERING)
    {
        uint8_t conidx = KE_IDX_GET(dest_id);
        // Get the address of the environment
        struct ctsc_env_tag *ctsc_env = PRF_ENV_GET(CTSC, ctsc);

        ASSERT_INFO(ctsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(ctsc_env->env[conidx] != NULL, dest_id, src_id);

        if (ctsc_env->env[conidx]->nb_svc == 0)
        {
            // Retrieve CTS characteristics
            prf_extract_svc_info(ind, CTSC_CHAR_MAX, ctsc_cts_char, ctsc_env->env[conidx]->cts.chars,
                                 CTSC_DESC_MAX, ctsc_cts_char_desc, ctsc_env->env[conidx]->cts.descs);

            // Even if we get multiple responses we only store 1 range
            ctsc_env->env[conidx]->cts.svc.shdl = ind->start_hdl;
            ctsc_env->env[conidx]->cts.svc.ehdl = ind->end_hdl;
        }

        ctsc_env->env[conidx]->nb_svc++;
    }

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Send response to read or write requests
 *
 * After function finishes, task goes to CTSC_IDLE state.
 *
 * @param[in] msgid Id of the message to send.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @param[in] response_data actual response to send
 * @param[in] response_size response size
 ****************************************************************************************
 */
static void send_rsp(ke_msg_id_t rsp_msg_id, ke_task_id_t dest_id, ke_task_id_t src_id,
                     const struct ctsc_rsp *response_data, uint16_t response_size)
{
    struct ctsc_rsp *rsp;

    rsp = ke_msg_alloc(rsp_msg_id, dest_id, src_id, response_size);
    memcpy(rsp, response_data, response_size);

    ke_state_set(src_id, CTSC_IDLE);

    ke_msg_send(rsp);
}

/**
 ****************************************************************************************
 * @brief Sends response to read or write requests
 *
 * This function is used for read request that failed.
 * If read request succeeded \p send_rsp should be called directly.
 * For write request it is always used.
 *
 * @param[in] rsp_msg_id Id of the message to send.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @param[in] char_code Characteristic or descriptor.
 * @param[in] status Response error code.
 ****************************************************************************************
 */
static void send_status_rsp(ke_msg_id_t rsp_msg_id, ke_task_id_t dest_id, ke_task_id_t src_id,
                            uint8_t char_code, uint8_t status)
{
    struct ctsc_rsp response_data;

    response_data.status = status;
    response_data.char_code = char_code;

    send_rsp(rsp_msg_id, dest_id, src_id, &response_data, sizeof(response_data));
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
static int gattc_cmp_evt_handler(const ke_msg_id_t msgid,
                                 const struct gattc_cmp_evt *param,
                                 const ke_task_id_t dest_id,
                                 const ke_task_id_t src_id)
{
    uint8_t state = ke_state_get(dest_id);
    struct ctsc_env_tag *ctsc_env = PRF_ENV_GET(CTSC, ctsc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = param->status;
    // Get response code that was prepared during request
    const ke_msg_id_t rsp_msg_id = ctsc_env->env[conidx]->rsp_msg_id;

    if (state == CTSC_DISCOVERING)
    {

        if (status == ATT_ERR_NO_ERROR)
        {
            if (ctsc_env->env[conidx]->nb_svc == 1)
            {
                status = prf_check_svc_char_validity(CTSC_CHAR_MAX,
                                                     ctsc_env->env[conidx]->cts.chars,
                                                     ctsc_cts_char);
            }
            // too many services
            else if (ctsc_env->env[conidx]->nb_svc > 1)
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
                status = prf_check_svc_char_desc_validity(CTSC_DESC_MAX,
                                                          ctsc_env->env[conidx]->cts.descs,
                                                          ctsc_cts_char_desc,
                                                          ctsc_env->env[conidx]->cts.chars);
            }
        }

        ctsc_enable_rsp_send(ctsc_env, conidx, status);
    }
    else if (state == CTSC_BUSY)
    {
        switch (param->operation)
        {
            case GATTC_WRITE:
            case GATTC_READ:
                send_status_rsp(rsp_msg_id,
                                prf_dst_task_get(&ctsc_env->prf_env, conidx),
                                prf_src_task_get(&ctsc_env->prf_env, conidx),
                                ctsc_env->env[conidx]->last_char_code, status);
                break;
        }
        ke_state_set(dest_id, CTSC_IDLE);
    }

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Get request message id for given request id.
 *
 * @param[in] msgid Id of the message.
 * @return response message id
 ****************************************************************************************
 */
static ke_msg_id_t req2rsp(ke_msg_id_t id)
{
    /*
     * Every read and write request has following read/write response message
     * it makes it easy to get response messages id by just adding 1 to request.
     */
    return id + 1;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the CTSC_READ_xxx_REQ message.
 * Check if the handle exists in profile(already discovered) and send request, otherwise
 * error to APP.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ctsc_read_req_handler(const ke_msg_id_t msgid,
                                 const struct ctsc_read_req *param,
                                 const ke_task_id_t dest_id,
                                 const ke_task_id_t src_id)
{
    struct ctsc_env_tag *ctsc_env = PRF_ENV_GET(CTSC, ctsc);;
    uint8_t conidx = KE_IDX_GET(dest_id);
    const ke_msg_id_t rsp_msg_id = req2rsp(msgid);
    uint8_t msg_status = KE_MSG_CONSUMED;
    uint8_t status = GAP_ERR_NO_ERROR;
    uint8_t state = ke_state_get(dest_id);

    if (state == CTSC_IDLE)
    {
        ASSERT_INFO(ctsc_env != NULL, dest_id, src_id);
        // environment variable not ready
        if (ctsc_env->env[conidx] == NULL)
        {
            status = PRF_APP_ERROR;
        }
        else
        {
            uint16_t search_hdl = ATT_INVALID_SEARCH_HANDLE;
            // Check if it is characteristic ...
            if (param->char_code < CTSC_CHAR_MAX)
            {
                search_hdl = ctsc_env->env[conidx]->cts.chars[param->char_code].val_hdl;
            }
            // ... or descriptor read request
            else if (((param->char_code & CTSC_DESC_MASK) == CTSC_DESC_MASK) &&
                    ((param->char_code & ~CTSC_DESC_MASK) < CTSC_DESC_MAX))
            {
                search_hdl = ctsc_env->env[conidx]->cts.descs[param->char_code & ~CTSC_DESC_MASK].desc_hdl;
            }

            // Handle was found?
            if (search_hdl != ATT_INVALID_SEARCH_HANDLE)
            {
                // Store characteristic or descriptor for response
                ctsc_env->env[conidx]->last_char_code = param->char_code;
                ctsc_env->env[conidx]->rsp_msg_id = rsp_msg_id;

                prf_read_char_send(&ctsc_env->prf_env, conidx,
                                   ctsc_env->env[conidx]->cts.svc.shdl,
                                   ctsc_env->env[conidx]->cts.svc.ehdl,
                                   search_hdl);

                ke_state_set(dest_id, CTSC_BUSY);

                status = ATT_ERR_NO_ERROR;
            }
            else
            {
                status = PRF_ERR_INEXISTENT_HDL;
            }
        }
    }
    else if (state == CTSC_FREE)
    {
        status = GAP_ERR_DISCONNECTED;
    }
    else
    {
        // Another procedure is pending, keep the command for later
        msg_status = KE_MSG_SAVED;
        status = GAP_ERR_NO_ERROR;
    }

    // request cannot be performed, send error response immediately
    if (status != GAP_ERR_NO_ERROR)
    {
        send_status_rsp(rsp_msg_id, src_id, dest_id, param->char_code, status);
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
 * @brief Helper function to check if contents of \p cts_local_time_info_t is valid
 *
 * \param [in] info     local time information
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
 * @brief Handles reception of the @ref GATTC_READ_IND message.
 * Generic event received after every simple read command sent to peer server.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_ind_handler(const ke_msg_id_t msgid,
                                  const struct gattc_read_ind *param,
                                  const ke_task_id_t dest_id,
                                  const ke_task_id_t src_id)
{
    struct ctsc_env_tag *ctsc_env = PRF_ENV_GET(CTSC, ctsc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Response message id was stored during request handling
    ke_msg_id_t rsp_msg_id = ctsc_env->env[conidx]->rsp_msg_id;
    // Storage for unpacked data of each supported type
    union
    {
         struct ctsc_read_ct_rsp ct;
         struct ctsc_read_lti_rsp lti;
         struct ctsc_read_rti_rsp rti;
         struct ctsc_read_desc_rsp desc;
         struct ctsc_rsp rsp;
    } rsp_data;
    // For now assume that only status is send back
    uint8_t rsp_len = sizeof(struct ctsc_rsp);
    rsp_data.rsp.status = GAP_ERR_NO_ERROR;
    rsp_data.rsp.char_code = ctsc_env->env[conidx]->last_char_code;

    // None of the attributes is long, verify that
    if (param->offset != 0)
    {
        rsp_data.rsp.status = ATT_ERR_ATTRIBUTE_NOT_LONG;
    }
    else
    {
        switch (rsp_msg_id)
        {
        case CTSC_READ_CT_RSP:
            // Verify that received time is adheres to specification
            if (param->length != 10)
            {
                rsp_data.rsp.status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
            }
            else
            {
                // Unpack and verify time
                ctsc_unpack_curr_time_value(param->value, &rsp_data.ct.ct_val);
                if (cts_is_current_time_valid(&rsp_data.ct.ct_val))
                {
                    rsp_len = sizeof(struct ctsc_read_ct_rsp);
                }
                else
                {
                    rsp_data.rsp.status = PRF_APP_ERROR;
                }
            }
            break;
        case CTSC_READ_LTI_RSP:
            if (param->length != 2)
            {
                rsp_data.rsp.status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
            }
            else
            {
                // Unpack and verify local time informaiton
                rsp_data.lti.lti_val.time_zone = (cts_time_zone) param->value[0];
                rsp_data.lti.lti_val.dst_offset = (cts_dst_offset) param->value[1];
                if (cts_is_local_time_info_valid(&rsp_data.lti.lti_val))
                {
                    rsp_len = sizeof(struct ctsc_read_lti_rsp);
                }
                else
                {
                    rsp_data.rsp.status = PRF_APP_ERROR;
                }
            }
            break;
        case CTSC_READ_RTI_RSP:
            // For reference time information verify length and values ranges required
            if (param->length != 4)
            {
                rsp_data.rsp.status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
            }
            else
            {
                rsp_data.rti.rti_val.time_source = (cts_time_source) param->value[0];
                rsp_data.rti.rti_val.time_accuracy = (cts_time_accuracy) param->value[0];
                rsp_data.rti.rti_val.days_update = param->value[2];
                rsp_data.rti.rti_val.hours_update = param->value[3];
                if (rsp_data.rti.rti_val.hours_update <= 23 || rsp_data.rti.rti_val.hours_update == 255)
                {
                    rsp_len = sizeof(struct ctsc_read_rti_rsp);
                }
                else
                {
                    rsp_data.rsp.status = PRF_APP_ERROR;
                }
            }
            break;
        case CTSC_READ_DESC_RSP:
            // For characteristic descriptor verify length and supported values
            if (param->length != 2)
            {
                rsp_data.rsp.status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
            }
            else
            {
                rsp_data.desc.ntf_cfg = co_read16(param->value);
                if (rsp_data.desc.ntf_cfg == PRF_CLI_STOP_NTFIND ||
                    rsp_data.desc.ntf_cfg == PRF_CLI_START_NTF)
                {
                    rsp_len = sizeof(struct ctsc_read_desc_rsp);
                }
                else
                {
                    rsp_data.rsp.status = PRF_APP_ERROR;
                }
            }
            break;
        }
    }
    send_rsp(rsp_msg_id,
             prf_dst_task_get(&ctsc_env->prf_env, conidx), prf_src_task_get(&ctsc_env->prf_env, conidx),
             &rsp_data.rsp, rsp_len);

    return KE_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the CTSC_WRITE_DESC_REQ messages.
 * It allows configuration of the peer ntf/stop characteristic for a specified characteristic.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ctsc_write_desc_req_handler(const ke_msg_id_t msgid,
                                       const struct ctsc_write_desc_req *param,
                                       const ke_task_id_t dest_id,
                                       const ke_task_id_t src_id)
{
    // Get the address of the environment
    struct ctsc_env_tag *ctsc_env = PRF_ENV_GET(CTSC, ctsc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

    // Message will most likely be consumed
    uint8_t msg_status = KE_MSG_CONSUMED;
    uint8_t state = ke_state_get(dest_id);

    if (state == CTSC_IDLE)
    {
        if (ctsc_env->env[conidx] != NULL)
        {
            status = PRF_ERR_INVALID_PARAM;
            // Only one descriptor is valid (CTSC_DESC_CTS_CT_CLI_CFG)
            if ((param->value == PRF_CLI_STOP_NTFIND) ||
                (param->value == PRF_CLI_START_NTF) ||
                (CTSC_DESC_CTS_CT_CLI_CFG == (param->desc.char_code & CTSC_DESC_MASK)))
            {
                // Status
                status = PRF_ERR_INEXISTENT_HDL;
                // Get handle of the client configuration
                uint16_t cfg_hdl =
                        ctsc_env->env[conidx]->cts.descs[CTSC_DESC_CTS_CT_CLI_CFG].desc_hdl;
                // check if the handle value exists
                if (cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
                {
                    ctsc_env->env[conidx]->last_char_code = CTSC_DESC_CTS_CT_CLI_CFG;
                    ctsc_env->env[conidx]->rsp_msg_id = CTSC_WRITE_DESC_RSP;

                    status = GAP_ERR_NO_ERROR;
                    ke_state_set(dest_id, CTSC_BUSY);
                    // Send GATT Write Request
                    prf_gatt_write_ntf_ind(&ctsc_env->prf_env, conidx, cfg_hdl, param->value);
                }
            }
        }
        else
        {
            status = PRF_APP_ERROR;
        }
    }
    else if (state == CTSC_FREE)
    {
        status = GAP_ERR_DISCONNECTED;
    }
    else
    {
        // Another procedure is pending, keep the command for later
        msg_status = KE_MSG_SAVED;
        status = GAP_ERR_NO_ERROR;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        struct ctsc_rsp *rsp = KE_MSG_ALLOC(CTSC_WRITE_DESC_RSP, src_id, dest_id, ctsc_rsp);
        rsp->status = status;
        rsp->char_code = param->desc.char_code;
        // Send the message
        ke_msg_send(rsp);
    }

    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the CTSC_WRITE_DESC_REQ messages.
 * It allows configuration of the peer ntf/stop characteristic for a specified characteristic.
 * @param[in] msgid Id of the message received.
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ctsc_write_char_req_handler(const ke_msg_id_t msgid,
                                       const struct ctsc_write_req *param,
                                       const ke_task_id_t dest_id,
                                       const ke_task_id_t src_id)
{
    struct ctsc_env_tag *ctsc_env = PRF_ENV_GET(CTSC, ctsc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    uint8_t msg_status = KE_MSG_CONSUMED;
    uint8_t state = ke_state_get(dest_id);
    // Buffer big enough to hold every characteristic
    uint8_t data[10];
    uint16_t data_len = 0;
    ke_msg_id_t rsp_msg_id = req2rsp(msgid);

    if (state == CTSC_IDLE)
    {
        if (ctsc_env->env[conidx] != NULL)
        {
            /*
             * In most cases write will have correct attribute specified
             * assume status with no error
             */
            status = GAP_ERR_NO_ERROR;
            switch (msgid)
            {
            case CTSC_WRITE_CT_REQ:
                ctsc_pack_curr_time_value(&((struct ctsc_write_ct_req *) param)->ct_val, data);
                data_len = 10;
                break;
            case CTSC_WRITE_LTI_REQ:
                data[0] = ((struct ctsc_write_lti_req *) param)->lti_val.time_zone;
                data[1] = ((struct ctsc_write_lti_req *) param)->lti_val.dst_offset;
                data_len = 2;
                break;
            default:
                status = PRF_ERR_INVALID_PARAM;
                break;
            }

            if (status == GAP_ERR_NO_ERROR)
            {
                // Get handle of the client configuration
                uint16_t hdl =
                        ctsc_env->env[conidx]->cts.chars[param->char_code].val_hdl;
                // check if the handle value exists
                if (hdl == ATT_INVALID_SEARCH_HANDLE)
                {
                    // Status
                    status = PRF_ERR_INEXISTENT_HDL;
                }
                else
                {
                    /*
                     * Remember requested characteristic for response
                     */
                    ctsc_env->env[conidx]->last_char_code = param->char_code;
                    ctsc_env->env[conidx]->rsp_msg_id = rsp_msg_id;

                    // Go to the Busy state
                    ke_state_set(dest_id, CTSC_BUSY);
                    // Send GATT Write Request
                    prf_gatt_write(&ctsc_env->prf_env, conidx, hdl, data, data_len, GATTC_WRITE);
                }
            }
        }
        else
        {
            status = PRF_APP_ERROR;
        }
    }
    else if (state == CTSC_FREE)
    {
        status = GAP_ERR_DISCONNECTED;
    }
    else
    {
        // Another procedure is pending, keep the command for later
        msg_status = KE_MSG_SAVED;
        status = GAP_ERR_NO_ERROR;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        send_status_rsp(rsp_msg_id, src_id, dest_id, param->char_code, status);
    }

    return msg_status;
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
static int gattc_event_ind_handler(const ke_msg_id_t msgid,
                                   const struct gattc_event_ind *param,
                                   const ke_task_id_t dest_id,
                                   const ke_task_id_t src_id)
{
    uint8_t conidx = KE_IDX_GET(dest_id);
    // Get the address of the environment
    struct ctsc_env_tag *ctsc_env = PRF_ENV_GET(CTSC, ctsc);

    if (ctsc_env != NULL)
    {
        if ((param->handle == ctsc_env->env[conidx]->cts.chars[CTSC_CHAR_CTS_CURRENT_TIME].val_hdl) &&
            (param->type == GATTC_NOTIFY))
        {
            struct ctsc_ct_ind *ind = KE_MSG_ALLOC(CTSC_CT_IND,
                                                   prf_dst_task_get(&ctsc_env->prf_env, conidx),
                                                   prf_src_task_get(&ctsc_env->prf_env, conidx),
                                                   ctsc_ct_ind);

            ind->status = GAP_ERR_NO_ERROR;
            // Unpack current time value
            ctsc_unpack_curr_time_value((uint8_t *) param->value, &ind->ct_val);

            ke_msg_send(ind);
        }
    }

    return KE_MSG_CONSUMED;
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Specified state message handlers for default state
const struct ke_msg_handler ctsc_default_state[] =
{
    {CTSC_ENABLE_REQ,          (ke_msg_func_t) ctsc_enable_req_handler},
    {GATTC_SDP_SVC_IND,        (ke_msg_func_t) gattc_sdp_svc_ind_handler},
    {GATTC_CMP_EVT,            (ke_msg_func_t) gattc_cmp_evt_handler},

    {CTSC_READ_CT_REQ,         (ke_msg_func_t) ctsc_read_req_handler},
    {CTSC_READ_LTI_REQ,        (ke_msg_func_t) ctsc_read_req_handler},
    {CTSC_READ_RTI_REQ,        (ke_msg_func_t) ctsc_read_req_handler},
    {CTSC_READ_DESC_RSP,       (ke_msg_func_t) ctsc_read_req_handler},
    {GATTC_READ_IND,           (ke_msg_func_t) gattc_read_ind_handler},

    {CTSC_WRITE_CT_REQ,        (ke_msg_func_t) ctsc_write_char_req_handler},
    {CTSC_WRITE_LTI_REQ,       (ke_msg_func_t) ctsc_write_char_req_handler},
    {CTSC_WRITE_DESC_REQ,      (ke_msg_func_t) ctsc_write_desc_req_handler},

    {GATTC_EVENT_IND,          (ke_msg_func_t) gattc_event_ind_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler ctsc_default_handler = KE_STATE_HANDLER(ctsc_default_state);

#endif // BLE_CTS_CLIENT

/// @} CTSCTASK
