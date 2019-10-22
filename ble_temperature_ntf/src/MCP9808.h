/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.h
 *
 * @brief Peripheral project Custom1 Server implementation header file.
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

#ifndef _MCP9808_H_
#define _MCP9808_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 #include "da1458x_config_basic.h"
 #if !defined(CFG_USE_INTERNAL_TEMP_SENSOR)
#include "stdint.h"

/*
 * USER TYPE DEFINITIONS
 ****************************************************************************************
 */
typedef enum{ HALF_RESOLUTION = 0,
                QUATER_RESOLUTION,
				EIGHT_RESOLUTION,
				SIXTEENTH_RESOLUTION} temperature_resolution; //The higher the resolution the longer the convursion time, for SIXTEENTH_RESOLUTION it is 250ms.
																														//Look in the datasheet of the MCP9808 Temperature sensor for the convursion time.
							
/*
 * DEFINES
 ****************************************************************************************
 */
#define TEMPERATURE_DATA_REGISTER		    0x05 									//Register 5 contains 12 bits of temperature data
#define TEMPERATURE_REGISTER_SIZE		    12	 									//In bits. Must be a unsigned integer greater than 0
#define RESOLUTION_CONFIG_REGISTER	        0X08 								 	//3 bits register containing the resolution
#define USED_TEMPERATURE_RESOLUTION	        SIXTEENTH_RESOLUTION 	                //0.0625 degrees Celsius resolution
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void temperature_sensor_init(void);

uint16_t read_MCP9808_temperature_register(void);

double calculate_temperature(const uint16_t);

double get_temperature(void);

#endif //CFG_USE_INTERNAL_TEMP_SENSOR

#endif // _MCP9808_H_
