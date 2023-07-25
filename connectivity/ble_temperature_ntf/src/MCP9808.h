/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.h
 *
 * @brief Peripheral project Custom1 Server implementation header file.
 *
 * Copyright (c) 2015-2021 Renesas Electronics Corporation and/or its affiliates
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
 * OR OTHER DEALINGS IN THE SOFTWARE.E.
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
typedef enum{ 	HALF_RESOLUTION = 0,
                QUATER_RESOLUTION,
								EIGHT_RESOLUTION,
								SIXTEENTH_RESOLUTION} temperature_resolution; //The higher the resolution the longer the convursion time, for SIXTEENTH_RESOLUTION it is 250ms.
																															//Look in the datasheet of the MCP9808 Temperature sensor for the convursion time.
							
/*
 * DEFINES
 ****************************************************************************************
 */
#define TEMPERATURE_DATA_REGISTER		    		0x05 									//Register 5 contains 12 bits of temperature data
#define TEMPERATURE_REGISTER_SIZE		    		12	 									//In bits. Must be a unsigned integer greater than 0
#define RESOLUTION_CONFIG_REGISTER	        0X08 								 	//2 bits register containing the resolution
#define RESOLUTION_CONFIG_REGISTER_SIZE			2
#define USED_TEMPERATURE_RESOLUTION	        SIXTEENTH_RESOLUTION 	//0.0625 degrees Celsius resolution

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
 
/**
****************************************************************************************
* @brief Declared function to deinitialize MCP9808, to be defined by the user if necessary
* @return void
****************************************************************************************
*/
void MCP9808_deinit(void);

void MCP9808_init(void);

void MCP9808_set_resolution_register(const temperature_resolution resolution);

temperature_resolution MCP9808_get_resolution_register(void);

uint16_t MCP9808_read_temperature_register(void);

double MCP9808_calculate_temperature(const uint16_t);

double MCP9808_get_temperature(void);

#endif //CFG_USE_INTERNAL_TEMP_SENSOR

#endif // _MCP9808_H_
