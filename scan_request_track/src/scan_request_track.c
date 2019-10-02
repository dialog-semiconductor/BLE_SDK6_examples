/**
 ****************************************************************************************
 *
 * @file scan_request_track.c
 *
 * @brief Scan response Track project source code.
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
scn_response_callback *scn_rsp_cb               __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

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
