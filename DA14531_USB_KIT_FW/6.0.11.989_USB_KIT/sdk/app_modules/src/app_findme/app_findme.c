/**
 ****************************************************************************************
 *
 * @file app_findme.c
 *
 * @brief Findme locator and target application.
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
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

#include "rwip_config.h"

#if (BLE_APP_PRESENT)

#include "app_findme.h"
#include "app_prf_perm_types.h"

#if (BLE_FINDME_LOCATOR)
#include "findl_task.h"
#include "app.h"
#endif

#if (BLE_FINDME_TARGET)
#include "gpio.h"
#include "findt_task.h"
#include "app.h"
#include "user_periph_setup.h"
#endif

/*
 * LOCAL VARIABLES DEFINITION
 ****************************************************************************************
 */

#if (BLE_FINDME_TARGET)
app_alert_state alert_state __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
#endif

#if (BLE_FINDME_TARGET)

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_findt_init()
{
    alert_state.port = GPIO_LED_PORT;
    alert_state.pin = GPIO_LED_PIN;
}

void app_findt_create_db(void)
{
    struct findt_db_cfg* db_cfg;
    
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd, 
                                                             sizeof(struct findt_db_cfg));
    
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_FINDT);
    req->prf_task_id = TASK_ID_FINDT;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct findt_db_cfg* ) req->param;
    db_cfg->dummy = 0;

    // Send the message
    ke_msg_send(req);
}

void app_findt_alert_start(uint8_t lvl)
{
    alert_state.lvl = lvl;

    if (alert_state.lvl == FINDT_ALERT_MILD)
    {
        alert_state.blink_timeout = 150;
    }
    else
    {
        alert_state.blink_timeout = 50;
    }

    alert_state.blink_toggle = 1;
    GPIO_SetActive(alert_state.port, alert_state.pin);

    ke_timer_set(APP_FINDT_TIMER, TASK_APP, alert_state.blink_timeout);
}

void app_findt_alert_stop(void)
{
    alert_state.lvl = FINDT_ALERT_NONE; // Clear alert level;

    alert_state.blink_timeout = 0;
    alert_state.blink_toggle = 0;

    GPIO_SetInactive(alert_state.port, alert_state.pin);

    ke_timer_clear(APP_FINDT_TIMER, TASK_APP);
}

void default_findt_alert_ind_handler(uint8_t conidx, uint8_t lvl)
{
    if (lvl)
    {
        app_findt_alert_start(lvl);
    }
    else
    {
        app_findt_alert_stop();
    }
}
#endif // BLE_FINDME_TARGET

#if (BLE_FINDME_LOCATOR)
void app_findl_init(void)
{
}

void app_findl_create_task(void)
{
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC(GAPM_PROFILE_TASK_ADD_CMD,
                                                             TASK_GAPM,
                                                             TASK_APP,
                                                             gapm_profile_task_add_cmd);
    
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = get_user_prf_srv_perm(TASK_ID_FINDL);
    req->prf_task_id = TASK_ID_FINDL;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Send the message
    ke_msg_send(req);
}

void app_findl_enable(uint8_t conidx)
{
    // Allocate the message
    struct findl_enable_req *req = KE_MSG_ALLOC(FINDL_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_FINDL),
                                                TASK_APP,
                                                findl_enable_req);

    // Fill in the parameter structure
    req->con_type = PRF_CON_DISCOVERY;

    // Send the message
    ke_msg_send(req);
}

void app_findl_set_alert(uint8_t lvl)
{
    struct findl_set_alert_req *req = KE_MSG_ALLOC(FINDL_SET_ALERT_REQ,
                                                   prf_get_task_from_id(TASK_ID_FINDL),
                                                   TASK_APP,
                                                   findl_set_alert_req);

    req->alert_lvl = lvl & (FINDL_ALERT_NONE | FINDL_ALERT_MILD | FINDL_ALERT_HIGH);

    // Send the message
    ke_msg_send(req);
}
#endif // BLE_FINDME_LOCATOR

#endif // BLE_APP_PRESENT

/// @} APP
