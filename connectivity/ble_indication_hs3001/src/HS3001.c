/**
 ****************************************************************************************
 *
 * @file HS3001.c
 *
 * @brief HS3001 module.
 *
 * Copyright (C) 2012-2023 Dialog Semiconductor
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration
#include "arch_console.h"
#include "gpio.h"
#include "user_periph_setup.h"
#include "i2c.h"
#include "math.h"
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"

#include "HS3001.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

	
	
		
void HS3001_wakeup(void){
	
	
	
	
	uint8_t buffer[4];
	i2c_abort_t abort_code = I2C_ABORT_NONE;
	i2c_master_transmit_buffer_sync((uint8_t*)DF_command,1,&abort_code,I2C_F_ADD_STOP);

	

	for (uint32_t i = 0;i<150000;i++) // Wait for about 45 ms  
    {
        __NOP();
    }
	
	if(abort_code != I2C_ABORT_NONE) {
		
		ASSERT_ERROR(0);
		
	}
}



void Data_Fetch_all (uint8_t* buffer){
	
	
			 HS3001_wakeup();
			 i2c_abort_t abort_code = I2C_ABORT_NONE;
       i2c_master_receive_buffer_sync(buffer,4,&abort_code,I2C_F_ADD_STOP);
	
	if(abort_code != I2C_ABORT_NONE) {
		ASSERT_ERROR(0);
		
		
		
	}

} 

double HS3001_get_temperature (uint8_t *buffer){
				
							uint8_t temperature;
							Data_Fetch_all(buffer);
								
              temperature = 165.0 * (float)((buffer[2] << 6) | (buffer[3] >> 2)) / 16383.0 - 40.0 ;
							
				
				      arch_printf("Temperature is :%d \n\r",temperature);
			return temperature;
			}

			
double HS3001_get_humidity (uint8_t *buffer){
				

				      uint8_t humidity;
	
					   	Data_Fetch_all(buffer);
							
              humidity = 100.0 * (float)(((buffer[0] & 0x3F) << 8 | buffer[1]))/16383.0 ;

							
				arch_printf("Humidity is :%d \n\r",humidity);

			return humidity;
			}
