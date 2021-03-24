/**
 ****************************************************************************************
 *
 * @file user_IR_driver.h
 *
 * @brief User IR encoding header file.
 *
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */



#ifndef _USER_IR_DRIVER_H_
#define _USER_IR_DRIVER_H_

#include <stdint.h>

/*
 * VARIABLE DEFINITIONS
 ****************************************************************************************
*/
extern bool IR_sending_in_progress; //flag for IR transmission status

//PWM structure def
struct pwm_settings
{
    /// High ticks for a PWM cycle
    uint16_t pwm_high;
    /// Low ticks for a PWM cycle
    uint16_t pwm_low;
};
//Protocol index
enum{
	IR_NEC = 1,
	IR_RC5,
	IR_Samsung,
	IR_Panasonic,
};

#define DEFAULT_REPEAT_EVENT 1

//Timer comstant for 1ms delay
#define TIMER_ON_1MS 16000

//RC5 related timing settings
#define TIMER_ON_RC5 13750
#define DELAY_RC5 128

//NEC related timing settings
#define TIMER_ON_NEC 8808
#define DELAY_NEC 192

//Samsung related timing settings
#define TIMER_ON_SAMSUNG 9057
#define DELAY_SAMSUNG 192

//Panasonic related timing settings
#define TIMER_ON_PANASONIC 6912
#define DELAY_PANASONIC 173
/*
 * FUNCTION CLAIM
 ****************************************************************************************
*/

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
void ir_send_sequence(struct pwm_settings freq, uint16_t width, uint8_t length, uint8_t* sequence);

/**
 ****************************************************************************************
 * @brief Adapter to send custom signal from BLE imput command 
 *       
 * @param[in] Command           Pointer to the incoming BLE command
 * @return void
 ****************************************************************************************
 */
void user_custom_send(const uint8_t* command);

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
void user_RC5_send(uint8_t addr, uint8_t command);

/**
 ****************************************************************************************
 * @brief Converts input commands to signal sequence and sends out with Philips NEC protocol
 * @param[in] addr              Address
 * @param[in] command           Command
 * @return void
 ****************************************************************************************
 */
void user_NEC_send(uint8_t addr, uint8_t command);

/**
 ****************************************************************************************
 * @brief Converts input commands to signal sequence and sends out with Samsung protocol
 * @param[in] addr              Address
 * @param[in] command           Command
 * @return void
 ****************************************************************************************
 */
void user_Samsung_send(uint8_t addr, uint8_t command);

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
void user_Panasonic_send(uint8_t D, uint8_t S, uint8_t X, uint8_t F);

#endif

