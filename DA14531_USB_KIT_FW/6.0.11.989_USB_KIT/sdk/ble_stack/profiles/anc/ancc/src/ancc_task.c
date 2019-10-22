/**
 ****************************************************************************************
 *
 * @file ancc_task.c
 *
 * @brief Apple Notification Center Service Client Task implementation.
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
 * @addtogroup ANCCTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_ANC_CLIENT)

#include "gap.h"
#include "ancc.h"
#include "ancc_task.h"
#include "gattc_task.h"
#include "prf_utils_128.h"

/// ANCS UUID
const struct att_uuid_128 ancc_anc_svc = {{0xD0, 0x00, 0x2D, 0x12, 0x1E, 0x4B, 0x0F, 0xA4, 0x99, 0x4E, 0xCE, 0xB5, 0x31, 0xF4, 0x05, 0x79}};

/// State machine used to retrieve ANCS characteristics information
const struct prf_char_def_128 ancc_anc_char[ANCC_CHAR_MAX] =
{
    /// Notification Source
    [ANCC_CHAR_NTF_SRC]  =  {{0xBD, 0x1D, 0xA2, 0x99, 0xE6, 0x25, 0x58, 0x8C, 0xD9, 0x42, 0x01, 0x63, 0x0D, 0x12, 0xBF, 0x9F},
                            ATT_MANDATORY,
                            ATT_CHAR_PROP_NTF},

    /// Control Point
    [ANCC_CHAR_CTRL_PT]  =  {{0xD9, 0xD9, 0xAA, 0xFD, 0xBD, 0x9B, 0x21, 0x98, 0xA8, 0x49, 0xE1, 0x45, 0xF3, 0xD8, 0xD1, 0x69},
                            ATT_OPTIONAL,
                            ATT_CHAR_PROP_WR},

    /// Data Source
    [ANCC_CHAR_DATA_SRC] =  {{0xFB, 0x7B, 0x7C, 0xCE, 0x6A, 0xB3, 0x44, 0xBE, 0xB5, 0x4B, 0xD6, 0x24, 0xE9, 0xC6, 0xEA, 0x22},
                            ATT_OPTIONAL,
                            ATT_CHAR_PROP_NTF},
};

/// State machine used to retrieve ANCS characteristic descriptor information
const struct prf_char_desc_def ancc_anc_char_desc[ANCC_DESC_MAX] =
{
    /// Notification Source Char. - Client Characteristic Configuration
    [ANCC_DESC_NTF_SRC_CL_CFG]   = {ATT_DESC_CLIENT_CHAR_CFG,
                                   ATT_MANDATORY,
                                   ANCC_CHAR_NTF_SRC},
    /// Data Source Char. - Client Characteristic Configuration
    [ANCC_DESC_DATA_SRC_CL_CFG] = {ATT_DESC_CLIENT_CHAR_CFG,
                                   ATT_OPTIONAL,
                                   ANCC_CHAR_DATA_SRC},
};

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref ANCC_ENABLE_REQ message.
 * The handler enables the ANCS Client Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ancc_enable_req_handler(ke_msg_id_t const msgid,
                                   struct ancc_enable_req const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t state = ke_state_get(dest_id);

    ASSERT_ERROR(ancc_env != NULL);

    ancc_env->env[conidx] = (struct ancc_cnx_env*) ke_malloc(sizeof(struct ancc_cnx_env), KE_MEM_ENV);
    memset(ancc_env->env[conidx], 0, sizeof(struct ancc_cnx_env));

    // Start discovering
    if (param->con_type == PRF_CON_DISCOVERY)
    {
        prf_disc_svc_sdp_send_128(&(ancc_env->prf_env), conidx, ancc_anc_svc.uuid);
        ancc_env->env[conidx]->last_uuid_req = ATT_DECL_PRIMARY_SERVICE;
        ke_state_set(dest_id, ANCC_DISCOVERING);
    }
    else
    {
        ancc_env->env[conidx]->anc = param->anc;
        ancc_enable_rsp_send(ancc_env, conidx, GAP_ERR_NO_ERROR);
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
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);

    ASSERT_ERROR(ancc_env != NULL);
    ASSERT_ERROR(ancc_env->env[conidx] != NULL);

    if (ancc_env->env[conidx]->nb_svc == 0)
    {
        // Retrieve ANCS characteristics and descriptors
        prf_extract_svc_sdp_info_128(ind,
                                     ANCC_CHAR_MAX, ancc_anc_char, ancc_env->env[conidx]->anc.chars,
                                     ANCC_DESC_MAX, ancc_anc_char_desc, ancc_env->env[conidx]->anc.descs);

        // Even if we get multiple responses we only store first range
        ancc_env->env[conidx]->anc.svc.shdl = ind->start_hdl;
        ancc_env->env[conidx]->anc.svc.ehdl = ind->end_hdl;
    }

    ancc_env->env[conidx]->nb_svc++;

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
static int gattc_cmp_evt_handler_discovering(ke_msg_id_t const msgid,
                                             struct gattc_cmp_evt const *param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = param->status;

    ASSERT_ERROR(ancc_env != NULL);
    ASSERT_ERROR(ancc_env->env[conidx] != NULL);

    if ((param->status == ATT_ERR_ATTRIBUTE_NOT_FOUND)||
        (param->status == ATT_ERR_NO_ERROR))
    {
        if (ancc_env->env[conidx]->nb_svc == 1)
        {
            status = prf_check_svc_char_validity_128(ANCC_CHAR_MAX,
                                                     ancc_env->env[conidx]->anc.chars,
                                                     ancc_anc_char);
        }
        else if (ancc_env->env[conidx]->nb_svc > 1)
        {
            status = PRF_ERR_MULTIPLE_SVC;
        }
        else
        {
            status = PRF_ERR_STOP_DISC_CHAR_MISSING;
        }

        if (status == GAP_ERR_NO_ERROR)
        {
            status = prf_check_svc_char_desc_validity(ANCC_DESC_MAX,
                                                      ancc_env->env[conidx]->anc.descs,
                                                      ancc_anc_char_desc,
                                                      ancc_env->env[conidx]->anc.chars);
        }
    }
    // Send response and go to IDLE state
    ancc_enable_rsp_send(ancc_env, conidx, status);
    ke_state_set(dest_id, ANCC_IDLE);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref ANCC_WR_CFG_NTF_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ancc_wr_cfg_ntf_req_handler(ke_msg_id_t const msgid,
                                       struct ancc_wr_cfg_ntf_req const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    uint16_t cfg_hdl = ATT_INVALID_SEARCH_HANDLE;
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;

    if ((param->cfg_val == PRF_CLI_STOP_NTFIND) ||
        (param->cfg_val == PRF_CLI_START_NTF))
    {
        if (param->op_code == ANCC_WR_NTF_NTF_SRC_OP_CODE)
        {
            cfg_hdl = ancc_env->env[conidx]->anc.descs[ANCC_DESC_NTF_SRC_CL_CFG].desc_hdl;
        }
        else if (param->op_code == ANCC_WR_NTF_DATA_SRC_OP_CODE)
        {
            cfg_hdl = ancc_env->env[conidx]->anc.descs[ANCC_DESC_DATA_SRC_CL_CFG].desc_hdl;
        }

        if (cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
        {
            prf_gatt_write_ntf_ind(&ancc_env->prf_env, conidx, cfg_hdl, param->cfg_val);
            ancc_env->env[conidx]->last_op_code = param->op_code;
            ke_state_set(dest_id, ANCC_BUSY);
            status = GAP_ERR_NO_ERROR;
        }
        else
        {
            status = PRF_ERR_INEXISTENT_HDL;
        }
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        ancc_wr_cfg_ntf_rsp_send(ancc_env, conidx, param->op_code, status);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref ANCC_RD_CFG_NTF_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ancc_rd_cfg_ntf_req_handler(ke_msg_id_t const msgid,
                                       struct ancc_rd_cfg_ntf_req const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    uint16_t cfg_hdl = ATT_INVALID_SEARCH_HANDLE;
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;


    if (param->op_code == ANCC_RD_NTF_NTF_SRC_OP_CODE)
    {
        cfg_hdl = ancc_env->env[conidx]->anc.descs[ANCC_DESC_NTF_SRC_CL_CFG].desc_hdl;
    }
    else if (param->op_code == ANCC_RD_NTF_DATA_SRC_OP_CODE)
    {
        cfg_hdl = ancc_env->env[conidx]->anc.descs[ANCC_DESC_DATA_SRC_CL_CFG].desc_hdl;
    }

    if (cfg_hdl != ATT_INVALID_SEARCH_HANDLE)
    {
        prf_read_char_send(&ancc_env->prf_env, conidx,
                           ancc_env->env[conidx]->anc.svc.shdl,
                           ancc_env->env[conidx]->anc.svc.ehdl,
                           cfg_hdl);

        ancc_env->env[conidx]->last_op_code = param->op_code;
        ke_state_set(dest_id, ANCC_BUSY);
        status = GAP_ERR_NO_ERROR;
    }
    else
    {
        status = PRF_ERR_INEXISTENT_HDL;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        ancc_rd_cfg_ntf_rsp_send(ancc_env, conidx, param->op_code, 0, status);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref ANCC_GET_NTF_ATTS_CMD_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ancc_get_ntf_atts_cmd_req_handler(ke_msg_id_t const msgid,
                                             struct ancc_get_ntf_atts_cmd_req *param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    uint8_t msg_status = KE_MSG_CONSUMED;
    uint8_t buffer[ANCC_GET_ATT_CMD_MAX_LEN];
    uint8_t length = 0;
    uint16_t handle = ancc_env->env[conidx]->anc.chars[ANCC_CHAR_CTRL_PT].val_hdl;

    if (handle != ATT_INVALID_HANDLE)
    {
        buffer[length] = CMD_ID_GET_NTF_ATTS;
        length += 1;
        co_write32p(&buffer[length], param->ntf_uid);
        length += 4;

        if (param->atts & NTF_ATT_ID_APP_ID_PRESENT)
        {
            // App Identifier
            buffer[length] = NTF_ATT_ID_APP_ID;
            length += 1;
        }
        if (param->atts & NTF_ATT_ID_TITLE_PRESENT)
        {
            // Title (Needs to be followed by a 2-byte max length parameter)
            buffer[length] = NTF_ATT_ID_TITLE;
            length += 1;
            co_write16p(&buffer[length], param->title_len);
            length += 2;
        }
        if (param->atts & NTF_ATT_ID_SUBTITLE_PRESENT)
        {
            // Subtitle (Needs to be followed by a 2-byte max length parameter)
            buffer[length] = NTF_ATT_ID_SUBTITLE;
            length += 1;
            co_write16p(&buffer[length], param->subtitle_len);
            length += 2;
        }
        if (param->atts &  NTF_ATT_ID_MSG_PRESENT)
        {
            // Message (Needs to be followed by a 2-byte max length parameter)
            buffer[length] = NTF_ATT_ID_MSG;
            length += 1;
            co_write16p(&buffer[length], param->message_len);
            length += 2;
        }
        if (param->atts &  NTF_ATT_ID_MSG_SIZE_PRESENT)
        {
            // Message Size
            buffer[length] = NTF_ATT_ID_MSG_SIZE;
            length += 1;
        }
        if (param->atts &  NTF_ATT_ID_DATE_PRESENT)
        {
            // Date
            buffer[length] = NTF_ATT_ID_DATE;
            length += 1;
        }

        if(length < 6)
        {
            // Check if no attributes have been requested.
            status = PRF_ERR_INVALID_PARAM;
        }
        else
        {
            status = GAP_ERR_NO_ERROR;
        }
    }
    else
    {
        status = PRF_ERR_INEXISTENT_HDL;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        // Send completion event with error status
        ancc_cmp_evt_send(ancc_env, conidx, ANCC_GET_NTF_ATTS_CMD_OP_CODE, status);
    }
    else
    {
        // Write command
        prf_gatt_write(&ancc_env->prf_env, conidx, handle, buffer, length, GATTC_WRITE);
        ancc_env->env[conidx]->operation = param;
        ancc_env->env[conidx]->last_op_code = ANCC_GET_NTF_ATTS_CMD_OP_CODE;
        if (ancc_env->env[conidx]->data_src.state != ANCC_DATA_SRC_NTF_CMD_UID_ERROR)
        {
            // Initialize the Data Source Notification reception state machine
            // If in error state, do not change the state until all the erroneous packets have been received
            ancc_env->env[conidx]->data_src.state = ANCC_DATA_SRC_NTF_IDLE;
        }

        ke_state_set(dest_id, ANCC_BUSY);
        msg_status = KE_MSG_NO_FREE;
    }

    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref ANCC_GET_APP_ATTS_CMD_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ancc_get_app_atts_cmd_req_handler(ke_msg_id_t const msgid,
                                             struct ancc_get_app_atts_cmd_req *param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = GAP_ERR_NO_ERROR;
    uint8_t msg_status = KE_MSG_CONSUMED;
    uint8_t length = 0;
    // Temporary buffer to store the Get App Attributes command
    uint8_t *buffer = (uint8_t*)ke_malloc(param->app_id_length + 2, KE_MEM_NON_RETENTION);

    uint16_t handle = ancc_env->env[conidx]->anc.chars[ANCC_CHAR_CTRL_PT].val_hdl;

    if (handle != ATT_INVALID_HANDLE)
    {
        // Pack command
        buffer[length] = CMD_ID_GET_APP_ATTS;
        length += 1;
        memcpy( &buffer[length], param->app_id, param->app_id_length);
        length += param->app_id_length;

        if (param->atts & APP_ATT_ID_DISPLAY_NAME_PRESENT)    // App Identifier
        {
            buffer[length] = NTF_ATT_ID_APP_ID;
            length += 1;
        }
        else
        {
            // No attributes have been requested.
            status = PRF_ERR_INVALID_PARAM;
        }
    }
    else
    {
        status = PRF_ERR_INEXISTENT_HDL;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        // Send completion event with error status
        ancc_cmp_evt_send(ancc_env, conidx, ANCC_GET_APP_ATTS_CMD_OP_CODE, status);
    }
    else
    {
        // Write command
        prf_gatt_write(&ancc_env->prf_env, conidx, handle, buffer, length, GATTC_WRITE);
        ancc_env->env[conidx]->operation = param;
        ancc_env->env[conidx]->last_op_code = ANCC_GET_APP_ATTS_CMD_OP_CODE;
        if (ancc_env->env[conidx]->data_src.state != ANCC_DATA_SRC_APP_CMD_ERROR)
        {
            // Initialize the APP ID reception state machine
            // If in error state, do not change the state until all the erroneous packets have been received
            ancc_env->env[conidx]->data_src.state = ANCC_DATA_SRC_APP_ID_IDLE;
        }

        ke_state_set(dest_id, ANCC_BUSY);
        msg_status = KE_MSG_NO_FREE;
    }

    ke_free(buffer);
    return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref ANCC_PERF_NTF_ACT_CMD_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ancc_perf_ntf_act_cmd_req_handler(ke_msg_id_t const msgid,
                                             struct ancc_perf_ntf_act_cmd_req const *param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint8_t status = PRF_ERR_REQ_DISALLOWED;
    uint8_t length = 0;
    uint8_t buffer[ANCC_PERF_NTF_ACT_CMD_MAX_LEN];
    uint16_t handle = ancc_env->env[conidx]->anc.chars[ANCC_CHAR_CTRL_PT].val_hdl;

    if (handle != ATT_INVALID_HANDLE)
    {
        if (param->action > ACT_ID_NB)
        {
            status = PRF_ERR_INVALID_PARAM;
        }
        else
        {
            // Pack command
            buffer[length++] = CMD_ID_PERF_NTF_ACT;
            co_write32p(&buffer[length], param->ntf_uid);
            length += 4;
            buffer[length++] = param->action;
            status = GAP_ERR_NO_ERROR;
        }
    }
    else
    {
        status = PRF_ERR_INEXISTENT_HDL;
    }

    if (status != GAP_ERR_NO_ERROR)
    {
        // Send completion event with error status
        ancc_cmp_evt_send(ancc_env, conidx, ANCC_PERF_NTF_ACT_CMD_OP_CODE, status);
    }
    else
    {
        // Write command
        prf_gatt_write(&ancc_env->prf_env, conidx, handle, buffer, length, GATTC_WRITE);
        ancc_env->env[conidx]->last_op_code= ANCC_PERF_NTF_ACT_CMD_OP_CODE;
        ke_state_set(dest_id, ANCC_BUSY);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_CMP_EVT message.
 * This generic event is received for different requests.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler_busy(ke_msg_id_t const msgid,
                                      struct gattc_cmp_evt const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);

    ASSERT_ERROR(ancc_env != NULL);
    ASSERT_ERROR(ancc_env->env[conidx] != NULL);

    uint8_t op_code = ancc_env->env[conidx]->last_op_code;

    switch (param->operation)
    {
        case GATTC_WRITE:
        case GATTC_WRITE_NO_RESPONSE:
        {
            switch (op_code)
            {
                case (ANCC_WR_NTF_NTF_SRC_OP_CODE):
                case (ANCC_WR_NTF_DATA_SRC_OP_CODE):
                {
                    ancc_wr_cfg_ntf_rsp_send(ancc_env, conidx, op_code, param->status);
                } break;

                case (ANCC_GET_NTF_ATTS_CMD_OP_CODE):
                case (ANCC_GET_APP_ATTS_CMD_OP_CODE):
                {
                    // If there was an error then command is considered completed.
                    if (param->status != GAP_ERR_NO_ERROR)
                    {
                        ancc_cmp_evt_send(ancc_env, conidx, op_code, param->status);
                    }
                } break;

                case (ANCC_PERF_NTF_ACT_CMD_OP_CODE):
                {
                    // No notifications after this command
                    ancc_cmp_evt_send(ancc_env, conidx, op_code, param->status);
                } break;

                default:
                    ASSERT_ERROR(0);
                    break;
            }
        } break;

        case GATTC_READ:
        {
            if (param->status != GAP_ERR_NO_ERROR)
            {
                if ((op_code == ANCC_RD_NTF_NTF_SRC_OP_CODE) || (op_code == ANCC_RD_NTF_DATA_SRC_OP_CODE))
                {
                    ancc_rd_cfg_ntf_rsp_send(ancc_env, conidx, op_code, 0, param->status);
                }
            }
        }break;

        default:
            break;
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
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);
    uint16_t cfg_val;

    ASSERT_ERROR(ancc_env != NULL);
    ASSERT_ERROR(ancc_env->env[conidx] != NULL);

    uint8_t op_code = ancc_env->env[conidx]->last_op_code;

    if ((op_code == ANCC_RD_NTF_NTF_SRC_OP_CODE) || (op_code == ANCC_RD_NTF_DATA_SRC_OP_CODE))
    {
        cfg_val = co_read16p(param->value);
        ancc_rd_cfg_ntf_rsp_send(ancc_env, conidx, op_code, cfg_val, GAP_ERR_NO_ERROR);
    }
    else
    {
        ASSERT_ERROR(0);
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
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    uint8_t conidx = KE_IDX_GET(dest_id);

    if (param->type == GATTC_NOTIFY)
    {
        if (param->handle == ancc_env->env[conidx]->anc.chars[ANCC_CHAR_NTF_SRC].val_hdl)
        {
            if (param->value[2] < CAT_ID_NB)
            {
                #if ANC_CFG_DROP_PREEXISTING_NTF
                if (param->value[1] & ANC_EVT_FLAG_PRE_EXISTING)
                {
                    // Drop pre-existing notifications, as we might run out of
                    // memory otherwise.
                    return KE_MSG_CONSUMED;
                }
                #endif

                struct ancc_ntf_src_ind *ind = KE_MSG_ALLOC(ANCC_NTF_SRC_IND,
                                                            prf_dst_task_get(&(ancc_env->prf_env), conidx),
                                                            prf_src_task_get(&(ancc_env->prf_env), conidx),
                                                            ancc_ntf_src_ind);

                ind->value.event_id     = param->value[0];
                ind->value.event_flags  = param->value[1];
                ind->value.cat_id       = param->value[2];
                ind->value.cat_cnt      = param->value[3];
                ind->value.ntf_uid      = co_read32p(&param->value[4]);

                ke_msg_send(ind);
            }
        }
        else if (param->handle == ancc_env->env[conidx]->anc.chars[ANCC_CHAR_DATA_SRC].val_hdl)
        {
            switch (ancc_env->env[conidx]->last_op_code)
            {
                case ANCC_GET_NTF_ATTS_CMD_OP_CODE:
                {
                    ancc_get_ntf_atts_rsp(ancc_env, conidx, param->value, param->length);
                }break;

                case ANCC_GET_APP_ATTS_CMD_OP_CODE:
                {
                    ancc_get_app_atts_rsp(ancc_env, conidx, param->value, param->length);
                }break;
            }
        }
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Save messages in BUSY state
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ancc_save_msg_handler(ke_msg_id_t const msgid,
                                 void const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    return (KE_MSG_SAVED);
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Idle state message handlers
const struct ke_msg_handler ancc_idle[] =
{
    {ANCC_ENABLE_REQ,                (ke_msg_func_t)ancc_enable_req_handler},
    {GATTC_EVENT_IND,                (ke_msg_func_t)gattc_event_ind_handler},
    {ANCC_WR_CFG_NTF_REQ,            (ke_msg_func_t)ancc_wr_cfg_ntf_req_handler},
    {ANCC_RD_CFG_NTF_REQ,            (ke_msg_func_t)ancc_rd_cfg_ntf_req_handler},
    {ANCC_GET_NTF_ATTS_CMD_REQ,      (ke_msg_func_t)ancc_get_ntf_atts_cmd_req_handler},
    {ANCC_GET_APP_ATTS_CMD_REQ,      (ke_msg_func_t)ancc_get_app_atts_cmd_req_handler},
    {ANCC_PERF_NTF_ACT_CMD_REQ,      (ke_msg_func_t)ancc_perf_ntf_act_cmd_req_handler},
};

/// Discovering state message handlers
const struct ke_msg_handler ancc_discovering[] =
{
    {GATTC_SDP_SVC_IND,             (ke_msg_func_t)gattc_sdp_svc_ind_handler},
    {GATTC_CMP_EVT,                 (ke_msg_func_t)gattc_cmp_evt_handler_discovering},
};

/// Busy state handlers definition
const struct ke_msg_handler ancc_busy[] =
{
    {GATTC_CMP_EVT,                  (ke_msg_func_t)gattc_cmp_evt_handler_busy},
    {GATTC_READ_IND,                 (ke_msg_func_t)gattc_read_ind_handler},
    {GATTC_EVENT_IND,                (ke_msg_func_t)gattc_event_ind_handler},
    {ANCC_WR_CFG_NTF_REQ,            (ke_msg_func_t)ancc_save_msg_handler},
    {ANCC_RD_CFG_NTF_REQ,            (ke_msg_func_t)ancc_save_msg_handler},
    {ANCC_GET_NTF_ATTS_CMD_REQ,      (ke_msg_func_t)ancc_save_msg_handler},
    {ANCC_GET_APP_ATTS_CMD_REQ,      (ke_msg_func_t)ancc_save_msg_handler},
    {ANCC_PERF_NTF_ACT_CMD_REQ,      (ke_msg_func_t)ancc_save_msg_handler},
};

/// Specifies the message handler structure for every state.
const struct ke_state_handler ancc_state_handler[ANCC_STATE_MAX] =
{
    [ANCC_FREE]        = KE_STATE_HANDLER_NONE,
    [ANCC_IDLE]        = KE_STATE_HANDLER(ancc_idle),
    [ANCC_DISCOVERING] = KE_STATE_HANDLER(ancc_discovering),
    [ANCC_BUSY]        = KE_STATE_HANDLER(ancc_busy),
};

/// @} ANCCTASK

#endif // BLE_ANC_CLIENT
