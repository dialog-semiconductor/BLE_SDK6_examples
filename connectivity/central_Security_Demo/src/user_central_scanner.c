/**
 ****************************************************************************************
 *
 * @file user_central_scanner.c
 *
 * @brief Simple implementation of an interactive central scanner project source code.
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs
 * included in this package ("Software") include confidential, proprietary information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR ("DIALOG") AND MAY
 * ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND.  
 * THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS.  
 * RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT 
 * DIALOG'S PRIOR WRITTEN PERMISSION.
 *
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT
 * TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED "AS IS", WITHOUT 
 * WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, 
 * BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, 
 * MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM 
 * COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS 
 * AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY 
 * REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED
 * SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, 
 * AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY 
 * OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED 
 * BY THE FCC OR OTHER LIKE AGENCIES.
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
#include "arch_console.h"
#include "uart.h"
#include "user_callback_config.h"
#include "app_easy_security.h"
#include "user_security.h"
#include "user_periph_setup.h"
#include "gpio.h"

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "user_central_scanner.h"


struct adv_device scanned_devices[30] __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t device_count __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
timer_hnd scan_timer __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t uart_input __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t uart2_read_in_progress __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
bool is_scanning __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Clear scanned devices in the list.
 *
 ****************************************************************************************
 */
void user_clear_devices(void){
	struct adv_device empty = {0};
	for(int i = 0;i<10;i++){
		scanned_devices[i] = empty;
	}
	device_count = 0;
}


void user_print_addr(const struct bd_addr addr)
{
	uint8_t addr_bytes[6] = {0};
	memcpy(&addr_bytes,&addr,6);
	for(int i = 5; i>=0; i--){
		if(addr_bytes[i]<=0xf)
			arch_printf("0");
		arch_printf("%x ",addr_bytes[i]);
	}
	arch_printf("\n\r");
}

bool user_match_bd_addr(const struct bd_addr addr1, const struct bd_addr addr2)
{
	uint8_t addr1_bytes[6] = {0};
	uint8_t addr2_bytes[6] = {0};
	memcpy(&addr1_bytes,&addr1,6);
	memcpy(&addr2_bytes,&addr2,6);
	for(int i = 0;i<6;i++){
		if(addr1_bytes[i] != addr2_bytes[i])
			return false;
	}
	return true;
}

void user_print_device_name(struct adv_device device){
	if(device.device_name_len == 0)
		arch_printf("N/A");
	else{
		for(int i =0; i<device.device_name_len; i++){
			arch_printf("%c", device.device_name[i]);
		}
	}
}

/**
 ****************************************************************************************
 * @brief Print basic device info.
 *
 ****************************************************************************************
 */
void user_print_device(struct adv_device device){
	arch_printf("---%d--------------\r\n",device.index);
	user_print_device_name(device);
	arch_printf_flush();
	arch_printf("     ");
	switch(device.evt_type){
		case ADV_CONN_UNDIR:{
			arch_printf("Undirected Connectable");
		}break;
		case ADV_CONN_DIR:{
			arch_printf("Directed Connectable");
		}break;
		case ADV_DISC_UNDIR:{
			arch_printf("Undirected Discoverable");
		}break;
		case ADV_NONCONN_UNDIR:{
			arch_printf("Undirected Unconnectable");
		}break;
		case ADV_CONN_DIR_LDC:{
			arch_printf("Low Duty Cycle Connectable");
		}break;
		default:{
			arch_printf("Unknown Type");	
		}break;			
	}
	arch_printf("\n\rBD ADDR: ");
	user_print_addr(device.adv_addr);
	arch_printf_flush();
	arch_printf("------------------\r\n");
	arch_printf_flush();
}

/**
 ****************************************************************************************
 * @brief Start scanning with settings listed
 *
 ****************************************************************************************
 */
void scan_start(void){
	struct gapm_start_scan_cmd* cmd = KE_MSG_ALLOC(
	GAPM_START_SCAN_CMD,
	TASK_GAPM,
	TASK_APP,
	gapm_start_scan_cmd
	);

	cmd->op.code = GAPM_SCAN_ACTIVE;
	cmd->op.addr_src = GAPM_STATIC_ADDR;
	cmd->interval = 600;
	cmd->window = 400;
	cmd->mode = GAP_GEN_DISCOVERY;
	cmd->filt_policy = SCAN_ALLOW_ADV_ALL;
	cmd->filter_duplic = SCAN_FILT_DUPLIC_EN;
	ke_msg_send(cmd);
	is_scanning = true;
}

/**
 ****************************************************************************************
 * @brief Connect to selected device.
 *
 ****************************************************************************************
 */
void	user_connect_to_device(struct adv_device device){
	wdg_resume();	
	arch_printf("Connecting to ");
	user_print_device_name(device);
	arch_printf_flush();
	app_easy_gap_start_connection_to_set(device.addr_type, (uint8_t *)&(device.adv_addr), MS_TO_DOUBLESLOTS(30));
	app_easy_gap_start_connection_to();
}

/**
 ****************************************************************************************
 * @brief UART2 read for input.
 *
 ****************************************************************************************
 */
static void uart_read_cb(uint16_t status)
{
	//Relaunch scanning if input is letter R
	if(uart_input == 'r' || uart_input == 'R'){
		wdg_resume();	
		user_clear_devices();
		user_scan_start();
	}
	//Simple input sanitycheck
	else{
		if(uart_input < '0' || uart_input-'0' >= device_count){
			arch_printf("Input not valid.\r\n");
			user_choose_device();
		}
		else{
			//Connect to device according to the input number
			uart2_read_in_progress = 0;
			uart_input = uart_input - '0';
			user_connect_to_device(scanned_devices[uart_input]);
		}
	}
}

void user_choose_device(void)
{
		wdg_freeze();
    // Indicate that the asynchronous read operation is in progress.
    uart2_read_in_progress = 1;
		//uart2_read(&uart_input, 1, uart_read_cb);
		//uart_read_buffer(UART2, );
	  uart_register_rx_cb(UART2, uart_read_cb);
		uart_receive(UART2, &uart_input, 1, UART_OP_INTR);
}

/**
 ****************************************************************************************
 * @brief Stop scanning, enable device choices
 *
 ****************************************************************************************
 */
void user_scan_stop(void)
{
	app_easy_gap_advertise_stop(); //stop scanning
	is_scanning = false;
	for(int i = 0;i<device_count;i++){
		user_print_device(scanned_devices[i]);	
	}
	arch_printf("Scan Completed!\r\n");
	arch_printf("Choose a device by input the number,\r\n");
	arch_printf("or press R to rescan.\r\n");
	arch_printf_flush();
	scan_timer = app_easy_timer(50, user_choose_device);
}


/**
 ****************************************************************************************
 * @brief Start scanning until 10 device found or timed out.
 *
 ****************************************************************************************
 */
void user_scan_start(void)
{
#ifdef CFG_PRINTF
	arch_printf("\r\nStart scanning...\n\r");
#endif
	app_easy_security_bdb_init();
	scan_start();
	scan_timer = app_easy_timer(1000, user_scan_stop);
}

void device_update(struct adv_report report, uint8_t idx){
	if(idx==device_count){
		scanned_devices[idx].index = idx;
		scanned_devices[idx].adv_addr = report.adv_addr;
		scanned_devices[idx].evt_type = report.evt_type;
		scanned_devices[idx].addr_type = report.adv_addr_type;
	}
	if(scanned_devices[idx].device_name_len == 0&&report.data_len!=0){
		for(int i = 1;i<report.data_len;i++){
			if(report.data[i] == 0x09 && report.data_len-i>=report.data[i-1]){
				scanned_devices[idx].device_name_len = report.data[i-1];
				memcpy(&scanned_devices[idx].device_name, &report.data[i+1], report.data[i-1]);
				
				break;
			}
		}
	}
	//scanned_devices[idx].rssi = ((47*report.rssi)/100)-112;
}


void device_check(struct adv_report report){
	bool device_exists = false;
	for(uint8_t i = 0;i<device_count;i++){
		if(user_match_bd_addr(report.adv_addr,scanned_devices[i].adv_addr)){
			device_exists = true;
			device_update(report, i);
			break;
		}
	}
	if(!device_exists){
		device_update(report, device_count);
		device_count++;
	}
	if(device_count==10){
		if(is_scanning){
			app_easy_timer_cancel(scan_timer);
			scan_timer = EASY_TIMER_INVALID_TIMER;
			user_scan_stop();
		}
	}
}

void user_scan_complete_cb(uint8_t status)
{
	if (status == GAP_ERR_TIMEOUT)
  {
		scan_start();
	}
}

/**
 ****************************************************************************************
 * @brief Callback when scanner gathered report for a scanned device.
 *
 ****************************************************************************************
 */
void user_scan_report_ind_cb(struct gapm_adv_report_ind const *param)
{
	device_check(param->report);
}


void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
		arch_printf("\r\nDevice connected.\r\n");
#ifdef CFG_SECURITY_ACTIVE
		user_security_send_encrypt_cmd(connection_idx);
#endif
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
		arch_printf("\r\nDevice disconnected.\r\n");
    default_app_on_disconnect(param);
		ke_state_set(TASK_APP, APP_CONNECTABLE);
		user_clear_devices();
		user_scan_start();
}

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {
        case GAPC_PARAM_UPDATED_IND:
        {
            // Cast the "param" pointer to the appropriate message structure
            struct gapc_param_updated_ind const *msg_param = (struct gapc_param_updated_ind const *)(param);

						
            // Check if updated Conn Params filled to preferred ones
            if ((msg_param->con_interval >= user_connection_param_conf.intv_min) &&
                (msg_param->con_interval <= user_connection_param_conf.intv_max) &&
                (msg_param->con_latency == user_connection_param_conf.latency) &&
                (msg_param->sup_to == user_connection_param_conf.time_out))
            {
            }
        } break;

        default:
            break;
    }
}

/// @} APP
