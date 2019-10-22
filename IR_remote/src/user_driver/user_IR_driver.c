/**
 ****************************************************************************************
 *
 * @file user_IR_driver.c
 *
 * @brief User implementation for IR remote protocol encoding and transmission.
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

#include "user_IR_driver.h"
#include "arch_api.h"
#include "timer0.h"
#include "gpio.h"
#include "user_periph_setup.h"

uint8_t ret_sequence[20] __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t ret_repeat_sequence[10] __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t sequence_counter __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t ret_length __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t ret_repeat_length __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t repeat_delay_length __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t repeat_delay_counter __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint8_t repeat_event_counter __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

bool signal_high __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
bool IR_sending_in_progress __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
bool repeat_on __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
bool unique_repeat_signal __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
bool conpensate_delay __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

struct pwm_settings PWM_36K = {147,295};
struct pwm_settings PWM_37K = {144,287};
struct pwm_settings PWM_38K = {138,280};
struct pwm_settings PWM_NULL = {0,0};


/*
 * Basic driver related functions
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Toggle the PWM by reconfigure the pin 
 *  
 * @return void
 ****************************************************************************************
 */
void pwm_toggle_on(void){
	if(!signal_high){
		GPIO_ConfigurePin(PWM_GPIO_PORT, PWM_GPIO_PIN, OUTPUT, PID_PWM0, true);
		signal_high = true;
	}
}

void pwm_toggle_off(void){
	if(signal_high){
		GPIO_ConfigurePin(PWM_GPIO_PORT, PWM_GPIO_PIN, OUTPUT, PID_GPIO, false);
		signal_high = false;
	}
}

/**
 ****************************************************************************************
 * @brief Individually set a bit from given bytes
 * @param[in] bytes             Pointer to the first byte
 * @param[in] bit               Index of the bit to be set
 * @param[in] Value             Desired bit value, 0 or 1 is expected
 * @return void
 ****************************************************************************************
 */
void set_bit(uint8_t* bytes, uint8_t bit, uint8_t value){
	uint8_t byte_index = bit/8;
	uint8_t bit_index = bit%8;
	if(value)
		bytes[byte_index] = bytes[byte_index]|(0x01<<(7-bit_index));
	else
		bytes[byte_index] = bytes[byte_index]&(0xFEFF<<(7-bit_index)); 
}

/**
 ****************************************************************************************
 * @brief Timer callback for sending repeat signal
 * @return void
 ****************************************************************************************
 */
void ir_repeat_cb(void){
	if(unique_repeat_signal){
		ir_send_sequence(PWM_NULL,NULL,ret_repeat_length, ret_repeat_sequence);
	}
	else{
		ir_send_sequence(PWM_NULL,NULL,ret_length, ret_sequence);
	}
}

/**
 ****************************************************************************************
 * @brief Timer callback for the delay before repeat
 * @return void
 ****************************************************************************************
 */
void ir_delay_cb(void){
	if(repeat_delay_counter){
		repeat_delay_counter--;
	}
	else{
		timer0_register_callback(ir_repeat_cb);
	}
}

/**
 ****************************************************************************************
 * @brief Timer callback for the sending procedule
 * @return void
 ****************************************************************************************
 */
void ir_send_sequence_cb(void){
	uint8_t byte_index = sequence_counter/8;
	uint8_t bit_index = sequence_counter%8;
	if(sequence_counter!=ret_length){
		if((ret_sequence[byte_index]>>(7-bit_index))&0x01){
			pwm_toggle_on();
		}
		else{
			pwm_toggle_off();
		}
	}
	else{
		if(!repeat_on&&!repeat_event_counter){
			IR_sending_in_progress = false;
			unique_repeat_signal = false;
			conpensate_delay = false;
			pwm_toggle_off();
			timer0_stop();
			arch_restore_sleep_mode();
		}
		else{
			pwm_toggle_off();
			repeat_event_counter--;
			if(conpensate_delay)
				repeat_delay_counter = repeat_delay_length - ret_length - 1;
			else
				repeat_delay_counter = repeat_delay_length - 1;
			timer0_register_callback(ir_delay_cb);
		}
	}
	sequence_counter++;
}

/**
 ****************************************************************************************
 * @brief Core function to encode and send given bitwise signal sequence with settings 
 *       
 * @param[in] freq              A pair of settings to decide carrier PWM frequency and duty cycle
 * @param[in] width             Duration of the smallest signal block in ticks
 * @param[in] length            Total length of the signal sequence, in bits.     
 * @param[in] sequence					pointer to first byte of the sequence to be send
 * @return void
 ****************************************************************************************
 */
void ir_send_sequence(struct pwm_settings freq, uint16_t width, uint8_t length, uint8_t* sequence){
	arch_force_active_mode();
	sequence_counter = 0;
	ret_length = length;
	uint8_t bytes = length/8;
	if(length%8)
		bytes++;
	memcpy(ret_sequence,sequence,bytes);
	if(width!=NULL)
		timer0_set(width, freq.pwm_high, freq.pwm_low);
	timer0_register_callback(ir_send_sequence_cb);
	timer0_enable_irq();
	IR_sending_in_progress = true;
	timer0_start();
}

/**
 ****************************************************************************************
 * @brief To store special repeat codes
 *       
 * @param[in] length            Total length of the signal sequence, in bits.     
 * @param[in] sequence					pointer to first byte of the sequence to be send
 * @return void
 ****************************************************************************************
 */
void ir_set_repeat_signal(uint8_t length, uint8_t* sequence){
	unique_repeat_signal = true;
	ret_repeat_length = length;
	uint8_t bytes = length/8;
	if(length%8)
		bytes++;
	memcpy(ret_repeat_sequence,sequence,bytes);
}
/**
 ****************************************************************************************
 * @brief Adapter to send custom signal from BLE imput command 
 *       
 * @param[in] Command           Pointer to the incoming BLE command
 * @return void
 ****************************************************************************************
 */
void user_custom_send(const uint8_t* command){
	struct pwm_settings custom_pwm;
	switch(command[1]){
		
		case 0x00:
			custom_pwm = PWM_38K;
			break;
		case 0x01:
			custom_pwm = PWM_36K;
			break;
		case 0x02:
			custom_pwm = PWM_37K;
			break;
		default:
			custom_pwm = PWM_38K;
			break;
	}
	uint16_t width_hw = ((uint16_t)command[2]<<8) + command[3];
	repeat_delay_length = command[4];
	repeat_event_counter = DEFAULT_REPEAT_EVENT;
	ir_send_sequence(custom_pwm, width_hw, command[5], (uint8_t *)&(command[6]));
}

/*
 * Protocols can be added or modified
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Converts input commands to signal sequence and sends out with RC5 protocol
 * @param[in] addr              Address
 * @param[in] command           Command
 * @return void
 ****************************************************************************************
 */
void user_RC5_send(uint8_t addr, uint8_t command){
	uint16_t msg_RC5 = (command&0x3F)|((((uint16_t)addr)&0x1F)<<6)|(0x3000);
	uint8_t sequence[4];
  uint8_t length = 0;
	for(int i = 0; i<14; i++){
		if((msg_RC5>>(13-i))&0x01){
			set_bit(sequence, length, 0);
			length++;
			set_bit(sequence, length, 1);
			length++;	
		}
		else{
			set_bit(sequence, length, 1);
			length++;
			set_bit(sequence, length, 0);
			length++;
		}
	}
	
	//repeat settings
	repeat_delay_length = DELAY_RC5;
	conpensate_delay = true;
	repeat_event_counter = DEFAULT_REPEAT_EVENT;
	
	ir_send_sequence(PWM_36K, TIMER_ON_NEC, length, sequence);		
}


/**
 ****************************************************************************************
 * @brief Converts input commands to signal sequence and sends out with Philips NEC protocol
 * @param[in] addr              Address
 * @param[in] command           Command
 * @return void
 ****************************************************************************************
 */
void user_NEC_send(uint8_t addr, uint8_t command){
	uint32_t msg_NEC = (~command&0xFF)<<24|((uint32_t)command)<<16|((uint32_t)(~addr&0xFF))<<8|((uint32_t)addr); //logical signal sequence
	uint8_t sequence[20]={0xFF,0xFF,0x00};
	uint8_t length = 24;
	
	for(int i = 0; i<32; i++){
		set_bit(sequence, length, 1);
		length++;
		set_bit(sequence, length, 0);
		length++;
		if((msg_NEC>>i)&0x01){
			set_bit(sequence, length, 0);
			length++;
			set_bit(sequence, length, 0);
			length++;
		}	
	}
	set_bit(sequence, length, 1);
	length++;
	
	//repeat settings
	repeat_delay_length = DELAY_NEC;
	conpensate_delay = true;
	repeat_event_counter = DEFAULT_REPEAT_EVENT;
	uint8_t repeat_sequence[] = {0xFF, 0xFF, 0x0F};
	ir_set_repeat_signal(21, repeat_sequence);
	
	ir_send_sequence(PWM_38K, TIMER_ON_NEC, length, sequence);
}

/**
 ****************************************************************************************
 * @brief Converts input commands to signal sequence and sends out with Samsung protocol
 * @param[in] addr              Address
 * @param[in] command           Command
 * @return void
 ****************************************************************************************
 */
void user_Samsung_send(uint8_t addr, uint8_t command){
	uint32_t msg_Samsung = (~command&0xFF)<<24|((uint32_t)command)<<16|((uint32_t)addr)<<8|((uint32_t)addr); //logical signal sequence
	uint8_t sequence[20]={0xFF,0x00};
	uint8_t length = 16;
	for(int i = 0; i<32; i++){
		set_bit(sequence, length, 1);
		length++;
		set_bit(sequence, length, 0);
		length++;
		if((msg_Samsung>>i)&0x01){
			set_bit(sequence, length, 0);
			length++;
			set_bit(sequence, length, 0);
			length++;
		}	
	}
	set_bit(sequence, length, 1);
	length++;
	
	//repeat settings
	repeat_delay_length = DELAY_SAMSUNG;
	conpensate_delay = true;
	repeat_event_counter = DEFAULT_REPEAT_EVENT;
	
	ir_send_sequence(PWM_38K, TIMER_ON_SAMSUNG, length, sequence);
}

/**
 ****************************************************************************************
 * @brief Converts input commands to signal sequence and sends out with Panasonic protocol
 * @param[in] D
 * @param[in] S
 * @param[in] F
 * @param[in] X     4 8bit inputs, definition unknown
 * @return void
 ****************************************************************************************
 */
void user_Panasonic_send(uint8_t D, uint8_t S, uint8_t X, uint8_t F){
	uint32_t msg_Panasonic = ((uint32_t)F)<<24|((uint32_t)X)<<16|((uint32_t)S)<<8|((uint32_t)D); //logical signal sequence
	uint16_t manufacture_code = 0x2002;
	uint8_t checksum = D^S^X^F;
	uint8_t sequence[20]={0xFF,0x00};
	uint8_t length = 12;
	for(int i = 0; i<56; i++){
		set_bit(sequence, length, 1);
		length++;
		set_bit(sequence, length, 0);
		length++;
		if((((manufacture_code>>i)&0x01)&&(i<16))||(((msg_Panasonic>>(i-16))&0x01)&&(i<48&&i>=16))||(((checksum>>(i-48))&0x01)&&(i>=32))){
			set_bit(sequence, length, 0);
			length++;
			set_bit(sequence, length, 0);
			length++;
		}	
	}
	set_bit(sequence, length, 1);
	length++;
	
	//repeat settings
	repeat_delay_length = DELAY_PANASONIC;
	conpensate_delay = false;
	unique_repeat_signal = false;
	repeat_event_counter = DEFAULT_REPEAT_EVENT;
	
	ir_send_sequence(PWM_37K, TIMER_ON_PANASONIC, length, sequence);
}
