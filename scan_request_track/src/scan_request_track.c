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

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
scn_request_callback *scn_rsp_cb               __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/


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

void user_scan_request_cb(struct scan_req_data_msg const * param)
{
		arch_printf("Scan request from ");
		arch_printf("%02x:%02x:%02x:%02x:%02x:%02x \n\r", 
				param->scn_bd_address[5],
				param->scn_bd_address[4],
				param->scn_bd_address[3],
				param->scn_bd_address[2],
				param->scn_bd_address[1],
				param->scn_bd_address[0]);
    arch_set_pxact_gpio();
}

void scan_request_callback_register(scn_request_callback* cb)
{
    if(cb!=NULL)
        scn_rsp_cb = cb;
}

/**
 ****************************************************************************************
 * @brief Check if there is a scan response packet received during the last BLE adv event
 *        if there is send an indication to application
 * @param[in] pkts Packets received during the last BLE event.
 ****************************************************************************************
*/
static void send_scan_req_msg(uint8_t pkts)
{
    uint8_t rx_cnt = pkts;
    uint8_t rx_hdl = co_buf_rx_current_get();
    
    while(rx_cnt--)
    {
        struct co_buf_rx_desc *rxdesc = co_buf_rx_get(rx_hdl);
        uint8_t status = rxdesc->rxstatus & 0x7F;
      
        if(llc_util_rxllid_getf(rxdesc)==LL_SCAN_REQ)
        {
            if (status & (BLE_MIC_ERR_BIT | BLE_CRC_ERR_BIT | BLE_LEN_ERR_BIT | BLE_TYPE_ERR_BIT | BLE_SYNC_ERR_BIT))
                return;
            struct scan_req_data_msg *ind = KE_MSG_ALLOC(SCAN_REQ_DATA_MSG, TASK_APP, TASK_APP, scan_req_data_msg);
            memcpy(ind->scn_bd_address, (uint8_t*)(_ble_base + (uint8_t*)rxdesc->rxdataptr), 6);
            ke_msg_send(ind);
        }
        rx_hdl = co_buf_rx_next(rx_hdl);
    }
}

/**
 ****************************************************************************************
 * @brief Check if there are data packets received during the last advertising event
 ****************************************************************************************
*/
void user_end_event_isr_hook(void)
{
    struct ea_elt_tag *elt_s = (struct ea_elt_tag *)co_list_pick(&lld_evt_env.elt_prog);
    struct lld_evt_tag *evt_s = LLD_EVT_ENV_ADDR_GET(elt_s);
    uint8_t rx_cnt = ble_rxdesccnt_getf(evt_s->conhdl);
    if(evt_s->conhdl == CFG_CON)
            send_scan_req_msg(rx_cnt - evt_s->rx_cnt);
}
 

/// @} APP
