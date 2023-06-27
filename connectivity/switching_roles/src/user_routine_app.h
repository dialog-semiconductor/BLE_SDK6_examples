/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.h
 *
 * @brief Empty peripheral template project header file.
 *
 * Copyright (C) 2012-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _USER_ROUTINE_APP_H_
#define _USER_ROUTINE_APP_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief 
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"
#include "app_task.h"                  // application task
#include "gapc_task.h"                 // gap functions and messages
#include "gapm_task.h"                 // gap functions and messages
#include "app.h"                       // application definitions
#include "co_error.h"                  // error code definitions
 

/****************************************************************************
Add here supported profiles' application header files.
i.e.
#if (BLE_DIS_SERVER)
#include "app_dis.h"
#include "app_dis_task.h"
#endif
*****************************************************************************/

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
 
typedef struct{
	bool 		isNewCbRoutine;
	uint32_t 	pulseTime;
	uint8_t		amountOfPulses;
}led_cb_params_t;

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param);

void user_on_disconnect( struct gapc_disconnect_ind const *param );

void user_advertise_operation(void);

void user_on_adv_nonconn_complete(const uint8_t);

void user_on_adv_undirect_complete(const uint8_t);

void user_on_scanning_completed(const uint8_t);

void user_on_adv_report_ind(struct gapm_adv_report_ind const *);

/// @} APP

#endif // _USER_ROUTINE_APP_H_
