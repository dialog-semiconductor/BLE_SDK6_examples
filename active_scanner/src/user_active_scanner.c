/**
 ****************************************************************************************
 *
 * @file user_active_scanner.c
 *
 * @brief Active scanner project source code.
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


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "user_active_scanner.h"
#include "arch_console.h"


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

struct scan_configuration {
    /// Operation code.
    uint8_t     code;
    /// Own BD address source of the device
    uint8_t     addr_src;
    /// Scan interval
    uint16_t    interval;
    /// Scan window size
    uint16_t    window;
    /// Scanning mode
    uint8_t     mode;
    /// Scan filter policy
    uint8_t     filt_policy;
    /// Scan duplicate filtering policy
    uint8_t     filter_duplic;
};

static const struct scan_configuration user_scan_conf ={
    /// Operation code.
    .code = GAPM_SCAN_PASSIVE,
    /// Own BD address source of the device
    .addr_src = GAPM_CFG_ADDR_PUBLIC,
    /// Scan interval
    .interval = 10,
    /// Scan window size
    .window = 5,
    /// Scanning mode
    .mode = GAP_GEN_DISCOVERY,
    /// Scan filter policy
    .filt_policy = SCAN_ALLOW_ADV_ALL,
    /// Scan duplicate filtering policy
    .filter_duplic = SCAN_FILT_DUPLIC_DIS
};

void user_scan_start( void );



void user_app_on_init(void)
{

    // Set sleep mode
    arch_set_sleep_mode(app_default_sleep_mode);
	
//		user_scan_start();
  
}

/**
 ****************************************************************************************
 * @brief Called upon device cofiguration complete message
 * @return void
 ****************************************************************************************
 */
void user_on_set_dev_config_complete( void )
{
		arch_printf( "Dev config complete\r\n");
    // Add the first required service in the database
    //if (app_db_init_start())
    //{
        user_scan_start();
    //}
}



//not allowed on connection ?
static void user_scan_start(void)
{
    struct gapm_start_scan_cmd* cmd = KE_MSG_ALLOC(GAPM_START_SCAN_CMD,
                            TASK_GAPM, TASK_APP,
                            gapm_start_scan_cmd);

    cmd->op.code = user_scan_conf.code;
    cmd->op.addr_src = user_scan_conf.addr_src;
    cmd->interval = user_scan_conf.interval;
    cmd->window = user_scan_conf.window;
    cmd->mode = user_scan_conf.mode;
    cmd->filt_policy = user_scan_conf.filt_policy;
    cmd->filter_duplic = user_scan_conf.filter_duplic;

    // Send the message
    ke_msg_send(cmd);

    // We are now connectable
    ke_state_set(TASK_APP, APP_CONNECTABLE);
    
		arch_printf( "SCAN START\r\n");
    
}


void user_on_scan_complete(const uint8_t param){
    arch_printf( "SCAN COMPLETE %X \r\n", (int)param );
		user_scan_start();
}

void user_adv_report_ind (struct gapm_adv_report_ind const * param ) {
/*

	int i;
  	if( app_control.scan_report_index <MAX_ADV_REPORTS ) 
    {
        
        memcpy( (void *)&app_control.adv_bd_addr[app_control.scan_report_index], param->report.adv_addr.addr, 6 );
      
    } else 
    {
         //Stop scanning
         app_easy_gap_advertise_stop();
         return;
    }    
	*/
    arch_printf( "BD ADDR %02x %02x %02x",  (int)param->report.adv_addr.addr[5], (int)param->report.adv_addr.addr[4], (int)param->report.adv_addr.addr[3] );
    arch_printf( " %02x %02x %02x\r\n", (int)param->report.adv_addr.addr[2], (int)param->report.adv_addr.addr[1], (int)param->report.adv_addr.addr[0] );
    /*
		app_control.scan_report_index++;
    
    //ademo
		//uint8_t target_address[6] = { 0x44, 0x44, 0x44, 0xca, 0xea, 0x80 };
		for( i=0; i<MAX_PERI_DEV; i++ )
		{
			if(!memcmp(param->report.adv_addr.addr, target_address[i], 6 ))
		//  if(param->report.adv_addr.addr[5] == 0x80 && param->report.adv_addr.addr[4] == 0xea && param->report.adv_addr.addr[3] == 0xca && param->report.adv_addr.addr[2] == 0x44 && param->report.adv_addr.addr[1] == 0x44 && param->report.adv_addr.addr[0] == 0x44)
			{
					g_dev[i] = (int)app_control.scan_report_index - 1; 
			}
		}	
		*/
    
}

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{	
		arch_printf("connect");
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    arch_printf("disconnect\n");
    default_app_on_disconnect(param);
}
 

 

/// @} APP
