/**
 ****************************************************************************************
 *
 * @file ds18b20.h
 *
 * @brief DS18B20 temperature sensor(s) driver - headerfile.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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
 
#include "co_bt.h"
#include "gapc_task.h"                 // gap functions and messages
#include "app_task.h"                  // application task
#include "app.h"                       // application definitions
#include "app_callback.h"
#include "systick.h"
#include "user_one_wire.h"

struct temperature_sensor
{
		uint8_t 	sensor_number;
		uint32_t 	address_low;
		uint32_t	address_high;
		uint16_t 	temperature;
		uint32_t 	scratchpad_low;
		uint32_t  scratchpad_high;
};
 
/**
****************************************************************************************
* @brief 	Initiates 1-Wire rom search and stores the DS18B20 least significant 32 bits
					and most significant 32 bits in their corresponding temperature_sensor instance
					and member. 
* @return void
****************************************************************************************
*/
void get_address(void);

/**
****************************************************************************************
* @brief 	Reads scratchpad of all DS18B20 sensors ands stores the contents in the
					corresponding temperature_sensor instance and member.
* @return void
****************************************************************************************
*/
void OneWire_readScratchpad(void);

/**
****************************************************************************************
* @brief 	Issues a convert temperature command (0x44) to all DS18B20 sensors.
* @return void
****************************************************************************************
*/
void OneWire_ConvertT(void);

/**
****************************************************************************************
* @brief 	Issue a match ROM command to target a single DS18B20 sensor.
* @param	the index of the temperature_sensor instance to be targeted
* @return void
****************************************************************************************
*/
void OneWire_matchRom(int sensor_number);

/**
****************************************************************************************
* @brief 	Writes all DS18B20 scratchpad contents to serial terminal over UART
* @return void
****************************************************************************************
*/
void print_scratchpad(void);

/**
****************************************************************************************
* @brief 	Writes all DS18B20 addresses to serial terminal over UART
* @return void
****************************************************************************************
*/
void print_address(void);

/**
****************************************************************************************
* @brief 	Writes all DS18B20 temperature values to serial terminal over UART
* @return void
****************************************************************************************
*/
void print_temperature(void);
