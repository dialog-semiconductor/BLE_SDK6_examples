/**
 ****************************************************************************************
 *
 * @file app_mid.h
 *
 * @brief A collection of macros that generate and send messages to the stack.
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

#ifndef _APP_MID_H_
#define _APP_MID_H_

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

#include "rwip_config.h"             // SW configuration
#include "app_task.h"                // Application task Definition
#include "app.h"                     // Application Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API
#include "arch_api.h"
#include "llm.h"
#include "co_math.h"

/*
 * DEFINITIONS
 ****************************************************************************************
 */

enum address_type
{
    ///Public BD address
    ADDRESS_PUBLIC                   = 0x00,
    ///Random BD Address
    ADDRESS_RAND,
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Create a disconnect message.
 * @param[in] conidx The connection id.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapc_disconnect_cmd* app_disconnect_msg_create(uint8_t conidx)
{
    struct gapc_disconnect_cmd *cmd = KE_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                                   KE_BUILD_ID(TASK_GAPC, conidx),
                                                   TASK_APP,
                                                   gapc_disconnect_cmd);
    cmd->operation = GAPC_DISCONNECT;
    return cmd;
}

/**
 ****************************************************************************************
 * @brief Send a disconnect message.
 * @param[in] gapc_disconnect_cmd    Pointer to the disconnect message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_disconnect_msg_send(struct gapc_disconnect_cmd *cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a connect confirmation message.
 * @param[in] conidx    The connection id.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapc_connection_cfm* app_connect_cfm_msg_create(uint8_t conidx)
{
    // confirm connection
    struct gapc_connection_cfm *cfm = KE_MSG_ALLOC(GAPC_CONNECTION_CFM,
                                                   KE_BUILD_ID(TASK_GAPC, conidx),
                                                   TASK_APP,
                                                   gapc_connection_cfm);

    return cfm;
}

/**
 ****************************************************************************************
 * @brief Send a connect confirmation message.
 * @param[in] gapc_connection_cfm    Pointer to the connect confirmation message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_connect_cfm_msg_send(struct gapc_connection_cfm* cmd)
{
     ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a start advertise message.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapm_start_advertise_cmd* app_advertise_start_msg_create(void)
{
    struct gapm_start_advertise_cmd* cmd = KE_MSG_ALLOC(GAPM_START_ADVERTISE_CMD,
                                                        TASK_GAPM,
                                                        TASK_APP,
                                                        gapm_start_advertise_cmd);

    return cmd;
}

/**
 ****************************************************************************************
 * @brief Send an advertise_start_msg message.
 * @param[in] gapm_start_advertise_cmd    Pointer to the advertise start message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_advertise_start_msg_send(struct gapm_start_advertise_cmd* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a cancel message.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapm_cancel_cmd* app_gapm_cancel_msg_create(void)
{
    struct gapm_cancel_cmd* cmd = KE_MSG_ALLOC(GAPM_CANCEL_CMD,
                                               TASK_GAPM,
                                               TASK_APP,
                                               gapm_cancel_cmd);

    cmd->operation = GAPM_CANCEL;
    return cmd;
}

/**
 ****************************************************************************************
 * @brief Send a cancellation message.
 * @param[in] gapm_cancel_cmd    Pointer to the cancellation message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapm_cancel_msg_send(struct gapm_cancel_cmd* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create an advertise stop message.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapm_cancel_cmd* app_advertise_stop_msg_create(void)
{
    return app_gapm_cancel_msg_create();
}

/**
 ****************************************************************************************
 * @brief Send an advertise stop  message.
 * @param[in] gapm_cancel_cmd    Pointer to the advertise stop message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_advertise_stop_msg_send(struct gapm_cancel_cmd* cmd)
{
    app_gapm_cancel_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a parameter update message.
 * @param[in] conidx    The connection id where the message should be sent.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapc_param_update_cmd* app_param_update_msg_create(uint8_t conidx)
{
    struct gapc_param_update_cmd* cmd = KE_MSG_ALLOC(GAPC_PARAM_UPDATE_CMD,
                                                     KE_BUILD_ID(TASK_GAPC, conidx),
                                                     TASK_APP,
                                                     gapc_param_update_cmd);

    cmd->operation = GAPC_UPDATE_PARAMS;
    return cmd;
}


/**
 ****************************************************************************************
 * @brief Send a parameter update message.
 * @param[in] gapc_param_update_cmd    Pointer to the parameter update message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_param_update_msg_send(struct gapc_param_update_cmd* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create an empty GAPM_START_CONNECTION_CMD message (connection message).
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapm_start_connection_cmd* app_connect_start_msg_create(void)
{
    struct gapm_start_connection_cmd* cmd = KE_MSG_ALLOC_DYN(GAPM_START_CONNECTION_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_start_connection_cmd,
                                                             CFG_MAX_CONNECTIONS * sizeof(struct gap_bdaddr));

    return cmd;
}

/**
 ****************************************************************************************
 * @brief Send a GAPM_START_CONNECTION_CMD message (connection start message).
 * @param[in] gapm_start_connection_cmd    Pointer to the GAPM_START_CONNECTION_CMD
 *                                         message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE  void app_connect_start_msg_send(struct gapm_start_connection_cmd* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create an empty GAPM_SET_DEV_CONFIG_CMD message (device configuration message).
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE  struct gapm_set_dev_config_cmd* app_gapm_configure_msg_create(void)
{
    struct gapm_set_dev_config_cmd* cmd=KE_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD,
                                                     TASK_GAPM,
                                                     TASK_APP,
                                                     gapm_set_dev_config_cmd);

    cmd->operation = GAPM_SET_DEV_CONFIG;
    return cmd;
}


/**
 ****************************************************************************************
 * @brief Send a gap manager configuration message.
 * @param[in] Pointer to the gap manager configuration message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapm_configure_msg_send(struct gapm_set_dev_config_cmd* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a gap bond confirmation message.
 * @param[in] conidx    The id of the connection.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapc_bond_cfm* app_gapc_bond_cfm_msg_create(uint8_t conidx)
{
    struct gapc_bond_cfm* cmd = KE_MSG_ALLOC(GAPC_BOND_CFM, KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP, gapc_bond_cfm);
    return cmd;
}

/**
 ****************************************************************************************
 * @brief Send a gap bond confirmation message.
 * @param[in] gapc_bond_cfm    Pointer to the gap bond confirmation message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapc_bond_cfm_msg_send (struct gapc_bond_cfm* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a GAPC_BOND_CFM pairing response message.
 * @param[in] conidx Connection index
 * @return The pointer to the created message
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapc_bond_cfm* app_gapc_bond_cfm_pairing_rsp_msg_create(uint8_t conidx)
{
    struct gapc_bond_cfm* cmd = app_gapc_bond_cfm_msg_create(conidx);
    cmd->request = GAPC_PAIRING_RSP;

    return cmd;
}

/**
 ****************************************************************************************
 * @brief Send a gap bond pairing message.
 * @param[in] gapc_bond_cfm    Pointer to the gap bond pairing message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapc_bond_cfm_pairing_rsp_msg_send(struct gapc_bond_cfm* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a gap bond temporary key exchange message.
 * @param[in] conidx    The id of the connection.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapc_bond_cfm* app_gapc_bond_cfm_tk_exch_msg_create(uint8_t conidx)
{
    struct gapc_bond_cfm* cmd = app_gapc_bond_cfm_msg_create(conidx);
    cmd->request = GAPC_TK_EXCH;

    return cmd;
}
/**
 ****************************************************************************************
 * @brief Send a gap bond temporary key exchange message.
 * @param[in] gapc_bond_cfm    Pointer to the gap bond temporary key exchange message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapc_bond_cfm_tk_exch_msg_send(struct gapc_bond_cfm* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create CSRK exchange message.
 * @param[in] conidx    The id of the connection.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapc_bond_cfm* app_gapc_bond_cfm_csrk_exch_msg_create(uint8_t conidx)
{
    struct gapc_bond_cfm* cmd = app_gapc_bond_cfm_msg_create(conidx);
    cmd->request = GAPC_CSRK_EXCH;
    cmd->accept = 0x01;
    return cmd;
}

/**
 ****************************************************************************************
 * @brief Send a CSRK exchange message.
 * @param[in] gapc_bond_cfm    Pointer to the gap bond CSRK exchange message to send.
 * @return void
 ****************************************************************************************
 */
 __STATIC_FORCEINLINE void app_gapc_bond_cfm_csrk_exch_msg_send(struct gapc_bond_cfm* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a gap bond Long Term Key (LTK) exchange message.
 * @param[in] conidx    The id of the connection.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapc_bond_cfm* app_gapc_bond_cfm_ltk_exch_msg_create(uint8_t conidx)
{
    struct gapc_bond_cfm* cmd = app_gapc_bond_cfm_msg_create(conidx);
    cmd->request = GAPC_LTK_EXCH;
    cmd->accept = 0x01;
    return cmd;
}

/**
 ****************************************************************************************
 * @brief Send a gap bond ltk exchange message.
 * @param[in] gapc_bond_cfm    Pointer to the gap bond ltk exchange message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapc_bond_cfm_ltk_exch_msg_send(struct gapc_bond_cfm* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a gap encrypt confirmation message.
 * @param[in] conidx    The id of the connection.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapc_encrypt_cfm* app_gapc_encrypt_cfm_msg_create(uint8_t conidx)
{
    struct gapc_encrypt_cfm* cmd = KE_MSG_ALLOC(GAPC_ENCRYPT_CFM, KE_BUILD_ID(TASK_GAPC, conidx), TASK_APP, gapc_encrypt_cfm);
    return cmd;
}

/**
 ****************************************************************************************
 * @brief Send a gap encrypt confirmation message.
 * @param[in] gapc_bond_cfm    Pointer to the gap encrypt confirmation message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapc_encrypt_cfm_msg_send(struct gapc_bond_cfm* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a gap security request message.
 * @param[in] conidx    The id of the connection.
 * @param[in] auth The authentication requirements.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapc_security_cmd* app_gapc_security_request_msg_create(uint8_t conidx, enum gap_auth auth)
{
    struct gapc_security_cmd * cmd = KE_MSG_ALLOC(GAPC_SECURITY_CMD,
                                                  KE_BUILD_ID(TASK_GAPC, conidx), 
                                                  TASK_APP, 
                                                  gapc_security_cmd);
    cmd->operation = GAPC_SECURITY_REQ;
    cmd->auth = auth;

    return cmd;
}

/**
 ****************************************************************************************
 * @brief Send a gap security request message.
 * @param[in] gapc_security_cmd    Pointer to the gap security request message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE  void app_gapc_security_request_msg_send(struct gapc_security_cmd* cmd)
{
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create a gap manager reset message.
 * @return The pointer to the created message.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE struct gapm_reset_cmd* app_gapm_reset_msg_create(void)
{
        struct gapm_reset_cmd* cmd = KE_MSG_ALLOC(GAPM_RESET_CMD, TASK_GAPM, TASK_APP,
                gapm_reset_cmd);

        cmd->operation = GAPM_RESET;

        return cmd;
}

/**
 ****************************************************************************************
 * @brief Send a gap manager reset message.
 * @param[in] gapm_reset_cmd    Pointer to the gap reset message to send.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapm_reset_msg_send (struct gapm_reset_cmd* cmd)
{
 ke_msg_send(cmd);
}

//--------------------------------OPERATIONS

/**
 ****************************************************************************************
 * @brief Send a gap manager reset operation.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapm_reset_op(void)
{
    struct gapm_reset_cmd* cmd = app_gapm_reset_msg_create();
    app_gapm_reset_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Disconnect from a given connection.
 * @param[in] conidx    The id of the given connection.
 * @param[in] reason    The reason for the disconnection.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_disconnect_op(uint8_t conidx, uint8_t reason)
{
    struct gapc_disconnect_cmd *cmd = app_disconnect_msg_create(conidx);
    cmd->reason = reason;
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Confirm connection operation.
 * @param[in] conidx                    The id of the given connection.
 * @param[in] auth                      The authentication requirements (@see gap_auth).
 * @param[in] svc_changed_ind_enable    Service changed indication enable/disable.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_connect_confirm_op(uint8_t conidx, enum gap_auth auth, uint8_t svc_changed_ind_enable)
{
    // confirm connection
    struct gapc_connection_cfm *cfm = app_connect_cfm_msg_create(conidx);
    cfm->auth = auth;
    cfm->svc_changed_ind_enable = svc_changed_ind_enable;

    // Send the message
    app_connect_cfm_msg_send(cfm);
}

/**
 ****************************************************************************************
 * @brief Create advertising message for nonconnectable undirected event (ADV_NONCONN_IND).
 * @param[in] address_src_type          The source address type used during the
 *                                      advertising operation:
 *                                         GAPM_STATIC_ADDR,
 *                                         GAPM_GEN_RSLV_ADDR,
 *                                         GAPM_GEN_NON_RSLV_ADDR
 * @param[in] interval                  The advertising interval.
 * @param[in] channel_map               The channels used during the advertising operation.
 * @param[in] advertise_mode            The advertising mode:
 *                                         GAP_NON_DISCOVERABLE,
 *                                         GAP_GEN_DISCOVERABLE,
 *                                         GAP_LIM_DISCOVERABLE,
 *                                         GAP_BROADCASTER_MODE
 * @param[in] adv_filt_policy           The advertising filter policy:
 *                                          ADV_ALLOW_SCAN_ANY_CON_ANY,
 *                                          ADV_ALLOW_SCAN_WLST_CON_ANY,
 *                                          ADV_ALLOW_SCAN_ANY_CON_WLST,
 *                                          ADV_ALLOW_SCAN_WLST_CON_WLST
 * @param[in] advertise_data            Pointer to an array with the advertising data.
 * @param[in] advertise_data_len        The length of the advertising data.
 * @param[in] scan_response_data        Pointer to an array with the scan response data.
 * @param[in] scan_response_data_len    The length of the scan response data.
 * @return void
 * @note This function supports also the advertising with scan response (ADV_SCAN_IND), if
 *       the scan response data are NOT empty.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_advertise_non_connectable_start_op(enum gapm_own_addr address_src_type,
                                                                 uint16_t interval,
                                                                 uint8_t channel_map,
                                                                 enum gap_adv_mode advertise_mode,
                                                                 enum adv_filter_policy adv_filt_policy,
                                                                 uint8_t* advertise_data,
                                                                 uint8_t advertise_data_len,
                                                                 uint8_t* scan_response_data,
                                                                 uint8_t scan_response_data_len)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_advertise_start_msg_create();

    cmd->op.code = GAPM_ADV_NON_CONN;
    cmd->op.addr_src = address_src_type;
    cmd->intv_max = interval;
    cmd->intv_min = interval;
    cmd->channel_map = channel_map;
    cmd->info.host.mode = advertise_mode;
    cmd->info.host.adv_filt_policy = adv_filt_policy;

    cmd->info.host.adv_data_len = advertise_data_len;
    ASSERT_ERROR(advertise_data_len <= ADV_DATA_LEN); // The Flags data type may be omitted (CCSv6)
    memcpy(cmd->info.host.adv_data, advertise_data, advertise_data_len);

    // If the scan response data are NOT empty then the advertising type is ADV_SCAN_IND
    cmd->info.host.scan_rsp_data_len = scan_response_data_len;
    ASSERT_ERROR(scan_response_data_len <= SCAN_RSP_DATA_LEN);
    memcpy(cmd->info.host.scan_rsp_data, scan_response_data, scan_response_data_len);

    app_advertise_start_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create advertising message for connectable undirected event (ADV_IND).
 * @param[in] address_src_type          The source address type used during the
 *                                      advertising operation:
 *                                         GAPM_STATIC_ADDR,
 *                                         GAPM_GEN_RSLV_ADDR,
 *                                         GAPM_GEN_NON_RSLV_ADDR
 * @param[in] interval                  The advertising interval.
 * @param[in] channel_map               The channels used during the advertising operation.
 * @param[in] advertise_mode            The advertising mode:
 *                                         GAP_NON_DISCOVERABLE,
 *                                         GAP_GEN_DISCOVERABLE,
 *                                         GAP_LIM_DISCOVERABLE,
 *                                         GAP_BROADCASTER_MODE
 * @param[in] adv_filt_policy           The advertising filter policy:
 *                                          ADV_ALLOW_SCAN_ANY_CON_ANY
 * @param[in] advertise_data            Pointer to an array with the advertising data.
 * @param[in] advertise_data_len        The length of the advertising data.
 * @param[in] scan_response_data        Pointer to an array with the scan response data.
 * @param[in] scan_response_data_len    The length of the scan response data.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_advertise_undirected_start_op(enum gapm_own_addr address_src_type,
                                                            uint16_t interval,
                                                            uint8_t channel_map,
                                                            enum gap_adv_mode advertise_mode,
                                                            enum adv_filter_policy adv_filt_policy,
                                                            uint8_t* advertise_data,
                                                            uint8_t advertise_data_len,
                                                            uint8_t* scan_response_data,
                                                            uint8_t scan_response_data_len)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_advertise_start_msg_create();

    cmd->op.code = GAPM_ADV_UNDIRECT;
    cmd->op.addr_src = address_src_type;
    cmd->intv_max = interval;
    cmd->intv_min = interval;
    cmd->channel_map = channel_map;
    cmd->info.host.mode = advertise_mode;
    cmd->info.host.adv_filt_policy = adv_filt_policy;

    cmd->info.host.adv_data_len = advertise_data_len;
    ASSERT_ERROR(advertise_data_len <= (ADV_DATA_LEN - 3));  // The Flags data type are added by the ROM
    memcpy(cmd->info.host.adv_data, advertise_data, advertise_data_len);

    cmd->info.host.scan_rsp_data_len = scan_response_data_len;
    ASSERT_ERROR(scan_response_data_len <= SCAN_RSP_DATA_LEN);
    memcpy(cmd->info.host.scan_rsp_data, scan_response_data, scan_response_data_len);

    app_advertise_start_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Create advertising message for connectable directed event (ADV_DIRECT_IND). It
 *        supports the low duty cycle directed advertising mode.
 * @param[in] address_src_type          The source address type used during the
 *                                      advertising operation:
 *                                         - GAPM_STATIC_ADDR
 *                                         - GAPM_GEN_RSLV_ADDR
 *                                         - GAPM_GEN_NON_RSLV_ADDR
 * @param[in] intv_min                  Minimum interval for advertising.
 * @param[in] intv_max                  Maximum interval for advertising.
 * @param[in] channel_map               The channels used during the advertising operation.
 * @param[in] target_address_type       Address type of the target device.
 * @param[in] target_address            Address of the target device.
 * @param[in] ldc_enable                Enable/disable low duty cycle mode.
 *                                          - 0 = disabled
 *                                          - 1 = enabled
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_advertise_directed_start_op(enum gapm_own_addr address_src_type,
                                                          uint16_t intv_min,
                                                          uint16_t intv_max,
                                                          uint8_t channel_map,
                                                          enum address_type target_address_type,
                                                          uint8_t* target_address,
                                                          uint8_t ldc_enable)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_advertise_start_msg_create();

    if (ldc_enable)
    {
        cmd->op.code = GAPM_ADV_DIRECT_LDC;
        cmd->intv_min = intv_min;
        cmd->intv_max = intv_max;
    }
    else
    {
        cmd->op.code = GAPM_ADV_DIRECT;
        cmd->intv_min = LLM_ADV_INTERVAL_MIN;
        cmd->intv_max = LLM_ADV_INTERVAL_MAX;
    }

    cmd->op.addr_src = address_src_type;
    cmd->channel_map = channel_map;
    memcpy(cmd->info.direct.addr.addr, target_address, BD_ADDR_LEN);
    cmd->info.direct.addr_type = target_address_type;
    app_advertise_start_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Stop the active advertise operation.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_advertise_stop_op(void)
{
    // Disable Advertising
    struct gapm_cancel_cmd *cmd = app_advertise_stop_msg_create();
    app_advertise_stop_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Send parameter update request message.
 * @param[in] conidx                      The connection id.
 * @param[in] intv_min                    The new preferred minimum connection interval
 *                                        measured in 1.25 ms slots.
 * @param[in] intv_max                    The new preferred maximum connection interval
 *                                        measured in 1.25 ms slots.
 * @param[in] latency                     The slave latency measured in connection event
 *                                        periods.
 * @param[in] supervision_time_out        The supervision time out in timer units (10 ms).
 * @param[in] connection_event_len_min    The new preferred minimum connection event length
 *                                        measured in 1.25 ms slots.
 * @param[in] connection_event_len_max    The new preferred maximum connection event length
 *                                        measured in 1.25 ms slots.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_param_update_op(uint8_t conidx,
                                              uint16_t intv_min,
                                              uint16_t intv_max,
                                              uint16_t latency,
                                              uint16_t supervision_time_out,
                                              uint16_t connection_event_len_min,
                                              uint16_t connection_event_len_max)
{
    struct gapc_param_update_cmd *cmd = app_param_update_msg_create(conidx);

    cmd->intv_min = intv_min;
    cmd->intv_max = intv_max;
    cmd->latency = latency;
    cmd->time_out = supervision_time_out;
    cmd->ce_len_min = connection_event_len_min;
    cmd->ce_len_max = connection_event_len_max;

    app_param_update_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Send parameter update request message.
 * @param[in] conidx                      The connection id.
 * @param[in] intv_min_us                 The new preferred minimum connection interval
 *                                        measured in us.
 * @param[in] intv_max_us                 The new preferred maximum connection interval
 *                                        measured in us.
 * @param[in] latency                     The slave latency measured in connection event
 *                                        periods.
 * @param[in] supervision_time_out        The supervision time out in us.
 * @param[in] connection_event_len_min_us The new preferred minimum connection event length
 *                                        measured in us.
 * @param[in] connection_event_len_max_us The new preferred maximum connection event length
 *                                        measured in us.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_param_update_op_us(uint8_t conidx,
                                                 uint32_t intv_min_us,
                                                 uint32_t intv_max_us,
                                                 uint16_t latency,
                                                 uint32_t supervision_time_out_us,
                                                 uint32_t connection_event_len_min_us,
                                                 uint32_t connection_event_len_max_us)
{
    app_param_update_op(conidx,
                        (uint16_t) US_TO_DOUBLESLOTS(intv_min_us),
                        (uint16_t) US_TO_DOUBLESLOTS(intv_max_us),
                        latency,
                        (uint16_t) US_TO_TIMERUNITS(supervision_time_out_us),
                        (uint16_t) US_TO_DOUBLESLOTS(connection_event_len_min_us),
                        (uint16_t) US_TO_DOUBLESLOTS(connection_event_len_max_us));
}

/**
 ****************************************************************************************
 * @brief Send GAPM_SET_DEV_CONFIG_CMD message (device configuration).
 * @param[in] role              Device Role - Central, Peripheral, Observer, Broadcaster
 *                              or All roles:
 *                                  GAP_ROLE_NONE,
 *                                  GAP_ROLE_OBSERVER,
 *                                  GAP_ROLE_BROADCASTER,
 *                                  GAP_ROLE_CENTRAL,
 *                                  GAP_ROLE_PERIPHERAL,
 *                                  GAP_ROLE_ALL
 * @param[in] max_mtu           Maximum mtu supported.
 * @param[in] addr_type         Device Address Type :
 *                                  GAPM_CFG_ADDR_PUBLIC,
 *                                  GAPM_CFG_ADDR_PRIVATE,
 *                                  GAPM_CFG_ADDR_PRIVACY
 * @param[in] renew_dur         Duration before regenerate device address when privacy is
 *                              enabled.
 * @param[in] bd_addr           Provided own Random Static address if addr_type is
 *                              GAPM_CFG_ADDR_PRIVATE.
 * @param[in] irk               Pointer to an array that contains the device Identity
 *                              Resolving Key (IRK).
 * @param[in] gap_start_hdl     GAP service start handle.
 * @param[in] gatt_start_hdl    GATT service start handle.
 * @param[in] max_mps           Maximal MPS.
 * @param[in] max_txoctets      Maximal Tx octets.
 * @param[in] max_txtime        Maximal Tx time.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapm_configure_op(enum gap_role role,
                                                uint16_t max_mtu,
                                                uint8_t addr_type,
                                                uint16_t renew_dur,
                                                uint8_t* bd_addr,
                                                uint8_t* irk,
                                                uint8_t att_cfg,
                                                uint16_t gap_start_hdl,
                                                uint16_t gatt_start_hdl,
                                                uint16_t max_mps,
                                                uint16_t max_txoctets,
                                                uint16_t max_txtime)
{
    struct gapm_set_dev_config_cmd* cmd = app_gapm_configure_msg_create();

    cmd->role = role;
    cmd->max_mtu = max_mtu;
    cmd->addr_type = addr_type;
    if (addr_type == GAPM_CFG_ADDR_PRIVATE)
    {
        memcpy(cmd->addr.addr, bd_addr, KEY_LEN * sizeof(uint8_t));
    }
    memcpy(cmd->irk.key, irk, KEY_LEN * sizeof(uint8_t));
    cmd->att_cfg = att_cfg;
    cmd->gap_start_hdl = gap_start_hdl;
    cmd->gatt_start_hdl = gatt_start_hdl;
    cmd->max_mps = max_mps;
    cmd->max_txoctets = max_txoctets;
    cmd->max_txtime = max_txtime;

    app_gapm_configure_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Start security operation.
 * @param[in] conidx The id of the connection.
 * @param[in] auth The authentication requirements.
 * @return None.
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_security_request_op(uint8_t conidx, enum gap_auth auth)
{
    // Send security request command
    struct gapc_security_cmd * cmd = app_gapc_security_request_msg_create(conidx,auth);
    app_gapc_security_request_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Gap Bonding Pairing Response operation.
 * @param[in] conidx The id of the connection.
 * @param[in] io_capabilities Device capabilities: GAP_IO_CAP_DISPLAY_ONLY, GAP_IO_CAP_DISPLAY_YES_NO,
 *  GAP_IO_CAP_KB_ONLY, GAP_IO_CAP_NO_INPUT_NO_OUTPUT, GAP_IO_CAP_KB_DISPLAY.
 * @param[in] oob Out of band info: GAP_OOB_AUTH_DATA_NOT_PRESENT, GAP_OOB_AUTH_DATA_PRESENT
 * @param[in] authentication The authentication requirements.
 * @param[in] key_size The key size.
 * @param[in] initiator_key_dist Initiator key distriburion flags: GAP_KDIST_NONE, GAP_KDIST_ENCKEY,
 *  GAP_KDIST_IDKEY, GAP_KDIST_SIGNKEY
 * @param[in] responder_key_dist Responder key distriburion flags: GAP_KDIST_NONE, GAP_KDIST_ENCKEY,
 *  GAP_KDIST_IDKEY, GAP_KDIST_SIGNKEY
 * @param[in] security_requirements Security definition.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapc_bond_cfm_pairing_rsp_op (uint8_t conidx, enum gap_io_cap io_capabilities, enum gap_oob oob, enum gap_auth authentication, uint8_t key_size,
                                           enum gap_kdist initiator_key_dist, enum gap_kdist responder_key_dist, enum gap_sec_req security_requirements)
{
    struct gapc_bond_cfm* cfm = app_gapc_bond_cfm_msg_create ( conidx );
    cfm->data.pairing_feat.oob=oob;
    cfm->data.pairing_feat.key_size=key_size;
    cfm->data.pairing_feat.iocap=io_capabilities;
    cfm->data.pairing_feat.auth=authentication;
    cfm->data.pairing_feat.sec_req=security_requirements;
    cfm->data.pairing_feat.ikey_dist=initiator_key_dist;
    cfm->data.pairing_feat.rkey_dist=responder_key_dist;
    app_gapc_bond_cfm_msg_send (cfm);
}

/**
 ****************************************************************************************
 * @brief Gap Bonding Temporary Key exchange operation.
 * @param[in] conidx The id of the connection.
 * @param[in] temporary_key Array containing the temporary key.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapc_bond_cfm_tk_exch_op (uint8_t conidx, uint8_t* temporary_key)
{
    struct gapc_bond_cfm* cmd=app_gapc_bond_cfm_tk_exch_msg_create(conidx);
    memcpy((void*)cmd->data.tk.key,temporary_key,KEY_LEN);
    app_gapc_bond_cfm_tk_exch_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Gap Bonding Connection Signature Resolving Key exchange operation.
 * @param[in] conidx The id of the connection.
 * @param[in] csrk Array containing the csrk key.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapc_bond_cfm_csrk_exch_op(uint8_t conidx, uint8_t* csrk)
{
   struct gapc_bond_cfm* cmd=app_gapc_bond_cfm_csrk_exch_msg_create(conidx);
   memcpy((void*)cmd->data.csrk.key,csrk,KEY_LEN);
   app_gapc_bond_cfm_csrk_exch_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Gap Bonding Long Term Key exchange operation.
 * @param[in] conidx The id of the connection.
 * @param[in] long_term_key Array containing the long term key.
 * @param[in] encryption_key_size Encryption key size.
 * @param[in] random_number Random number.
 * @param[in] encryption_diversifier Encryption diversifier.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE void app_gapc_bond_cfm_ltk_exch_op(uint8_t conidx, uint8_t* long_term_key, uint8_t encryption_key_size,
                                            uint8_t* random_number, uint16_t encryption_diversifier)
{
    struct gapc_bond_cfm* cmd=app_gapc_bond_cfm_ltk_exch_msg_create(conidx);

    cmd->data.ltk.key_size = encryption_key_size;
    cmd->data.ltk.ediv = encryption_diversifier;

    memcpy(&(cmd->data.ltk.randnb), &(random_number), RAND_NB_LEN);
    memcpy(&(cmd->data.ltk.ltk), &(long_term_key), KEY_LEN);

    app_gapc_bond_cfm_ltk_exch_msg_send(cmd);
}

/**
 ****************************************************************************************
 * @brief Gap Bonding Encrypt confirmation operation.
 * @param[in] conidx The id of the connection.
 * @param[in] found Confirm that the entry has been found.
 * @param[in] key_size Size of the key.
 * @param[in] long_term_key The long term key.
 * @return void
 ****************************************************************************************
 */
__STATIC_FORCEINLINE  void app_gapc_encrypt_cfm_op (uint8_t conidx, bool found, uint8_t key_size, uint8_t* long_term_key)
{
    struct gapc_encrypt_cfm* cmd=app_gapc_encrypt_cfm_msg_create(conidx);
    cmd->found = found;
    if (found == true)
    {
        cmd->key_size = key_size;
        memcpy(&(cmd->ltk), long_term_key, KEY_LEN);
    }
}

/// @} APP

#endif // _APP_MID_H_
