/**
 ****************************************************************************************
 *
 * @file scan_request_track.c
 *
 * @brief Scan response Track project source code.
 *
 * Copyright (c) 2012-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration
#include "co_buf.h"
#include "llc_util.h"

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "arch_console.h"
#include "scan_request_track.h"
#include "user_periph_setup.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
scn_response_callback *scn_rsp_cb               __attribute__((section(".bss."))); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
static void user_scan_request_cb(struct scan_req_data_msg const * param);

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}

void user_app_init(void)
{
    scan_request_callback_register(user_scan_request_cb);
    default_app_on_init();
}

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {	
        case SCAN_REQ_DATA_MSG:
        {
            if(scn_rsp_cb!=NULL)
                scn_rsp_cb(param);
        }break;

        default:
            break;
    }
}



static void user_scan_request_cb(struct scan_req_data_msg const * param)
{
    arch_printf("Scan request from ");
	arch_printf("%02x:%02x:%02x:%02x:%02x:%02x \n\r", 
                param->scn_bd_address[5],
                param->scn_bd_address[4],
                param->scn_bd_address[3],
                param->scn_bd_address[2],
                param->scn_bd_address[1],
                param->scn_bd_address[0]);
#if defined (__DA14531__)
    arch_set_pxact_gpio(GPIO_CURSOR_PORT, GPIO_CURSOR_PIN);
#else
    arch_set_pxact_gpio();
#endif
}

void scan_request_callback_register(scn_response_callback* cb)
{
    if(cb!=NULL)
        scn_rsp_cb = cb;
}
 

/// @} APP