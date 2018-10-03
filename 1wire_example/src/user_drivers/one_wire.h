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
 
#include "co_bt.h"
#include "gapc_task.h"                 // gap functions and messages
#include "app_task.h"                  // application task
#include "app.h"                       // application definitions
#include "app_callback.h"
#include "systick.h"

#define MAX_NO_SENSORS 10

struct OneWire_sensor
{
		uint8_t 	sensor_number;
		uint32_t 	address_low;
		uint32_t	address_high;
		uint16_t 	temperature;
		uint32_t 	scratchpad_low;
		uint32_t  scratchpad_high;
};

// OneWire transmission functions
void OneWire_readSlot(systick_callback_function_t systick_callback);
void OneWire_write_1(void);
void OneWire_write_0(void);
void OneWire_write_byte(uint8_t byte);
void OneWire_reset(void);
void OneWire_presence(void);
void OneWire_init(void);

// OneWire Search ROM algorithm functions
void OneWire_SearchROM(void);

// Function commands
void OneWire_readScratchpad(void);
void OneWire_ConvertT(void);

// ROM commands
void OneWire_readRom(void);
void OneWire_matchRom(int sensor_number);

// Uart Print functions
void print_scratchpad(void);
void print_address(void);
void print_temperature(void);
