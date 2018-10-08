/**
 ****************************************************************************************
 *
 * @file user_MCP9808.h
 *
 * @brief MCP9808 function defenitions.
 *
 * Copyright (c) 2016-2018 Dialog Semiconductor. All rights reserved.
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
#ifndef _MCP9808_
#define _MCP9808_


#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */

#define NOTIFICATION_DELAY 100 //Time between notifications

#define MCP9808_SDA_PORT     GPIO_PORT_1
#define MCP9808_SDA_PIN      GPIO_PIN_2
												     
#define MCP9808_SCL_PORT     GPIO_PORT_0
#define MCP9808_SCL_PIN      GPIO_PIN_7

#define MCP9808_I2C_ADDRESS  0x18 //Address is between 0x18 and 0xF (excludes r/w bit)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief      Initializes the MCP9808.
 *
 * @param[in]  address, The I2C slave of the MCP9808
 * @param[in]  bit rate, The I2C slave bit rate (1:100kbit/s, 2:400kbit/s)
 * @param[in]  register_width, The I2C slave register width (8 or 16 bit)
 *
 * @return     void
 *****************************************************************************************
 */
void init_MCP9808(uint8_t address);

/**
 ****************************************************************************************
 * @brief      Reads out the contents of the MCP9808 temperature data register.
 *
 * @return     Temperature data
 *****************************************************************************************
 */
int16_t read_MCP9808(void);

#endif //_MCP9808_
