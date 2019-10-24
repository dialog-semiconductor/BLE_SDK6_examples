/**
 ****************************************************************************************
 *
 * @file user_ADXL345.c
 *
 * @brief Functions to interface with the ADXL345 accelerometer.
 *
 * Copyright (c) 2012-2019 Dialog Semiconductor. All rights reserved.
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

#include "user_ADXL345.h"
#include "gpio.h"
#include "app_easy_timer.h"
#include "user_accelerometer.h"
#include "i2c.h"


void ADXL345_init(){
#ifdef NO_SENSOR
	return;//If the demo is ran without a sensor return immediately
#else
    const uint8_t power_ctl_cmd[] = {ADXL345_REG_POWER_CTL, 0x08};      //Set measure mode
    const uint8_t data_format_cmd[] = {ADXL345_REG_DATA_FORMAT, 0x0B};  //16g range, LSB mode with sign ext, full resolution
    const uint8_t bw_rate_cmd[] = {ADXL345_REG_BW_RATE, 0x0A};          //100Hz output

    i2c_abort_t abort_code; //May be used for error checking

	//Initialize sensor configuration registers
    i2c_master_transmit_buffer_sync(power_ctl_cmd, 2, &abort_code, I2C_F_NONE);
    i2c_master_transmit_buffer_sync(data_format_cmd, 2, &abort_code, I2C_F_NONE);
    i2c_master_transmit_buffer_sync(bw_rate_cmd, 2, &abort_code, I2C_F_NONE);

#endif //NO_SENSOR
}

int16_t ADXL345_read_X(){
#ifndef NO_SENSOR
    i2c_abort_t abort_code;                 //May be used for error checking
    uint8_t reg_addr = ADXL345_REG_DATAX0;  //Variable to hold the register address
    uint8_t byte_received = 0;              //Intermediate storage of the received byte
    
    //Get measurement LSB
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
    X_data = byte_received & 0xff;          //Store X LSB
    
    //Get measurement MSB
    reg_addr = ADXL345_REG_DATAX1;
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
    X_data |= (byte_received & 0xff) << 8;  //Store X MSB
#else
	//If no sensor is present just increase current data
	X_data++;
#endif //NO_SENSOR
	return X_data;
}

int16_t ADXL345_read_Y(){
#ifndef NO_SENSOR
    i2c_abort_t abort_code;                 //May be used for error checking
    uint8_t reg_addr = ADXL345_REG_DATAY0;  //Variable to hold the register address
    uint8_t byte_received = 0;              //Intermediate storage of the received byte
    
    //Get measurement LSB
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
	Y_data = byte_received & 0xff;          //Store Y LSB
    
    //Get measurement MSB
    reg_addr = ADXL345_REG_DATAY1;
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
   	Y_data |= (byte_received & 0xff) << 8;  //Store Y MSB
#else
	//If no sensor is present just increase current data
	Y_data++;
#endif //NO_SENSOR
	return Y_data;
}

int16_t ADXL345_read_Z(){
#ifndef NO_SENSOR
    i2c_abort_t abort_code;                 //May be used for error checking
    uint8_t reg_addr = ADXL345_REG_DATAZ0;  //Variable to hold the register address
    uint8_t byte_received = 0;              //Intermediate storage of the received byte
    
    //Get measurement LSB
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
	Z_data = byte_received & 0xff;          //Store Z LSB
    
    //Get measurement MSB
    reg_addr = ADXL345_REG_DATAZ1;
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
   	Z_data |= (byte_received & 0xff) << 8;  //Store Z MSB
#else
	//If no sensor is present just increase current data
	Z_data++;
#endif //NO_SENSOR
	return Z_data;
}

void ADXL345_read_XYZ(uint8_t* xyz){
    i2c_abort_t abort_code;     //May be used for error checking
    static uint8_t previous[6]; //Holds the previous measurement

    //Setup multiple-byte read from DATAX0 to DATAZ1 registers
    const uint8_t reg_addr = ADXL345_REG_DATAX0;
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(xyz, 6, &abort_code, I2C_F_NONE);
    
    //Process the received register values
	for(int i = 0; i < 3; i++){
		int16_t current =   (xyz[i*2+1] & 0xff) << 8 | (xyz[i*2] & 0xff); //Convert the received data to a 16 bit int
		int16_t prev = (previous[i*2+1] & 0xff) << 8 | (previous[i*2] & 0xff);
		current = 0.75 * current + 0.25 * prev; //A very simple smoothing algorithm
		xyz[i*2] = current & 0xff;
		xyz[i*2+1] = (current >> 8) & 0xff;
	}
	
    //Save the data
	memcpy(previous, xyz, 6);
}
