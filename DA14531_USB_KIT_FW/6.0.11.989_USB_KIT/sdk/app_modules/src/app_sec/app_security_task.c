/**
 ****************************************************************************************
 *
 * @file app_sec_task.c
 *
 * @brief Application Security Task implementation
 *
 * Copyright (C) RivieraWaves 2009-2013
 * Copyright (C) 2017-2018 Modified by Dialog Semiconductor
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APPSECTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_APP_SEC)

#include "arch.h"
#include "gapc_task.h"          // GAP Controller Task API
#include "gapm_task.h"          // GAP Manager Task API
#include "app_security.h"       // Application Security Definition
#include "app_security_task.h"  // Application Security Task Definition
#include "app_api.h"
#include "user_callback_config.h"
#include "user_config.h"

#if defined (CFG_PRINTF)
#include "arch_console.h"
#endif

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles reception of bond request command.
 * @param[in] msgid     Id of the message received
 * @param[in] param     Pointer to the parameters of the message
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP)
 * @param[in] src_id    ID of the sending task instance
 * @return If the message was consumed or not
 ****************************************************************************************
 */
static int gapc_bond_req_ind_handler(ke_msg_id_t const msgid,
                                     struct gapc_bond_req_ind *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);
    switch(param->request)
    {
        // Bond Pairing request
        case GAPC_PAIRING_REQ:
        {
            app_env[conidx].pairing_in_progress = true;
            app_sec_env[conidx].valid_keys = NOKEY_PRESENT;
            CALLBACK_ARGS_2(user_app_callbacks.app_on_pairing_request, conidx, param)
        }
        break;

        // Retrieve Temporary Key (TK)
        case GAPC_TK_EXCH:
        {
            CALLBACK_ARGS_2(user_app_callbacks.app_on_tk_exch, conidx, param)
        }
        break;

        // Retrieve Identity Resolving Key (IRK)
        case GAPC_IRK_EXCH:
        {
            CALLBACK_ARGS_1(user_app_callbacks.app_on_irk_exch, param)
        }
        break;

        // Retrieve Connection Signature Resolving Key (CSRK)
        case GAPC_CSRK_EXCH:
        {
            CALLBACK_ARGS_2(user_app_callbacks.app_on_csrk_exch, conidx, param)
        }
        break;

        // Retrieve Long Term Key (LTK)
        case GAPC_LTK_EXCH:
        {
            CALLBACK_ARGS_2(user_app_callbacks.app_on_ltk_exch, conidx, param)
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
 * @brief Handles reception of bond indication.
 * @param[in] msgid     Id of the message received
 * @param[in] param     Pointer to the parameters of the message
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP)
 * @param[in] src_id    ID of the sending task instance
 * @return If the message was consumed or not
 ****************************************************************************************
 */
static int gapc_bond_ind_handler(ke_msg_id_t const msgid,
                                 struct gapc_bond_ind *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);
    switch(param->info)
    {
        // Bond Pairing request
        case GAPC_PAIRING_SUCCEED:
        {
            // Save Authentication level
            app_sec_env[conidx].auth = (enum gap_auth) param->data.auth;

            if (app_sec_env[conidx].auth & GAP_AUTH_BOND)
            {
                memcpy(&app_sec_env[conidx].peer_bdaddr.addr, app_env[conidx].peer_addr.addr, BD_ADDR_LEN);
                app_sec_env[conidx].peer_bdaddr.addr_type = app_env[conidx].peer_addr_type;
            }
            CALLBACK_ARGS_1(user_app_callbacks.app_on_pairing_succeeded, conidx)
            app_env[conidx].pairing_in_progress = false;
        }
        break;

        case GAPC_PAIRING_FAILED:
        {
            app_env[conidx].pairing_in_progress = false;
            // Clear bond data
            app_sec_env[conidx].auth = GAP_AUTH_NONE;
            // Disconnect
            app_easy_gap_disconnect(conidx);
        }
        break;

        case (GAPC_IRK_EXCH):
            // Store Remote IRK to security environment
            app_sec_env[conidx].rirk = param->data.irk;
            app_sec_env[conidx].valid_keys |= RIRK_PRESENT;
#if (USER_CFG_ADDRESS_MODE == APP_CFG_CNTL_PRIV_RPA_PUB) || (USER_CFG_ADDRESS_MODE == APP_CFG_CNTL_PRIV_RPA_RAND)
            struct gap_ral_dev_info dev_info;
            memcpy(&dev_info.addr, &param->data.irk.addr.addr.addr, BD_ADDR_LEN*sizeof(uint8_t));
            dev_info.addr_type = param->data.irk.addr.addr_type;
            memcpy(&dev_info.peer_irk, &param->data.irk.irk.key, KEY_LEN*sizeof(uint8_t));
            memcpy(&dev_info.local_irk, user_gapm_conf.irk, KEY_LEN*sizeof(uint8_t));
            app_easy_security_ral_op(APP_RMV_DEV_FRM_RAL, &dev_info);
            app_easy_security_ral_op(APP_ADD_DEV_IN_RAL, &dev_info);
#endif
        break;

        case (GAPC_CSRK_EXCH):
            // Store Remote CSRK to security environment
            app_sec_env[conidx].rcsrk = param->data.csrk;
            app_sec_env[conidx].valid_keys |= RCSRK_PRESENT;
        break;

        case (GAPC_LTK_EXCH):
#if (ENABLE_SMP_SECURE)
            // In case of Secure Connection (ediv = 0), store LTK to app_sec_env
            if (param->data.ltk.ediv == 0)
            {
                app_sec_env[conidx].ltk = param->data.ltk;
                app_sec_env[conidx].valid_keys |= LTK_PRESENT;
                #if defined (CFG_PRINTF)
                // Print LTK for debugging (e.g. when using a sniffer)
                arch_printf(" \r\n LTK: 0x");
                for (int count = 0; count < sizeof(struct gap_sec_key); count ++)
                    arch_printf("%02x", app_sec_env[conidx].ltk.ltk.key[15 - count]);
                #endif
            }
            // In case of Legacy Pairing (ediv != 0) RLTK has been sent by initiator for future reversed-role usage
            else
#endif // ENABLE_SMP_SECURE
            {
                app_sec_env[conidx].rltk = param->data.ltk;
                app_sec_env[conidx].valid_keys |= RLTK_PRESENT;
            }
        break;

        default:
        {
            if (app_process_catch_rest_cb != NULL)
            {
                app_process_catch_rest_cb(msgid, param, dest_id, src_id);
            }
        }
        break;
    }
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of encrypt request command.
 * @param[in] msgid     Id of the message received
 * @param[in] param     Pointer to the parameters of the message
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP)
 * @param[in] src_id    ID of the sending task instance
 * @return If the message was consumed or not
 ****************************************************************************************
 */
static int gapc_encrypt_req_ind_handler(ke_msg_id_t const msgid,
                                        struct gapc_encrypt_req_ind *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_callbacks.app_on_encrypt_req_ind, KE_IDX_GET(src_id), param)

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of encrypt indication.
 * @param[in] msgid     Id of the message received
 * @param[in] param     Pointer to the parameters of the message
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP)
 * @param[in] src_id    ID of the sending task instance
 * @return If the message was consumed or not
 ****************************************************************************************
 */
static int gapc_encrypt_ind_handler(ke_msg_id_t const msgid,
                                    struct gapc_encrypt_ind *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    uint8_t conidx = KE_IDX_GET(src_id);
    app_sec_env[conidx].auth = param->auth;
    CALLBACK_ARGS_2(user_app_callbacks.app_on_encrypt_ind, conidx, param->auth)

    return (KE_MSG_CONSUMED);
}

static int gapm_addr_solved_ind_handler(ke_msg_id_t const msgid,
                                        struct gapm_addr_solved_ind *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_callbacks.app_on_addr_solved_ind, KE_IDX_GET(src_id), param)

    return (KE_MSG_CONSUMED);
}

static int gapm_ral_size_ind_handler(ke_msg_id_t const msgid,
                                        struct gapm_ral_size_ind *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    CALLBACK_ARGS_1(user_app_callbacks.app_on_ral_size_ind, param->size)

    return (KE_MSG_CONSUMED);
}

static int gapm_ral_addr_ind_handler(ke_msg_id_t const msgid,
                                        struct gapm_ral_addr_ind *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    CALLBACK_ARGS_2(user_app_callbacks.app_on_ral_addr_ind, param->operation, param->addr)

    return (KE_MSG_CONSUMED);
}

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

static const struct ke_msg_handler app_sec_process_handlers[] =
{
    {GAPC_BOND_REQ_IND,                     (ke_msg_func_t)gapc_bond_req_ind_handler},
    {GAPC_BOND_IND,                         (ke_msg_func_t)gapc_bond_ind_handler},
    {GAPC_ENCRYPT_REQ_IND,                  (ke_msg_func_t)gapc_encrypt_req_ind_handler},
    {GAPC_ENCRYPT_IND,                      (ke_msg_func_t)gapc_encrypt_ind_handler},
    {GAPM_ADDR_SOLVED_IND,                  (ke_msg_func_t)gapm_addr_solved_ind_handler},
    {GAPM_RAL_SIZE_IND,                     (ke_msg_func_t)gapm_ral_size_ind_handler},
    {GAPM_RAL_ADDR_IND,                     (ke_msg_func_t)gapm_ral_addr_ind_handler},
};

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

enum process_event_response app_sec_process_handler(ke_msg_id_t const msgid,
                                                    void const *param,
                                                    ke_task_id_t const dest_id,
                                                    ke_task_id_t const src_id,
                                                    enum ke_msg_status_tag *msg_ret)
{
    return app_std_process_event(msgid,
                                 param,
                                 src_id,
                                 dest_id,
                                 msg_ret,
                                 app_sec_process_handlers,
                                 sizeof(app_sec_process_handlers) / sizeof(struct ke_msg_handler));
}

#endif // (BLE_APP_SEC)

/// @} APPSECTASK
