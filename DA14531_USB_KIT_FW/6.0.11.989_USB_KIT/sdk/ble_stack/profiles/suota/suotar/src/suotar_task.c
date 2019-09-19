/**
 ****************************************************************************************
 *
 * @file suotar_task.c
 *
 * @brief Software Programming Over The Air (SUOTA) Receiver Task Implementation.
 *
 * Copyright (c) 2013-2018 Dialog Semiconductor. All rights reserved.
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
 * @addtogroup SUOTARTASK
 * @{
 ****************************************************************************************
 */

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_SUOTAR__)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_SUOTA_RECEIVER)
#include "suotar_task.h"


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref SUOTAR_STATUS_UPDATE_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int suotar_status_update_req_handler (ke_msg_id_t const msgid,
                                         struct suotar_status_update_req const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
    struct suotar_env_tag *suotar_env = PRF_ENV_GET(SUOTAR, suotar);

    // Update the value in the attribute database
    uint8_t status = attmdb_att_set_value(suotar_env->suota_shdl + SUOTA_IDX_PATCH_STATUS_VAL,
                                  sizeof(uint8_t), 0, (uint8_t *) &param->status);
    ASSERT_ERROR(status == ATT_ERR_NO_ERROR);

    if (suotar_env->ntf_cfg != GATT_CCC_STOP_NTFIND)
    {
        // Allocate the GATT notification message
        struct gattc_send_evt_cmd *req = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
                                         KE_BUILD_ID(TASK_GATTC, KE_IDX_GET(src_id)),
                                         dest_id, gattc_send_evt_cmd,
                                         sizeof(param->status));

        // Fill in the parameter structure
        req->operation = (suotar_env->ntf_cfg == GATT_CCC_START_NTF) ? GATTC_NOTIFY :
                                                 GATTC_INDICATE;
        req->seq_num = 0;
        req->handle = (suotar_env->suota_shdl + SUOTA_IDX_PATCH_STATUS_VAL);
        req->length = sizeof(param->status);
        memcpy(req->value, &param->status, req->length);

        // Send the event
        ke_msg_send(req);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Update Memory info characteristic value.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int suotar_mem_info_update_req_handler(ke_msg_id_t const msgid,
                                    struct suotar_patch_mem_info_update_req const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    struct suotar_env_tag *suotar_env = PRF_ENV_GET(SUOTAR, suotar);
    // Update Memory info charecteristic value
    uint8_t status = attmdb_att_set_value(suotar_env->suota_shdl + SUOTA_IDX_PATCH_MEM_INFO_VAL,
                                          sizeof(uint32_t), 0,
                                          (uint8_t *) &param->mem_info);

    ASSERT_ERROR(status == ATT_ERR_NO_ERROR);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATT_WRITE_CMD_IND message.
 * The handler will analyse what has been set and decide alert level
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
    int msg_status = KE_MSG_CONSUMED;

    if(ke_state_get(dest_id) == SUOTAR_IDLE)
    {
        // Allocate write confirmation message.
        struct gattc_write_cfm *cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id,
                                                   gattc_write_cfm);

        struct suotar_env_tag *suotar_env = PRF_ENV_GET(SUOTAR, suotar);
        uint8_t att_idx = param->handle - suotar_env->suota_shdl;
        uint8_t conidx = KE_IDX_GET(src_id);

        // Fill in the parameter structure
        cfm->handle = param->handle;
        cfm->status = PRF_APP_ERROR;

        if (att_idx == SUOTA_IDX_PATCH_MEM_DEV_VAL)
        {
            cfm->status = attmdb_att_set_value(param->handle, param->length,
                                               param->offset,
                                               (uint8_t *) &param->value[0]);

            // Inform APP. Allocate the Patch Mem value change indication
            struct suotar_patch_mem_dev_ind *ind = KE_MSG_ALLOC(SUOTAR_PATCH_MEM_DEV_IND,
                                        prf_dst_task_get(&(suotar_env->prf_env), conidx),
                                        TASK_ID_SUOTAR, suotar_patch_mem_dev_ind);

            // Fill in the parameter structure
            ind->conhdl = gapc_get_conhdl(conidx);

            ind->mem_dev  = 0;
            ind->mem_dev  = (uint32_t) (param->value[3] << 24);
            ind->mem_dev |= (uint32_t) (param->value[2] << 16);
            ind->mem_dev |= (uint32_t) (param->value[1] << 8);
            ind->mem_dev |= (uint32_t) (param->value[0]);
            ind->char_code = SUOTAR_PATCH_MEM_DEV_CHAR;

            // Send the message
            ke_msg_send(ind);
        }
        else if (att_idx == SUOTA_IDX_GPIO_MAP_VAL)
        {
            cfm->status = attmdb_att_set_value(param->handle, param->length,
                                               param->offset,
                                               (uint8_t *) &param->value[0]);

            // Inform APP. Allocate the Patch Data value change indication
            struct suotar_gpio_map_ind *ind = KE_MSG_ALLOC(SUOTAR_GPIO_MAP_IND,
                                        prf_dst_task_get(&(suotar_env->prf_env), conidx),
                                        TASK_ID_SUOTAR, suotar_gpio_map_ind);

            // Fill in the parameter structure
            ind->conhdl = gapc_get_conhdl(conidx);

            ind->gpio_map  = 0;
            ind->gpio_map  = (uint32_t) (param->value[3] << 24);
            ind->gpio_map |= (uint32_t) (param->value[2] << 16);
            ind->gpio_map |= (uint32_t) (param->value[1] << 8);
            ind->gpio_map |= (uint32_t) (param->value[0]);
            ind->char_code = SUOTAR_GPIO_MAP_CHAR;

            // Send the message
            ke_msg_send(ind);
        }
        else if (att_idx == SUOTA_IDX_PATCH_DATA_VAL )
        {
            cfm->status = attmdb_att_set_value(param->handle, param->length,
                                               param->offset,
                                               (uint8_t *) &param->value[0]);

            // Inform APP. Allocate the Patch Data value change indication
            struct suotar_patch_data_ind *ind = KE_MSG_ALLOC_DYN(SUOTAR_PATCH_DATA_IND,
                                        prf_dst_task_get(&(suotar_env->prf_env), conidx),
                                        TASK_ID_SUOTAR, suotar_patch_data_ind, param->length);

            // Fill in the parameter structure
            ind->conhdl = gapc_get_conhdl(conidx);

            memcpy(ind->pd, &param->value[0], param->length);
            ind->len = param->length;
            ind->char_code = SUOTAR_PATCH_DATA_CHAR;

            // Send the message
            ke_msg_send(ind);
        }
        else if (att_idx == SUOTA_IDX_PATCH_LEN_VAL)
        {
            cfm->status = attmdb_att_set_value(param->handle, param->length,
                                               param->offset,
                                               (uint8_t *) &param->value[0]);

            // Inform APP. Allocate the Patch Data value change indication
            struct suotar_patch_len_ind *ind = KE_MSG_ALLOC(SUOTAR_PATCH_LEN_IND,
                                        prf_dst_task_get(&(suotar_env->prf_env), conidx),
                                        TASK_ID_SUOTAR, suotar_patch_len_ind);

            // Fill in the parameter structure
            ind->conhdl = gapc_get_conhdl(conidx);

            ind->len  = 0;
            ind->len  = (uint16_t) (param->value[1] << 8);
            ind->len |= (uint16_t) (param->value[0]);
            ind->char_code = SUOTAR_PATCH_LEN_CHAR;

            // Send the message
            ke_msg_send(ind);
        }
        else if (att_idx == SUOTA_IDX_PATCH_STATUS_NTF_CFG)
        {
            suotar_env->ntf_cfg = co_read16p(&param->value[0]);
            cfm->status = ATT_ERR_NO_ERROR;
        }
        else
        {
            cfm->status = ATT_ERR_APP_ERROR;
        }

        ke_msg_send(cfm);
    }
    else if(ke_state_get(dest_id) == SUOTAR_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_REQ_IND message.
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
    int msg_status = KE_MSG_CONSUMED;

    if(ke_state_get(dest_id) == SUOTAR_IDLE)
    {
        struct suotar_env_tag *suotar_env = PRF_ENV_GET(SUOTAR, suotar);
        uint8_t att_idx = param->handle - suotar_env->suota_shdl;
        uint8_t status = PRF_APP_ERROR;
        struct gattc_read_cfm* cfm;

        att_size_t length = 0;
        uint8_t *value = NULL;
#if SUOTA_VERSION >= SUOTA_VERSION_1_3
        uint16_t mtu = 0;
#endif

        // Get the requested attribute value
        if (att_idx == SUOTA_IDX_PATCH_STATUS_NTF_CFG)
        {
            status = ATT_ERR_NO_ERROR;
            length = sizeof(uint16_t);
            value = (uint8_t *) &suotar_env->ntf_cfg;
        }
#if SUOTA_VERSION >= SUOTA_VERSION_1_3
        else if(att_idx == SUOTA_IDX_MTU_VAL)
        {
            status = ATT_ERR_NO_ERROR;
            length = sizeof(uint16_t);
            mtu = gattc_get_mtu(KE_IDX_GET(src_id));
            value = (uint8_t*)&mtu;
        }
#endif
        else
        {
            status = attmdb_get_value(param->handle, &length, &value);
        }

        // Send data to peer device
        cfm = KE_MSG_ALLOC_DYN(GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, length);
        cfm->handle = param->handle;
        cfm->status = status;
        cfm->length = length;

        if (length > 0)
        {
            memcpy(cfm->value, value, length);
        }

        ke_msg_send(cfm);
    }
    else if(ke_state_get(dest_id) == SUOTAR_BUSY)
    {
        msg_status = KE_MSG_SAVED;
    }

    return msg_status;
}

/// Default State handler definition.
const struct ke_msg_handler suotar_default_state[] =
{
    {GATTC_READ_REQ_IND,                (ke_msg_func_t) gattc_read_req_ind_handler},
    {GATTC_WRITE_REQ_IND,               (ke_msg_func_t) gattc_write_req_ind_handler},
    {SUOTAR_PATCH_MEM_INFO_UPDATE_REQ,  (ke_msg_func_t) suotar_mem_info_update_req_handler},
    {SUOTAR_STATUS_UPDATE_REQ,          (ke_msg_func_t) suotar_status_update_req_handler},
};

/// Specifies the message handlers that are common to all states.
const struct ke_state_handler suotar_default_handler = KE_STATE_HANDLER(suotar_default_state);

#endif //BLE_SUOTA_RECEIVER

#endif

/// @} SUOTARTASK
