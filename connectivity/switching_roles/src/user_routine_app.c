/**
 ****************************************************************************************
 *
 * @file .c
 *
 * @brief Example project on how to switch advertising modes and scanning modes 
 * project source code.
 *
 * Copyright (C) 2012-2022 Renesas Electronics Corporation and/or its affiliates
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


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "arch_console.h"
#include "user_ble_gap.h"
#include "user_routine_app.h"
#include "user_routine_config.h"
#include "gpio.h"
#include "user_periph_setup.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */



static led_cb_params_t led_cb_params = {
	.isNewCbRoutine = false,
	.pulseTime = 0,
	.amountOfPulses = 0
};

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
*/
#if defined(CFG_ENABLE_LED)
void led_cycle_start(uint8_t, uint32_t);
void led_cycle_cb(void);
void toggle_led(void);
void turn_led_off_cb(void);
#endif

void start_bluetooth_routine(void);

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param){
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param ){
    default_app_on_disconnect(param);
}

void start_bluetooth_routine(void){
	#if defined(CFG_PRINTF)
	arch_puts("\n\rStart bluetooth routine\n\r");
	arch_puts("Nonconnectable mode\n\r");
#endif

#if defined(CFG_ENABLE_LED)
	led_cycle_start(LED_NONCONN_PULSES, LED_NONCONN_TIMER_TIME);
#endif
	app_easy_gap_non_connectable_advertise_start();
	set_routine_timer();
}

void user_on_adv_nonconn_complete(const uint8_t error){
#if defined(CFG_PRINTF)
	arch_puts("Connectable mode\n\r");
#endif
	
	
#if defined(CFG_ENABLE_LED)
	led_cycle_start(LED_CONN_PULSES, LED_CONN_TIMER_TIME);
#endif
	app_easy_gap_undirected_advertise_start();
	set_routine_timer();
}

void user_advertise_operation(void){
	start_bluetooth_routine();
}

void user_on_adv_undirect_complete(const uint8_t error){
#if defined(CFG_PRINTF)
	arch_puts("Scanning mode\n\rFound Devices:\n\r");
#endif
	
#if defined(CFG_ENABLE_LED)
	led_cycle_start(LED_SCAN_PULSES, LED_SCAN_TIMER_TIME);
#endif

	user_ble_gap_start_scan(false, 50, 50, true); //TODO: remove these magic numbers
	set_routine_timer();
}

void user_on_scanning_completed(const uint8_t error){
	if (error == GAP_ERR_CANCELED){
#if defined(CFG_PRINTF)
		arch_puts("Pause...\n\r");
#endif 	
		app_easy_timer(ROUTINE_PAUSE_TIME, start_bluetooth_routine);
	}else{
#if defined(CFG_PRINTF)
		arch_printf("Error code : %02x\n\r", error);
#endif
	}
}

void user_on_adv_report_ind(struct gapm_adv_report_ind const * report){
#if defined(CFG_PRINTF)
	arch_puts("-------------------------------------------------\n\r");
	//print local name
	arch_printf("Device name:%s\n\r",
		format_local_name(report->report.data, report->report.data_len)
	);
	//print bluetooth addr
	arch_printf("Device address: %s\n\r", format_bd_address(&report->report.adv_addr));
	//print rssi
	arch_printf("RSSI:%d\n\r", 255 - report->report.rssi);
	arch_puts("-------------------------------------------------\n\r");
#endif
	
#if defined(CFG_ENABLE_LED)
	GPIO_SetActive(LED_PIN_PORT, LED_PIN);
	app_easy_timer(LED_RECEIVED_TIMER_TIME, turn_led_off_cb);
#endif
}

#if defined(CFG_ENABLE_LED)
void led_cycle_start(uint8_t amountOfPulses, uint32_t pulseTime){
	led_cb_params.isNewCbRoutine = true;
	led_cb_params.amountOfPulses = CONVERT_TO_CB_AMOUNT(amountOfPulses);
	led_cb_params.pulseTime = pulseTime;
	
	led_cycle_cb();
}

void led_cycle_cb(void){
	static uint8_t callbacksLeft = 0;
	
	if (led_cb_params.isNewCbRoutine){
		callbacksLeft = led_cb_params.amountOfPulses;
		led_cb_params.isNewCbRoutine = false;
	}
	
	if (callbacksLeft > 0){
		toggle_led();
		callbacksLeft--;
		app_easy_timer(led_cb_params.pulseTime, led_cycle_cb);
	}
	
	if (callbacksLeft == 0) {
		GPIO_SetInactive(LED_PIN_PORT, LED_PIN);
	}	
}
void toggle_led(void){
	if (GPIO_GetPinStatus(LED_PIN_PORT, LED_PIN)){
		GPIO_SetInactive(LED_PIN_PORT, LED_PIN);
	}else{
		GPIO_SetActive(LED_PIN_PORT, LED_PIN);
	}
}
void turn_led_off_cb(void){
	GPIO_SetInactive(LED_PIN_PORT, LED_PIN);
}
#endif
/// @} APP
