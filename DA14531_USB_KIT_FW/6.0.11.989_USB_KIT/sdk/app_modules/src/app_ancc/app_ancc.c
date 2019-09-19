/**
 ****************************************************************************************
 *
 * @file app_ancc.c
 *
 * @brief Apple Notification Center Service Application entry point
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
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_PRESENT)

#if (BLE_ANC_CLIENT)

#include "app_ancc.h"
#include "app.h"
#include "app_task.h"
#include "ancc_task.h"
#include "app_prf_perm_types.h"
#include "user_profiles_config.h"

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Send Write Notification Configuration Request
 * @param[in] enable            Notification enable flag
 * @param[in] opcode            Operation code
 * @param[in] conidx            The connection index
 * @return void
 ****************************************************************************************
 */
static void app_ancc_wr_ntf_cfg_send(bool enable, uint8_t op_code, uint8_t conidx)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    struct ancc_wr_cfg_ntf_req *req = KE_MSG_ALLOC(ANCC_WR_CFG_NTF_REQ,
                                                   prf_src_task_get(&ancc_env->prf_env, conidx),
                                                   TASK_APP,
                                                   ancc_wr_cfg_ntf_req);

    req->op_code = op_code;
    req->cfg_val = (enable) ? PRF_CLI_START_NTF : PRF_CLI_STOP_NTFIND;

    ke_msg_send(req);
}

/**
 ****************************************************************************************
 * @brief Send Read Notification Configuration Request
 * @param[in] opcode            Operation code
 * @param[in] conidx            The connection index
 * @return void
 ****************************************************************************************
 */
static void app_ancc_rd_ntf_cfg_send(uint8_t op_code, uint8_t conidx)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    struct ancc_rd_cfg_ntf_req *req = KE_MSG_ALLOC(ANCC_RD_CFG_NTF_REQ,
                                                   prf_src_task_get(&ancc_env->prf_env, conidx),
                                                   TASK_APP,
                                                   ancc_rd_cfg_ntf_req);

    req->op_code = op_code;

    ke_msg_send(req);
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_ancc_init(void)
{
    // Nothing to do
}

void app_ancc_create_db(void)
{
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC(GAPM_PROFILE_TASK_ADD_CMD,
                                                         TASK_GAPM,
                                                         TASK_APP,
                                                         gapm_profile_task_add_cmd);

    // Fill the message
    req->operation   = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl     = get_user_prf_srv_perm(TASK_ID_ANCC);
    req->prf_task_id = TASK_ID_ANCC;
    req->app_task    = TASK_APP;
    req->start_hdl   = 0;

    // Send the message
    ke_msg_send(req);
}

void app_ancc_enable(uint8_t conidx)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);
    struct ancc_enable_req *req = KE_MSG_ALLOC(ANCC_ENABLE_REQ,
                                               prf_src_task_get(&ancc_env->prf_env, conidx),
                                               TASK_APP,
                                               ancc_enable_req);

    // Fill the parameter structure
    req->con_type = PRF_CON_DISCOVERY;

    // Send the message
    ke_msg_send(req);
}

void app_ancc_wr_cfg_ntf_src(uint8_t conidx, bool enable)
{
    app_ancc_wr_ntf_cfg_send(enable, ANCC_WR_NTF_NTF_SRC_OP_CODE, conidx);
}

void app_ancc_wr_cfg_data_src(uint8_t conidx, bool enable)
{
    app_ancc_wr_ntf_cfg_send(enable, ANCC_WR_NTF_DATA_SRC_OP_CODE, conidx);
}

void app_ancc_rd_cfg_ntf_src(uint8_t conidx)
{
    app_ancc_rd_ntf_cfg_send(ANCC_RD_NTF_NTF_SRC_OP_CODE, conidx);
}

void app_ancc_rd_cfg_data_src(uint8_t conidx)
{
    app_ancc_rd_ntf_cfg_send(ANCC_RD_NTF_DATA_SRC_OP_CODE, conidx);
}

void app_ancc_get_ntf_atts(uint8_t conidx, uint32_t uid, uint8_t atts, uint16_t title_len,
                               uint16_t sub_len, uint16_t msg_len)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);

    struct ancc_get_ntf_atts_cmd_req *req = KE_MSG_ALLOC(ANCC_GET_NTF_ATTS_CMD_REQ,
                                                         prf_src_task_get(&ancc_env->prf_env, conidx),
                                                         TASK_APP,
                                                         ancc_get_ntf_atts_cmd_req);

    req->ntf_uid      = uid;
    req->atts         = atts;
    req->title_len    = title_len;
    req->subtitle_len = sub_len;
    req->message_len  = msg_len;

    ke_msg_send(req);
}

void app_ancc_get_app_atts(uint8_t conidx, uint8_t atts, uint8_t *app_id)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);

    uint8_t id_len = strlen((char*)app_id) + 1;

    struct ancc_get_app_atts_cmd_req *req = KE_MSG_ALLOC_DYN(ANCC_GET_APP_ATTS_CMD_REQ,
                                                             prf_src_task_get(&ancc_env->prf_env, conidx),
                                                             TASK_APP,
                                                             ancc_get_app_atts_cmd_req,
                                                             id_len);

    req->atts          = atts;
    req->app_id_length = id_len;

    memcpy(req->app_id, app_id, id_len);

    ke_msg_send(req);
}

void app_ancc_ntf_action(uint8_t conidx, uint32_t uid, bool act_positive)
{
    struct ancc_env_tag *ancc_env = PRF_ENV_GET(ANCC, ancc);

    struct ancc_perf_ntf_act_cmd_req *req = KE_MSG_ALLOC(ANCC_PERF_NTF_ACT_CMD_REQ,
                                                         prf_src_task_get(&ancc_env->prf_env, conidx),
                                                         TASK_APP,
                                                         ancc_perf_ntf_act_cmd_req);

    req->ntf_uid = uid;
    req->action  = (act_positive) ? ACT_ID_POS : ACT_ID_NEG;

    ke_msg_send(req);
}

#endif // (BLE_ANC_CLIENT)

#endif // BLE_APP_PRESENT

/// @} APP
