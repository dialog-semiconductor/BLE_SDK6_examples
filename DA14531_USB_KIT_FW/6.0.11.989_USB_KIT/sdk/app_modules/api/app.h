/**
 ****************************************************************************************
 *
 * @file app.h
 *
 * @brief Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2013
 * Copyright (C) 2017-2019 Modified by Dialog Semiconductor
 *
 ****************************************************************************************
 */

#ifndef _APP_H_
#define _APP_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_APP_PRESENT)

#include <stdint.h>
#include "co_bt.h"
#include "ke_msg.h"
#include "app_easy_gap.h"
#include "app_msg_utils.h"
#include "app_easy_msg_utils.h"
#include "app_easy_timer.h"

#if (BLE_PROX_REPORTER)
#include "app_proxr.h"
#include "app_proxr_task.h"
#endif

#if (BLE_DIS_SERVER)
#include "app_diss.h"
#include "app_diss_task.h"
#endif

#if (BLE_BATT_SERVER)
#include "app_bass.h"
#include "app_bass_task.h"
#endif

#if (BLE_BMS_SERVER)
#include "app_bmss.h"
#include "app_bmss_task.h"
#endif

#if (BLE_ANC_CLIENT)
#include "app_ancc.h"
#include "app_ancc_task.h"
#endif

#if (BLE_BCS_SERVER)
#include "app_bcss.h"
#include "app_bcss_task.h"
#endif

#if (BLE_UDS_SERVER)
#include "app_udss.h"
#include "app_udss_task.h"
#endif

#if (BLE_CTS_SERVER)
#include "app_ctss.h"
#include "app_ctss_task.h"
#endif

#if (BLE_CTS_CLIENT)
#include "app_ctsc.h"
#include "app_ctsc_task.h"
#endif

#if (BLE_GATT_CLIENT)
#include "app_gattc.h"
#include "app_gattc_task.h"
#endif

#if (BLE_SUOTA_RECEIVER)
#include "app_suotar.h"
#include "app_suotar_task.h"
#endif

#if (BLE_FINDME_TARGET) || (BLE_FINDME_LOCATOR)
#include "app_findme.h"
#include "app_findme_task.h"
#endif

#if (BLE_WSS_SERVER)
#include "app_wsss.h"
#include "app_wsss_task.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

/// Advertising data maximal length
#define APP_ADV_DATA_MAX_SIZE               (ADV_DATA_LEN - 3) // (CSSv6)

/// Scan Response data maximal length
#define APP_SCAN_RESP_DATA_MAX_SIZE         (SCAN_RSP_DATA_LEN)

/// Max connections supported by application task
#define APP_EASY_MAX_ACTIVE_CONNECTION      (1)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// APP Task messages
enum APP_MSG
{
    APP_MODULE_INIT_CMP_EVT = KE_FIRST_MSG(TASK_APP),

    APP_CREATE_TIMER,
    APP_CANCEL_TIMER,
    APP_MODIFY_TIMER,

    //Do not alter the order of the next messages
    //they are considered a range
    APP_TIMER_API_MES0,
    APP_TIMER_API_MES1=APP_TIMER_API_MES0+1,
    APP_TIMER_API_MES2=APP_TIMER_API_MES0+2,
    APP_TIMER_API_MES3=APP_TIMER_API_MES0+3,
    APP_TIMER_API_MES4=APP_TIMER_API_MES0+4,
    APP_TIMER_API_MES5=APP_TIMER_API_MES0+5,
    APP_TIMER_API_MES6=APP_TIMER_API_MES0+6,
    APP_TIMER_API_MES7=APP_TIMER_API_MES0+7,
    APP_TIMER_API_MES8=APP_TIMER_API_MES0+8,
    APP_TIMER_API_MES9=APP_TIMER_API_MES0+9,
    APP_TIMER_API_LAST_MES=APP_TIMER_API_MES9,

    APP_MSG_UTIL_API_MES0,
    APP_MSG_UTIL_API_MES1=APP_MSG_UTIL_API_MES0+1,
    APP_MSG_UTIL_API_MES2=APP_MSG_UTIL_API_MES0+2,
    APP_MSG_UTIL_API_MES3=APP_MSG_UTIL_API_MES0+3,
    APP_MSG_UTIL_API_MES4=APP_MSG_UTIL_API_MES0+4,
    APP_MSG_UTIL_API_LAST_MES=APP_MSG_UTIL_API_MES4,

#if (BLE_PROX_REPORTER)
    APP_PROXR_TIMER,
#endif //BLE_PROX_REPORTER

#if (BLE_FINDME_TARGET)
    APP_FINDT_TIMER,
#endif //BLE_FINDME_TARGET

#if (BLE_BATT_SERVER)
    APP_BASS_TIMER,
    APP_BASS_ALERT_TIMER,
#endif //BLE_BATT_SERVER
};

/// Application environment structure
struct app_env_tag
{
    /// Connection handle
    uint16_t conhdl;

    /// Connection Id
    uint8_t conidx; // Should be used only with KE_BUILD_ID()

    /// Connection active flag
    bool connection_active;

    /// Last initialized profile
    //uint8_t next_prf_init;

    // Last paired peer address type
    uint8_t peer_addr_type;

    // Last paired peer address
    struct bd_addr peer_addr;

    // pairing in progress
    bool pairing_in_progress;
};

/// Device name structure
struct app_device_name
{
    ///Length for name
    uint8_t length;
    ///Array of bytes for name
    uint8_t name[GAP_MAX_NAME_SIZE];
};

/// Device info structure
struct app_device_info
{
    /// Device Name
    struct app_device_name dev_name;
    
    /// Device Appearance Icon
    uint16_t appearance;
};

/*
 * EXTERNAL VARIABLE DECLARATION
 ****************************************************************************************
 */

/// Application environment
extern struct app_env_tag app_env[APP_EASY_MAX_ACTIVE_CONNECTION];

/// Device info
extern struct app_device_info device_info;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_APP_TASK__)
bool app_db_init_next(void);
#endif

/**
 ****************************************************************************************
 * @brief Initialize the BLE application.
 * @return void
 ****************************************************************************************
 */
void app_init(void);

/**
 ****************************************************************************************
 * @brief Start placing services in the database.
 * @return true if succeeded, else false
 ****************************************************************************************
 */
bool app_db_init_start(void);

/**
 ****************************************************************************************
 * @brief Database initialization. Add a required service in the database.
 * @return true if succeeded, else false
 ****************************************************************************************
 */
bool app_db_init(void);

/**
 ****************************************************************************************
 * @brief Start a kernel timer.
 * @param[in] timer_id Timer Id
 * @param[in] task_id Task Id
 * @param[in] delay Timer delay
 * @return void
 ****************************************************************************************
 */
void app_timer_set(ke_msg_id_t const timer_id, ke_task_id_t const task_id, uint32_t delay);

/**
 ****************************************************************************************
 * @brief Calls all the enable function of the profile registered in prf_func,
 *        custs_prf_func and user_prf_func.
 * @param[in] conidx The connection index
 * @return void
 ****************************************************************************************
 */
void app_prf_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Sends the GATTC_SEND_SVC_CHANGED_CMD message to the stack.
 * @param[in] conidx    Connection index
 * @param[in] seq_num   Operation sequence number
 * @param[in] svc_shdl  Start of Affected Attribute Handle Range
 * @param[in] svc_ehdl  End of Affected Attribute Handle Range
 * @return void
 ****************************************************************************************
*/
void app_gattc_svc_changed_cmd_send(uint8_t conidx, uint16_t seq_num, uint16_t svc_shdl, uint16_t svc_ehdl);

#if defined (__DA14531__) && !defined (__EXCLUDE_ROM_APP_TASK__)
/**
 ****************************************************************************************
 * @brief Append the device name in the advertising data or scan response data.
 * @param[in|out] len        The advertising or scan response data length.
 * @param[in] name_length    The device name length.
 * @param[in|out] data       Pointer to the buffer which will carry the device name.
 * @param[in] name_data      The device name.
 * @return void
 ****************************************************************************************
 */
void append_device_name(uint8_t *len,
                        const uint8_t name_length,
                        uint8_t *data,
                        const void *name_data);
#endif

#endif //(BLE_APP_PRESENT)

/// @} APP

#endif // _APP_H_
