/**
 ****************************************************************************************
 *
 * @file user_peripheral.c
 *
 * @brief Peripheral project source code.
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

/*
 * INCLUDE FILES                                                                        s
 ****************************************************************************************
 */
#include "MCP9808.h"
#if !defined(CFG_USE_INTERNAL_TEMP_SENSOR)
#include "i2c.h"
#include "math.h"
/**
 ****************************************************************************************
 * @brief Initialize the temperature sensor, the function sets the resolution to maximum
 * @return void
 ****************************************************************************************
 */
void temperature_sensor_init(void){
	uint8_t buffer[2];
	i2c_abort_t abort_code = I2C_ABORT_NONE;
	
	buffer[0] = RESOLUTION_CONFIG_REGISTER;
	buffer[1] = USED_TEMPERATURE_RESOLUTION;
	
	i2c_master_transmit_buffer_sync((uint8_t*)buffer,sizeof(buffer),&abort_code,I2C_F_WAIT_FOR_STOP);
	
	if(abort_code != I2C_ABORT_NONE) {
		//insert error handler
	}
	
}

/**
 ****************************************************************************************
 * @brief read the 12bits temperature data from register 5. 
 * @return uint16_t 
 ****************************************************************************************
 */
uint16_t read_MCP9808_temperature_register(void){
	if(TEMPERATURE_REGISTER_SIZE%1 != 0 || TEMPERATURE_REGISTER_SIZE<1){//pre defined conditions size must be an unsigned integer greater than 0
		//insert error handler
		return NULL;
	}

	uint8_t buffer[2];
	i2c_abort_t abort_code = I2C_ABORT_NONE;
		
	buffer[0] = TEMPERATURE_DATA_REGISTER; 
	
	i2c_master_transmit_buffer_sync((uint8_t*)buffer,1,&abort_code,I2C_F_WAIT_FOR_STOP);
	if(abort_code != I2C_ABORT_NONE) {
		//insert error handler
	}
	
	i2c_master_receive_buffer_sync((uint8_t*)buffer,sizeof(buffer),&abort_code,I2C_F_WAIT_FOR_STOP);
	if(abort_code != I2C_ABORT_NONE) {
		//insert error handler
	}
	
	return ( ((buffer[0]<<8) + buffer[1]) &((unsigned)pow(2,TEMPERATURE_REGISTER_SIZE)-1) ); 
}

/**
 ****************************************************************************************
 * @brief calculate the temperature in celsius  
 * @return double 
 ****************************************************************************************
 */
double calculate_temperature(const uint16_t data){
		return (data & 0x800) ? ( ((double)data/16)-256 ) : ( (double)data/16 ); //See "EQUATION 5-1:" of the MCP9808 datasheet 
}


/**
 ****************************************************************************************
 * @brief Combines reading the temperature register and calculating the temperature in celsius  
 * @return double 
 ****************************************************************************************
 */
double get_temperature(void){
	return calculate_temperature(read_MCP9808_temperature_register());
}

#endif
