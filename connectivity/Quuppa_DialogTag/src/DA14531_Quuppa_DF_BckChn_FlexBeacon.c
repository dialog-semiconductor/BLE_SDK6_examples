/**
 ****************************************************************************************
 *
 * @file DA14531_Quuppa_DF_BckChn_FlexBeacon.c
 *
 * @brief DA14531_Quuppa_DF_BckChn_FlexBeacon project source code.
 *
 * Copyright (C) 2015-2021 Renesas Electronics Corporation and/or its affiliates
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "da1458x_config_basic.h"
#include "arch_console.h"
#include "rwip_config.h"             // SW configuration
#include "gap.h"
#include "app_easy_timer.h"
#include "DA14531_Quuppa_DF_BckChn_FlexBeacon.h"
//#include "user_custs1_impl.h"
//#include "user_custs1_def.h"
#include "co_bt.h"
#include "rf_531.h"

#include "port_bmi270.h"

#include "adc.h"
#include "battery.h"

#ifdef HAS_WKUP_KEYS
#include "wkup_keys.h"
#endif // HAS_WKUP_KEYS

#ifdef CFG_PRINTF // If UART debug is enabled SysTick Timer is used to get Timestamps
#include "uart_utils.h"

#include "systick.h"

#define SYSTICK_PERIOD_US   15000000   // Systick timer is 24bit(max value 16.777.215), period for systick timer in us, so 15.000.000 ticks = 15 seconds
#define SYSTICK_EXCEPTION   0          // do not generate exceptions

/**
 ****************************************************************************************
 * @brief SysTick ISR routine
 ****************************************************************************************
 */
static void systick_isr(void)
{
}

void systick_init(void)
{
    systick_register_callback(systick_isr);
    // Systick will be initialized to use a reference clock frequency of 1 MHz
    systick_start(SYSTICK_PERIOD_US, SYSTICK_EXCEPTION);
}
#endif


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

// Manufacturer Specific Data ADV structure type
struct mnf_specific_data_ad_structure
{
    uint8_t ad_structure_size;
    uint8_t ad_structure_type;
    uint8_t company_id[APP_AD_MSD_COMPANY_ID_LEN];
    uint8_t proprietary_data[APP_AD_MSD_DATA_LEN];
};

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
    .addr_src = GAPM_STATIC_ADDR,
    /// Scan interval
    .interval = SCAN_INTERVAL_MS,                     
    /// Scan window size
    .window = SCAN_WINDOW_MS,                         
    /// Scanning mode
    .mode = GAP_OBSERVER_MODE,
    /// Scan filter policy
    .filt_policy = SCAN_ALLOW_ADV_ALL,
    /// Scan duplicate filtering policy
    .filter_duplic = SCAN_FILT_DUPLIC_DIS
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint8_t app_connection_idx                                __SECTION_ZERO("retention_mem_area0");

timer_hnd user_periodic_rx_on_timer                       __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_rx_on_cancel_timer                         __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_rsp_cancel_timer                           __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_scan_cancel_timer                          __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_enable_extended_sleep_timer                __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd user_dev_info_cancel_timer                      __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

timer_hnd app_param_update_request_timer_used             __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd app_push_button_timer                           __SECTION_ZERO("retention_mem_area0");

advertising_state adv_state                               __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
advertising_state adv_state_before_sleep                  __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t user_periodic_timer_init_once                     __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t cancel_motion_adv                                 __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
static advertising_state adv_state_before                 __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

extern last_ble_evt arch_rwble_last_event;
uint16_t ui16temperature_k;
uint16_t ui16battery_CR2032_mv;
uint8_t ui8status;

// Retained variables
struct mnf_specific_data_ad_structure mnf_data            __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t own_bd_addr[6]                                    __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t report_data[ADV_DATA_LEN + 1]                     __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
bool req_found                                            __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
// Index of manufacturer data in advertising data or scan response data (when MSB is 1)
uint8_t mnf_data_index                                    __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_adv_data_len                               __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data_len                          __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_adv_data[ADV_DATA_LEN]                     __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t stored_scan_rsp_data[SCAN_RSP_DATA_LEN]           __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint8_t user_led_on_display                               __SECTION_ZERO("retention_mem_area0");
extern bool led_state;
uint32_t actual_size;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

#define TOPBIT   (((uint8_t)1) << 7)

/*********************************************************************
 * Function:    CalculateCRC()
 * Description: Calculate the CRC-8 value withouth a lookup table
 *              with Initial value 0x00, Polinomial 0x97
 * Returns:		  The CRC of the Data array
 *********************************************************************/
uint8_t CalculateCRC(uint8_t const Data[], uint16_t nrBytes)
{
    uint8_t	CRCValue = 0x00;
    int16_t i,j;

    for (i = 0; i < nrBytes; i++)
    {
        CRCValue = CRCValue ^ Data[i];			
			  for (j = 0; j < 8; j++) {
					if ((CRCValue & TOPBIT) !=0) {
            CRCValue = (uint8_t)((CRCValue << 1) ^ 0x97);
					}
					else {
            CRCValue = (CRCValue << 1);
					}
        }
    }
    return (CRCValue ^ 0x00);
}

static void get_device_info(void)
{

	int8_t i8temperature_c;
	
	// Measure Temperature using internal temp sensor
	adc_config_t temp_config = 
	{
			.input_mode = ADC_INPUT_MODE_SINGLE_ENDED,
			.input = ADC_INPUT_SE_TEMP_SENS,
	};
			
	adc_init(&temp_config);						
	i8temperature_c = adc_get_temp();						
	adc_disable();
	
	// Calculate Temperature in K according to Quuppa formula
	// (hex)(TempC + 273.15)*10; example (20.0C + 273.15)*10 = 2931.5 (hex)(2932)=0x0B74 
	ui16temperature_k = (uint16_t)((i8temperature_c + 273.15)*10);

	// Measure Battery voltage assuming it's a CR2032
	ui16battery_CR2032_mv = battery_get_voltage(BATT_CR2032);

	ui8status = 0;

	if(ui16battery_CR2032_mv < 2500)
	{
		ui8status = (ui8status & ~(1UL << 0)) | (1 << 0); // Battery voltage <2500 mV => Status bit 0 (Battery Alarm) = 1
	}

	if(!GPIO_GetPinStatus(PUSH_BUTTON_1_PORT, PUSH_BUTTON_1_PIN))  // Read the current status of Button (if pressed = Low)
	{
		ui8status = (ui8status & ~(1UL << 1)) | (1 << 1); // Button is pressed => Status bit 1 (Btn1) = 1
	}

	#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1) 
	 uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	 arch_printf("\n\r[%08d us] 5.Status: %d, Battery Voltage[mV]: %d, Temp[C]: %d, Temp[K]: 0x%04X, ", current_sys_tick, ui8status, ui16battery_CR2032_mv, i8temperature_c, ui16temperature_k);
	#endif

}



/**
 ****************************************************************************************
 * @brief Initialize Manufacturer Specific Data for Direction Finding Payload
 * @return void
 ****************************************************************************************
 */
static void df_adv_data_init()
{
  	mnf_data.ad_structure_size = sizeof(struct mnf_specific_data_ad_structure ) - sizeof(uint8_t); // minus the size of the ad_structure_size field
    mnf_data.ad_structure_type = GAP_AD_TYPE_MANU_SPECIFIC_DATA;
    mnf_data.company_id[0] = APP_AD_MSD_COMPANY_ID & 0xFF; // LSB (APP_AD_MSD_COMPANY_ID >> 8 )& 0xFF; // MSB
    mnf_data.company_id[1] = (APP_AD_MSD_COMPANY_ID >> 8 )& 0xFF; // MSB
	  mnf_data.proprietary_data[0] = 0x01; // Quuppa Packet ID: 0x01 = Direction Finding; 0xF0 = Data Packet; 0xF2 = RSP Packet; 0xF1 = REQ Packet from locator
    mnf_data.proprietary_data[1] = QUUPPA_DEVICE_TYPE; // Quuppa Device Type
    mnf_data.proprietary_data[2] = 0x68; // Quuppa Header 0x68 = [7] RX ON [0], [6] Bck Chn [1], [5-4] TagID=BDAdd [10],[3-2] 0dBm Tx power [10], [1-0] DF TX Rate(0.2Hz)[00]
    mnf_data.proprietary_data[3] = own_bd_addr[5]; // Quuppa Tag ID [MSB]	
    mnf_data.proprietary_data[4] = own_bd_addr[4]; // Quuppa Tag ID
    mnf_data.proprietary_data[5] = own_bd_addr[3]; // Quuppa Tag ID
    mnf_data.proprietary_data[6] = own_bd_addr[2]; // Quuppa Tag ID
    mnf_data.proprietary_data[7] = own_bd_addr[1]; // Quuppa Tag ID		
    mnf_data.proprietary_data[8] = own_bd_addr[0]; // Quuppa Tag ID [LSB]
    mnf_data.proprietary_data[9] = CalculateCRC(&mnf_data.proprietary_data[1], 8);
    mnf_data.proprietary_data[10] = 0x67; // DF Field 0
    mnf_data.proprietary_data[11] = 0xF7; // DF Field 1	
    mnf_data.proprietary_data[12] = 0xDB; // DF Field 2
    mnf_data.proprietary_data[13] = 0x34; // DF Field 3
    mnf_data.proprietary_data[14] = 0xC4; // DF Field 4
    mnf_data.proprietary_data[15] = 0x03; // DF Field 5		
    mnf_data.proprietary_data[16] = 0x8E; // DF Field 6
    mnf_data.proprietary_data[17] = 0x5C; // DF Field 7
    mnf_data.proprietary_data[18] = 0x0B; // DF Field 8
    mnf_data.proprietary_data[19] = 0xAA; // DF Field 9	
    mnf_data.proprietary_data[20] = 0x97; // DF Field 10
    mnf_data.proprietary_data[21] = 0x30; // DF Field 11
    mnf_data.proprietary_data[22] = 0x56; // DF Field 12
    mnf_data.proprietary_data[23] = 0xE6; // DF Field 13		
}

/**
 ****************************************************************************************
 * @brief Initialize Manufacturer Specific Data for Device Info Payload
 * @return void
 ****************************************************************************************
 */
static void dev_info_adv_data_init()
{
    get_device_info();

		mnf_data.proprietary_data[0] = 0xF0; // Quuppa Packet ID: 0xF0 = Data Packet;
		mnf_data.proprietary_data[1] = 0x00; // Header containing payload counter and Tag ID type (TBD)
		mnf_data.proprietary_data[2] = own_bd_addr[5]; // Quuppa Tag ID [MSB]	
		mnf_data.proprietary_data[3] = own_bd_addr[4]; // Quuppa Tag ID
		mnf_data.proprietary_data[4] = own_bd_addr[3]; // Quuppa Tag ID
		mnf_data.proprietary_data[5] = own_bd_addr[2]; // Quuppa Tag ID
		mnf_data.proprietary_data[6] = own_bd_addr[1]; // Quuppa Tag ID		
		mnf_data.proprietary_data[7] = own_bd_addr[0]; // Quuppa Tag ID [LSB]
		mnf_data.proprietary_data[8] = 0x01;  // Payload Type: 0x01 Device Info 
		mnf_data.proprietary_data[9] = 0x00;  // Version 
		mnf_data.proprietary_data[10] = 0x1B; // Supported features (1 = supported, Bit[7-0]=[RFU, RFU, RFU, Temp, Battery, Btn2=NA=0, Btn1, Battery Alarm])  
		mnf_data.proprietary_data[11] = ui8status; // Status	(Bit[7-0]=[RFU,..., Btn2=NA=0, Btn1(1=pressed), Battery Alarm(0=Battery OK, 1=Battery Low)]) 
		mnf_data.proprietary_data[12] = ui16battery_CR2032_mv & 0xFF; // Battery Voltage [LSB]
		mnf_data.proprietary_data[13] = (ui16battery_CR2032_mv >> 8 )& 0xFF; // Battery Voltage [MSB] => 0x0BB8 = 3.00V (1mV per bit, 0x0000 if not supported and 0 in supp feat bit)
		mnf_data.proprietary_data[14] = ui16temperature_k & 0xFF; // Temperature [LSB]     => Formula (hex)(C + 273.15)*10 => (20.0C + 273.15)*10 = 2931.5 (hex)(2932) = 0x0B74  
		mnf_data.proprietary_data[15] = (ui16temperature_k >> 8 )& 0xFF; // Temperature [MSB]     => 0x0B74 = 20.0C (0.1Kelvin per bit, 0x0000 if not supported and 0 in supp feat bit)		
		mnf_data.proprietary_data[16] = DEVELOPER_ID & 0xFF; // Developer ID [LSB]
		mnf_data.proprietary_data[17] = (DEVELOPER_ID >> 8 )& 0xFF; // Developer ID [MSB]
		mnf_data.proprietary_data[18] = 0x00; // FW Version [LSB]
		mnf_data.proprietary_data[19] = 0x01; // FW Version [MSB]	  => FW verion 1.0
		mnf_data.proprietary_data[20] = 0x00; // HW Version [LSB]
		mnf_data.proprietary_data[21] = 0x01; // HW Version [MSB]	  => HW verion 1.0
		mnf_data.proprietary_data[22] = 0x00; // RFU
		mnf_data.proprietary_data[23] = 0x00; // RFU

}

/**
 ****************************************************************************************
 * @brief Add an AD structure in the Advertising or Scan Response Data of the
 *        GAPM_START_ADVERTISE_CMD parameter struct.
 * @param[in] cmd               GAPM_START_ADVERTISE_CMD parameter struct
 * @param[in] ad_struct_data    AD structure buffer
 * @param[in] ad_struct_len     AD structure length
 * @param[in] adv_connectable   Connectable advertising event or not. It controls whether
 *                              the advertising data use the full 31 bytes length or only
 *                              28 bytes (Document CCSv6 - Part 1.3 Flags).
 ****************************************************************************************
 */
static void app_add_ad_struct(struct gapm_start_advertise_cmd *cmd, void *ad_struct_data, uint8_t ad_struct_len, uint8_t adv_connectable)
{
    uint8_t adv_data_max_size = (adv_connectable) ? (ADV_DATA_LEN - 3) : (ADV_DATA_LEN);
    
    if ((adv_data_max_size - cmd->info.host.adv_data_len) >= ad_struct_len)
    {
        // Append manufacturer data to advertising data
        memcpy(&cmd->info.host.adv_data[cmd->info.host.adv_data_len], ad_struct_data, ad_struct_len);

        // Update Advertising Data Length
        cmd->info.host.adv_data_len += ad_struct_len;
        
        // Store index of manufacturer data which are included in the advertising data
        mnf_data_index = cmd->info.host.adv_data_len - sizeof(struct mnf_specific_data_ad_structure);
    }
    else if ((SCAN_RSP_DATA_LEN - cmd->info.host.scan_rsp_data_len) >= ad_struct_len)
    {
        // Append manufacturer data to scan response data
        memcpy(&cmd->info.host.scan_rsp_data[cmd->info.host.scan_rsp_data_len], ad_struct_data, ad_struct_len);

        // Update Scan Response Data Length
        cmd->info.host.scan_rsp_data_len += ad_struct_len;
        
        // Store index of manufacturer data which are included in the scan response data
        mnf_data_index = cmd->info.host.scan_rsp_data_len - sizeof(struct mnf_specific_data_ad_structure);
        // Mark that manufacturer data is in scan response and not advertising data
        mnf_data_index |= 0x80;
    }
    else
    {
        // Manufacturer Specific Data do not fit in either Advertising Data or Scan Response Data
        ASSERT_WARNING(0);
    }
    // Store advertising data length
    stored_adv_data_len = cmd->info.host.adv_data_len;
    // Store advertising data
    memcpy(stored_adv_data, cmd->info.host.adv_data, stored_adv_data_len);
    // Store scan response data length
    stored_scan_rsp_data_len = cmd->info.host.scan_rsp_data_len;
    // Store scan_response data
    memcpy(stored_scan_rsp_data, cmd->info.host.scan_rsp_data, stored_scan_rsp_data_len);
}

void toggle_led(void){

	if (led_state){
		user_set_led_state(false);		
	}else{
		user_set_led_state(true);
	}

}

/**
 ****************************************************************************************
 * @brief This cancels advertisments or scans
 * @return void
 ****************************************************************************************
 */
static void user_ble_gap_stop_ongoing_operation(void)
{
	//allocate message space for command
	struct gapm_cancel_cmd *cmd = KE_MSG_ALLOC(GAPM_CANCEL_CMD,
																						TASK_GAPM,
																						TASK_APP, 
																						gapm_cancel_cmd);
	//cancel air operation
	cmd->operation = GAPM_CANCEL;

	ke_msg_send(cmd);
	
}


static void scan_cancell_timer_cb()
{	
	#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
     uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	   arch_printf("\n\r[%08d us] X.Scan_cancell_timer_cb...", current_sys_tick);
	#endif	

	user_scan_cancel_timer = EASY_TIMER_INVALID_TIMER;
	user_ble_gap_stop_ongoing_operation();
}

/**
 ****************************************************************************************
 * @brief Scan for devices
 * @return void
 ****************************************************************************************
 */
static void user_scan_start(void)
{
    struct gapm_start_scan_cmd* cmd = KE_MSG_ALLOC(GAPM_START_SCAN_CMD,
                                                   TASK_GAPM,
                                                   TASK_APP,
                                                   gapm_start_scan_cmd);

    cmd->op.code = user_scan_conf.code;
    cmd->op.addr_src = user_scan_conf.addr_src;
    cmd->interval = user_scan_conf.interval;
    cmd->window = user_scan_conf.window;
    cmd->mode = user_scan_conf.mode;
    cmd->filt_policy = user_scan_conf.filt_policy;
    cmd->filter_duplic =user_scan_conf.filter_duplic;
		
		#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
        uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	      arch_printf("\n\r[%08d us] 4.Start Scanning...", current_sys_tick);
		#endif
    // Schedule SCAN cancel timeout
		user_scan_cancel_timer = app_easy_timer(SCAN_CANCEL_TIMER, scan_cancell_timer_cb);
    // Send the message
    ke_msg_send(cmd);
}

/**
 ****************************************************************************************
* @brief RX_ON cancel timer: used to have only one RX_ON adv packet
 * @return void
 ****************************************************************************************
*/
static void rx_on_cancel_timer_cb()
{	
 		#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
        uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	      arch_printf("\n\r[%08d us] 3.RX_ON_timer_cancellation_cb...", current_sys_tick);
		#endif	
		user_rx_on_cancel_timer = EASY_TIMER_INVALID_TIMER;
	  adv_state = SCAN;
    app_easy_gap_advertise_stop();
}



/**
 ****************************************************************************************
 * @brief Periodic scan timer callback function.
 * @return void
 ****************************************************************************************
*/
static void periodic_rx_on_timer_cb()
{	
 		#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
        uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	      arch_printf("\n\r[%08d us] 1.Periodic_RX_ON_timer_cb...", current_sys_tick);
		#endif	
	
		user_periodic_rx_on_timer = EASY_TIMER_INVALID_TIMER;

	  // Save current adv state
  	adv_state_before = adv_state;
 
    // change state
  	adv_state = RX_ON_ADVERTISING; //user_change_adv_state(RX_ON_ADVERTISING);
	  app_easy_gap_advertise_stop();
	
}

static void user_enable_extended_sleep_timer_cb()
{	
	  user_enable_extended_sleep_timer = EASY_TIMER_INVALID_TIMER;
			#ifdef CFG_PRINTF
			 arch_printf("\n\n\r SLEEP timer elapsed...");
			#endif
//			  user_change_adv_state(SLEEP);
//			  app_easy_gap_advertise_stop();
				adv_state = SLEEP;
				user_enable_extended_sleep();
				user_ble_gap_stop_ongoing_operation();

}

static void rsp_cancel_timer_cb()
{	
 		#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
        uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	      arch_printf("\n\r[%08d us] 8.RSP_timer_cancellation cb...", current_sys_tick);
		#endif	
		user_rsp_cancel_timer = EASY_TIMER_INVALID_TIMER;    

	  user_change_adv_state(DEV_INFO);
    app_easy_gap_advertise_stop();
}

static void dev_info_cancel_timer_cb()
{	
 		#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
        uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	      arch_printf("\n\r[%08d us] 9.Dev_info_timer_cancellation cb...", current_sys_tick);
		#endif	
		user_dev_info_cancel_timer = EASY_TIMER_INVALID_TIMER;    
	 if(adv_state_before == MOTION_ADVERTISING &&	cancel_motion_adv == 1)
		 {
			#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
					uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
					arch_printf("\n\r[%08d us] X.Motion_ADV_cancellation...", current_sys_tick);
			#endif
			cancel_motion_adv = 0;
			adv_state_before = STATIC_ADVERTISING;
		 }
		 if(adv_state_before == STATIC_ADVERTISING){
			 user_periodic_rx_on_timer = app_easy_timer(PERIODIC_RX_ON_INTERVAL_STATIC, periodic_rx_on_timer_cb);
		 }else{
			 user_periodic_rx_on_timer = app_easy_timer(PERIODIC_RX_ON_INTERVAL_MOTION, periodic_rx_on_timer_cb);
		 }
	  user_change_adv_state(adv_state_before);
    app_easy_gap_advertise_stop();
}


void user_app_adv_undirect_complete(uint8_t status)
{
		 #if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
        uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	      arch_printf("\n\r[%08d us] X.User_app_adv_undirect_complete...", current_sys_tick);
		 #endif	

    if (status == GAP_ERR_CANCELED)
    {
			switch (adv_state)
			{
				case MOTION_ADVERTISING:
					if (user_dev_info_cancel_timer != EASY_TIMER_INVALID_TIMER)
					{
							app_easy_timer_cancel(user_dev_info_cancel_timer);
							user_dev_info_cancel_timer = EASY_TIMER_INVALID_TIMER;
					}
					user_app_adv_start();
				break;
				case STATIC_ADVERTISING:
					if (user_dev_info_cancel_timer != EASY_TIMER_INVALID_TIMER)
					{
							app_easy_timer_cancel(user_dev_info_cancel_timer);
							user_dev_info_cancel_timer = EASY_TIMER_INVALID_TIMER;
					}
					user_app_adv_start();
				break;
				case RX_ON_ADVERTISING:
					if (user_periodic_rx_on_timer != EASY_TIMER_INVALID_TIMER)
						{
								app_easy_timer_cancel(user_periodic_rx_on_timer);
								user_periodic_rx_on_timer = EASY_TIMER_INVALID_TIMER;
						}
						user_app_adv_start();
				break;				
				case SCAN:
					if (user_rx_on_cancel_timer != EASY_TIMER_INVALID_TIMER)
						{
								app_easy_timer_cancel(user_rx_on_cancel_timer);
								user_rx_on_cancel_timer = EASY_TIMER_INVALID_TIMER;
						}
					user_scan_start();
				break;
			  case SLEEP:
				  user_enable_extended_sleep();
					#ifdef CFG_PRINTF
            arch_puts("\n\n\rAdv Cancel, Sleep case, set arch_ble_ext_wakeup_on...\n");
					#endif
			  break;
				case DEV_INFO:
					if (user_rsp_cancel_timer != EASY_TIMER_INVALID_TIMER)
					{
							app_easy_timer_cancel(user_rsp_cancel_timer);
							user_rsp_cancel_timer = EASY_TIMER_INVALID_TIMER;
					}
					user_app_adv_start();
				default :
				break;			
			}
    }
}

/**
 ****************************************************************************************
 * @brief Parameter update request timer callback function.
 ****************************************************************************************
*/
static void param_update_request_timer_cb()
{
    app_easy_gap_param_update_start(app_connection_idx);
    app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
}

void user_app_init(void)
{
#ifdef CFG_PRINTF
 arch_printf("\n\n\r user_app_init");
#endif
	
		adv_state = MOTION_ADVERTISING;
	  cancel_motion_adv = 0;
	  
    //Reset all timers
  	app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;	
		user_enable_extended_sleep_timer = EASY_TIMER_INVALID_TIMER;    
    user_periodic_rx_on_timer = EASY_TIMER_INVALID_TIMER;

    // Get own BD Address
  	uint32_t bdaddr_msb = (GetWord32(BLE_BDADDRU_REG))&0x1FFFF;
    uint32_t bdaddr_lsb = (GetWord32(BLE_BDADDRL_REG));
    memcpy(&own_bd_addr, &bdaddr_lsb, 4);
    memcpy(&own_bd_addr[4], &bdaddr_msb, 2);	
    
    //Set the flag to update periodic timers once
  	user_periodic_timer_init_once = 1;

		user_set_led_state(false);
	
    // Initialize Advertising and Scan Response Data
    memcpy(stored_adv_data, USER_ADVERTISE_DATA, USER_ADVERTISE_DATA_LEN);
    stored_adv_data_len = USER_ADVERTISE_DATA_LEN;
    memcpy(stored_scan_rsp_data, USER_ADVERTISE_SCAN_RESPONSE_DATA, USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN);
    stored_scan_rsp_data_len = USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN;
    
#ifdef CFG_PRINTF
    systick_init();
#endif
    req_found = false;
	  default_app_on_init();
}


/**
 ****************************************************************************************
 * @brief Start undirected motion advertising                
 * @return void
 ****************************************************************************************
*/ 
void user_motion_undirect_adv_start(void)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();
    
		cmd->intv_min = MS_TO_BLESLOTS(MOTION_ADV_INTERVAL);
		cmd->intv_max = MS_TO_BLESLOTS(MOTION_ADV_INTERVAL);
	  cmd->channel_map = ADV_ALL_CHNLS_EN;
		
#if defined (__DA14531__)  // Power output change is possible for DA14531 only
  	rf_pa_pwr_set(MOTION_ADV_POWER);
	  set_recommended_settings();
#endif	
    // Initialize Manufacturer Specific Data with Direction Finding data
    df_adv_data_init();

	  mnf_data.proprietary_data[2] = MOTION_ADV_HEADER;
    mnf_data.proprietary_data[9] = CalculateCRC(&mnf_data.proprietary_data[1], 8); // Checksum
		
    // Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

		if(user_enable_extended_sleep_timer != EASY_TIMER_INVALID_TIMER)
		{
				app_easy_timer_cancel(user_enable_extended_sleep_timer);
				user_enable_extended_sleep_timer = EASY_TIMER_INVALID_TIMER;		
		}
		user_enable_extended_sleep_timer =	app_easy_timer(TIMEOUT_FOR_SLEEP, user_enable_extended_sleep_timer_cb);
	
    app_easy_gap_undirected_advertise_start();
		#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
      systick_stop();
      systick_start(SYSTICK_PERIOD_US, SYSTICK_EXCEPTION);
      uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
			arch_printf("\n\n\r[%08d us] 0.Motion Advertising...", current_sys_tick);
		#endif
}

/**
 ****************************************************************************************
 * @brief start static advertising
 * @return void
 ****************************************************************************************
*/ 
void user_static_undirect_adv_start(void)
{
    struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();
    
		cmd->intv_min = MS_TO_BLESLOTS(STATIC_ADV_INTERVAL);
		cmd->intv_max = MS_TO_BLESLOTS(STATIC_ADV_INTERVAL);
	  cmd->channel_map = ADV_ALL_CHNLS_EN;

#if defined (__DA14531__)  // Power output change is possible for DA14531 only		
		rf_pa_pwr_set(STATIC_ADV_POWER);
		set_recommended_settings();
#endif	
    // Initialize Manufacturer Specific Data with Direction Finding data
    df_adv_data_init();

	  mnf_data.proprietary_data[2] = STATIC_ADV_HEADER;
    mnf_data.proprietary_data[9] = CalculateCRC(&mnf_data.proprietary_data[1], 8); // Checksum = 0x??
		
    // Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

//		if(user_enable_extended_sleep_timer != EASY_TIMER_INVALID_TIMER)
//		{
//				app_easy_timer_cancel(user_enable_extended_sleep_timer);
//				user_enable_extended_sleep_timer = EASY_TIMER_INVALID_TIMER;		
//		}
//		user_enable_extended_sleep_timer =	app_easy_timer(TIMEOUT_FOR_SLEEP, user_enable_extended_sleep_timer_cb);

	
    app_easy_gap_undirected_advertise_start();
		#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
      systick_stop();
      systick_start(SYSTICK_PERIOD_US, SYSTICK_EXCEPTION);
      uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
			arch_printf("\n\n\r[%08d us] 0.Static Advertising...", current_sys_tick);
		#endif
}

void user_app_adv_start(void)
{
	
	if(user_periodic_timer_init_once)
		 {
  	   user_periodic_timer_init_once = 0;

  		if(user_periodic_rx_on_timer != EASY_TIMER_INVALID_TIMER)
        {
            app_easy_timer_cancel(user_periodic_rx_on_timer);
            user_periodic_rx_on_timer = EASY_TIMER_INVALID_TIMER;		
        }
        // Schedule the next periodic scan to check for REQ packets
        if(adv_state == STATIC_ADVERTISING)
        {
          user_periodic_rx_on_timer = app_easy_timer(PERIODIC_RX_ON_INTERVAL_STATIC, periodic_rx_on_timer_cb);
				}
        else
        {
            user_periodic_rx_on_timer = app_easy_timer(PERIODIC_RX_ON_INTERVAL_MOTION, periodic_rx_on_timer_cb);
        }

//				if(user_enable_extended_sleep_timer != EASY_TIMER_INVALID_TIMER)
//				{
//						app_easy_timer_cancel(user_enable_extended_sleep_timer);
//						user_enable_extended_sleep_timer = EASY_TIMER_INVALID_TIMER;		
//				}

//			  user_enable_extended_sleep_timer =	app_easy_timer(TIMEOUT_FOR_SLEEP, user_enable_extended_sleep_timer_cb);
			
		 } 


	 switch (adv_state)
		{
			case MOTION_ADVERTISING:
				user_motion_undirect_adv_start();
			break;
			case STATIC_ADVERTISING:
				user_static_undirect_adv_start();
			break;
      case RX_ON_ADVERTISING:
				user_rx_on_undirect_adv_start();
			break;				
      case RSP_ADVERTISING:
				user_rsp_undirect_adv_start();
			break;
			case DEV_INFO:
			  user_dev_info_undirect_adv_start();
			break;
			case SLEEP:
			  user_enable_extended_sleep();
			break;
			default :
				user_static_undirect_adv_start();
			break;			
		}

}

void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    if (app_env[connection_idx].conidx != GAP_INVALID_CONIDX)
    {
        app_connection_idx = connection_idx;

        // Stop the advertising data update timer
        app_easy_timer_cancel(user_periodic_rx_on_timer);

        // Check if the parameters of the established connection are the preferred ones.
        // If not then schedule a connection parameter update request.
        if ((param->con_interval < user_connection_param_conf.intv_min) ||
            (param->con_interval > user_connection_param_conf.intv_max) ||
            (param->con_latency != user_connection_param_conf.latency)  ||
            (param->sup_to != user_connection_param_conf.time_out))
        {
            // Connection params are not these that we expect
            app_param_update_request_timer_used = app_easy_timer(APP_PARAM_UPDATE_REQUEST_TO, param_update_request_timer_cb);
        }
    }
    else
    {
        // No connection has been established, restart advertising
				user_periodic_timer_init_once = 1;
			  if(adv_state_before == MOTION_ADVERTISING)
				{
					cancel_motion_adv = 1;
				}
		    user_change_adv_state(adv_state_before);
			  user_app_adv_start();
    }
  	adv_state_before = adv_state;
    default_app_on_connection(connection_idx, param);
}


void user_app_disconnect(struct gapc_disconnect_ind const *param)
{
    // Cancel the parameter update request timer
    if (app_param_update_request_timer_used != EASY_TIMER_INVALID_TIMER)
    {
        app_easy_timer_cancel(app_param_update_request_timer_used);
        app_param_update_request_timer_used = EASY_TIMER_INVALID_TIMER;
    }
    // Restart Advertising
		
		user_change_adv_state(adv_state_before);
    user_app_adv_start();
}

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
/*     switch(msgid)
    {
        case CUSTS1_VAL_WRITE_IND:
        {
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

            switch (msg_param->handle)
            {
                case SVC1_IDX_CONTROL_POINT_VAL:
                    user_svc1_ctrl_wr_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                case SVC1_IDX_LED_STATE_VAL:
                    user_svc1_led_wr_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                case SVC1_IDX_ADC_VAL_1_NTF_CFG:
                    user_svc1_adc_val_1_cfg_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                case SVC1_IDX_BUTTON_STATE_NTF_CFG:
                    user_svc1_button_cfg_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                case SVC1_IDX_INDICATEABLE_IND_CFG:
                    user_svc1_long_val_cfg_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                case SVC1_IDX_LONG_VALUE_NTF_CFG:
                    user_svc1_long_val_cfg_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                case SVC1_IDX_LONG_VALUE_VAL:
                    user_svc1_long_val_wr_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                default:
                    break;
            }
        } break;

        case CUSTS1_VAL_NTF_CFM:
        {
            struct custs1_val_ntf_cfm const *msg_param = (struct custs1_val_ntf_cfm const *)(param);

            switch (msg_param->handle)
            {
                case SVC1_IDX_ADC_VAL_1_VAL:
                    break;

                case SVC1_IDX_BUTTON_STATE_VAL:
                    break;

                case SVC1_IDX_LONG_VALUE_VAL:
                    break;

                default:
                    break;
            }
        } break;

        case CUSTS1_VAL_IND_CFM:
        {
            struct custs1_val_ind_cfm const *msg_param = (struct custs1_val_ind_cfm const *)(param);

            switch (msg_param->handle)
            {
                case SVC1_IDX_INDICATEABLE_VAL:
                    break;

                default:
                    break;
             }
        } break;

        case CUSTS1_ATT_INFO_REQ:
        {
            struct custs1_att_info_req const *msg_param = (struct custs1_att_info_req const *)param;

            switch (msg_param->att_idx)
            {
                case SVC1_IDX_LONG_VALUE_VAL:
                    user_svc1_long_val_att_info_req_handler(msgid, msg_param, dest_id, src_id);
                    break;

                default:
                    user_svc1_rest_att_info_req_handler(msgid, msg_param, dest_id, src_id);
                    break;
             }
        } break;

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

        case CUSTS1_VALUE_REQ_IND:
        {
            struct custs1_value_req_ind const *msg_param = (struct custs1_value_req_ind const *) param;

            switch (msg_param->att_idx)
            {
                case SVC3_IDX_READ_4_VAL:
                {
                    user_svc3_read_non_db_val_handler(msgid, msg_param, dest_id, src_id);
                } break;

                default:
                {
                    // Send Error message
                    struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC(CUSTS1_VALUE_REQ_RSP,
                                                                    src_id,
                                                                    dest_id,
                                                                    custs1_value_req_rsp);

                    // Provide the connection index.
                    rsp->conidx  = app_env[msg_param->conidx].conidx;
                    // Provide the attribute index.
                    rsp->att_idx = msg_param->att_idx;
                    // Force current length to zero.
                    rsp->length = 0;
                    // Set Error status
                    rsp->status  = ATT_ERR_APP_ERROR;
                    // Send message
                    ke_msg_send(rsp);
                } break;
             }
        } break;

        default:
            break;
    } */
}
/**
 ****************************************************************************************
 * @brief Start rx on undirected advertising                
 * @return void
 ****************************************************************************************
*/ 
void user_rx_on_undirect_adv_start(void)
{
    // Schedule the rx on advertising cancellation
    user_rx_on_cancel_timer = app_easy_timer(RX_ON_CANCEL_TIMER, rx_on_cancel_timer_cb);

	  struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();
    
		cmd->intv_min = MS_TO_BLESLOTS(RX_ON_ADV_INTERVAL);
		cmd->intv_max = MS_TO_BLESLOTS(RX_ON_ADV_INTERVAL);
	  cmd->channel_map = ADV_ALL_CHNLS_EN;

#if defined (__DA14531__)  // Power output change is possible for DA14531 only	
		rf_pa_pwr_set(RX_ON_ADV_POWER);
	  set_recommended_settings();
#endif	

    if(adv_state_before == MOTION_ADVERTISING){
  	  mnf_data.proprietary_data[2] = RX_ON_MOTION_ADV_HEADER;
		} else{
			mnf_data.proprietary_data[2] = RX_ON_STATIC_ADV_HEADER;
		}
    mnf_data.proprietary_data[9] = CalculateCRC(&mnf_data.proprietary_data[1], 8); // Checksum
		
    // Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

    app_easy_gap_undirected_advertise_start();
	
		#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
		  uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
			arch_printf("\n\r[%08d us] 2.RX_ON_adv_start... Header: %02X RX_ON_Hanldler: %02X", current_sys_tick, mnf_data.proprietary_data[2], user_periodic_rx_on_timer);
		#endif
	
}

/**
 ****************************************************************************************
 * @brief Start rsp undirected advertising                
 * @return void
 ****************************************************************************************
*/ 
void user_rsp_undirect_adv_start(void)
{
    // Schedule the rsp advertising cancellation
    user_rsp_cancel_timer = app_easy_timer(RSP_CANCEL_TIMER, rsp_cancel_timer_cb);

	  struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();
    
		cmd->intv_min = MS_TO_BLESLOTS(RSP_ADV_INTERVAL);
		cmd->intv_max = MS_TO_BLESLOTS(RSP_ADV_INTERVAL);
	  cmd->channel_map = ADV_CHNL_37_EN;
		//cmd->channel_map = ADV_ALL_CHNLS_EN;
	
#if defined (__DA14531__)  // Power output change is possible for DA14531 only	
		rf_pa_pwr_set(RSP_ADV_POWER);
	  set_recommended_settings();
#endif

  	// Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

    app_easy_gap_undirected_advertise_start();
	
		#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
		  uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
			arch_printf("\n\r[%08d us] 7.User_RSP_undirect_adv_start...", current_sys_tick);
		#endif
	
}

/**
 ****************************************************************************************
 * @brief Start device info undirected advertising                
 * @return void
 ****************************************************************************************
*/ 
void user_dev_info_undirect_adv_start(void)
{
    // Schedule the device info advertising cancellation
    user_dev_info_cancel_timer = app_easy_timer(DEV_INFO_CANCEL_TIMER, dev_info_cancel_timer_cb);

	  struct gapm_start_advertise_cmd* cmd;
    cmd = app_easy_gap_undirected_advertise_get_active();
    
		cmd->intv_min = MS_TO_BLESLOTS(DEV_INFO_ADV_INTERVAL);
		cmd->intv_max = MS_TO_BLESLOTS(DEV_INFO_ADV_INTERVAL);
		cmd->channel_map = ADV_ALL_CHNLS_EN;
	
#if defined (__DA14531__)  // Power output change is possible for DA14531 only	
		rf_pa_pwr_set(DEV_INFO_ADV_POWER);
	  set_recommended_settings();
#endif

    dev_info_adv_data_init();

	// Add manufacturer data to initial advertising or scan response data, if there is enough space
    app_add_ad_struct(cmd, &mnf_data, sizeof(struct mnf_specific_data_ad_structure), 1);

    app_easy_gap_undirected_advertise_start();
	
		#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
		  uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
			arch_printf("\n\r[%08d us] 9.User_DEV_INFO_undirect_adv_start...", current_sys_tick);
		#endif
	
}

/**
 ****************************************************************************************
 * @brief Send the device to sleep.
 * @return void
 ****************************************************************************************
*/                   
void user_enable_extended_sleep(void)
{
    arch_ble_ext_wakeup_on();
	//TBD: set here instruction to go to HIBERNATION
}

/**
 ****************************************************************************************
 * @brief Function to set the advertising state
 * @return void
 ****************************************************************************************
*/
void user_change_adv_state(advertising_state state)
{
	switch (state)
	{
			case MOTION_ADVERTISING:
					adv_state = MOTION_ADVERTISING;
					break;
			case STATIC_ADVERTISING:
					adv_state = STATIC_ADVERTISING;
					break;
			case RX_ON_ADVERTISING:
					adv_state = RX_ON_ADVERTISING;
					break;
			case RSP_ADVERTISING:
					adv_state = RSP_ADVERTISING;
					break;
			case SLEEP:
					adv_state = SLEEP;
					#ifdef CFG_PRINTF
							arch_puts("\n\n\rSystem going to sleep...");
					#endif
					break;
			case DEV_INFO:
					adv_state = DEV_INFO;
					break;
			default :
					adv_state = MOTION_ADVERTISING;
					break;
	}	
}

static void _user_app_sensor_init()
{
	#if 0
    struct bmi2_sens_config config[2] = {0};
    
     /* Array to select sensors */
    uint8_t sens_list[3] = { BMI2_ACCEL, BMI2_NO_MOTION, BMI2_ANY_MOTION };
    
    struct bmi2_int_pin_config int_cfg;
    
    int_cfg.pin_type = BMI2_INT1;
    int_cfg.int_latch = 0;
    int_cfg.pin_cfg[0].lvl = 1;
    int_cfg.pin_cfg[0].output_en = 1;
    int_cfg.pin_cfg[0].od = 1;
    
    /* Select features and their pins to be mapped to */
    struct bmi2_sens_int_config sens_int[2] = { 
        {.type = BMI2_NO_MOTION, .hw_int_pin = BMI2_INT1},
        {.type = BMI2_ANY_MOTION, .hw_int_pin = BMI2_INT1}
    };
    
    
    port_bmi270_itf_begin();
    
    // Initialize the sensor
    port_bmi270_init();
    
    // Enable the sensors of interest
    bmi2_sensor_enable(sens_list, 3, port_bmi270_dev_ptr());
    
    config[0].type = BMI2_NO_MOTION;
    config[1].type = BMI2_ANY_MOTION;

    bmi2_set_int_pin_config(&int_cfg,port_bmi270_dev_ptr());
    
    /* Get default configurations for the type of feature selected */
    bmi2_get_sensor_config(config, 2, port_bmi270_dev_ptr());

    /* Set the new configuration */
    bmi2_set_sensor_config(config, 2, port_bmi270_dev_ptr());
    
    // Enable the sensor interrupts of interest
    bmi2_map_feat_int(sens_int, 2, port_bmi270_dev_ptr());

    port_bmi270_itf_end();
    #else
    uint8_t var;
    uint8_t feat_cfg[4];
    
    // Start communication
    port_bmi270_itf_begin();
    
    // Initialize the sensor
    port_bmi270_init();

    /* Feature Configuration */
    bmi2_set_adv_power_save(BMI2_DISABLE,  port_bmi270_dev_ptr());
    
    var = 0x01; // go to page 1
    bmi2_set_regs(BMI2_FEAT_PAGE_ADDR, &var, 1, port_bmi270_dev_ptr());
    
    // Any motion feature configuration
    bmi2_get_regs(BMI2_FEATURES_REG_ADDR + 0x0C, (uint8_t*)feat_cfg, 4, port_bmi270_dev_ptr());
    feat_cfg[3] |= 0x80; // Enabling the any motion feature
    bmi2_set_regs(BMI2_FEATURES_REG_ADDR + 0x0C, (uint8_t*)feat_cfg, 4, port_bmi270_dev_ptr());

    var = 0x02; // go to page 2
    bmi2_set_regs(BMI2_FEAT_PAGE_ADDR, &var, 1, port_bmi270_dev_ptr());
    
    // No motion feature configuration
    bmi2_get_regs(BMI2_FEATURES_REG_ADDR, (uint8_t*)feat_cfg, 4, port_bmi270_dev_ptr());
//    feat_cfg[0] = 0xFE;      // Setting the no motion threshold to 5 seconds @ 50Hz
    feat_cfg[0] = 0xB0;      // Setting the no motion threshold to 20 seconds LSB => 20000ms/20ms=1000 => 0x03E8 @ 50Hz
    feat_cfg[1] |= 0x04;      // Setting the no motion threshold to 20 seconds MSB (duration is 12 bit, higher bits are x,y,z enable bits) @ 50Hz		
    feat_cfg[3] |= 0x80; // Enabling the no motion feature
    bmi2_set_regs(BMI2_FEATURES_REG_ADDR, (uint8_t*)feat_cfg, 4, port_bmi270_dev_ptr());
    
    bmi2_set_adv_power_save(BMI2_ENABLE,  port_bmi270_dev_ptr());
		
		#ifdef CFG_PRINTF
		arch_printf("\n\n\r bmi2_set_adv_power_save = %s", rslt==0? "OK":"NOT OK");
		#endif
    
    var = 0x00; // latched
    bmi2_set_regs(BMI2_INT_LATCH_ADDR, &var, 1, port_bmi270_dev_ptr());
    
    var = 0x20 | 0x40; // no_motion | any_motion
    bmi2_set_regs(BMI2_INT1_MAP_FEAT_ADDR, &var, 1, port_bmi270_dev_ptr());
    
//    var = 0x02 | 0x08; // active_high | push_pull | output_enabled
    var = 0x00 | 0x08; // active_low | push_pull | output_enabled		
    bmi2_set_regs(BMI2_INT1_IO_CTRL_ADDR, &var, 1, port_bmi270_dev_ptr());

//    var = 0x07; // acc_conf at 50Hz and low power mode
		bmi2_set_regs(BMI2_ACC_CONF_ADDR, &var, 1, port_bmi270_dev_ptr());
    
    var = 0x04; // acc_enable
    bmi2_set_regs(BMI2_PWR_CTRL_ADDR, &var, 1, port_bmi270_dev_ptr());

    // End communication
    port_bmi270_itf_end();
    
    #endif
}




// Configuration struct for Flash SPI
static const spi_cfg_t flash_spi_cfg = {
    .spi_ms = SPI_MS_MODE_MASTER,
    .spi_cp = SPI_CP_MODE_0,
    .spi_speed = SPI_SPEED_MODE_4MHz,
    .spi_wsz = SPI_MODE_8BIT,
    .spi_cs = SPI_CS_0,
    .cs_pad.port = SPI_EN_PORT,
    .cs_pad.pin = SPI_EN_PIN,
#if defined (__DA14531__)
    .spi_capture = SPI_MASTER_EDGE_CAPTURE,
#endif
#if defined (CFG_SPI_DMA_SUPPORT)
    .spi_dma_channel = SPI_DMA_CHANNEL_01,
    .spi_dma_priority = DMA_PRIO_0,
#endif
};

// SPI Flash device parameters environment(P25Q11U)
static const spi_flash_cfg_t flash_cfg_env = {

    .dev_index = 0x10,
    .jedec_id = 0x854011, 
	  .chip_size = 0x20000,
};


void flash_itf_de_init(void)
{

    GPIO_ConfigurePin(SPI_EN_PORT,  SPI_EN_PIN,  OUTPUT, PID_GPIO, true);
    GPIO_ConfigurePin(SPI_CLK_PORT, SPI_CLK_PIN, INPUT, PID_GPIO, false);
    GPIO_ConfigurePin(SPI_DO_PORT, SPI_DO_PIN, INPUT, PID_GPIO, false);
	  GPIO_ConfigurePin(SPI_DI_PORT, SPI_DI_PIN, INPUT_PULLDOWN, PID_GPIO, false);
    
    spi_release();
}

void flash_itf_init(void)
{
    GPIO_ConfigurePin(SPI_EN_PORT,  SPI_EN_PIN,  OUTPUT, PID_SPI_EN, true);
    GPIO_ConfigurePin(SPI_CLK_PORT, SPI_CLK_PIN, OUTPUT, PID_SPI_CLK, false);
    GPIO_ConfigurePin(SPI_DO_PORT, SPI_DO_PIN, OUTPUT, PID_SPI_DO, false);
	  GPIO_ConfigurePin(SPI_DI_PORT, SPI_DI_PIN, INPUT_PULLDOWN, PID_SPI_DI, false);

	  spi_flash_configure_env(&flash_cfg_env); 
  	spi_initialize(&flash_spi_cfg);
}

void user_app_on_db_init_complete(void)
{
#ifdef CFG_PRINTF
 arch_printf("\n\n\r user_app_on_db_init_complete");
#endif	
	
#ifdef HAS_WKUP_KEYS
    wkup_keys_setup();
#endif // HAS_WKUP_KEYS
    
    _user_app_sensor_init();
	

		flash_itf_init();
	  spi_flash_power_down();
    user_app_adv_start();
}


static void _user_pb1_long_press_handler()
{
	app_push_button_timer = EASY_TIMER_INVALID_TIMER;

	#ifdef CFG_PRINTF
			arch_printf("\n\n\r Button pressed > %d sec...\n",APP_BUTTON_LONG_PRESS_TO/100);
	#endif
}

static void _user_pb1_key_handler(bool pressed)
{
	if(pressed) 
		{ // Do something when button is firstly pressed
			// Start timer to check about long press
			app_push_button_timer = app_easy_timer(APP_BUTTON_LONG_PRESS_TO, _user_pb1_long_press_handler);

#if (WKUP_KEYS_NUM > 1)			

			// Cancel Motion_Sleep_timeout timer	
			if(user_enable_extended_sleep_timer != EASY_TIMER_INVALID_TIMER)
			{
					app_easy_timer_cancel(user_enable_extended_sleep_timer);
					user_enable_extended_sleep_timer = EASY_TIMER_INVALID_TIMER;		
			}

			#ifdef CFG_PRINTF
			arch_printf("\n\n\r Button pressed: Started long press timer and cancelled Sleep timer...\n");
			#endif

#endif

		} 
	else
		{ // Button Released

			// Clear the long press timer if we did not reach the long press timeout
			if(app_push_button_timer != EASY_TIMER_INVALID_TIMER) 
				{ //Released after a Short press
					app_easy_timer_cancel(app_push_button_timer);
					app_push_button_timer = EASY_TIMER_INVALID_TIMER;

#if (WKUP_KEYS_NUM > 1)
			
					#ifdef CFG_PRINTF
						arch_printf("\n\n\r Button released after a short press...\n");
					#endif
	
						if (adv_state_before == MOTION_ADVERTISING && !arch_ble_ext_wakeup_get())				
						{// If already in MOTION ADVERTISING do nothing (accelerometer is taking care)
							#ifdef CFG_PRINTF
							arch_printf("\n\n\r Motion adv still on, do nothing as accelerometer is in NO_MOTION detect mode\n");
							#endif
						}
						else
							{
							int_feat_enable = 0x20; // enable the NO_MOTION interrupt and disable the ANY_MOTION
							arch_set_sleep_mode(app_default_sleep_mode);
							arch_ble_force_wakeup();
							adv_state = MOTION_ADVERTISING;
							user_periodic_timer_init_once = 1;
							//cancel_motion_adv = 1;

							if (adv_state == SLEEP || arch_ble_ext_wakeup_get())
								{// If we are in sleep => Wake up from sleep
									 #ifdef CFG_PRINTF
									 arch_printf("\n\n\rWake up from SLEEP due to Button press");
									 arch_printf("\n\n\rRestart Motion adv\n");
									 #endif
									 arch_ble_ext_wakeup_off();
									 user_app_adv_start();			
								}	
							else
								{// Switch to motion advertising
									 #ifdef CFG_PRINTF
									 arch_printf("\n\n\rSwitch to Motion adv due to Button press");
									 #endif
									 app_easy_gap_advertise_stop();
								}

							} // end of adv_state_before =! MOTION_ADVERTISING
#endif					
        }// End of Released after a Short press
			else 
				{//Released after a Long press

#if (WKUP_KEYS_NUM > 1)				
					if (adv_state != SLEEP)
						{
							// Go to sleep
							int_feat_enable = 0x40; // enable the ANY_MOTION interrupt and disable the NO_MOTION
							#ifdef CFG_PRINTF
							arch_puts("\n\n\rSet Sensor to ANY_MOTION detection, Stop adv and going to sleep...\n");
							#endif
							adv_state = SLEEP;
//							app_easy_gap_advertise_stop();
              user_enable_extended_sleep();
							user_ble_gap_stop_ongoing_operation();
						}
					else
						{
							// if system is already in SLEEP this point will never been reached as app_push_button_timer is not started
						}
#endif

			  } // End of Released after a Long press

// Due to the fact that PB1 is shared with SDA pin, below feature needs to implement I2C release feature and right now cannot be used
#if (WKUP_KEYS_NUM > 1)
				if(int_feat_enable) {
						// Do the interrupt updating if needed
						port_bmi270_itf_begin();
						bmi2_set_regs(BMI2_INT1_MAP_FEAT_ADDR, &int_feat_enable, 1, port_bmi270_dev_ptr());
						port_bmi270_itf_end();
						#ifdef CFG_PRINTF
								arch_puts("\n\n\r Do Interrupt updating...\n");
						#endif
				}
#endif
		}// End of Button Released
}

static void _user_inertial_handle_interrupt()
{
    uint8_t int0_status;
    uint8_t int_feat_enable = 0;
    
		arch_set_sleep_mode(app_default_sleep_mode);
		arch_ble_force_wakeup();

	  port_bmi270_itf_begin();
    bmi2_get_regs(BMI2_INT_STATUS_0_ADDR, &int0_status, 1, port_bmi270_dev_ptr());
    port_bmi270_itf_end();
    
  	// Check which interrupt we have received
  	if(int0_status & BMI270_ANY_MOT_STATUS_MASK) {
        // ANY_MOTION interrupt received wake up from sleep or switch to MOTION ADV
        int_feat_enable = 0x20; // enable the NO_MOTION interrupt and disable the ANY_MOTION
			
				if (adv_state == SLEEP || arch_ble_ext_wakeup_get())
					{// Wake up from sleep
					 #ifdef CFG_PRINTF
						arch_printf("\n\n\rWake up from SLEEP due to interrupt received");
						arch_printf("\n\n\rRestart Motion adv\n");
					 #endif
						arch_ble_ext_wakeup_off();
				    adv_state = MOTION_ADVERTISING;
		        // This flag will recharge user_sleep_timout_timer with TIMEOUT_FOR_SLEEP and RX_ON timer with RX_ON value
		        user_periodic_timer_init_once = 1;
						user_app_adv_start();			
					}	
        else
					{// Switch to MOTION ADV
					 #ifdef CFG_PRINTF
						arch_printf("\n\n\rSwitch to Motion adv\n");
					 #endif
				    adv_state = MOTION_ADVERTISING;
		        // This flag will recharge user_sleep_timout_timer with TIMEOUT_FOR_SLEEP and RX_ON timer with RX_ON value
		        user_periodic_timer_init_once = 1;
						app_easy_gap_advertise_stop(); // Stop Static ADV
					}
        
    } else if(int0_status & BMI270_NO_MOT_STATUS_MASK) {
        // Timeout for NO_MOTION elapsed got to Static Adv!
        int_feat_enable = 0x40; // enable the ANY_MOTION interrupt and disable the NO_MOTION
				cancel_motion_adv = 1;
    }
    
    if(int_feat_enable) {
        // Do the interrupt updating
        port_bmi270_itf_begin();
        bmi2_set_regs(BMI2_INT1_MAP_FEAT_ADDR, &int_feat_enable, 1, port_bmi270_dev_ptr());
        port_bmi270_itf_end();
    }
}
		
#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
/* return static buffer with formatted hex string */
static char *format_hex_string(uint8_t *data, uint8_t len)	
{
	static char buf[61];
	uint8_t i;
	for(i = 0; i < len; i++)
	{
		sprintf(&buf[i*2], "%02x", data[i]);
	}
	
	buf[(i+1)*2+1] = '\0';
	
	return buf;
}
#endif

void user_on_adv_report_ind(struct gapm_adv_report_ind const * param)
{
    const uint8_t user_man_comp_f1_id[] = MAN_QUUPPA_COMP_F1_ID;
	  uint8_t reverse_bd_addr[6];
    uint8_t temp_report_data[ADV_DATA_LEN + 1];

    memcpy(temp_report_data, param->report.data, param->report.data_len);
    temp_report_data[param->report.data_len] = '\0';
	
	  reverse_bd_addr[5] = own_bd_addr [0];
	  reverse_bd_addr[4] = own_bd_addr [1];	  
	  reverse_bd_addr[3] = own_bd_addr [2];
	  reverse_bd_addr[2] = own_bd_addr [3];	
		reverse_bd_addr[1] = own_bd_addr [4];
	  reverse_bd_addr[0] = own_bd_addr [5];	
	
			#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
			//arch_printf("\r\n5.ADV: %s", format_hex_string(temp_report_data, param->report.data_len));
	  	#endif

//    // Check if the Adv Report is a REQ packet coming from Quuppa locator and is referred to own BD Address
//    if(!memcmp(report_data + 4, user_man_comp_f1_id, 4) && !memcmp(report_data + 9, reverse_bd_addr, 6))

// Check if the Adv Report is a REQ packet coming from Quuppa locator
	  if(!memcmp(temp_report_data + 4, user_man_comp_f1_id, 4))
			{
			#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
  			uint16_t DeveloperID;
				memcpy(&DeveloperID, &temp_report_data[16], sizeof(uint16_t));
		    uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
			  arch_printf("\n\r[%08d us] 5.REQ F1 found,", current_sys_tick);
  			arch_printf("\r\n              5.Raw Data: %s", format_hex_string(temp_report_data, param->report.data_len));
  			arch_printf("\r\n              5.Header: %02x, Payload Type: %02x, DevID: %04x, Data: %s", temp_report_data[8], temp_report_data[15], DeveloperID, format_hex_string(temp_report_data + 16, param->report.data_len-16));
	      arch_printf("\r\n              5.BD Tgt: %s , BD Own: %s", format_hex_string(temp_report_data + 9, 6), format_hex_string(reverse_bd_addr, 6));
			#endif
    // Check if is referred to own BD Address
    if(!memcmp(temp_report_data + 9, reverse_bd_addr, 6))
			{
			#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
		    uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	      arch_printf("\r\n[%08d us] 5.BD Tgt: %s = BD Own: %s => STOP SCAN", current_sys_tick, format_hex_string(temp_report_data + 9, 6), format_hex_string(reverse_bd_addr, 6));
			#endif
      req_found = true;
      memcpy(report_data, temp_report_data, param->report.data_len);
			report_data[param->report.data_len] = '\0';
			
         // Check the Payload Type
		     switch(report_data[15])
		     { 
					case BACK_CHANNEL_INFO: // Payload Type: 0x02
					{
				    #if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
		         uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
			       arch_printf("\n\r[%08d us] 5.Payload Type: 0x02 Back Channel Info received", current_sys_tick);
				    #endif

						mnf_data.proprietary_data[0] = 0xF2; // Quuppa Packet ID: 0xF2 = RSP Packet;
						mnf_data.proprietary_data[1] = report_data[8]; // Header containing the REQ/RSP Pair ID
						mnf_data.proprietary_data[2] = own_bd_addr[5]; // Quuppa Tag ID [MSB]	
						mnf_data.proprietary_data[3] = own_bd_addr[4]; // Quuppa Tag ID
						mnf_data.proprietary_data[4] = own_bd_addr[3]; // Quuppa Tag ID
						mnf_data.proprietary_data[5] = own_bd_addr[2]; // Quuppa Tag ID
						mnf_data.proprietary_data[6] = own_bd_addr[1]; // Quuppa Tag ID		
						mnf_data.proprietary_data[7] = own_bd_addr[0]; // Quuppa Tag ID [LSB]
						mnf_data.proprietary_data[8] = 0x02;  // Payload Type: 0x02 Back Channel Info 
						mnf_data.proprietary_data[9] = 0x5B;  // Back Channel Spec Number minor: 91 
						mnf_data.proprietary_data[10] = 0x00; // Back Channel Spec Number major: 00 
						mnf_data.proprietary_data[11] = DEVELOPER_ID & 0xFF; // Developer ID [LSB]
						mnf_data.proprietary_data[12] = (DEVELOPER_ID >> 8 )& 0xFF; // Developer ID [MSB]
						mnf_data.proprietary_data[13] = 0x00; // Developer Data 1
						mnf_data.proprietary_data[14] = 0x00; // Developer Data 2
						mnf_data.proprietary_data[15] = 0x00; // Developer Data 3		
						mnf_data.proprietary_data[16] = 0x00; // Developer Data 4
						mnf_data.proprietary_data[17] = 0x00; // Developer Data 5
						mnf_data.proprietary_data[18] = 0x00; // Developer Data 6
						mnf_data.proprietary_data[19] = 0x00; // Developer Data 7	
						mnf_data.proprietary_data[20] = 0x00; // Developer Data 8
						mnf_data.proprietary_data[21] = 0x00; // Developer Data 9
						mnf_data.proprietary_data[22] = 0x00; // Developer Data 10
						mnf_data.proprietary_data[23] = 0x00; // Developer Data 11
						
						break;
					}
					case DEVICE_INFO: // Payload Type: 0x01
					{
				    #if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
		         uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
			       arch_printf("\n\r[%08d us] 5.Payload Type: 0x01 Device Info received", current_sys_tick);
				    #endif

						get_device_info();
						
						mnf_data.proprietary_data[0] = 0xF2; // Quuppa Packet ID: 0xF2 = RSP Packet;
						mnf_data.proprietary_data[1] = report_data[8]; // Header containing the REQ/RSP Pair ID
						mnf_data.proprietary_data[2] = own_bd_addr[5]; // Quuppa Tag ID [MSB]	
						mnf_data.proprietary_data[3] = own_bd_addr[4]; // Quuppa Tag ID
						mnf_data.proprietary_data[4] = own_bd_addr[3]; // Quuppa Tag ID
						mnf_data.proprietary_data[5] = own_bd_addr[2]; // Quuppa Tag ID
						mnf_data.proprietary_data[6] = own_bd_addr[1]; // Quuppa Tag ID		
						mnf_data.proprietary_data[7] = own_bd_addr[0]; // Quuppa Tag ID [LSB]
						mnf_data.proprietary_data[8] = 0x01;  // Payload Type: 0x01 Device Info 
						mnf_data.proprietary_data[9] = 0x00;  // Version 
						mnf_data.proprietary_data[10] = 0x1B; // Supported features (1 = supported, Bit[7-0]=[RFU, RFU, RFU, Temp, Battery, Btn2=NA=0, Btn1, Battery Alarm])  
						mnf_data.proprietary_data[11] = ui8status; // Status	(Bit[7-0]=[RFU,..., Btn2=NA=0, Btn1(1=pressed), Battery Alarm(0=Battery OK, 1=Battery Low)]) 
						mnf_data.proprietary_data[12] = ui16battery_CR2032_mv & 0xFF; // Battery Voltage [LSB]
						mnf_data.proprietary_data[13] = (ui16battery_CR2032_mv >> 8 )& 0xFF; // Battery Voltage [MSB] => 0x0BB8 = 3.00V (1mV per bit, 0x0000 if not supported and 0 in supp feat bit)
						mnf_data.proprietary_data[14] = ui16temperature_k & 0xFF; // Temperature [LSB]     => Formula (hex)(C + 273.15)*10 => (20.0C + 273.15)*10 = 2931.5 (hex)(2932) = 0x0B74  
						mnf_data.proprietary_data[15] = (ui16temperature_k >> 8 )& 0xFF; // Temperature [MSB]     => 0x0B74 = 20.0C (0.1Kelvin per bit, 0x0000 if not supported and 0 in supp feat bit)		
						mnf_data.proprietary_data[16] = DEVELOPER_ID & 0xFF; // Developer ID [LSB]
						mnf_data.proprietary_data[17] = (DEVELOPER_ID >> 8 )& 0xFF; // Developer ID [MSB]
						mnf_data.proprietary_data[18] = 0x00; // FW Version [LSB]
						mnf_data.proprietary_data[19] = 0x01; // FW Version [MSB]	  => FW verion 1.0
						mnf_data.proprietary_data[20] = 0x00; // HW Version [LSB]
						mnf_data.proprietary_data[21] = 0x01; // HW Version [MSB]	  => HW verion 1.0
						mnf_data.proprietary_data[22] = 0x00; // RFU
						mnf_data.proprietary_data[23] = 0x00; // RFU
						
						break;
					}

					case DEVELOPER_SPECIFIC: // Payload Type: 0xFF
					{
				    #if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
		         uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
			       arch_printf("\n\r[%08d us] 5.Payload Type: 0xFF Developer Specific received", current_sys_tick);
				    #endif

            // Prepare manufacturer specific data general part
						mnf_data.proprietary_data[0] = 0xF2; // Quuppa Packet ID: 0xF2 = RSP Packet;
						mnf_data.proprietary_data[1] = report_data[8]; // Header containing the REQ/RSP Pair ID
						mnf_data.proprietary_data[2] = own_bd_addr[5]; // Quuppa Tag ID [MSB]	
						mnf_data.proprietary_data[3] = own_bd_addr[4]; // Quuppa Tag ID
						mnf_data.proprietary_data[4] = own_bd_addr[3]; // Quuppa Tag ID
						mnf_data.proprietary_data[5] = own_bd_addr[2]; // Quuppa Tag ID
						mnf_data.proprietary_data[6] = own_bd_addr[1]; // Quuppa Tag ID		
						mnf_data.proprietary_data[7] = own_bd_addr[0]; // Quuppa Tag ID [LSB]
						mnf_data.proprietary_data[8] = 0xFF;  // Payload Type: 0xFF Developer Specific 
						mnf_data.proprietary_data[9] =  DEVELOPER_ID & 0xFF; // Developer ID [LSB]
						mnf_data.proprietary_data[10] = (DEVELOPER_ID >> 8 )& 0xFF; // Developer ID [MSB]  

            // Check which command has been received
		        switch(report_data[18])
						{
    					case 0x01:
    					{
                // Execute command
                toggle_led();
								#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
		              uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
			            arch_printf("\n\r[%08d us] 5.Command LED Toggled executed", current_sys_tick);
			          #endif
                // Prepare the rest of the payload
								mnf_data.proprietary_data[11] = 0x01;      // Command type: 0x01 = Toggle LED
								mnf_data.proprietary_data[12] = led_state; // Report the current status of LED
								mnf_data.proprietary_data[13] = 0x00; // Developer Data 2
								mnf_data.proprietary_data[14] = 0x00; // Developer Data 3
								mnf_data.proprietary_data[15] = 0x00; // Developer Data 4		
								mnf_data.proprietary_data[16] = 0x00; // Developer Data 5
								mnf_data.proprietary_data[17] = 0x00; // Developer Data 6
								mnf_data.proprietary_data[18] = 0x00; // Developer Data 7
								mnf_data.proprietary_data[19] = 0x00; // Developer Data 8	
								mnf_data.proprietary_data[20] = 0x00; // Developer Data 9
								mnf_data.proprietary_data[21] = 0x00; // Developer Data 10
								mnf_data.proprietary_data[22] = 0x00; // Developer Data 11
								mnf_data.proprietary_data[23] = 0x00; // Developer Data 12

								break;
							}

							default: //RFU
							{
								mnf_data.proprietary_data[11] = 0x00; // Developer Data 0
								mnf_data.proprietary_data[12] = 0x00; // Developer Data 1
								mnf_data.proprietary_data[13] = 0x00; // Developer Data 2
								mnf_data.proprietary_data[14] = 0x00; // Developer Data 3
								mnf_data.proprietary_data[15] = 0x00; // Developer Data 4		
								mnf_data.proprietary_data[16] = 0x00; // Developer Data 5
								mnf_data.proprietary_data[17] = 0x00; // Developer Data 6
								mnf_data.proprietary_data[18] = 0x00; // Developer Data 7
								mnf_data.proprietary_data[19] = 0x00; // Developer Data 8	
								mnf_data.proprietary_data[20] = 0x00; // Developer Data 9
								mnf_data.proprietary_data[21] = 0x00; // Developer Data 10
								mnf_data.proprietary_data[22] = 0x00; // Developer Data 11
								mnf_data.proprietary_data[23] = 0x00; // Developer Data 12
								
								break;
							}

						}
						break; // DEVELOPER_SPECIFIC
					}					

					default: //UNSUPPORTED_REQ
					{
				    #if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
              uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	            arch_printf("\n\r[%08d us] 5.Payload Type: 0x00 Unsupported REQ received", current_sys_tick);
				    #endif

						mnf_data.proprietary_data[0] = 0xF2; // Quuppa Packet ID: 0xF2 = RSP Packet;
						mnf_data.proprietary_data[1] = report_data[8]; // Header containing the REQ/RSP Pair ID
						mnf_data.proprietary_data[2] = own_bd_addr[5]; // Quuppa Tag ID [MSB]	
						mnf_data.proprietary_data[3] = own_bd_addr[4]; // Quuppa Tag ID
						mnf_data.proprietary_data[4] = own_bd_addr[3]; // Quuppa Tag ID
						mnf_data.proprietary_data[5] = own_bd_addr[2]; // Quuppa Tag ID
						mnf_data.proprietary_data[6] = own_bd_addr[1]; // Quuppa Tag ID		
						mnf_data.proprietary_data[7] = own_bd_addr[0]; // Quuppa Tag ID [LSB]
						mnf_data.proprietary_data[8] = 0x00;  // Payload Type: 0x00 Unsupported REQ 
						mnf_data.proprietary_data[9] = 0x5B;  // Back Channel Spec Number minor: 91 
						mnf_data.proprietary_data[10] = 0x00; // Back Channel Spec Number major: 00 
						mnf_data.proprietary_data[11] = DEVELOPER_ID & 0xFF; // Developer ID [LSB]
						mnf_data.proprietary_data[12] = (DEVELOPER_ID >> 8 )& 0xFF; // Developer ID [MSB] 
						mnf_data.proprietary_data[13] = 0x00; // Developer Data 1
						mnf_data.proprietary_data[14] = 0x00; // Developer Data 2
						mnf_data.proprietary_data[15] = 0x00; // Developer Data 3		
						mnf_data.proprietary_data[16] = 0x00; // Developer Data 4
						mnf_data.proprietary_data[17] = 0x00; // Developer Data 5
						mnf_data.proprietary_data[18] = 0x00; // Developer Data 6
						mnf_data.proprietary_data[19] = 0x00; // Developer Data 7	
						mnf_data.proprietary_data[20] = 0x00; // Developer Data 8
						mnf_data.proprietary_data[21] = 0x00; // Developer Data 9
						mnf_data.proprietary_data[22] = 0x00; // Developer Data 10
						mnf_data.proprietary_data[23] = 0x00; // Developer Data 11
						
						break;
					}
        }	// switch(report_data[15])			
				#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
					current_sys_tick = SYSTICK_PERIOD_US - systick_value();
					arch_printf("\n\r[%08d us] 5.Stop Scan", current_sys_tick);
				#endif
				user_ble_gap_stop_ongoing_operation();	
			} //if(!memcmp(temp_report_data + 9, reverse_bd_addr, 6))
		} //if(!memcmp(temp_report_data + 4, user_man_comp_f1_id, 4))
}

/**
 ****************************************************************************************
 * @brief Callback from SDK when disconnect occurs.
 * @param[in] reason reason for disconnect BT SIG reason defined in co_error.h
 * @return void
 ****************************************************************************************
 */
void user_on_scanning_completed(uint8_t reason)
{
			#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
         uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
	       arch_printf("\n\r[%08d us] 6.Scanning completed", current_sys_tick);
			#endif

	if (adv_state != SLEEP)
		{
			if(reason == GAP_ERR_CANCELED)
				{
					#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
						 uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
						 arch_printf("\n\r[%08d us] 6.Scanning cancelled", current_sys_tick);
					#endif
		 
					if (user_scan_cancel_timer != EASY_TIMER_INVALID_TIMER)
						{
								app_easy_timer_cancel(user_scan_cancel_timer);
								user_scan_cancel_timer = EASY_TIMER_INVALID_TIMER;
						}

					if (req_found == false)
						{
						 #if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
							 uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
							 arch_printf("\n\r[%08d us] 6.No REQ packet received", current_sys_tick);
						 #endif

						 user_change_adv_state(DEV_INFO);
						}
					if (req_found == true)
						{
						 #if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
							 uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
							 arch_printf("\n\r[%08d us] 6.REQ packet received, start sending RSP", current_sys_tick);
						 #endif
						 user_change_adv_state(RSP_ADVERTISING);
						 req_found = false;
						}
					user_app_adv_start();
				}
				else
				{
					#if (defined CFG_PRINTF && SHOW_ALL_MESSAGES == 1)
						uint32_t current_sys_tick = SYSTICK_PERIOD_US - systick_value();
						arch_printf("\n\r[%08d us] 6.Scan Error", current_sys_tick);
						arch_printf("\n\r                 %s: ERR: reason: %d", __func__, reason);
		//			arch_puts("\n\rscanning completed, start advertising");
					#endif
				}
			}
}

static void _user_inertial_sensor_int_key_handler(bool pressed)
{
    // If an interrupt from the sensor occurs, go and read the interrupt reason
    // (what gesture is detected)
    if(pressed) {
        _user_inertial_handle_interrupt();
    }
}

#ifdef HAS_WKUP_KEYS
void user_wkup_keys_notification_cb(enum wkup_keys key, bool pressed)
{
    switch(key)
    {
        case PUSH_BUTTON_1_KEY:
            _user_pb1_key_handler(pressed);
            break;
            
        case INERTIAL_SENSOR_INTERRUPT_KEY:
            _user_inertial_sensor_int_key_handler(pressed);
            break;
            
        default:
            ASSERT_ERROR(0);
    }
}
#endif
/// @} APP
