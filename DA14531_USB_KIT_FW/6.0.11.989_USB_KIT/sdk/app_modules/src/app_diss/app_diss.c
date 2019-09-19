/**
 ****************************************************************************************
 *
 * @file app_diss.c
 *
 * @brief Device Information Service Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2013
 * Copyright (C) 2017 Modified by Dialog Semiconductor
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

#if (BLE_DIS_SERVER)
#include "app_diss.h"                 // Device Information Service Application Definitions
#include "app.h"                     // Application Definitions
#include "app_task.h"                // Application Task Definitions
#include "diss_task.h"               // Health Thermometer Functions
#include "app_prf_perm_types.h"
#include "user_profiles_config.h"

/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
 
void app_dis_init(void)
{
    // Nothing to do
}

void app_diss_create_db(void)
{
    struct diss_db_cfg* db_cfg;
    
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM, 
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd, 
                                                             sizeof(struct diss_db_cfg));
    
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_DISS);
    req->prf_task_id = TASK_ID_DISS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct diss_db_cfg* ) req->param;
    db_cfg->features = APP_DIS_FEATURES;

    // Send the message
    ke_msg_send(req);
}

#endif // (BLE_DIS_SERVER)

#endif // BLE_APP_PRESENT

/// @} APP
