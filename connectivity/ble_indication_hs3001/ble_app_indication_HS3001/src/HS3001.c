/**
 ****************************************************************************************
 *
 * @file HS3001.c
 *
 * @brief HS3001 module.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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
		//insert error handler
	}
}

void Data_Fetch_all (uint8_t* buffer){
	
	
			 HS3001_wakeup();
	
			 i2c_abort_t abort_code = I2C_ABORT_NONE;


	     i2c_master_receive_buffer_sync(buffer,4,&abort_code,I2C_F_ADD_STOP);
	if(abort_code != I2C_ABORT_NONE) {
		//insert error handler
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
