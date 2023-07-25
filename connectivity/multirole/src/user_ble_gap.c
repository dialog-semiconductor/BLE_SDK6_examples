/**
 ****************************************************************************************
 *
 * @file user_ble_gap.c
 *
 * @brief User BLE GAP source code.
 *
 * Copyright (c) 2012-2021 Dialog Semiconductor
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.E.
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

#include "user_ble_gap.h"
#include "gapm_task.h"
#include "user_ble_common.h"
#include "user_multirole.h"

/*Min and Max are not limitations of the stack but these windows provide better functionality*/
#define BLE_MIN_SCAN_INTVL_MS		(10)
#if defined (__DA14531__)
//531 has an LDO refresh period where we need to give it time to refresh
#define BLE_MAX_SCAN_INTVL_MS		(50)
#else
#define BLE_MAX_SCAN_INTVL_MS		(250)
#endif



/**
 ****************************************************************************************
 * @brief Start a central scan
 * @param[in] scan_active - perform active or passive scan
 * @param[in] interval_ms - scan interval in milliseconds
 * @param[in] window_ms		- scan window in milliseconds
 * @param[in] filter_dupl - filter duplicate advertisements (buffer only size 10)
 * @return ble_gap_eror_t error code
 ****************************************************************************************
 */
ble_gap_error_t user_ble_gap_start_scan(bool scan_active, uint16_t interval_ms, uint16_t window_ms, bool filter_dupl)
{
	
	uint16_t interval, window;
	enum scan_dup_filter_policy filter_policy = filter_dupl ? SCAN_FILT_DUPLIC_EN:SCAN_FILT_DUPLIC_DIS; 
	uint8_t scan_code = scan_active ? GAPM_SCAN_ACTIVE: GAPM_SCAN_PASSIVE;
	
	

	//Window must be >= interval
	if(window_ms > interval_ms)
	{
		return BLE_GAP_ERROR_INVALID_PARAMS;
	}
	
	if(interval_ms < BLE_MIN_SCAN_INTVL_MS){
		return BLE_GAP_ERROR_INVALID_PARAMS;
	}
	
	if(interval_ms > BLE_MAX_SCAN_INTVL_MS)
	{
		return BLE_GAP_ERROR_INVALID_PARAMS;
	}
	
	interval	= MS_TO_BLESLOTS(interval_ms);
	window 		= MS_TO_BLESLOTS(window_ms);
	
	
	struct gapm_start_scan_cmd *cmd = KE_MSG_ALLOC(GAPM_START_SCAN_CMD,
																							 TASK_GAPM,
																							 TASK_APP,
																							 gapm_start_scan_cmd);

	//GAPM requested operation:
	cmd->op.code = scan_code;
	cmd->op.addr_src = user_central_conf.addr_src;
	cmd->op.state = 0;

	// Scan interval
	cmd->interval = interval;
	// Scan window size
	cmd->window = window;

	// Scanning mode :
	// - GAP_GEN_DISCOVERY: General discovery mode
	// - GAP_LIM_DISCOVERY: Limited discovery mode
	// - GAP_OBSERVER_MODE: Observer mode
	cmd->mode = GAP_OBSERVER_MODE;

	// Scan filter policy:
	// - SCAN_ALLOW_ADV_ALL: Allow advertising packets from anyone
	// - SCAN_ALLOW_ADV_WLST: Allow advertising packets from White List devices only
	cmd->filt_policy = SCAN_ALLOW_ADV_ALL;
	// Scan duplicate filtering policy:
	// - SCAN_FILT_DUPLIC_DIS: Disable filtering of duplicate packets
	// - SCAN_FILT_DUPLIC_EN: Enable filtering of duplicate packets
	cmd->filter_duplic = filter_policy;

	ke_msg_send(cmd);
	
	//Timer in 10s of ms
	//app_easy_timer(SCAN_TIME, scan_timeout_cb); 
	
	dbg_printf("%s \r\n", __func__);
			
	return BLE_GAP_ERROR_NO_ERROR;
	
}

/**
 ****************************************************************************************
 * @brief Get number of advertisement elements
 * @param[in] data - pointer to advertisement data
 * @param[in] len - data length
 * @return number of elements
 ****************************************************************************************
 */
uint8_t user_ble_gap_get_adv_num_elements(const uint8_t *data, uint8_t len)
{
	uint8_t i;
	//start with 1 to comply with BT SIG
	uint8_t num_elements = 1;
	i=data[0] + 1;
	while( i<len)
	{
		num_elements++;
		i += data[i] + 1;
		
	}
	
	return num_elements;
	
}

/**
 ****************************************************************************************
 * @brief parse advertisement data into ad_elements structure
 * @param[in] in_len - advertisement data length to parse
 * @param[in] in_data - data to parse
 * @param[out] ad_elements - parsed data
 * @return void
 ****************************************************************************************
 */
void user_ble_gap_parse_adv_data(uint8_t in_len, const uint8_t *in_data, ble_gap_adv_struct_t *ad_elements)
{

	uint8_t i = 0;
	uint8_t elem_count = 0;
	while( i<in_len)
	{
		ad_elements[elem_count].len = 	in_data[i] - 1;
		ad_elements[elem_count].type = 	in_data[i+1];
		ad_elements[elem_count].data = 	(uint8_t *)in_data + i + 2;
		i += in_data[i] + 1;
		elem_count++;
		
	}
	
}

//static char *format_bd_address123(struct gap_bdaddr *addr)
//{
//        static char buf[19];
//        int i;

//        for (i = 0; i < sizeof(addr->addr); i++) {
//                int idx;

//                // for printout, address should be reversed
//                idx = sizeof(addr->addr) - i - 1;
//                sprintf(&buf[i * 3], "%02x:", addr->addr.addr[idx]);
//        }

//        buf[sizeof(buf) - 2] = '\0';

//        return buf;
//}

/**
 ****************************************************************************************
 * @brief Start a connection
 * @param[in] bd_addr - bluetooth device address to connect to
 * @param[in] addr_type - PUBLIC/PRIVATE
 * @return void 
 ****************************************************************************************
 */
void user_ble_gap_connect(const uint8_t *bd_addr, uint8_t addr_type)
{
		struct gapm_start_connection_cmd *cmd = KE_MSG_ALLOC_DYN(GAPM_START_CONNECTION_CMD,
																										 TASK_GAPM,
																										 TASK_APP,
																										 gapm_start_connection_cmd,
																										 CFG_MAX_CONNECTIONS * sizeof(struct gap_bdaddr));
		
		struct gap_bdaddr *gaddr = ke_malloc(sizeof(struct gap_bdaddr), KE_MEM_NON_RETENTION);
		
		memcpy(&gaddr->addr, &bd_addr[0], BD_ADDR_LEN);
		gaddr->addr_type = addr_type;
	
		//cmd->op.code = GAPM_CONNECTION_AUTO;
		cmd->op.code = user_central_conf.code; //working
		
		cmd->op.addr_src = user_central_conf.addr_src;
		cmd->op.state = 0;
		cmd->scan_interval = user_central_conf.scan_interval;
		cmd->scan_window = user_central_conf.scan_window;
		cmd->con_intv_min = user_central_conf.con_intv_min;
		cmd->con_intv_max = user_central_conf.con_intv_max;
		cmd->con_latency = user_central_conf.con_latency;
		cmd->superv_to = user_central_conf.superv_to;
		cmd->ce_len_min = user_central_conf.ce_len_min;
		cmd->ce_len_max = user_central_conf.ce_len_max;
		
		//cmd->nb_peers = 1;
		cmd->nb_peers = 1;
		memcpy(&cmd->peers[0], gaddr, sizeof(struct gap_bdaddr));
		
		ke_msg_send(cmd);
		ke_free(gaddr);		
}

/**
 ****************************************************************************************
 * @brief This cancels advertisements or scans
 * @return void
 ****************************************************************************************
 */
static void ble_stop_air_operation(void)
{
	dbg_printf("%s \r\n", __func__);
	//allocate message space for command
	struct gapm_cancel_cmd *cmd = KE_MSG_ALLOC(GAPM_CANCEL_CMD,
																						TASK_GAPM,
																						TASK_APP, 
																						gapm_cancel_cmd);
	//cancel air operation
	cmd->operation = GAPM_CANCEL;

	ke_msg_send(cmd);
	
}
/**
 ****************************************************************************************
 * @brief Cancel scan operation
 * @return void
 ****************************************************************************************
 */
void user_ble_gap_stop_scan(void)
{
	ble_stop_air_operation();
}

void user_ble_gap_stop_adv(void)
{
	ble_stop_air_operation();
}
/**
 ****************************************************************************************
 * @brief Cancel connect operation
 * @return void
 ****************************************************************************************
 */
void user_ble_cancel_connect(void)
{
	dbg_printf("%s \r\n", __func__);
	ble_stop_air_operation();
}


/// @} APP


