/**
 ****************************************************************************************
 *
 * @file user_ble_gap.c
 *
 * @brief ble tools for scanning and formatting ble data
 *
 * Copyright (c) 2022 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
 */
 
 #include "user_ble_gap.h"
 #include "user_routine_config.h"
 
const char *format_local_name(const uint8_t * data, const uint8_t data_len){
	uint8_t i;
	static char buffer[MAX_ADV_PAYLOAD_LEN];
	bool is_name_found;

	is_name_found = false;
	i = 0;

	while(i < data_len && !is_name_found){
			if(data[i + 1] == GAP_AD_TYPE_COMPLETE_NAME){
					is_name_found = true;
			}else{
					i += (data[i] + 1);
			}
	}
	
	if(!is_name_found){
			memcpy(buffer, NO_NAME_FOUND, NO_NAME_FOUND_LEN);
	}else{
			memcpy(buffer, &data[i + 2], data[i] - 1);
			buffer[data[i]] = '\0';
	}
	
	return buffer;
}
 

const char *format_bd_address(const struct bd_addr *addr){
	static char buf[19];
	int i;
			
	for (i = 0; i < sizeof(addr->addr); i++) {
			int idx;

			// for printout, address should be reversed
			idx = sizeof(addr->addr) - i - 1;
			sprintf(&buf[i * 3], "%02x:", addr->addr[idx]);
	}

	buf[sizeof(buf) - 2] = '\0';

	return buf;
}

timer_hnd set_routine_timer(void){
	//static uint8_t timerID = EASY_TIMER_INVALID_TIMER;

	//if (timerID != EASY_TIMER_INVALID_TIMER) {
	//	app_easy_timer_cancel(timerID);
	//}
	timer_hnd handle;
	
	handle = app_easy_timer(ROUTINE_CYCLE_TIME, app_easy_gap_advertise_stop);
	
	return handle;
}

ble_gap_error_t user_ble_gap_start_scan(bool scan_active, uint16_t interval_ms, uint16_t window_ms, bool filter_dupl){
	
	uint16_t interval, window;
	enum scan_dup_filter_policy filter_policy = filter_dupl ? SCAN_FILT_DUPLIC_EN:SCAN_FILT_DUPLIC_DIS; 
	uint8_t scan_code = scan_active ? GAPM_SCAN_ACTIVE: GAPM_SCAN_PASSIVE;
	
	

	//Window must be >= interval
	if(window_ms < interval_ms)
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
			
	return BLE_GAP_ERROR_NO_ERROR;
	
}