/**
 ****************************************************************************************
 *
 * @file ancc.c
 *
 * @brief C file - Apple Notification Center Service Client Implementation.
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
 * @addtogroup ANCC
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "anc_common.h"

#if (BLE_ANC_CLIENT)

#include <string.h>

#include "ancc.h"
#include "ancc_task.h"

/**
 ****************************************************************************************
 * @brief Initialization of the ANCC module.
 * This function performs the initialization of the Profile module:
 *  - Creation of database (if it's a service)
 *  - Allocation of profile required memory
 *  - Initialization of task descriptor to register application:
 *      - Task State array
 *      - Number of tasks
 *      - Default task handler
 *
 * @param[out]    env        Client or Service allocated environment data.
 * @param[in,out] start_hdl  Service start handle (0 - dynamically allocated), only applies for services.
 * @param[in]     app_task   Application task number.
 * @param[in]     sec_lvl    Security level (AUTH, EKS and MI field of @enum attm_svc_perm_mask)
 * @param[in]     param      Configuration parameters of profile collector or service (32 bits aligned)
 *
 * @return status code to know if profile initialization succeeds
 ****************************************************************************************
 */
static uint8_t ancc_init(struct prf_task_env *env, uint16_t *start_hdl, uint16_t app_task,
                         uint8_t sec_lvl, void *params)
{
    uint8_t idx;

    struct ancc_env_tag *ancc_env =
            (struct ancc_env_tag *) ke_malloc(sizeof(struct ancc_env_tag), KE_MEM_ATT_DB);
    memset(ancc_env, 0, sizeof(struct ancc_env_tag));

    // Allocate ANCC required environment variable
    env->env = (prf_env_t*) ancc_env;

    ancc_env->prf_env.app_task = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
    ancc_env->prf_env.prf_task = env->task | PERM(PRF_MI, ENABLE);

    // Initialize environment variable
    env->id                     = TASK_ID_ANCC;
    env->desc.idx_max           = ANCC_IDX_MAX;
    env->desc.state             = ancc_env->state;
    env->desc.state_max         = ANCC_STATE_MAX;
    env->desc.state_handler     = ancc_state_handler;
    env->desc.default_handler   = NULL;

    for (idx = 0; idx < ANCC_IDX_MAX ; idx++)
    {
        ancc_env->env[idx] = NULL;
        // Service is ready, go into the Free state
        ke_state_set(KE_BUILD_ID(env->task, idx), ANCC_FREE);
    }

    return GAP_ERR_NO_ERROR;
}


/**
 ****************************************************************************************
 * @brief Clean up of connection dedicated environment parameters
 * This function performs clean up of ongoing operations
 * @param[in,out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @param[in]        reason     Detach reason
 * @return void
 ****************************************************************************************
 */
static void ancc_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    struct ancc_env_tag *ancc_env = (struct ancc_env_tag *) env->env;

    // Clean up environment variable allocated for task instance
    if (ancc_env->env[conidx] != NULL)
    {
        prf_unregister_atthdl2gatt(&ancc_env->prf_env, conidx, &ancc_env->env[conidx]->anc.svc);

        if (ancc_env->env[conidx]->data_src.alloc_rsp_ind != NULL)
        {
            ke_free(ke_param2msg(ancc_env->env[conidx]->data_src.alloc_rsp_ind));
        }

        if (ancc_env->env[conidx]->operation != NULL)
        {
            ke_free(ke_param2msg(ancc_env->env[conidx]->operation));
        }

        ke_free(ancc_env->env[conidx]);
        ancc_env->env[conidx] = NULL;
    }

    // Put ANCS Client in Free state
    ke_state_set(KE_BUILD_ID(env->task, conidx), ANCC_FREE);
}


/**
 ****************************************************************************************
 * @brief Destruction of the ANCC module - due to a reset for instance.
 * This function cleans up allocated memory (attribute database is destroyed by another
 * procedure)
 *
 * @param[in,out]    env        Client or Service allocated environment data.
 * @return void
 ****************************************************************************************
 */
static void ancc_destroy(struct prf_task_env *env)
{
    uint8_t idx;
    struct ancc_env_tag *ancc_env = (struct ancc_env_tag *) env->env;

    // Cleanup environment variable for each task instances
    for (idx = 0; idx < ANCC_IDX_MAX ; idx++)
    {
        if (ancc_env->env[idx] != NULL)
        {
            ancc_cleanup(env, idx, 0);
        }
    }

    // Free profile environment variables
    env->env = NULL;
    ke_free(ancc_env);
}

/**
 ****************************************************************************************
 * @brief Handles Connection creation
 *
 * @param[in,out]    env        Collector or Service allocated environment data.
 * @param[in]        conidx     Connection index
 * @return void
 ****************************************************************************************
 */
static void ancc_create(struct prf_task_env *env, uint8_t conidx)
{
    // Put ANP Client in Idle state
    ke_state_set(KE_BUILD_ID(env->task, conidx), ANCC_IDLE);
}

/**
 ****************************************************************************************
 * @brief Set IDLE state if prevoius state was BUSY
 *
 * @param[in,out]    env        Collector allocated environment data.
 * @param[in]        conidx     Connection index
 * @return void
 ****************************************************************************************
 */
static void ancc_idle_state_set(struct ancc_env_tag *ancc_env, uint8_t conidx)
{
    if (ke_state_get(prf_src_task_get(&(ancc_env->prf_env), conidx)) == ANCC_BUSY)
    {
        ke_state_set(prf_src_task_get(&(ancc_env->prf_env), conidx), ANCC_IDLE);
    }
}

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// ANCC Task interface required by profile manager
const struct prf_task_cbs ancc_itf =
{
    (prf_init_fnct)ancc_init,
    ancc_destroy,
    ancc_create,
    ancc_cleanup,
};

/*
 * GLOBAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

const struct prf_task_cbs *ancc_prf_itf_get(void)
{
   return &ancc_itf;
}

void ancc_enable_rsp_send(struct ancc_env_tag *ancc_env, uint8_t conidx, uint8_t status)
{
    // Send APP the details of the discovered attributes on ANCC
    struct ancc_enable_rsp *rsp = KE_MSG_ALLOC(ANCC_ENABLE_RSP,
                                               prf_dst_task_get(&(ancc_env->prf_env), conidx),
                                               prf_src_task_get(&(ancc_env->prf_env), conidx),
                                               ancc_enable_rsp);

    rsp->status = status;

    if (status == GAP_ERR_NO_ERROR)
    {
        rsp->anc = ancc_env->env[conidx]->anc;
        // Register ANCC task in gatt for indication/notifications
        prf_register_atthdl2gatt(&ancc_env->prf_env, conidx, &ancc_env->env[conidx]->anc.svc);
        // Go to connected state
        ke_state_set(prf_src_task_get(&(ancc_env->prf_env), conidx), ANCC_IDLE);
    }
    ke_msg_send(rsp);
}

void ancc_wr_cfg_ntf_rsp_send(struct ancc_env_tag *ancc_env, uint8_t conidx, uint8_t op_code,
                              uint8_t status)
{
    // Go back to IDLE state
    ancc_idle_state_set(ancc_env, conidx);

    struct ancc_wr_cfg_ntf_rsp *rsp = KE_MSG_ALLOC(ANCC_WR_CFG_NTF_RSP,
                                                   prf_dst_task_get(&(ancc_env->prf_env), conidx),
                                                   prf_src_task_get(&(ancc_env->prf_env), conidx),
                                                   ancc_wr_cfg_ntf_rsp);

    rsp->op_code = op_code;
    rsp->status  = status;

    ke_msg_send(rsp);
}

void ancc_rd_cfg_ntf_rsp_send(struct ancc_env_tag *ancc_env, uint8_t conidx, uint8_t op_code,
                              uint16_t value, uint8_t status)
{
    // Go back to IDLE state
    ancc_idle_state_set(ancc_env, conidx);

    struct ancc_rd_cfg_ntf_rsp *rsp = KE_MSG_ALLOC(ANCC_RD_CFG_NTF_RSP,
                                                   prf_dst_task_get(&(ancc_env->prf_env), conidx),
                                                   prf_src_task_get(&(ancc_env->prf_env), conidx),
                                                   ancc_rd_cfg_ntf_rsp);

    rsp->op_code = op_code;
    rsp->cfg_val = value;
    rsp->status  = status;

    ke_msg_send(rsp);
}

void ancc_cmp_evt_send(struct ancc_env_tag *ancc_env, uint8_t conidx, uint8_t op_code,
                       uint8_t status)
{
    // Free stored request message
    if (ancc_env->env[conidx]->operation != NULL)
    {
        ke_msg_free(ke_param2msg(ancc_env->env[conidx]->operation));
        ancc_env->env[conidx]->operation = NULL;
    }
    // Go back to IDLE state
    ancc_idle_state_set(ancc_env, conidx);

    struct ancc_cmp_evt *evt = KE_MSG_ALLOC(ANCC_CMP_EVT,
                                            prf_dst_task_get(&(ancc_env->prf_env), conidx),
                                            prf_src_task_get(&(ancc_env->prf_env), conidx),
                                            ancc_cmp_evt);

    evt->operation = op_code;
    evt->status    = status;

    ke_msg_send(evt);
}

void ancc_get_ntf_atts_rsp(struct ancc_env_tag *env_tag, uint8_t conidx, const uint8_t *value,
                           uint16_t length)
{
    uint16_t idx = 0;
    struct ancc_cnx_env *ancc_env = env_tag->env[conidx];

    while (idx < length)
    {
        switch (ancc_env->data_src.state)
        {
            case ANCC_DATA_SRC_NTF_IDLE:
            {
                if (value[0] == CMD_ID_GET_NTF_ATTS &&
                    ancc_env->operation != NULL &&
                    ((struct ancc_get_ntf_atts_cmd_req *)ancc_env->operation)->ntf_uid == co_read32p(&value[1]))
                {
                    uint8_t mask = ((struct ancc_get_ntf_atts_cmd_req *)ancc_env->operation)->atts;
                    uint8_t i = 0;
                    idx += 5; // skip cmd id and uid

                    ancc_env->data_src.nb_req_atts = 0;
                    for (i = 0; i < 8; ++i)
                    {
                        if (mask & 1)
                            ancc_env->data_src.nb_req_atts += 1;
                        mask >>= 1;
                    }
                    ancc_env->data_src.state = ANCC_DATA_SRC_NTF_NEXT_ATT;
                }
                else
                {
                    idx = length;
                    ancc_env->data_src.state = ANCC_DATA_SRC_NTF_CMD_UID_ERROR;
                    // Since only one NTF_ATTS_CMD is sent at a time and then wait for the completion event
                    // to send the next, send the completion event now so the next command can be sent
                    ancc_cmp_evt_send(env_tag, conidx, ANCC_GET_NTF_ATTS_CMD_OP_CODE, GAP_ERR_NO_ERROR);
                }
            }break;

            case ANCC_DATA_SRC_NTF_NEXT_ATT:
            {
                ancc_env->data_src.curr_att_id = value[idx++];
                ancc_env->data_src.state       = ANCC_DATA_SRC_NTF_ATT_LENGTH;
            }break;

            case ANCC_DATA_SRC_NTF_ATT_LENGTH:
            {
                ancc_env->data_src.curr_att_length = value[idx++];
                ancc_env->data_src.state           = ANCC_DATA_SRC_NTF_ATT_MSB;
            }break;

            case ANCC_DATA_SRC_NTF_ATT_MSB:
            {
                ancc_env->data_src.curr_att_length |= (value[idx++] << 8);
                ancc_env->data_src.curr_value_idx   = 0;

                // Allocate the indication
                struct ancc_ntf_att_ind *ind = KE_MSG_ALLOC_DYN(ANCC_NTF_ATT_IND,
                                                                prf_dst_task_get(&(env_tag->prf_env), conidx),
                                                                prf_src_task_get(&(env_tag->prf_env), conidx),
                                                                ancc_ntf_att_ind,
                                                                ancc_env->data_src.curr_att_length);

                ind->ntf_uid = ((struct ancc_get_ntf_atts_cmd_req *)ancc_env->operation)->ntf_uid;
                ind->att_id  = ancc_env->data_src.curr_att_id;
                ind->length  = ancc_env->data_src.curr_att_length;

                ancc_env->data_src.alloc_rsp_ind = ind;

                if (ancc_env->data_src.curr_att_length == 0)
                {
                    ancc_env->data_src.nb_req_atts -= 1;
                    // Send indication to app
                    ke_msg_send(ancc_env->data_src.alloc_rsp_ind);
                    ancc_env->data_src.alloc_rsp_ind = NULL;

                    // Check if all requested attributes have been received
                    if (ancc_env->data_src.nb_req_atts > 0)
                    {
                        // Advance to the next attribute
                        ancc_env->data_src.state = ANCC_DATA_SRC_NTF_NEXT_ATT; // set next state
                    }
                    else
                    {
                        // Command complete
                        ancc_cmp_evt_send(env_tag, conidx, ANCC_GET_NTF_ATTS_CMD_OP_CODE, GAP_ERR_NO_ERROR);
                        ancc_env->data_src.state = ANCC_DATA_SRC_NTF_IDLE;
                    }
                }
                else
                {
                    // Wait for att value
                    ancc_env->data_src.state = ANCC_DATA_SRC_NTF_ATT_VALUE;
                }
            }break;

            case ANCC_DATA_SRC_NTF_ATT_VALUE:
            {
                ((struct ancc_ntf_att_ind *) ancc_env->data_src.alloc_rsp_ind)->value[ancc_env->data_src.curr_value_idx] = value[idx++];
                ancc_env->data_src.curr_value_idx += 1;

                // Check if attribute value has been read
                if (ancc_env->data_src.curr_value_idx == ancc_env->data_src.curr_att_length)
                {
                    ancc_env->data_src.nb_req_atts -= 1;
                    // Send indication to app
                    ke_msg_send(ancc_env->data_src.alloc_rsp_ind);
                    ancc_env->data_src.alloc_rsp_ind = NULL;

                    // Check if all requested attributes have been received
                    if (ancc_env->data_src.nb_req_atts > 0)
                    {
                        // Advance to the next attribute
                        ancc_env->data_src.state = ANCC_DATA_SRC_NTF_NEXT_ATT; // set next state
                    }
                    else
                    {
                        // Command complete
                        ancc_cmp_evt_send(env_tag, conidx, ANCC_GET_NTF_ATTS_CMD_OP_CODE,
                                          GAP_ERR_NO_ERROR);
                        ancc_env->data_src.state = ANCC_DATA_SRC_NTF_IDLE;
                    }
                }
            }break;

            case ANCC_DATA_SRC_NTF_CMD_UID_ERROR:
            {
                if (value[0] == CMD_ID_GET_NTF_ATTS &&
                    ancc_env->operation != NULL &&
                    ((struct ancc_get_ntf_atts_cmd_req *)ancc_env->operation)->ntf_uid == co_read32p(&value[1]))
                {
                    idx += 5; // skip cmd id and uid

                    uint8_t mask  = ((struct ancc_get_ntf_atts_cmd_req *)ancc_env->operation)->atts;

                    ancc_env->data_src.nb_req_atts = 0;
                    for(int kk=0; kk < 8; ++kk)
                    {
                        if (mask & 1)
                            ancc_env->data_src.nb_req_atts += 1;
                        mask >>= 1;
                    }

                    ancc_env->data_src.state = ANCC_DATA_SRC_NTF_NEXT_ATT;
                }
                else
                {
                    idx = length;
                    // Do nothing, just ignore the packet until we get the write cmd and uid
                    ancc_env->data_src.state = ANCC_DATA_SRC_NTF_CMD_UID_ERROR;
                }
            }break;
        }
    }
}

void ancc_get_app_atts_rsp(struct ancc_env_tag *env_tag, uint8_t conidx,
                           const uint8_t *value, uint16_t length)
{
    uint16_t idx = 0;
    struct ancc_cnx_env *ancc_env = env_tag->env[conidx];
    while (idx < length)
    {
        switch (ancc_env->data_src.state)
        {
            case ANCC_DATA_SRC_APP_ID_IDLE:
            {
                if ((value[0] == CMD_ID_GET_APP_ATTS) && ancc_env->operation != NULL)
                {
                    uint8_t mask = ((struct ancc_get_app_atts_cmd_req *)ancc_env->operation)->atts;
                    uint8_t i = 0;
                    idx += 1; // Skip Command ID

                    ancc_env->data_src.nb_req_atts = 0;
                    for(i = 0; i < 8; ++i)
                    {
                        if (mask & 1)
                            ancc_env->data_src.nb_req_atts += 1;
                        mask >>= 1;
                    }
                    // Initialize index for the buffer that will hold the value
                    ancc_env->data_src.curr_value_idx = 0;
                    ancc_env->data_src.state = ANCC_DATA_SRC_NTF_APP_ID;
                }
                else
                {
                    idx = length;
                    ancc_env->data_src.state = ANCC_DATA_SRC_APP_CMD_ERROR;
                    // Since only one APP_ATTS_CMD is sent at a time and then wait for the completion event
                    // to send the next, send the completion event now so the next command can be sent
                    ancc_cmp_evt_send(env_tag, conidx, ANCC_GET_APP_ATTS_CMD_OP_CODE, GAP_ERR_NO_ERROR);
                }
            }break;

            case ANCC_DATA_SRC_NTF_APP_ID:
            {
                if (((struct ancc_get_app_atts_cmd_req *) ancc_env->operation)->app_id[ancc_env->data_src.curr_value_idx++]
                != value[idx++])
                {
                    ASSERT_ERROR(0);
                };

                // Check if app id has been read
                if (ancc_env->data_src.curr_value_idx == ((struct ancc_get_app_atts_cmd_req *) ancc_env->operation)->app_id_length)
                {
                    ancc_env->data_src.state              = ANCC_DATA_SRC_NTF_NEXT_ATT;
                    // Save current App ID length
                    ancc_env->data_src.curr_app_id_length = ancc_env->data_src.curr_value_idx;

                }
            }break;

            case ANCC_DATA_SRC_NTF_NEXT_ATT:
            {
                ancc_env->data_src.curr_att_id = value[idx++];
                ancc_env->data_src.state       = ANCC_DATA_SRC_NTF_ATT_LENGTH;
            }break;

            case ANCC_DATA_SRC_NTF_ATT_LENGTH:
            {
                ancc_env->data_src.curr_att_length = value[idx++];
                ancc_env->data_src.state           = ANCC_DATA_SRC_NTF_ATT_MSB;
            }

            case ANCC_DATA_SRC_NTF_ATT_MSB:
            {
                ancc_env->data_src.curr_att_length |= (value[idx++] << 8);

                // Allocate the indication
                struct ancc_app_att_ind *ind = KE_MSG_ALLOC_DYN(ANCC_APP_ATT_IND,
                                                                prf_dst_task_get(&(env_tag->prf_env), conidx),
                                                                prf_src_task_get(&(env_tag->prf_env), conidx),
                                                                ancc_app_att_ind,
                                                                ancc_env->data_src.curr_app_id_length + ancc_env->data_src.curr_att_length);

                ind->app_id_length = ancc_env->data_src.curr_app_id_length;
                ind->att_id        = ancc_env->data_src.curr_att_id;
                ind->att_length    = ancc_env->data_src.curr_att_length;

                memcpy(&ind->values,
                       &(((struct ancc_get_app_atts_cmd_req *)ancc_env->operation)->app_id),
                       ((struct ancc_get_app_atts_cmd_req *)ancc_env->operation)->app_id_length);

                ancc_env->data_src.alloc_rsp_ind = ind;
                if (ancc_env->data_src.curr_att_length == 0)
                {
                    ancc_env->data_src.nb_req_atts -= 1;

                    // Send indication to app
                    ke_msg_send(ancc_env->data_src.alloc_rsp_ind);
                    ancc_env->data_src.alloc_rsp_ind = NULL;

                    // Check if all requested attributes have been received
                    if (ancc_env->data_src.nb_req_atts > 0)
                    {
                        // Advance to the next attr
                        ancc_env->data_src.state = ANCC_DATA_SRC_NTF_NEXT_ATT;
                    }
                    else
                    {
                        // Command complete
                        ancc_cmp_evt_send(env_tag, conidx, ANCC_GET_APP_ATTS_CMD_OP_CODE,
                                          GAP_ERR_NO_ERROR);
                        ancc_env->data_src.state = ANCC_DATA_SRC_APP_ID_IDLE;

                    }
                }
                else
                {
                    ancc_env->data_src.state = ANCC_DATA_SRC_NTF_ATT_VALUE;
                }

            }break;

            case ANCC_DATA_SRC_NTF_ATT_VALUE:
            {
                ((struct ancc_app_att_ind *) ancc_env->data_src.alloc_rsp_ind)->values[ancc_env->data_src.curr_value_idx] = value[idx++];
                ancc_env->data_src.curr_value_idx += 1;

                // Check if the attribute value has been read
                if (ancc_env->data_src.curr_value_idx == ancc_env->data_src.curr_att_length
                                                       + ancc_env->data_src.curr_app_id_length)
                {
                    ancc_env->data_src.nb_req_atts -= 1;

                    // Send indication to app
                    ke_msg_send(ancc_env->data_src.alloc_rsp_ind);
                    ancc_env->data_src.alloc_rsp_ind = NULL;

                    // Check if all requested attributes have been received
                    if (ancc_env->data_src.nb_req_atts > 0)
                    {
                        // Advance to the next attr
                        ancc_env->data_src.state = ANCC_DATA_SRC_NTF_NEXT_ATT;
                    }
                    else
                    {
                        // Command complete
                        ancc_cmp_evt_send(env_tag, conidx, ANCC_GET_APP_ATTS_CMD_OP_CODE, GAP_ERR_NO_ERROR);
                        ancc_env->data_src.state = ANCC_DATA_SRC_APP_ID_IDLE;

                    }
                }
            }break;

            case ANCC_DATA_SRC_APP_CMD_ERROR:
            {
                if ((value[0] == CMD_ID_GET_APP_ATTS) && ancc_env->operation != NULL)
                {

                    // Skip Command ID
                    idx += 1;

                    // Compute the amount of requested attributes
                    uint8_t mask = ((struct ancc_get_app_atts_cmd_req *)ancc_env->operation)->atts;

                    ancc_env->data_src.nb_req_atts = 0;
                    for(int kk=0; kk < 8; ++kk)
                    {
                        if (mask & 1)
                            ancc_env->data_src.nb_req_atts += 1;
                        mask >>= 1;
                    }

                    // Initialize index for the buffer that will hold the value
                    ancc_env->data_src.curr_value_idx = 0;

                    // Set next state
                    ancc_env->data_src.state = ANCC_DATA_SRC_NTF_APP_ID;
                }
                else
                {
                    idx = length;
                    // Do nothing, just ignore the packet until we get the write cmd
                    ancc_env->data_src.state = ANCC_DATA_SRC_APP_CMD_ERROR;
                }
            }break;
        }
    }
}

#endif // (BLE_ANC_CLIENT)

/// @} ANCC
