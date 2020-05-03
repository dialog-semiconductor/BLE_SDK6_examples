/**
 ****************************************************************************************
 *
 * @file user_gamepad.c
 *
 * @brief Gamepad control implementation source code.
 *
 * Copyright (c) 2015-2020 Dialog Semiconductor. All rights reserved.
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
 
 /*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
 #include "app_hogpd.h"
 #include "app_hogpd_task.h"
 #include "user_gamepad.h"
 #include "user_hogpd_config.h"
 #include "wkupct_quadec.h"
 #include "arch_console.h"
 #include "app_entry_point.h"
 #include "adc.h"
 
 
 /*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
 
 struct gamepad_status_t gamepad_status  						__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 struct gamepad_digitizer_report_t gamepad_digitizer_report  	__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 uint32_t raw_lx,raw_ly,raw_rx,raw_ry																	__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 struct gamepad_digitizer_map_joystick_t LS_config,RS_config      __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 uint8_t available_index														__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 uint32_t sel_pins																	__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 uint32_t pol_pins																	__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 bool    axis_polling_on														__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 timer_hnd axis_update_timer_used										__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
 

 
 /*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * Register button to GPIO and configure active edge 
 ****************************************************************************************
 */
void user_gamepad_register_button(button_map_t button_index, uint8_t port, uint8_t pin, uint8_t polarity){
	gamepad_status.buttons[button_index].valid = true;
	gamepad_status.buttons[button_index].port = port;
	gamepad_status.buttons[button_index].pin = pin;
	gamepad_status.buttons[button_index].polarity = polarity;
}

/**
 ****************************************************************************************
 * Button hardware registration
 ****************************************************************************************
 */
void user_gamepad_button_config(void){
	user_gamepad_register_button(BUTTON_STA, GAMEPAD_BUTTON_STA_PORT, GAMEPAD_BUTTON_STA_PIN, WKUPCT_PIN_POLARITY_LOW);
	user_gamepad_register_button(BUTTON_SEL, GAMEPAD_BUTTON_SEL_PORT, GAMEPAD_BUTTON_SEL_PIN, WKUPCT_PIN_POLARITY_LOW);
	user_gamepad_register_button(BUTTON_A, GAMEPAD_BUTTON_A_PORT, GAMEPAD_BUTTON_A_PIN, WKUPCT_PIN_POLARITY_HIGH);
	user_gamepad_register_button(BUTTON_B, GAMEPAD_BUTTON_B_PORT, GAMEPAD_BUTTON_B_PIN, WKUPCT_PIN_POLARITY_HIGH);
}


/**
 ****************************************************************************************
 * Initialize gamepad buttons
 ****************************************************************************************
 */
void user_gamepad_init(void){
	memset(&gamepad_status, 0, sizeof(gamepad_status));
	user_gamepad_button_config();
	user_gamepad_config_digitizer();
	user_gamepad_enable_buttons();
}


/**
 ****************************************************************************************
 * Sending digitizer reports
 ****************************************************************************************
 */
void user_gamepad_send_report(void){
	//arch_printf("%x %x %x %x\r\n", gamepad_report.is_pressed, gamepad_report.mt_index, gamepad_report.x, gamepad_report.y);
	app_hogpd_send_report(gamepad_digitizer_report.mt_index + 1, (uint8_t *)&gamepad_digitizer_report, sizeof(gamepad_digitizer_report), HOGPD_REPORT);
}

/**
 ****************************************************************************************
 * Get available slot in the multitouc index
 ****************************************************************************************
 */
uint8_t user_get_avlb_mt_index(void){
	for(int i = 0; i< MAX_MULTITOUCH; i++){
		if(!(gamepad_status.mt_state&(1UL << i))) return i;
	}
	return 0xFF;
}

/**
 ****************************************************************************************
 * Register usage of a multitouch index, mark corresponding bit as 1
 ****************************************************************************************
 */
bool user_add_mt_index(void){
	for(int i = 0; i< MAX_MULTITOUCH; i++){
		if(!(gamepad_status.mt_state&(1UL << i))){
			gamepad_status.mt_state |= (1UL << i);
			return true;
		}
	}
	return false;
}

/**
 ****************************************************************************************
 * Remove the mark for multitouch index usage
 ****************************************************************************************
 */
void user_remove_mt_index(uint8_t index){
	gamepad_status.mt_state &= ~(1UL << index);
}
#if CFG_USE_DIGITIZER
Remove the mark for multitouch index usage
/**
 ****************************************************************************************
 * Button push handling and HID report for iOS device as digitizer
 ****************************************************************************************
 */
void user_gamepad_button_cb(void){
		wkupct_disable_irq();
		for(int i = 0; i< BUTTON_COUNT; i++){
			if(gamepad_status.buttons[i].valid){
				if((uint8_t)GPIO_GetPinStatus(gamepad_status.buttons[i].port, gamepad_status.buttons[i].pin) != gamepad_status.buttons[i].polarity){
					if(!gamepad_status.buttons[i].is_pressed){
						gamepad_digitizer_report.is_pressed = BUTTON_PRESSED;
						gamepad_digitizer_report.mt_index = available_index;
						gamepad_digitizer_report.x = gamepad_status.buttons[i].digitizer_map.x;
						gamepad_digitizer_report.y = gamepad_status.buttons[i].digitizer_map.y;
						//update button state
						gamepad_status.buttons[i].is_pressed = true;
						gamepad_status.buttons[i].mt_index = available_index;
						user_add_mt_index();
						available_index = user_get_avlb_mt_index();
					}
				}
				else{
					//arch_printf("Btn %d up\r\n", i);
					if(gamepad_status.buttons[i].is_pressed){
						gamepad_digitizer_report.is_pressed = BUTTON_RELEASED;
						gamepad_digitizer_report.mt_index = gamepad_status.buttons[i].mt_index;
						gamepad_digitizer_report.x = gamepad_status.buttons[i].digitizer_map.x;
						gamepad_digitizer_report.y = gamepad_status.buttons[i].digitizer_map.y;
						
						gamepad_status.buttons[i].is_pressed = false;
						user_remove_mt_index(gamepad_status.buttons[i].mt_index);
						gamepad_status.buttons[i].mt_index = 0;
						available_index = user_get_avlb_mt_index();
					}
				}
			}
		}
		user_gamepad_send_report();
		user_gamepad_enable_buttons();
}
#else

/**
 ****************************************************************************************
 * Button push handling and HID report as standard gamepad
 ****************************************************************************************
 */
void user_gamepad_button_cb(void){
		wkupct_disable_irq();
		uint16_t button_report_hword = 0;
		for(int i = 0; i< BUTTON_COUNT; i++){
			if(gamepad_status.buttons[i].valid){
				if((uint8_t)GPIO_GetPinStatus(gamepad_status.buttons[i].port, gamepad_status.buttons[i].pin) != gamepad_status.buttons[i].polarity){
					if(!gamepad_status.buttons[i].is_pressed){
						gamepad_digitizer_report.is_pressed = BUTTON_PRESSED;
						//update button state
						gamepad_status.buttons[i].is_pressed = true;
						button_report_hword|=(1UL << i);
					}
				}
				else{
					//arch_printf("Btn %d up\r\n", i);
					if(gamepad_status.buttons[i].is_pressed){
						gamepad_digitizer_report.is_pressed = BUTTON_RELEASED;
						gamepad_status.buttons[i].is_pressed = false;
						button_report_hword&= ~(1UL << i);
					}
				}
			}
			
		}
		app_hogpd_send_report(HID_GAMEPAD_BUTTONS_REPORT_IDX, (uint8_t *)&button_report_hword, HID_GAMEPAD_BUTTONS_REPORT_SIZE, HOGPD_REPORT);
		user_gamepad_enable_buttons();
}
#endif

/**
 ****************************************************************************************
 * Enable GPIO IRQ
 ****************************************************************************************
 */
void user_gamepad_enable_buttons(void){
		sel_pins = 0;
		pol_pins = 0;
		for(int i = 0; i< BUTTON_COUNT; i++){
			if(gamepad_status.buttons[i].valid){
				sel_pins |= WKUPCT_PIN_SELECT(gamepad_status.buttons[i].port,gamepad_status.buttons[i].pin);
				pol_pins |= WKUPCT_PIN_POLARITY(gamepad_status.buttons[i].port,gamepad_status.buttons[i].pin,(uint8_t)GPIO_GetPinStatus(gamepad_status.buttons[i].port, gamepad_status.buttons[i].pin));
			}
		}
		
		wkupct_enable_irq(sel_pins, pol_pins,1,10); // debouncing time = 10
		
		wkupct_register_callback(user_gamepad_button_cb);
}

/**
 ****************************************************************************************
 * Set screen location for digitizer button
 ****************************************************************************************
 */
void user_gamepad_set_button_location(button_map_t button_index, struct gamepad_digitizer_map_button_t config){
	if(gamepad_status.buttons[button_index].valid){
		gamepad_status.buttons[button_index].digitizer_map.x = (uint16_t)((uint32_t)config.x*DIGITIZER_MAX_RANGE/100);
		gamepad_status.buttons[button_index].digitizer_map.y = (uint16_t)((uint32_t)config.y*DIGITIZER_MAX_RANGE/100);
	}
}

/**
 ****************************************************************************************
 * Set configs for joysticks, used only for digitizer
 ****************************************************************************************
 */
void user_gamepad_config_joystick(struct gamepad_digitizer_map_joystick_t* config, struct gamepad_digitizer_map_joystick_t params){
	*config = params;
}

/**
 ****************************************************************************************
 * Config key mapping for digitizer
 ****************************************************************************************
 */
void user_gamepad_config_digitizer(void){
	user_gamepad_set_button_location(BUTTON_STA, DEFAULT_BUTTON_STA);
	user_gamepad_set_button_location(BUTTON_SEL, DEFAULT_BUTTON_SEL);
	user_gamepad_set_button_location(BUTTON_A, DEFAULT_BUTTON_A);
	user_gamepad_set_button_location(BUTTON_B, DEFAULT_BUTTON_B);
	
	user_gamepad_config_joystick(&LS_config, DEFAULT_LS_CONFIG);
	user_gamepad_config_joystick(&RS_config, DEFAULT_RS_CONFIG);
}

/**
 ****************************************************************************************
 * @brief Introduces a variable microsend delay for use with ADC peripheral.
 * @param[in] nof_us Number of microseconds to delay
 * @return void
 ****************************************************************************************
 */
void user_usDelay(uint32_t nof_us)
{
    while( nof_us-- ){
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
    }
}

/**
 ****************************************************************************************
 * @brief Gets ADC sample using the 20 usec delay.
 * @param[in] sample_vbat1v :true = sample VBAT1V, false = sample VBAT3V
 * @return ADC VBAT1V or VBAT3V sample
 ****************************************************************************************
 */
uint32_t adc_get_raw_sample(adc_input_se_t channel)
{
    uint32_t adc_sample, adc_sample2;
    adc_config_t cfg =
    {
        .mode = ADC_INPUT_MODE_SINGLE_ENDED,
        .sign = true,
        .attn = true
    };

    adc_init(&cfg);
    user_usDelay(20);

    adc_set_se_input(channel);

    adc_sample = adc_get_sample();
    user_usDelay(1);

    cfg.sign = false;
    adc_init(&cfg);
    adc_set_se_input(channel);

    adc_sample2 = adc_get_sample();
    // We have to divide the following result by 2 if the 10-bit accuracy is
    // enough
    adc_sample = (adc_sample2 + adc_sample);
    adc_disable();

    return adc_sample;
}

/**
 ****************************************************************************************
 * Check joystick tilt. Return true if it's out of deadzone.
 ****************************************************************************************
 */
bool deadzone_check(uint32_t raw_x, uint32_t raw_y){
	int r_x = (raw_x >= ADC_SAMPLE_MAX/2)? raw_x - ADC_SAMPLE_MAX/2:ADC_SAMPLE_MAX/2 - raw_x;
	int r_y = (raw_y >= ADC_SAMPLE_MAX/2)? raw_y - ADC_SAMPLE_MAX/2:ADC_SAMPLE_MAX/2 - raw_y;
	return (r_x*r_x + r_y*r_y >= (ADC_SAMPLE_MAX*R_DEADZONE/100)*(ADC_SAMPLE_MAX*R_DEADZONE/100));
}

#if CFG_USE_DIGITIZER

/**
 ****************************************************************************************
 * Joystick location calculate and report sending for digitizer
 ****************************************************************************************
 */
void user_gamepad_update_joystick(void){
  double screen_height = SCREEN_RATIO_H;
	double screen_width = SCREEN_RATIO_W;
	//arch_printf("%d %d\r\n",raw_lx,raw_ly);
	//arch_printf("%d\r\n", deadzone_check(raw_lx, raw_ly));
	//left stick
	int r_lx = gamepad_status.ls.raw_x - ADC_SAMPLE_MAX/2;
	int r_ly = gamepad_status.ls.raw_y - ADC_SAMPLE_MAX/2;
	int r_rx = gamepad_status.rs.raw_x - ADC_SAMPLE_MAX/2;
	int r_ry = gamepad_status.rs.raw_y - ADC_SAMPLE_MAX/2;
	if(deadzone_check(gamepad_status.ls.raw_x,gamepad_status.ls.raw_y)){
		if(!gamepad_status.ls.activated){
			gamepad_status.ls.mt_index = available_index;
			user_add_mt_index();
			available_index = user_get_avlb_mt_index();
			gamepad_status.ls.activated = true;
		}
		gamepad_status.ls.x = (uint16_t)(LS_config.center_x + (int)(LS_config.radius*r_lx/(ADC_SAMPLE_MAX/2)*screen_height/screen_width))*DIGITIZER_MAX_RANGE/100;
		gamepad_status.ls.y = (uint16_t)(LS_config.center_y + (int)(LS_config.radius*r_ly/(ADC_SAMPLE_MAX/2)))*DIGITIZER_MAX_RANGE/100;
		
		gamepad_digitizer_report.is_pressed = BUTTON_PRESSED;
		gamepad_digitizer_report.mt_index = gamepad_status.ls.mt_index;
		gamepad_digitizer_report.x = gamepad_status.ls.x;
		gamepad_digitizer_report.y = gamepad_status.ls.y;
		
		user_gamepad_send_report();
	}
	else{
		if(gamepad_status.ls.activated){
			gamepad_digitizer_report.is_pressed = BUTTON_RELEASED;
			gamepad_digitizer_report.mt_index = gamepad_status.ls.mt_index;
			gamepad_digitizer_report.x = gamepad_status.ls.x;
			gamepad_digitizer_report.y = gamepad_status.ls.y;
		
			user_gamepad_send_report();
			
			user_remove_mt_index(gamepad_status.ls.mt_index);
			gamepad_status.ls.mt_index = 0;
			gamepad_status.ls.activated = false;
			available_index = user_get_avlb_mt_index();
		}
	}
	//Right stick
	if(deadzone_check(gamepad_status.rs.raw_x,gamepad_status.rs.raw_y)){
		if(!gamepad_status.rs.activated){
			gamepad_status.rs.mt_index = available_index;
			user_add_mt_index();
			available_index = user_get_avlb_mt_index();
			gamepad_status.rs.activated = true;
			gamepad_status.rs.x = RS_config.center_x*DIGITIZER_MAX_RANGE/100;
			gamepad_status.rs.y = RS_config.center_y*DIGITIZER_MAX_RANGE/100;
		}
		gamepad_status.rs.x += (int)(r_rx*screen_height*RS_config.velocity/screen_width/(ADC_SAMPLE_MAX/2));
		gamepad_status.rs.y += (int)(r_ry*RS_config.velocity/(ADC_SAMPLE_MAX/2));
		
		gamepad_digitizer_report.is_pressed = BUTTON_PRESSED;
		gamepad_digitizer_report.mt_index = gamepad_status.rs.mt_index;
		gamepad_digitizer_report.x = gamepad_status.rs.x;
		gamepad_digitizer_report.y = gamepad_status.rs.y;
		
		user_gamepad_send_report();
	}
	else{
		if(gamepad_status.rs.activated){
			gamepad_digitizer_report.is_pressed = BUTTON_RELEASED;
			gamepad_digitizer_report.mt_index = gamepad_status.rs.mt_index;
			gamepad_digitizer_report.x = gamepad_status.rs.x;
			gamepad_digitizer_report.y = gamepad_status.rs.y;
		
			user_gamepad_send_report();
			
			user_remove_mt_index(gamepad_status.rs.mt_index);
			gamepad_status.rs.mt_index = 0;
			gamepad_status.rs.activated = false;
			available_index = user_get_avlb_mt_index();
		}
	}
}
#else

/**
 ****************************************************************************************
 * Convert raw ADC reading to a 0 - 255 level value
 ****************************************************************************************
 */
uint8_t user_sample_conv(uint16_t input, uint16_t cap){
	input = (cap<input)?cap:input;
	return 0xFF&(input*255/cap);
}

/**
 ****************************************************************************************
 * Joystick location calculate and report sending for gamepad
 ****************************************************************************************
 */
void user_gamepad_update_joystick(void){
	uint8_t axis_values[HID_GAMEPAD_AXIS_REPORT_SIZE];
		axis_values[0] = user_sample_conv(gamepad_status.ls.raw_x,ADC_SAMPLE_MAX);
		axis_values[1] = user_sample_conv(gamepad_status.ls.raw_y,ADC_SAMPLE_MAX);
		axis_values[2] = user_sample_conv(gamepad_status.rs.raw_x,ADC_SAMPLE_MAX);
		axis_values[3] = user_sample_conv(gamepad_status.rs.raw_y,ADC_SAMPLE_MAX);
	if(deadzone_check(gamepad_status.ls.raw_x,gamepad_status.ls.raw_y)|deadzone_check(gamepad_status.rs.raw_x,gamepad_status.rs.raw_y)){
		gamepad_status.rs.activated = true;
		app_hogpd_send_report(HID_GAMEPAD_AXIS_REPORT_IDX, (uint8_t*)axis_values, HID_GAMEPAD_AXIS_REPORT_SIZE, HOGPD_REPORT);
	}
	else{
		if(gamepad_status.rs.activated){
			memset(axis_values, 0x7F,sizeof(axis_values));
			app_hogpd_send_report(HID_GAMEPAD_AXIS_REPORT_IDX, (uint8_t*)axis_values, HID_GAMEPAD_AXIS_REPORT_SIZE, HOGPD_REPORT);
			gamepad_status.rs.activated = false;
		}
	}
}
#endif

/**
 ****************************************************************************************
 * Periodically ADC value polling and joystick update callback
 ****************************************************************************************
 */
void user_gamepad_axis_polling_cb(void){
	gamepad_status.ls.raw_x = adc_get_raw_sample(ADC_INPUT_SE_P0_0);
	gamepad_status.ls.raw_y = adc_get_raw_sample(ADC_INPUT_SE_P0_1);
	gamepad_status.rs.raw_x = adc_get_raw_sample(ADC_INPUT_SE_P0_2);
	gamepad_status.rs.raw_y = adc_get_raw_sample(ADC_INPUT_SE_P0_3);
	user_gamepad_update_joystick();
	if(axis_polling_on)
		axis_update_timer_used = app_easy_timer(AXIS_UPDATE_PER, user_gamepad_axis_polling_cb);
}


/**
 ****************************************************************************************
 * Toggle the ADC reading cycle, enbale the joysticks control
 ****************************************************************************************
 */
void user_gamepad_toggle_axis_polling(bool on){
	if(axis_polling_on!=on){
		if(on){
			axis_polling_on = on;
			user_gamepad_axis_polling_cb();
		}
		else{
			axis_polling_on = on;
			axis_update_timer_used = EASY_TIMER_INVALID_TIMER;
		}
	}
}

/**
 ****************************************************************************************
 * HID event handler entrance
 ****************************************************************************************
 */
void app_hid_gamepad_event_handler(ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
#if BLE_HID_DEVICE
    //enum ke_msg_status_tag result;
    app_hogpd_process_handler(msgid, param, dest_id, src_id);
#endif
}
