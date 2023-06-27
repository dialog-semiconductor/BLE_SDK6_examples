/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "user_segger_rtt_periph.h"
#include "SEGGER_SWD_printf.h"
#include "app_easy_timer.h"

timer_hnd timer_test;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
static void test_timer_cb(void)
{
		SWD_printf("Test Timer CB\r\n\r\n", NULL);
		timer_test = app_easy_timer(100, test_timer_cb);
}
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
		SWD_printf("%s\r\n", __func__);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
		SWD_printf("%s\r\n", __func__);
}

void user_app_on_set_dev_config_complete(void)
{
		default_app_on_set_dev_config_complete();
		SWD_printf("%s\r\n\r\n", __func__);
	
		timer_test = app_easy_timer(100, test_timer_cb);
}
 

/// @} APP
