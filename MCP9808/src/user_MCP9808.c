/**
 ****************************************************************************************
 *
 * @file user_MCP9808.c
 *
 * @brief MCP9808 functions
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

#include "user_MCP9808.h"
#include "user_i2c.h"
#include "gpio.h"

void init_MCP9808(uint8_t address){
	RESERVE_GPIO(,MCP9808_SDA_PORT, MCP9808_SDA_PIN, PID_I2C_SDA);
	RESERVE_GPIO(,MCP9808_SCL_PORT, MCP9808_SCL_PIN, PID_I2C_SCL);
	
	GPIO_ConfigurePin(MCP9808_SDA_PORT, MCP9808_SDA_PIN, INPUT_PULLUP, PID_I2C_SDA, true);
	GPIO_ConfigurePin(MCP9808_SCL_PORT, MCP9808_SCL_PIN, INPUT_PULLUP, PID_I2C_SCL, true);
	
	user_i2c_init(MCP9808_I2C_ADDRESS, I2C_7BIT_ADDR, I2C_FAST, I2C_1BYTE_REGISTER, I2C_1BYTE_ADDR); //Init with one byte registers as
																																																	 //the resolution register is 8 bit
	user_i2c_write_reg(0x8, 0x3); //Set the resolution to maximum                                    
	user_i2c_set_register_width(I2C_2BYTES_REGISTER);//Change to two byte registers for the temperature data
}

int16_t read_MCP9808(){
	return user_i2c_read_reg(5) & 0xFFF; //Register 5 contains 12 bits of temperature data
}
