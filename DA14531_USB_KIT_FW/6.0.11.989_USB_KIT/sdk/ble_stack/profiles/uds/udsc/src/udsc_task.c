/**
 ****************************************************************************************
 *
 * @file udsc_task.c
 *
 * @brief C file - User Data Service Client Role Task Implementation.
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
 * @addtogroup UDSCTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_UDS_CLIENT)

#include <string.h>

#include "gap.h"
#include "udsc.h"
#include "udsc_task.h"
#include "gattc_task.h"
#include "prf_utils.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Length of the maximum User Control Point value to write
#define UDS_UCP_REQ_MAX_LEN     4

/// Current db operation state flags
#define UDS_DB_OP_READ          (0x01)
#define UDS_DB_OP_WRITE         (0x02)
#define UDS_DB_OP_FINALISING    (0x04)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Used to retrieve User Data Service characteristics information
const struct prf_char_def udsc_uds_char[UDSC_CHAR_MAX] =
{
    // Optionally UDSC_CHAR_USER_DB_CHANGE_INCR can have ATT_CHAR_PROP_NTF
    [UDSC_CHAR_USER_DB_CHANGE_INCR]         = {ATT_CHAR_UDS_USER_DB_CHANGE_INCR,
                                               ATT_MANDATORY,
                                               ATT_CHAR_PROP_RD | ATT_CHAR_PROP_WR},
    [UDSC_CHAR_USER_INDEX]                  = {ATT_CHAR_UDS_USER_INDEX,
                                               ATT_MANDATORY,
                                               ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_CONTROl_POINT]          = {ATT_CHAR_UDS_USER_CTRL_PT,
                                               ATT_MANDATORY,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_IND},
    // Optional characteristics
    [UDSC_CHAR_USER_AEROBIC_HR_LOW_LIM]     = {ATT_CHAR_UDS_USER_AEROBIC_HR_LOW_LIM,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_AEROBIC_THRESHOLD]      = {ATT_CHAR_UDS_USER_AEROBIC_THRESHOLD,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_AGE]                    = {ATT_CHAR_UDS_USER_AGE,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_ANAEROBIC_HR_LOW_LIM]   = {ATT_CHAR_UDS_USER_ANAEROBIC_HR_LOW_LIM,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_ANAEROBIC_HR_UP_LIM]    = {ATT_CHAR_UDS_USER_ANAEROBIC_HR_UP_LIM,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_ANAEROBIC_THRESHOLD]    = {ATT_CHAR_UDS_USER_ANAEROBIC_THRESHOLD,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_AEROBIC_HR_UP_LIM]      = {ATT_CHAR_UDS_USER_AEROBIC_HR_UP_LIM,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_DATE_OF_BIRTH]          = {ATT_CHAR_UDS_USER_DATE_OF_BIRTH,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_DATE_OF_THRESHOLD_ASS]  = {ATT_CHAR_UDS_USER_DATE_OF_THRESHOLD_ASS,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_EMAIL_ADDR]             = {ATT_CHAR_UDS_USER_EMAIL_ADDR,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_FAT_BURN_HR_LOW_LIM]    = {ATT_CHAR_UDS_USER_FAT_BURN_HR_LOW_LIM,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_FAT_BURN_HR_UP_LIM]     = {ATT_CHAR_UDS_USER_FAT_BURN_HR_UP_LIM,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_FIRST_NAME]             = {ATT_CHAR_UDS_USER_FIRST_NAME,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_5ZONE_HR_LIM]           = {ATT_CHAR_UDS_USER_5ZONE_HR_LIM,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_GENDER]                 = {ATT_CHAR_UDS_USER_GENDER,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_HEART_RATE_MAX]         = {ATT_CHAR_UDS_USER_HEART_RATE_MAX,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_HEIGHT]                 = {ATT_CHAR_UDS_USER_HEIGHT,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_HIP_CIRCUMFERENCE]      = {ATT_CHAR_UDS_USER_HIP_CIRCUMFERENCE,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_LAST_NAME]              = {ATT_CHAR_UDS_USER_LAST_NAME,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_MAX_REC_HEART_RATE]     = {ATT_CHAR_UDS_USER_MAX_REC_HEART_RATE,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_RESTING_HEART_RATE]     = {ATT_CHAR_UDS_USER_RESTING_HEART_RATE,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_THRESHOLDS_SPORT_TYPE]  = {ATT_CHAR_UDS_USER_THRESHOLDS_SPORT_TYPE,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_3ZONE_HR_LIM]           = {ATT_CHAR_UDS_USER_3ZONE_HR_LIM,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_2ZONE_HR_LIM]           = {ATT_CHAR_UDS_USER_2ZONE_HR_LIM,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_VO2_MAX]                = {ATT_CHAR_UDS_USER_VO2_MAX,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_WAIST_CIRCUMFERENCE]    = {ATT_CHAR_UDS_USER_WAIST_CIRCUMFERENCE,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_WEIGHT]                 = {ATT_CHAR_UDS_USER_WEIGHT,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
    [UDSC_CHAR_USER_LANGUAGE]               = {ATT_CHAR_UDS_USER_LANGUAGE,
                                               ATT_OPTIONAL,
                                               ATT_CHAR_PROP_WR | ATT_CHAR_PROP_RD},
};

/// Used to retrieve User Data Service characteristics descriptors information
const struct prf_char_desc_def udsc_uds_char_desc[UDSC_DESC_MAX] =
{
    [UDSC_DESC_DB_CHANGE_INCR_CFG]  = {ATT_DESC_CLIENT_CHAR_CFG,
                                       ATT_OPTIONAL,
                                       UDSC_CHAR_USER_DB_CHANGE_INCR},
    [UDSC_DESC_CNTL_POINT_CFG]      = {ATT_DESC_CLIENT_CHAR_CFG,
                                       ATT_MANDATORY,
                                       UDSC_CHAR_USER_CONTROl_POINT},
};

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSC_ENABLE_REQ message.
 * The handler enables the User Data Service Client Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udsc_enable_req_handler(ke_msg_id_t const msgid,
                                   struct udsc_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if (state == UDSC_IDLE)
    {
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t state = ke_state_get(dest_id);

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        udsc_env->env[conidx] = (struct udsc_cnx_env*) ke_malloc(sizeof(struct udsc_cnx_env), KE_MEM_ENV);
        memset(udsc_env->env[conidx], 0, sizeof(struct udsc_cnx_env));

        // Config connection, start discovering
        if (param->con_type == PRF_CON_DISCOVERY)
        {
            // Start discovering UDS on peer
            prf_disc_svc_send(&(udsc_env->prf_env), conidx, ATT_SVC_USER_DATA);

            udsc_env->env[conidx]->last_uuid_req = ATT_SVC_USER_DATA;

            // Go to DISCOVERING state
            ke_state_set(dest_id, UDSC_DISCOVERING);
        }
        // Normal connection, get saved att details
        else
        {
            udsc_env->env[conidx]->uds = param->uds;

            // Send confirmation to APP and stay in IDLE state
            udsc_enable_rsp_send(udsc_env, conidx, GAP_ERR_NO_ERROR);
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
    uint8_t state = ke_state_get(dest_id);

    if (state == UDSC_DISCOVERING)
    {
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        if(udsc_env->env[conidx]->nb_svc == 0)
        {
            // Retrieve UDS characteristics and descriptors
            prf_extract_svc_info(ind,
                                 UDSC_CHAR_MAX, udsc_uds_char, udsc_env->env[conidx]->uds.chars,
                                 UDSC_DESC_MAX,udsc_uds_char_desc, udsc_env->env[conidx]->uds.descs);

            // Even if we get multiple responses we only store 1-st range
            udsc_env->env[conidx]->uds.svc.shdl = ind->start_hdl;
            udsc_env->env[conidx]->uds.svc.ehdl = ind->end_hdl;
        }

        udsc_env->env[conidx]->nb_svc++;
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

    if (state == UDSC_DISCOVERING)
    {
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
        uint8_t conidx = KE_IDX_GET(dest_id);
        uint8_t status = param->status;

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        if ((param->status == ATT_ERR_ATTRIBUTE_NOT_FOUND)||
            (param->status == ATT_ERR_NO_ERROR))
        {
            if(udsc_env->env[conidx]->nb_svc == 1)
            {
                status = prf_check_svc_char_validity(UDSC_CHAR_MAX,
                                                     udsc_env->env[conidx]->uds.chars,
                                                     udsc_uds_char);
            }
            // too much services
            else if (udsc_env->env[conidx]->nb_svc > 1)
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
                status = prf_check_svc_char_desc_validity(UDSC_DESC_MAX,
                                                          udsc_env->env[conidx]->uds.descs,
                                                          udsc_uds_char_desc,
                                                          udsc_env->env[conidx]->uds.chars);
            }
        }

        // Send response and go to IDLE state
        udsc_enable_rsp_send(udsc_env, conidx, status);
        ke_state_set(dest_id, UDSC_IDLE);
    }
    else if (state == UDSC_BUSY)
    {
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        switch (param->operation)
        {
            case GATTC_READ:
            {
                if ((param->status != GAP_ERR_NO_ERROR) && \
                    (udsc_env->env[conidx]->db_op_state & UDS_DB_OP_READ))
                {
                    udsc_env->env[conidx]->db_op_state &= ~UDS_DB_OP_READ;

                    // An error occurs while reading peer device attribute
                    udsc_read_char_val_rsp_send(udsc_env, conidx, param->status,
                                                UDSC_CHAR_MAX, 0, NULL);
                }
                // Do nothing on success as GATTC_READ_IND handler will be called
            }
            break;

            case GATTC_WRITE:
            case GATTC_WRITE_NO_RESPONSE:
            {
                if (udsc_env->env[conidx]->op_code != UDS_NO_OP)
                {
                    // Notify right away in case of failure
                    if (param->status != GAP_ERR_NO_ERROR)
                    {
                        struct udsc_ucp_op_cfm *cfm = KE_MSG_ALLOC(UDSC_UCP_OP_CFM,
                                                                   prf_dst_task_get(&(udsc_env->prf_env), conidx),
                                                                   prf_src_task_get(&(udsc_env->prf_env), conidx),
                                                                   udsc_ucp_op_cfm);

                        cfm->status = param->status;

                        cfm->cfm.req_op_code = udsc_env->env[conidx]->op_code;
                        cfm->cfm.rsp_val = UDS_RSP_OPERATION_FAILED;
                        cfm->cfm.op_code = UDS_REQ_RSP_CODE;

                        ke_msg_send(cfm);

                        // Clear ongoing operation indicator on failure
                        udsc_env->env[conidx]->op_code = UDS_NO_OP;
                    }
                    else
                    {
                        // On UCP write start timeout procedure
                        ke_timer_set(UDSC_TIMEOUT_TIMER_IND, dest_id, ATT_TRANS_RTX);
                    }
                }
                // Regular characteristic or DB Change Increment char. write confirmation
                else if (udsc_env->env[conidx]->db_op_state)
                {
                    if (udsc_env->env[conidx]->db_op_state & UDS_DB_OP_FINALISING)
                    {
                        udsc_set_char_val_cfm_send(udsc_env, conidx, param->status);
                        udsc_env->env[conidx]->db_op_state &= ~UDS_DB_OP_FINALISING;
                    } else if (udsc_env->env[conidx]->db_op_state & UDS_DB_OP_WRITE)
                    {
                        // Notify regular write requests (not the internal db change incr. write)
                        udsc_set_char_val_cfm_send(udsc_env, conidx, param->status);
                        udsc_env->env[conidx]->db_op_state &= ~UDS_DB_OP_WRITE;
                    }
                }
                // It is descriptor write confirmation or write_begin_req confirmation
                else
                {
                    udsc_cmp_evt_ind_send(udsc_env, conidx, param->status);
                }
            }
            break;

            default:
            {
                // Do nothing
            }
            break;
        }

        ke_state_set(dest_id, UDSC_IDLE);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_IND message.
 * Generic event received after every single read command sent to peer server.
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

    if (state == UDSC_BUSY)
    {
        // Get the address of the environment
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        // Store db_change value
        if ((udsc_env->env[conidx]->last_char_code == UDSC_CHAR_USER_DB_CHANGE_INCR) &&
            (param->length >= sizeof(udsc_env->env[conidx]->db_change_incr)))
        {
                memcpy(&udsc_env->env[conidx]->db_change_incr, param->value,
                       sizeof(udsc_env->env[conidx]->db_change_incr));
        }

        if (udsc_env->env[conidx]->db_op_state & UDS_DB_OP_READ)
        {
            udsc_read_char_val_rsp_send(udsc_env, conidx, GAP_ERR_NO_ERROR,
                                        udsc_env->env[conidx]->last_char_code,
                                        param->length, param->value);

            udsc_env->env[conidx]->db_op_state &= ~UDS_DB_OP_READ;
        }
        else
        {
            udsc_cmp_evt_ind_send(udsc_env, conidx, GAP_ERR_NO_ERROR);
        }

        ke_state_set(dest_id, UDSC_IDLE);
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
    // Get the address of the environment
    struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
    uint8_t conidx = KE_IDX_GET(dest_id);

    ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
    ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

    switch (param->type)
    {
        case GATTC_NOTIFY:
        {
            // Handle DB Change Characteristic value notification
            if (param->length >= sizeof(udsc_env->env[conidx]->db_change_incr))
            {
                struct udsc_db_change_ntf *ntf = KE_MSG_ALLOC(UDSC_DB_CHANGE_NTF,
                                                              prf_dst_task_get(&(udsc_env->prf_env), conidx),
                                                              prf_src_task_get(&(udsc_env->prf_env), conidx),
                                                              udsc_db_change_ntf);

                memcpy(&udsc_env->env[conidx]->db_change_incr, param->value,
                                            sizeof(udsc_env->env[conidx]->db_change_incr));

                ntf->incr_val = udsc_env->env[conidx]->db_change_incr;

                ke_msg_send(ntf);
            }
        }
        break;

        case GATTC_INDICATE:
        {
            // Stop the UCP operation timeout timer
            if (udsc_env->env[conidx]->op_code != UDS_NO_OP)
            {
                struct gattc_event_cfm * cfm = KE_MSG_ALLOC(GATTC_EVENT_CFM, src_id,
                                                            dest_id, gattc_event_cfm);

                cfm->handle = param->handle;
                ke_msg_send(cfm);

                // At least resp_op_code, req_op_code & resp_val are required
                if (param->length >= 3)
                {
                    struct udsc_ucp_op_cfm *cfm;

                    cfm = KE_MSG_ALLOC(UDSC_UCP_OP_CFM,
                                       prf_dst_task_get(&(udsc_env->prf_env), conidx),
                                       prf_src_task_get(&(udsc_env->prf_env), conidx),
                                       udsc_ucp_op_cfm);

                    cfm->status = GAP_ERR_NO_ERROR;

                    cfm->cfm.op_code = param->value[0];
                    cfm->cfm.req_op_code = param->value[1];
                    cfm->cfm.rsp_val = param->value[2];

                    // The only expected parameter (if any) is the user index byte
                    if ((param->length > 3) &&
                            (cfm->cfm.req_op_code == UDS_REQ_REG_NEW_USER) &&
                            (cfm->cfm.rsp_val == UDS_RSP_SUCCESS))
                    {
                        cfm->cfm.parameter.reg_new_user.user_idx = param->value[3];
                    }

                    ke_msg_send(cfm);
                }

                ke_timer_clear(UDSC_TIMEOUT_TIMER_IND, dest_id);
                udsc_env->env[conidx]->op_code = UDS_NO_OP;
            }
        }
        break;

        default:
        {
            ASSERT_ERROR(0);
        }
        break;
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSC_READ_CHAR_VAL_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udsc_read_char_val_req_handler(ke_msg_id_t const msgid,
                                          struct udsc_read_char_val_req const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if (state == UDSC_IDLE)
    {
        // Get the address of the environment
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        // This function should not be called for reading UCP
        if (param->char_code == UDSC_CHAR_USER_CONTROl_POINT)
        {
            udsc_read_char_val_rsp_send(udsc_env, conidx, PRF_ERR_INVALID_PARAM,
                                        UDSC_CHAR_MAX, 0, NULL);
            return (KE_MSG_CONSUMED);
        }

        if (udsc_env->env[conidx]->uds.chars[param->char_code].val_hdl ==
                                                        ATT_INVALID_SEARCH_HANDLE)
        {
            udsc_read_char_val_rsp_send(udsc_env, conidx, PRF_ERR_INEXISTENT_HDL,
                                        UDSC_CHAR_MAX, 0, NULL);
        }
        else
        {
            udsc_env->env[conidx]->last_char_code = param->char_code;
            udsc_env->env[conidx]->db_op_state |= UDS_DB_OP_READ;

            // Send GATT Read Request
            prf_read_char_send(&udsc_env->prf_env, conidx,
                               udsc_env->env[conidx]->uds.svc.shdl,
                               udsc_env->env[conidx]->uds.svc.ehdl,
                               udsc_env->env[conidx]->uds.chars[param->char_code].val_hdl);
            // On failure - GATTC_CMP_EVT will come with req_type == GATTC_READ
            // On success - GATTC_READ_IND will come

            ke_state_set(dest_id, UDSC_BUSY);
        }
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSC_SET_CHAR_VAL_BEGIN_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udsc_set_char_val_begin_req_handler(ke_msg_id_t const msgid,
                                               void const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if (state == UDSC_IDLE)
    {
        // Get the address of the environment
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        // Trigger db_change_incr read
        if (udsc_env->env[conidx]->uds.chars[UDSC_CHAR_USER_DB_CHANGE_INCR].val_hdl !=
                                                                ATT_INVALID_SEARCH_HANDLE)
        {
            udsc_env->env[conidx]->last_char_code = UDSC_CHAR_USER_DB_CHANGE_INCR;

            // Send GATT Read Request
            prf_read_char_send(&udsc_env->prf_env, conidx,
                               udsc_env->env[conidx]->uds.svc.shdl,
                               udsc_env->env[conidx]->uds.svc.ehdl,
                               udsc_env->env[conidx]->uds.chars[UDSC_CHAR_USER_DB_CHANGE_INCR].val_hdl);
            // On failure - GATTC_CMP_EVT will come with req_type == GATTC_READ
            // On success - GATTC_READ_IND will come

            ke_state_set(dest_id, UDSC_BUSY);
        }
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSC_SET_CHAR_VAL_END_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udsc_set_char_val_end_req_handler(ke_msg_id_t const msgid,
                                             void const *param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if (state == UDSC_IDLE)
    {
        // Get the address of the environment
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        if (udsc_env->env[conidx]->db_change_incr == 0)
        {
            udsc_set_char_val_cfm_send(udsc_env, conidx, UDS_ERROR_IMPROPERLY_CONFIGURED);
            return (KE_MSG_CONSUMED);
        }

        udsc_env->env[conidx]->db_op_state |= UDS_DB_OP_FINALISING;
        udsc_env->env[conidx]->db_change_incr++;

        prf_gatt_write(&udsc_env->prf_env, conidx,
                       udsc_env->env[conidx]->uds.chars[UDSC_CHAR_USER_DB_CHANGE_INCR].val_hdl,
                       (uint8_t *) &udsc_env->env[conidx]->db_change_incr,
                       sizeof(udsc_env->env[conidx]->db_change_incr), GATTC_WRITE);
        // GATTC_CMP_EVT will come with req_type == GATTC_WRITE

        ke_state_set(dest_id, UDSC_BUSY);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSC_READ_CHAR_VAL_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udsc_set_char_val_req_handler(ke_msg_id_t const msgid,
                                         struct udsc_set_char_val_req const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if (state == UDSC_IDLE)
    {
        // Get the address of the environment
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        // If its the first write - we need to know the db_change_incr value.
        // It is stored on read or when notified (the App needs to register for it)
        if (udsc_env->env[conidx]->db_change_incr == 0)
        {
            udsc_set_char_val_cfm_send(udsc_env, conidx, UDS_ERROR_IMPROPERLY_CONFIGURED);
            return (KE_MSG_CONSUMED);
        }

        // This function should not be called for writing opcodes to UCP nor for
        // incrementing db change increment characteristic as it is done automatically
        if ((param->char_code <= UDSC_CHAR_USER_CONTROl_POINT) ||
            (param->char_code == UDSC_CHAR_USER_DB_CHANGE_INCR))
        {
            udsc_set_char_val_cfm_send(udsc_env, conidx, PRF_ERR_INVALID_PARAM);
            return (KE_MSG_CONSUMED);
        }

        if (udsc_env->env[conidx]->uds.chars[param->char_code].val_hdl ==
                                                        ATT_INVALID_SEARCH_HANDLE)
        {
            udsc_set_char_val_cfm_send(udsc_env, conidx, PRF_ERR_INEXISTENT_HDL);
        }
        else
        {
            udsc_env->env[conidx]->db_op_state = UDS_DB_OP_WRITE;
            udsc_env->env[conidx]->last_char_code = param->char_code;

            // Send GATT Write Request
            prf_gatt_write(&udsc_env->prf_env, conidx,
                           udsc_env->env[conidx]->uds.chars[param->char_code].val_hdl,
                           (uint8_t *) &param->val, param->val_len, GATTC_WRITE);
            // GATTC_CMP_EVT will come with req_type == GATTC_WRITE

            ke_state_set(dest_id, UDSC_BUSY);
        }
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSC_TIMEOUT_TIMER_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udsc_timeout_timer_ind_handler(ke_msg_id_t const msgid,
                                          void const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    // Get the address of the environment
    struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);
    uint8_t conidx = KE_IDX_GET(dest_id);

    ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
    ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

    ASSERT_ERROR(udsc_env->env[conidx]->op_code != UDS_NO_OP);

    udsc_env->env[conidx]->op_code = UDS_NO_OP;

    // Send the complete event message with error
    udsc_cmp_evt_ind_send(udsc_env, conidx, PRF_ERR_PROC_TIMEOUT);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSC_NTFIND_CFG_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udsc_ntfind_cfg_req_handler(ke_msg_id_t const msgid,
                                       struct udsc_ntfind_cfg_req const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if (state == UDSC_IDLE)
    {
        uint16_t cfg_hdl = 0x0000;
        uint16_t allowed_cfg_val = 0x0000;
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);

        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        cfg_hdl = udsc_env->env[conidx]->uds.descs[param->desc_code].desc_hdl;

        if (param->desc_code == UDSC_DESC_DB_CHANGE_INCR_CFG)
        {
            allowed_cfg_val = PRF_CLI_START_NTF;
        }
        else if (param->desc_code == UDSC_DESC_CNTL_POINT_CFG)
        {
            allowed_cfg_val = PRF_CLI_START_IND;
        }
        else
        {
            udsc_cmp_evt_ind_send(udsc_env, conidx, PRF_ERR_INVALID_PARAM);
        }

        if(!((param->cfg_val == PRF_CLI_STOP_NTFIND) ||
                                            (param->cfg_val == allowed_cfg_val)))
        {
            udsc_cmp_evt_ind_send(udsc_env, conidx, PRF_ERR_INVALID_PARAM);
        }
        else if (cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
        {
            prf_gatt_write_ntf_ind(&udsc_env->prf_env, conidx, cfg_hdl, param->cfg_val);
            ke_state_set(dest_id, UDSC_BUSY);
        }
        else
        {
            udsc_cmp_evt_ind_send(udsc_env, conidx, PRF_ERR_INEXISTENT_HDL);
        }
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Helper function for packing UCP request structure.
 * @param[in] req Request from the App.
 * @param[in] buf Output buffer.
 * @return length
 ****************************************************************************************
 */
static uint8_t uds_pack_ucp_req(struct uds_ucp_req const *req, uint8_t *buf)
{
    uint8_t length = 1;

    buf[0] = req->op_code;

    switch (req->op_code)
    {
        case UDS_REQ_REG_NEW_USER:
        {
            co_write16p(&buf[1], req->parameter.reg_new_user.consent_code);
            length += 2;

        }
        break;

        case UDS_REQ_CONSENT:
        {
            buf[1] = req->parameter.consent.user_idx;
            co_write16p(&buf[2], req->parameter.consent.consent_code);
            length += 3;

        }
        break;

        case UDS_REQ_DEL_USER_DATA:
        default:
        {
            /// Do nothing
        }
        break;
    }

    return length;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref UDSC_UCP_OP_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int udsc_ucp_op_req_handler(ke_msg_id_t const msgid,
                                   struct udsc_ucp_op_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    uint8_t state = ke_state_get(dest_id);

    if (state == UDSC_IDLE)
    {
        struct udsc_env_tag *udsc_env = PRF_ENV_GET(UDSC, udsc);

        uint8_t conidx = KE_IDX_GET(dest_id);

        ASSERT_INFO(udsc_env != NULL, dest_id, src_id);
        ASSERT_INFO(udsc_env->env[conidx] != NULL, dest_id, src_id);

        if (udsc_env->env[conidx]->op_code != UDS_NO_OP)
        {
            udsc_cmp_evt_ind_send(udsc_env, conidx, PRF_ERR_REQ_DISALLOWED);
        }
        else
        {
            uint8_t req[UDS_UCP_REQ_MAX_LEN];
            uint8_t nb;

            // Store the opcode for UCP operation
            udsc_env->env[conidx]->op_code = param->req.op_code;

            nb = uds_pack_ucp_req(&param->req, &req[0]);

            // Send write request
            prf_gatt_write(&udsc_env->prf_env, conidx,
                           udsc_env->env[conidx]->uds.chars[UDSC_CHAR_USER_CONTROl_POINT].val_hdl,
                           (uint8_t *)&req[0], nb, GATTC_WRITE);
            ke_state_set(dest_id, UDSC_BUSY);
        }
    }

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler udsc_default_state[] =
{
    {GATTC_EVENT_IND,               (ke_msg_func_t)gattc_event_ind_handler},
    {GATTC_EVENT_REQ_IND,           (ke_msg_func_t)gattc_event_ind_handler},
    {UDSC_TIMEOUT_TIMER_IND,        (ke_msg_func_t)udsc_timeout_timer_ind_handler},
    {UDSC_ENABLE_REQ,               (ke_msg_func_t)udsc_enable_req_handler},
    {UDSC_READ_CHAR_VAL_REQ,        (ke_msg_func_t)udsc_read_char_val_req_handler},
    {UDSC_SET_CHAR_VAL_BEGIN_REQ,   (ke_msg_func_t)udsc_set_char_val_begin_req_handler},
    {UDSC_SET_CHAR_VAL_REQ,         (ke_msg_func_t)udsc_set_char_val_req_handler},
    {UDSC_SET_CHAR_VAL_END_REQ,     (ke_msg_func_t)udsc_set_char_val_end_req_handler},
    {UDSC_UCP_OP_REQ,               (ke_msg_func_t)udsc_ucp_op_req_handler},
    {UDSC_NTFIND_CFG_REQ,           (ke_msg_func_t)udsc_ntfind_cfg_req_handler},
    {GATTC_SDP_SVC_IND,             (ke_msg_func_t)gattc_sdp_svc_ind_handler},
    {GATTC_CMP_EVT,                 (ke_msg_func_t)gattc_cmp_evt_handler},
    {GATTC_READ_IND,                (ke_msg_func_t)gattc_read_ind_handler}
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler udsc_default_handler = KE_STATE_HANDLER(udsc_default_state);

#endif // BLE_UDS_CLIENT

/// @} UDSCTASK
