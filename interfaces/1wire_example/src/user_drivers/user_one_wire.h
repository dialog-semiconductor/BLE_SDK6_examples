/**
 ****************************************************************************************
 *
 * @file one_wire.h
 *
 * @brief one_wire application header file.
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
 
#ifndef USER_ONE_WIRE_H
#define USER_ONE_WIRE_H
 
#include "co_bt.h"
#include "gapc_task.h"                 // gap functions and messages
#include "app_task.h"                  // application task
#include "app.h"                       // application definitions
#include "app_callback.h"
#include "systick.h"

#define MAX_NO_SENSORS 10

struct OneWire_sensor
{
		uint32_t 	address_low;
		uint32_t	address_high;
};

/**
****************************************************************************************
* @brief 			Transmits 1.5 µSec pulse initiating master readslot (>70 µSec in total)
* @return 		void
****************************************************************************************
*/
void OneWire_readSlot(systick_callback_function_t systick_callback);

/**
****************************************************************************************
* @brief 			Initiates the OneWirebus. Since data is transmitted using the SPI-
*							controller, it is initialized accordingly. 
* @return 		void
****************************************************************************************
*/
void OneWire_init(void);

/**
****************************************************************************************
* @brief 			Let master wait for (approximately 500µSec) to detect presence pulse. 
* @return 		void
****************************************************************************************
*/
void OneWire_presence(void);

/**
****************************************************************************************
* @brief 			Initiates a resetpulse of approximately 500 µSec.
* @return 		void
****************************************************************************************
*/
void OneWire_reset(void);

/**
****************************************************************************************
* @brief 			Transmit a byte value on the bus (LSB first)
*
*	@param			Byte to be transmitted
*
* @return 		void
****************************************************************************************
*/
void OneWire_write_byte(uint8_t byte);

/**
****************************************************************************************
* @brief 			Transmit a high level bit to the bus (Pulse < 15 µSec) using SPI.
* @return 		void
****************************************************************************************
*/
void OneWire_write_1(void);

/**
****************************************************************************************
* @brief 			Transmit a low level bit to the bus (Pulse > 15 µSec) using SPI.
* @return 		void
****************************************************************************************
*/
void OneWire_write_0(void);

/**
****************************************************************************************
* @brief 	1-Wire search algorithm. Tree search finding all sensor addresses (least
					significant 32 bits and most significant 32 bits) and storing them in 
					OneWire_sensors array. 
* @return void
****************************************************************************************
*/
void OneWire_SearchROM(void);

#endif
