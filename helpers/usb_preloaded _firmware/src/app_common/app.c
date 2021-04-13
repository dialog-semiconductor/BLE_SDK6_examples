/**
 ****************************************************************************************
 *
 * @file app.c
 *
 * @brief Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2013
 * Copyright (C) 2017-2020 Modified by Dialog Semiconductor
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

#include "rwip_config.h"        // SW configuration

#if (BLE_APP_PRESENT)
#include "app_task.h"           // Application task Definition
#include "app.h"                // Application Definition
#include "gapm_task.h"          // GAP Manager Task API
#include "gapc_task.h"          // GAP Controller Task API
#include "gattc_task.h"         // GATT Controller Task API
#include "co_math.h"            // Common Maths Definition
#include "app_api.h"            // Application task Definition
#include "app_prf_types.h"
#include "app_prf_perm_types.h"
#include "app_security.h"       // Application security Definition
#include "nvds.h"               // NVDS Definitions
#include "user_callback_config.h"
#include "app_default_handlers.h"
#include "app_mid.h"
#include "ke_mem.h"
#include "app_adv_data.h"
#include "llm.h"
#include "app_callback.h"

#include "user_config.h"

#if BLE_CUSTOM_SERVER
#include "user_custs_config.h"
#endif

#include "user_periph_setup.h"
#include "user_config.h"
#include "uart_utils.h"
#include "timer0_2.h"
#include "timer0.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define APP_EASY_GAP_MAX_CONNECTION     APP_EASY_MAX_ACTIVE_CONNECTION


//volatile uint8_t timeout_expiration;
static tim0_2_clk_div_config_t clk_div_config =
{
    .clk_div  = TIM0_2_CLK_DIV_8
};
void timer0_general_test(uint32_t times_seconds);
// Timer0 settings
#define NO_PWM            0x0                       // PWM not used
#define RELOAD_100MS      20000                     // reload value for 100ms
#define TEST_LENGTH_SEC   10                        // length of the test in seconds
/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Application Environment Structure
struct app_env_tag app_env[APP_EASY_MAX_ACTIVE_CONNECTION]  __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

app_prf_srv_sec_t app_prf_srv_perm[PRFS_TASK_ID_MAX]        __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

struct app_device_info device_info                          __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

const struct prf_func_callbacks prf_funcs[] =
{
#if (BLE_GATT_CLIENT)
    {TASK_ID_GATT_CLIENT,   app_gattc_create_task, NULL},
#endif

#if (BLE_PROX_REPORTER)
    {TASK_ID_PROXR,         app_proxr_create_db, NULL},
#endif

#if (BLE_BATT_SERVER)
    {TASK_ID_BASS,          app_bass_create_db, app_bass_enable},
#endif

#if (BLE_FINDME_TARGET)
    {TASK_ID_FINDT,         app_findt_create_db, NULL},
#endif

#if (BLE_FINDME_LOCATOR)
    {TASK_ID_FINDL,         app_findl_create_task, app_findl_enable},
#endif

#if (BLE_DIS_SERVER)
    {TASK_ID_DISS,          app_diss_create_db, NULL},
#endif

#if (BLE_BMS_SERVER)
    {TASK_ID_BMSS,          app_bmss_create_db, NULL},
#endif

#if (BLE_ANC_CLIENT)
    {TASK_ID_ANCC,          app_ancc_create_db, NULL},
#endif

#if (BLE_BCS_SERVER)
    {TASK_ID_BCSS,          app_bcss_create_db, app_bcss_enable},
#endif

#if (BLE_UDS_SERVER)
    {TASK_ID_UDSS,          app_udss_create_db, NULL},
#endif

#if (BLE_CTS_SERVER)
    {TASK_ID_CTSS,          app_ctss_create_db, NULL},
#endif

#if (BLE_CTS_CLIENT)
    {TASK_ID_CTSC,          app_ctsc_create_task, app_ctsc_enable},
#endif

#if (BLE_SUOTA_RECEIVER)
    {TASK_ID_SUOTAR,        app_suotar_create_db, NULL},
#endif

#if (BLE_WSS_SERVER)
    {TASK_ID_WSSS,          app_wsss_create_db, app_wsss_enable},
#endif

    {TASK_ID_INVALID,       NULL, NULL},   // DO NOT MOVE. Must always be last
};

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Application Task Descriptor
static const struct ke_task_desc TASK_DESC_APP = {NULL,
                                                  &app_default_handler,
                                                  app_state,
                                                  APP_STATE_MAX,
                                                  APP_IDX_MAX};

static struct gapc_param_update_cmd *param_update_cmd[APP_EASY_GAP_MAX_CONNECTION] __SECTION_ZERO("retention_mem_area0"); // @RETENTION MEMORY

static struct gapm_set_dev_config_cmd *set_dev_config_cmd                          __SECTION_ZERO("retention_mem_area0"); // @RETENTION MEMORY

static struct gapm_start_advertise_cmd *adv_cmd                                    __SECTION_ZERO("retention_mem_area0"); // @RETENTION MEMORY

static struct gapm_start_connection_cmd *start_connection_cmd                      __SECTION_ZERO("retention_mem_area0"); // @RETENTION MEMORY

static timer_hnd adv_timer_id                                                      __SECTION_ZERO("retention_mem_area0"); // @RETENTION MEMORY

static void (*adv_timeout_callback)(void)                                          __SECTION_ZERO("retention_mem_area0"); // @RETENTION MEMORY

#if (USER_CFG_ADDRESS_MODE == APP_CFG_ADDR_STATIC)
struct bd_addr app_random_addr                                              __SECTION_ZERO("retention_mem_area0"); //@ RETENTION MEMORY
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Check if the task_id has an entry in the user_prf_func
 * @param[in] task_id The task_id to check
 * @return true if the task_id has an entry in the user_prf_func.
 ****************************************************************************************
 */
static bool app_task_in_user_app(enum KE_API_ID task_id)
{
    uint8_t i = 0;

    while(user_prf_funcs[i].task_id != TASK_ID_INVALID)
    {
        if (user_prf_funcs[i].task_id == task_id)
        {
            return true;
        }
        i++;
    }
     return false;
}

/**
 ****************************************************************************************
 * @brief Initialize the database for all the included profiles.
 * @return true if succeeded, else false
 ****************************************************************************************
 */
#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)
static bool app_db_init_next(void)
#else
bool app_db_init_next(void)
#endif
{
    static uint8_t i __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY;
    static uint8_t k __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY;

    // initialise the databases for all the included profiles
    while(user_prf_funcs[k].task_id != TASK_ID_INVALID)
    {
        if (user_prf_funcs[k].db_create_func != NULL)
        {
            user_prf_funcs[k++].db_create_func();
            return false;
        }
        else k++;
    }

    // initialise the databases for all the included profiles
    while(prf_funcs[i].task_id != TASK_ID_INVALID)
    {
        if ((prf_funcs[i].db_create_func != NULL)
            && (!app_task_in_user_app(prf_funcs[i].task_id)))    //case that the this task has an entry in the user_prf as well
        {
            prf_funcs[i++].db_create_func();
            return false;
        }
        else i++;
    }

#if (BLE_CUSTOM_SERVER)
    {
        static uint8_t j __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY;

        while(cust_prf_funcs[j].task_id != TASK_ID_INVALID)
        {
            if(cust_prf_funcs[j].db_create_func != NULL)
            {
                cust_prf_funcs[j++].db_create_func();
                return false;
            }
            else j++;
        }
        j = 0;
    }
#endif

    k = 0;
    i = 0;

    return true;
}

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)
bool app_db_init_start(void)
{
    // Indicate if more services need to be added in the database
    bool end_db_create;

    // We are now in Initialization State
    ke_state_set(TASK_APP, APP_DB_INIT);

    end_db_create = app_db_init_next();

    return end_db_create;
}

bool app_db_init(void)
{
    // Indicate if more services need to be added in the database
    bool end_db_create;

    end_db_create = app_db_init_next();

    return end_db_create;
}
#endif

/**
 ****************************************************************************************
 * @brief Calls all the enable function of the profile registered in prf_func,
 *        custs_prf_func and user_prf_func.
 * @param[in] conidx The connection handle
 ****************************************************************************************
 */
void app_prf_enable(uint8_t conidx)
 {
     uint8_t i = 0;

     while(user_prf_funcs[i].task_id != TASK_ID_INVALID)
     {
        if(user_prf_funcs[i].enable_func != NULL)
        {
            user_prf_funcs[i++].enable_func(conidx);
        }
            else i++;
     }

     i = 0;

    /*--------------------------------------------------------------
    * ENABLE REQUIRED PROFILES
    *-------------------------------------------------------------*/
    while(prf_funcs[i].task_id != TASK_ID_INVALID)
    {
        if(( prf_funcs[i].enable_func != NULL ) && (!app_task_in_user_app(prf_funcs[i].task_id)))
        {
            prf_funcs[i++].enable_func(conidx);
        }
        else i++;
    }

    i = 0;

#if (BLE_CUSTOM_SERVER)
    while(cust_prf_funcs[i].task_id != TASK_ID_INVALID)
    {
        if(cust_prf_funcs[i].enable_func != NULL)
        {
            cust_prf_funcs[i++].enable_func(conidx);
        }
        else i++;
    }
#endif
}

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)
/**
 ****************************************************************************************
 * @brief Append the device name in the advertising data or scan response data.
 * @param[in|out] len        The advertising or scan response data length.
 * @param[in] name_length    The device name length.
 * @param[in|out] data       Pointer to the buffer which will carry the device name.
 * @param[in] name_data     The device name.
 ****************************************************************************************
 */
static void append_device_name(uint8_t *len,
                               const uint8_t name_length,
                               uint8_t *data,
                               const void *name_data)
{
    // Fill Length
    data[0] = name_length + 1;

    // Fill Device Name Flag
    data[1] = GAP_AD_TYPE_COMPLETE_NAME;

    // Copy device name
    memcpy(&data[2], name_data, name_length);

    // Update advertising or scan response data length
    *len += name_length + 2;
}
#endif

/**
 ****************************************************************************************
 * @brief Create advertising message for nonconnectable undirected event (ADV_NONCONN_IND).
 * @return gapm_start_advertise_cmd     Pointer to the advertising message
 * @note This function supports also the advertising with scan response (ADV_SCAN_IND),
 * if the scan response data are NOT empty.
 ****************************************************************************************
 */
static struct gapm_start_advertise_cmd* app_easy_gap_non_connectable_advertise_start_create_msg(void)
{
    // Allocate a message for GAP
    if (adv_cmd == NULL)
    {
        ASSERT_ERROR(USER_ADVERTISE_DATA_LEN <= ADV_DATA_LEN); // The Flags data type may be omitted (CCSv6)
        ASSERT_ERROR(USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN <= SCAN_RSP_DATA_LEN);

        struct gapm_start_advertise_cmd *cmd;
        cmd = app_advertise_start_msg_create();
        adv_cmd = cmd;

        cmd->op.code = GAPM_ADV_NON_CONN;
        cmd->op.addr_src = user_adv_conf.addr_src;
        cmd->intv_min = user_adv_conf.intv_min;
        cmd->intv_max = user_adv_conf.intv_max;
        cmd->channel_map = user_adv_conf.channel_map;
        cmd->info.host.mode = user_adv_conf.mode;
        cmd->info.host.adv_filt_policy = user_adv_conf.adv_filt_policy;
        cmd->info.host.adv_data_len = USER_ADVERTISE_DATA_LEN;
        memcpy(&(cmd->info.host.adv_data[0]), USER_ADVERTISE_DATA, USER_ADVERTISE_DATA_LEN);
        cmd->info.host.scan_rsp_data_len = USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN;
        memcpy(&(cmd->info.host.scan_rsp_data[0]), USER_ADVERTISE_SCAN_RESPONSE_DATA, USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN);

        // Place the Device Name in the Advertising Data or in the Scan Response Data
        if (USER_DEVICE_NAME_LEN > 0)
        {
            // Get remaining space in the Advertising Data ( plus 2 bytes are used for the length and flag bytes of the Device Name)
            uint16_t total_adv_space = adv_cmd->info.host.adv_data_len + 2 + USER_DEVICE_NAME_LEN;
            // Get remaining space in the Scan Response Data ( plus 2 bytes are used for the length and flag bytes of the Device Name)
            uint16_t total_scan_space = adv_cmd->info.host.scan_rsp_data_len + 2 + USER_DEVICE_NAME_LEN;

            if (total_adv_space <= ADV_DATA_LEN)
            {
                append_device_name(&cmd->info.host.adv_data_len,
                                   USER_DEVICE_NAME_LEN,
                                   &(cmd->info.host.adv_data[cmd->info.host.adv_data_len]),
                                   USER_DEVICE_NAME);
            }
            else if (total_scan_space <= SCAN_RSP_DATA_LEN)
            {
                append_device_name(&cmd->info.host.scan_rsp_data_len,
                                   USER_DEVICE_NAME_LEN,
                                   &(cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len]),
                                   USER_DEVICE_NAME);
            }
         }
    }
    return adv_cmd;
}

/**
 ****************************************************************************************
 * @brief Create advertising message for connectable undirected event (ADV_IND).
 * @return gapm_start_advertise_cmd Pointer to the advertising message
 ****************************************************************************************
 */
static struct gapm_start_advertise_cmd* app_easy_gap_undirected_advertise_start_create_msg(void)
{
    // Allocate a message for GAP
    if (adv_cmd == NULL)
    {
        ASSERT_ERROR(USER_ADVERTISE_DATA_LEN <= (ADV_DATA_LEN - 3)); // The Flags data type are added by the ROM
        ASSERT_ERROR(USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN <= SCAN_RSP_DATA_LEN);

        struct gapm_start_advertise_cmd *cmd;
        cmd = app_advertise_start_msg_create();
        adv_cmd = cmd;

        cmd->op.code = GAPM_ADV_UNDIRECT;
        cmd->op.addr_src = user_adv_conf.addr_src;
        cmd->intv_min = user_adv_conf.intv_min;
        cmd->intv_max = user_adv_conf.intv_max;
        cmd->channel_map = user_adv_conf.channel_map;
        cmd->info.host.mode = user_adv_conf.mode;
        cmd->info.host.adv_filt_policy = user_adv_conf.adv_filt_policy;
        if (user_adv_conf.adv_filt_policy == ADV_ALLOW_SCAN_ANY_CON_WLST)
        {
            cmd->info.host.mode = GAP_NON_DISCOVERABLE;
        }
        else
        {
            cmd->info.host.mode = user_adv_conf.mode;
        }
        adv_cmd->info.host.adv_data_len = USER_ADVERTISE_DATA_LEN;
        memcpy(&(cmd->info.host.adv_data[0]), USER_ADVERTISE_DATA, USER_ADVERTISE_DATA_LEN);
        adv_cmd->info.host.scan_rsp_data_len = USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN;
        memcpy(&(cmd->info.host.scan_rsp_data[0]), USER_ADVERTISE_SCAN_RESPONSE_DATA, USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN);

#if (USER_CFG_ADDRESS_MODE == APP_CFG_CNTL_PRIV_RPA_RAND)
        // Local Address has been added to RAL. Use this entry to advertise with RPA
        memcpy(cmd->info.host.peer_info.addr.addr, &(gapm_env.addr), BD_ADDR_LEN * sizeof(uint8_t));
        cmd->info.host.peer_info.addr_type = ((GAPM_F_GET(gapm_env.cfg_flags, ADDR_TYPE) == GAPM_CFG_ADDR_PUBLIC) ? ADDR_PUBLIC : ADDR_RAND);

#elif (USER_CFG_ADDRESS_MODE == APP_CFG_CNTL_PRIV_RPA_PUB)
        // If there is at least one bond device in Bond Database, use its address to advertise with RPA, else use Public address
        uint8_t bdb_size;
        bool bonded_dev_found = false;
        struct gap_ral_dev_info bonded_dev_info;

        // Get Bond Database size
        bdb_size = app_easy_security_bdb_get_size();
        for (uint8_t bdb_slot = 0; bdb_slot < bdb_size; bdb_slot++)
        {
            bonded_dev_found = app_easy_security_bdb_get_device_info_from_slot(bdb_slot, &bonded_dev_info);
            if (bonded_dev_found)
            {
                // Add peer device identity found in advertising command.
                memcpy(cmd->info.host.peer_info.addr.addr, &bonded_dev_info.addr, BD_ADDR_LEN * sizeof(uint8_t));
                cmd->info.host.peer_info.addr_type = bonded_dev_info.addr_type;
                break;
            }
        }
#endif

        // Place the Device Name in the Advertising Data or in the Scan Response Data
        if (USER_DEVICE_NAME_LEN > 0)
        {
            // Get remaining space in the Advertising Data ( plus 2 bytes are used for the length and flag bytes of the Device Name and 3 bytes for the AD type flags)
            uint16_t total_adv_space = 3 + adv_cmd->info.host.adv_data_len + 2 + USER_DEVICE_NAME_LEN;
            // Get remaining space in the Scan Response Data ( plus 2 bytes are used for the length and flag bytes of the Device Name)
            uint16_t total_scan_space = adv_cmd->info.host.scan_rsp_data_len + 2 + USER_DEVICE_NAME_LEN;

            if (total_adv_space <= ADV_DATA_LEN)
            {
                append_device_name(&cmd->info.host.adv_data_len,
                                   USER_DEVICE_NAME_LEN,
                                   &(cmd->info.host.adv_data[cmd->info.host.adv_data_len]),
                                   USER_DEVICE_NAME);
            }
            else if (total_scan_space <= SCAN_RSP_DATA_LEN)
            {
                append_device_name(&cmd->info.host.scan_rsp_data_len,
                                   USER_DEVICE_NAME_LEN,
                                   &(cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len]),
                                   USER_DEVICE_NAME);
            }
         }
    }
    return adv_cmd;
}

/**
 ****************************************************************************************
 * @brief Create advertising message for connectable directed event (ADV_DIRECT_IND). It
 *        supports the low duty cycle directed advertising mode.
 * @param[in] ldc_enable       Enable/disable low duty cycle mode.
 *                                 - 0 = disabled
 *                                 - 1 = enabled
 * @return gapm_start_advertise_cmd Pointer to the advertising message
 ****************************************************************************************
 */
static struct gapm_start_advertise_cmd* app_easy_gap_directed_advertise_start_create_msg(uint8_t ldc_enable)
{
    // Allocate a message for GAP
    if (adv_cmd == NULL)
    {
        struct gapm_start_advertise_cmd *cmd;
        cmd = app_advertise_start_msg_create();
        adv_cmd = cmd;

        if (ldc_enable)
        {
            cmd->op.code = GAPM_ADV_DIRECT_LDC;
            cmd->intv_min = user_adv_conf.intv_min;
            cmd->intv_max = user_adv_conf.intv_max;
        }
        else
        {
            cmd->op.code = GAPM_ADV_DIRECT;
            cmd->intv_min = LLM_ADV_INTERVAL_MIN;
            cmd->intv_max = LLM_ADV_INTERVAL_MAX;
        }
        cmd->op.addr_src = user_adv_conf.addr_src;
        cmd->channel_map = user_adv_conf.channel_map;
        memcpy(cmd->info.direct.addr.addr, user_adv_conf.peer_addr, BD_ADDR_LEN*sizeof(uint8_t));
        cmd->info.direct.addr_type = user_adv_conf.peer_addr_type;
    }
    return adv_cmd;
}

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)
void app_easy_gap_update_adv_data(const uint8_t *update_adv_data,
                                  uint8_t update_adv_data_len,
                                  const uint8_t *update_scan_rsp_data,
                                  uint8_t update_scan_rsp_data_len)
{
    // Instantiate the advertising update message to be sent
    struct gapm_update_advertise_data_cmd *cmd = KE_MSG_ALLOC(GAPM_UPDATE_ADVERTISE_DATA_CMD,
                                                              TASK_GAPM,
                                                              TASK_APP,
                                                              gapm_update_advertise_data_cmd);

    cmd->operation = GAPM_UPDATE_ADVERTISE_DATA;
    cmd->adv_data_len = update_adv_data_len;
    memcpy(cmd->adv_data, update_adv_data, update_adv_data_len);
    cmd->scan_rsp_data_len = update_scan_rsp_data_len;
    memcpy(cmd->scan_rsp_data, update_scan_rsp_data, update_scan_rsp_data_len);

    // Send the message
    ke_msg_send(cmd);
}
#endif

/**
 ****************************************************************************************
 * @brief Create parameter update request message.
 * @return gapc_param_update_cmd Pointer to the parameter update request message
 ****************************************************************************************
 */
static struct gapc_param_update_cmd* app_easy_gap_param_update_msg_create(uint8_t conidx)
{
    // Allocate a message for GAP
    if (param_update_cmd[conidx] == NULL)
    {
        struct gapc_param_update_cmd* cmd;
        cmd = app_param_update_msg_create(conidx);
        ASSERT_WARNING(conidx < APP_EASY_GAP_MAX_CONNECTION);
        param_update_cmd[conidx] = cmd;

        cmd->intv_max = user_connection_param_conf.intv_max;
        cmd->intv_min = user_connection_param_conf.intv_min;
        cmd->latency = user_connection_param_conf.latency;
        cmd->time_out = user_connection_param_conf.time_out;
        cmd->ce_len_min = user_connection_param_conf.ce_len_min;
        cmd->ce_len_max = user_connection_param_conf.ce_len_max;
    }
    return param_update_cmd[conidx];
}

/**
 ****************************************************************************************
 * @brief Create GAPM_START_CONNECTION_CMD message (connection message).
 * @return gapm_start_connection_cmd Pointer to GAPM_START_CONNECTION_CMD message
 ****************************************************************************************
 */
static struct gapm_start_connection_cmd* app_easy_gap_start_connection_to_msg_create(void)
{
    // Allocate a message for GAP
    if (start_connection_cmd == NULL)
    {
        struct gapm_start_connection_cmd *cmd;
        cmd = app_connect_start_msg_create();
        start_connection_cmd = cmd;

        cmd->op.code = user_central_conf.code;
        cmd->op.addr_src = user_central_conf.addr_src;
        cmd->scan_interval = user_central_conf.scan_interval;
        cmd->scan_window = user_central_conf.scan_window;
        cmd->con_intv_min = user_central_conf.con_intv_min;
        cmd->con_intv_max = user_central_conf.con_intv_max;
        cmd->con_latency = user_central_conf.con_latency;
        cmd->superv_to = user_central_conf.superv_to;
        cmd->ce_len_min = user_central_conf.ce_len_min;
        cmd->ce_len_max = user_central_conf.ce_len_max;

        /// Number of peer device information present in message.
        /// Shall be 1 for GAPM_CONNECTION_DIRECT or GAPM_CONNECTION_NAME_REQUEST operations
        /// Shall be greater than 0 for other operations
        if ((user_central_conf.code == GAPM_CONNECTION_DIRECT) || (user_central_conf.code == GAPM_CONNECTION_NAME_REQUEST))
        {
            cmd->nb_peers = 1;
        }
        else
        {
            cmd->nb_peers = CFG_MAX_CONNECTIONS;

            #if (CFG_MAX_CONNECTIONS >= 1)
            memcpy(cmd->peers[0].addr.addr, user_central_conf.peer_addr_0, BD_ADDR_LEN*sizeof(uint8_t));
            cmd->peers[0].addr_type = user_central_conf.peer_addr_0_type;
            #endif

            #if (CFG_MAX_CONNECTIONS >= 2)
            memcpy(cmd->peers[1].addr.addr, user_central_conf.peer_addr_1, BD_ADDR_LEN*sizeof(uint8_t));
            cmd->peers[1].addr_type = user_central_conf.peer_addr_1_type;
            #endif

            #if (CFG_MAX_CONNECTIONS >= 3)
            memcpy(cmd->peers[2].addr.addr, user_central_conf.peer_addr_2, BD_ADDR_LEN*sizeof(uint8_t));
            cmd->peers[2].addr_type = user_central_conf.peer_addr_2_type;
            #endif

            #if (CFG_MAX_CONNECTIONS >= 4)
            memcpy(cmd->peers[3].addr.addr, user_central_conf.peer_addr_3, BD_ADDR_LEN*sizeof(uint8_t));
            cmd->peers[3].addr_type = user_central_conf.peer_addr_3_type;
            #endif

            #if (CFG_MAX_CONNECTIONS >= 5)
            memcpy(cmd->peers[4].addr.addr, user_central_conf.peer_addr_4, BD_ADDR_LEN*sizeof(uint8_t));
            cmd->peers[4].addr_type = user_central_conf.peer_addr_4_type;
            #endif

            #if (CFG_MAX_CONNECTIONS >= 6)
            memcpy(cmd->peers[5].addr.addr, user_central_conf.peer_addr_5, BD_ADDR_LEN*sizeof(uint8_t));
            cmd->peers[5].addr_type = user_central_conf.peer_addr_5_type;
            #endif

            #if (CFG_MAX_CONNECTIONS >= 7)
            memcpy(cmd->peers[6].addr.addr, user_central_conf.peer_addr_6, BD_ADDR_LEN*sizeof(uint8_t));
            cmd->peers[6].addr_type = user_central_conf.peer_addr_6_type;
            #endif

            #if (CFG_MAX_CONNECTIONS >= 8)
            memcpy(cmd->peers[7].addr.addr, user_central_conf.peer_addr_7, BD_ADDR_LEN*sizeof(uint8_t));
            cmd->peers[7].addr_type = user_central_conf.peer_addr_7_type;
            #endif
        }
    }
    return start_connection_cmd;
}

/**
 ****************************************************************************************
 * @brief Create GAPM_SET_DEV_CONFIG_CMD message (device configuration).
 * @return gapm_set_dev_config_cmd Pointer to GAPM_SET_DEV_CONFIG_CMD message
 ****************************************************************************************
 */
static struct gapm_set_dev_config_cmd* app_easy_gap_dev_config_create_msg(void)
{
    // Allocate a message for GAP
    if (set_dev_config_cmd == NULL)
    {
        struct gapm_set_dev_config_cmd* cmd;
        cmd = app_gapm_configure_msg_create();
        set_dev_config_cmd = cmd;

        cmd->role = user_gapm_conf.role;
        cmd->max_mtu = user_gapm_conf.max_mtu;
        cmd->addr_type = user_gapm_conf.addr_type;
// If privacy is disabled, there is no need to set the renew duration
#if (USER_CFG_ADDRESS_MODE == APP_CFG_ADDR_PUB) || (USER_CFG_ADDRESS_MODE == APP_CFG_ADDR_STATIC)
        cmd->renew_dur = 0;
        memset(cmd->irk.key, 0, KEY_LEN*sizeof(uint8_t));
#else
        cmd->renew_dur = user_gapm_conf.renew_dur;
        memcpy(cmd->irk.key, user_gapm_conf.irk, KEY_LEN*sizeof(uint8_t));
#endif

#if (USER_CFG_ADDRESS_MODE == APP_CFG_ADDR_STATIC)
        const uint8_t tmp_addr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        // Check whether the user-defined Random Static address is null.
        if (memcmp(user_gapm_conf.addr, &co_null_bdaddr, BD_ADDR_LEN) == 0)
        {
            CALLBACK_ARGS_1(user_app_callbacks.app_on_generate_static_random_addr, &app_random_addr)
            memcpy(cmd->addr.addr, app_random_addr.addr, BD_ADDR_LEN * sizeof(uint8_t));
						printf_string(UART1, "#########################################################################################");
					  printf_string(UART1, "\n\r");
					  printf_string(UART1, " This is your Unique Static Random Address For your Device embed in the DA14531 USB KIT : \n\r");
					  printf_string(UART1, "#########################################################################################");
					   
					  printf_string(UART1, "\n\r");
					  printf_string(UART1, "\n\r");
					          
						printf_byte(UART1, app_random_addr.addr[5]);
						printf_string(UART1, ":");
						printf_byte(UART1, app_random_addr.addr[4]);
						printf_string(UART1, ":");
						printf_byte(UART1, app_random_addr.addr[3]);
						printf_string(UART1, ":");
						printf_byte(UART1, app_random_addr.addr[2]);
						printf_string(UART1, ":");
						printf_byte(UART1, app_random_addr.addr[1]);
						printf_string(UART1, ":");
					  printf_byte(UART1, app_random_addr.addr[0]);

						printf_string(UART1, "\n\r");
						printf_string(UART1, "\n\r");
						printf_string(UART1, "\n\r");
					
        }
        else
        {
            // All the (BD_ADDR_LEN - 1) * sizeof(uint8_t) least significant
            // bits of the user-defined Random Static address shall NOT be equal to 1.
            ASSERT_ERROR(memcmp(user_gapm_conf.addr, tmp_addr, BD_ADDR_LEN - 1) != 0);
            // The two most significant bits of the user-defined Random Static
            // address shall be equal to 1.
            ASSERT_ERROR((user_gapm_conf.addr[BD_ADDR_LEN - 1] & GAP_STATIC_ADDR) == GAP_STATIC_ADDR)
            memcpy(cmd->addr.addr, user_gapm_conf.addr, BD_ADDR_LEN * sizeof(uint8_t));
        }
#endif
        cmd->att_cfg = user_gapm_conf.att_cfg;
        cmd->gap_start_hdl = user_gapm_conf.gap_start_hdl;
        cmd->gatt_start_hdl = user_gapm_conf.gatt_start_hdl;
        cmd->max_mps = user_gapm_conf.max_mps;
        cmd->max_txoctets = co_min(user_gapm_conf.max_txoctets, llm_le_env.supportedMaxTxOctets);
        cmd->max_txtime = co_min(user_gapm_conf.max_txtime, llm_le_env.supportedMaxTxTime);
    }
    return set_dev_config_cmd;
}

/**
 ****************************************************************************************
 * @brief Advertising stop handler.
 ****************************************************************************************
 */
static void app_easy_gap_advertise_stop_handler(void)
{
    void (*timeout_callback)(void);
    app_easy_gap_advertise_stop();
    adv_timer_id=EASY_TIMER_INVALID_TIMER;
    if(adv_timeout_callback!=NULL)
    {
        timeout_callback = adv_timeout_callback;
        adv_timeout_callback = NULL;
        timeout_callback();
    }
}

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)
int16_t active_conidx_to_conhdl(uint8_t conidx)
{
    ASSERT_WARNING(conidx < APP_EASY_MAX_ACTIVE_CONNECTION);
    if(app_env[conidx].connection_active == true)
        return(app_env[conidx].conhdl);
    else
        return(-1);
}

int8_t active_conhdl_to_conidx(uint16_t conhdl)
{
    uint8_t i;
        for(i=0; i < APP_EASY_MAX_ACTIVE_CONNECTION; i++)
            if (app_env[i].conhdl == conhdl)
            {
                if (app_env[i].connection_active == true)
                    return(app_env[i].conidx);
                else
                    return(GAP_INVALID_CONIDX);
            }
         return (GAP_INVALID_CONIDX);      //returns -1 if the conidx is not found
}
#endif

void app_init(void)
{
    // Reset the environment
    for (int i = 0; i < APP_EASY_MAX_ACTIVE_CONNECTION; i++) { 
        app_env[i] = (struct app_env_tag) {0};
    }
    
    // Initialize device info
    device_info.dev_name.length = (USER_DEVICE_NAME_LEN <= GAP_MAX_NAME_SIZE) ? USER_DEVICE_NAME_LEN : GAP_MAX_NAME_SIZE;
    memcpy(device_info.dev_name.name, USER_DEVICE_NAME, device_info.dev_name.length);
    device_info.appearance = 0x0000;

    // Create APP task
    ke_task_create(TASK_APP, &TASK_DESC_APP);

    // Initialize Task state
    ke_state_set(TASK_APP, APP_DISABLED);
}

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)
void app_easy_gap_disconnect(uint8_t conidx)
{
    struct gapc_disconnect_cmd *cmd = app_disconnect_msg_create(conidx);

    cmd->reason = CO_ERROR_REMOTE_USER_TERM_CON;
    app_env[conidx].connection_active=false;

    // Send the message
    app_disconnect_msg_send(cmd);
}

void app_easy_gap_confirm(uint8_t conidx, enum gap_auth auth, uint8_t svc_changed_ind_enable)
{
    // confirm connection
    struct gapc_connection_cfm *cfm = app_connect_cfm_msg_create(conidx);

    cfm->auth = auth;
    cfm->svc_changed_ind_enable = svc_changed_ind_enable;

    // Send the message
    ke_msg_send(cfm);
}
#endif

struct gapm_start_advertise_cmd* app_easy_gap_undirected_advertise_get_active(void)
{
    return(app_easy_gap_undirected_advertise_start_create_msg());
}

void app_easy_gap_undirected_advertise_start(void)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_start_create_msg();

    // Send the message
    app_advertise_start_msg_send(cmd);
    adv_cmd = NULL ;

    // We are now connectable
    ke_state_set(TASK_APP, APP_CONNECTABLE);
}

struct gapm_start_advertise_cmd* app_easy_gap_directed_advertise_get_active(uint8_t ldc_enable)
{
    return app_easy_gap_directed_advertise_start_create_msg(ldc_enable);
}

void app_easy_gap_directed_advertise_start(uint8_t ldc_enable)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_directed_advertise_start_create_msg(ldc_enable);

    // Send the message
    app_advertise_start_msg_send(cmd);
    adv_cmd = NULL ;

    // We are now connectable
    ke_state_set(TASK_APP, APP_CONNECTABLE);
}

struct gapm_start_advertise_cmd* app_easy_gap_non_connectable_advertise_get_active(void)
{
    return(app_easy_gap_non_connectable_advertise_start_create_msg());
}

void app_easy_gap_non_connectable_advertise_start(void)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_non_connectable_advertise_start_create_msg();

    // Send the message
    app_advertise_start_msg_send(cmd);
    adv_cmd = NULL ;

    uint8_t state = ke_state_get(TASK_APP);

    // Check if we are not already in a connected state
    if (!(state == APP_CONNECTED))
    {
        // We are now connectable
        ke_state_set(TASK_APP, APP_CONNECTABLE);
    }
}

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)
void app_easy_gap_advertise_stop(void)
{
    // Disable Advertising
    struct gapm_cancel_cmd *cmd = app_gapm_cancel_msg_create();
    // Send the message
    app_gapm_cancel_msg_send(cmd);
}
#endif

#if (!EXCLUDE_DLG_TIMER)
void app_easy_gap_undirected_advertise_with_timeout_start(uint32_t delay, void (*timeout_callback)(void))
{
    //stop the current running timer
    if(adv_timer_id != EASY_TIMER_INVALID_TIMER)
        app_easy_timer_cancel(adv_timer_id);
    if(timeout_callback != NULL)
        adv_timeout_callback = timeout_callback;
    adv_timer_id = app_easy_timer(delay, app_easy_gap_advertise_stop_handler);
    app_easy_gap_undirected_advertise_start();
}

void app_easy_gap_advertise_with_timeout_stop(void)
{
    //stop the current running timer
    if (adv_timer_id != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(adv_timer_id);
    }
    adv_timer_id = EASY_TIMER_INVALID_TIMER;
    adv_timeout_callback = NULL;
}
#endif

struct gapc_param_update_cmd* app_easy_gap_param_update_get_active(uint8_t conidx)
{
    return app_easy_gap_param_update_msg_create(conidx);
}

void app_easy_gap_param_update_start(uint8_t conidx)
{
    struct gapc_param_update_cmd* cmd;
    cmd = app_easy_gap_param_update_msg_create(conidx);

    // Send the message
    app_param_update_msg_send(cmd);
    param_update_cmd[conidx] = NULL;
}

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)
void app_timer_set(ke_msg_id_t const timer_id, ke_task_id_t const task_id, uint32_t delay)
{
    // Delay shall not be more than maximum allowed
    if(delay > KE_TIMER_DELAY_MAX)
    {
        delay = KE_TIMER_DELAY_MAX;
    }
    // Delay should not be zero
    else if(delay == 0)
    {
        delay = 1;
    }

    ke_timer_set(timer_id, task_id, delay);
}
#endif

struct gapm_start_connection_cmd* app_easy_gap_start_connection_to_get_active(void)
{
    return(app_easy_gap_start_connection_to_msg_create());
}

void app_easy_gap_start_connection_to_set(uint8_t peer_addr_type,
                                          const uint8_t *peer_addr, uint16_t intv)
{
    struct gapm_start_connection_cmd* msg;
    msg = app_easy_gap_start_connection_to_msg_create();
    msg->nb_peers = 1;
    memcpy(&msg->peers[0].addr, peer_addr, BD_ADDR_LEN);
    msg->peers[0].addr_type = peer_addr_type;
    msg->con_intv_max = intv;
    msg->con_intv_min = intv;
    return;
}

void app_easy_gap_start_connection_to(void)
{
    struct gapm_start_connection_cmd* msg;
    msg = app_easy_gap_start_connection_to_msg_create();
    app_connect_start_msg_send((void *) msg);
    start_connection_cmd = NULL;
}

struct gapm_set_dev_config_cmd* app_easy_gap_dev_config_get_active(void)
{
     return app_easy_gap_dev_config_create_msg();
}


/**
 ****************************************************************************************
 * @brief Timer0 general user callback function
 *
 ****************************************************************************************
 */
static void timer0_general_user_callback_function(void)
{
	    static uint8_t n = 0;

    // when pass  10 * 100ms
    if ( 10 == n )
    {
        n = 0;
//        timeout_expiration--;

					printf_string(UART1, "#########################################################################################");
					printf_string(UART1, "\n\r");
					printf_string(UART1, " This is your Unique Static Random Address For your Device embed in the DA14531 USB KIT : \n\r");
					printf_string(UART1, "#########################################################################################");
					   
					printf_string(UART1, "\n\r");
					printf_string(UART1, "\n\r");
					          
				  printf_byte(UART1, app_random_addr.addr[5]);
					printf_string(UART1, ":");
					printf_byte(UART1, app_random_addr.addr[4]);
					printf_string(UART1, ":");
					printf_byte(UART1, app_random_addr.addr[3]);
					printf_string(UART1, ":");
					printf_byte(UART1, app_random_addr.addr[2]);
					printf_string(UART1, ":");
					printf_byte(UART1, app_random_addr.addr[1]);
					printf_string(UART1, ":");
					printf_byte(UART1, app_random_addr.addr[0]);

					printf_string(UART1, "\n\r");
					printf_string(UART1, "\n\r");
					printf_string(UART1, "\n\r");
     }
		
     n++;
}


void timer0_general_test(uint32_t times_seconds)
{

    // Stop timer for enter settings
    timer0_stop();

    // register callback function for SWTIM_IRQn irq
    timer0_register_callback(timer0_general_user_callback_function);

    // Enable the Timer0/Timer2 input clock
    timer0_2_clk_enable();

    // Set the Timer0/Timer2 input clock division factor to 8, so 16 MHz / 8 = 2 MHz input clock
    timer0_2_clk_div_set(&clk_div_config);

    // clear PWM settings register to not generate PWM
    timer0_set_pwm_high_counter(NO_PWM);
    timer0_set_pwm_low_counter(NO_PWM);

    // Set timer with 2MHz source clock divided by 10 so Fclk = 2MHz/10 = 200kHz
    timer0_init(TIM0_CLK_FAST, PWM_MODE_ONE, TIM0_CLK_DIV_BY_10);

    // reload value for 100ms (T = 1/200kHz * RELOAD_100MS = 0,000005 * 20000 = 100ms)
    timer0_set_pwm_on_counter(RELOAD_100MS);

    // Enable SWTIM_IRQn irq
    timer0_enable_irq();

    // Start Timer0
    timer0_start();

    // Disable the Timer0/Timer2 input clock
    timer0_2_clk_disable();
}


void app_easy_gap_dev_configure(void)
{
    struct gapm_set_dev_config_cmd* cmd = app_easy_gap_dev_config_create_msg();
    app_gapm_configure_msg_send(cmd);
    set_dev_config_cmd = NULL;
}

void app_easy_gap_get_peer_features(uint8_t conidx)
{
    struct gapc_get_info_cmd *pkt = KE_MSG_ALLOC(GAPC_GET_INFO_CMD,
                                                 KE_BUILD_ID(TASK_GAPC, conidx),
                                                 TASK_APP, gapc_get_info_cmd);

    pkt->operation = GAPC_GET_PEER_FEATURES;
    ke_msg_send(pkt);
}

#if !defined (__DA14531__) || defined (__EXCLUDE_ROM_APP_TASK__)
void app_easy_gap_set_data_packet_length(uint8_t conidx, uint16_t tx_octets, uint16_t tx_time)
{
    struct gapc_set_le_pkt_size_cmd *cmd = KE_MSG_ALLOC(GAPC_SET_LE_PKT_SIZE_CMD,
                                                        KE_BUILD_ID(TASK_GAPC, conidx),
                                                        TASK_APP,
                                                        gapc_set_le_pkt_size_cmd);

    cmd->operation = GAPC_SET_LE_PKT_SIZE;
    cmd->tx_octets = co_min(tx_octets, llm_le_env.supportedMaxTxOctets);
    cmd->tx_time = co_min(tx_time, llm_le_env.supportedMaxTxTime);

    ke_msg_send(cmd);
}

app_prf_srv_perm_t get_user_prf_srv_perm(enum KE_API_ID task_id)
{
    uint8_t i;
    for(i = 0; i < PRFS_TASK_ID_MAX; i++)
    {
        if(app_prf_srv_perm[i].task_id == task_id)
            return app_prf_srv_perm[i].perm;
    }
    return SRV_PERM_ENABLE;
}

void app_set_prf_srv_perm(enum KE_API_ID task_id, app_prf_srv_perm_t srv_perm)
{
    uint8_t i;
    for(i = 0; i < PRFS_TASK_ID_MAX; i++)
    {
        if((app_prf_srv_perm[i].task_id == task_id) || (app_prf_srv_perm[i].task_id == TASK_ID_INVALID))
        {
            app_prf_srv_perm[i].task_id = task_id;
            app_prf_srv_perm[i].perm = srv_perm;

            break;
        }
    }
}

void prf_init_srv_perm(void)
{
    uint8_t i;
    for(i = 0; i < PRFS_TASK_ID_MAX; i++)
    {
        app_prf_srv_perm[i].task_id = TASK_ID_INVALID;
        app_prf_srv_perm[i].perm = SRV_PERM_ENABLE;
    }
}

void app_gattc_svc_changed_cmd_send(uint8_t conidx, uint16_t seq_num, uint16_t svc_shdl, uint16_t svc_ehdl)
{
    struct gattc_send_svc_changed_cmd *msg = KE_MSG_ALLOC(GATTC_SEND_SVC_CHANGED_CMD,
                                                          KE_BUILD_ID(TASK_GATTC, conidx),
                                                          TASK_APP,
                                                          gattc_send_svc_changed_cmd);

    msg->operation = GATTC_SVC_CHANGED;
    msg->seq_num   = seq_num;
    msg->svc_shdl  = svc_shdl;
    msg->svc_ehdl  = svc_ehdl;

    ke_msg_send(msg);
}
#endif

#endif //(BLE_APP_PRESENT)

/// @} APP
