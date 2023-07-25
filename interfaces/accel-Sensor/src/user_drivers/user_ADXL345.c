/**
 ****************************************************************************************
 *
 * @file user_ADXL345.c
 *
 * @brief Functions to interface with the ADXL345 accelerometer.
 *
 * Copyright (c) 2012-2021 Renesas Electronics Corporation and/or its affiliates
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

#include "user_ADXL345.h"
#include "gpio.h"
#include "app_easy_timer.h"
#include "user_accelerometer.h"
#include "i2c.h"
#include "compiler.h"

#ifdef NO_SENSOR
uint16_t dummy_x     __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint16_t dummy_y     __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
uint16_t dummy_z     __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
#endif

/**
 ****************************************************************************************
 * @brief ADXL345 sensor initialization function
 * @return void
 ****************************************************************************************
 */
void ADXL345_init(void){
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

/**
 ****************************************************************************************
 * @brief Read X-axis acceleration registers
 * @return X-axis acceleration raw value
 ****************************************************************************************
 */
int16_t ADXL345_read_X(void)
{
    int16_t axis_read;                      //Readout of the sensor or dummy
#ifndef NO_SENSOR
    i2c_abort_t abort_code;                 //May be used for error checking
    uint8_t reg_addr = ADXL345_REG_DATAX0;  //Variable to hold the register address
    uint8_t byte_received = 0;              //Intermediate storage of the received byte
    
    //Get measurement LSB
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
    axis_read = byte_received & 0xff;          //Store X LSB
    
    //Get measurement MSB
    reg_addr = ADXL345_REG_DATAX1;
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
    axis_read |= (byte_received & 0xff) << 8;  //Store X MSB
#else
	//If no sensor is present just increase current data
    dummy_x++;
	axis_read = dummy_x;
#endif //NO_SENSOR
	return axis_read;
}

/**
 ****************************************************************************************
 * @brief Read Y-axis acceleration registers
 * @return Y-axis acceleration raw value
 ****************************************************************************************
 */
int16_t ADXL345_read_Y(void)
{
    int16_t axis_read;                      //Readout of the sensor or dummy
#ifndef NO_SENSOR
    i2c_abort_t abort_code;                 //May be used for error checking
    uint8_t reg_addr = ADXL345_REG_DATAY0;  //Variable to hold the register address
    uint8_t byte_received = 0;              //Intermediate storage of the received byte
    
    //Get measurement LSB
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
	axis_read = byte_received & 0xff;          //Store Y LSB
    
    //Get measurement MSB
    reg_addr = ADXL345_REG_DATAY1;
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
   	axis_read |= (byte_received & 0xff) << 8;  //Store Y MSB
#else
	//If no sensor is present just increase current data
    dummy_y++;
	axis_read = (int16_t)dummy_y;
#endif //NO_SENSOR
	return axis_read;
}

/**
 ****************************************************************************************
 * @brief Read Z-axis acceleration registers
 * @return Z-axis acceleration raw value
 ****************************************************************************************
 */
int16_t ADXL345_read_Z(void)
{
    int16_t axis_read;                      //Readout of the sensor or dummy
#ifndef NO_SENSOR
    i2c_abort_t abort_code;                 //May be used for error checking
    uint8_t reg_addr = ADXL345_REG_DATAZ0;  //Variable to hold the register address
    uint8_t byte_received = 0;              //Intermediate storage of the received byte
    
    //Get measurement LSB
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
	axis_read = byte_received & 0xff;          //Store Z LSB
    
    //Get measurement MSB
    reg_addr = ADXL345_REG_DATAZ1;
    i2c_master_transmit_buffer_sync(&reg_addr, 1, &abort_code, I2C_F_NONE);
    i2c_master_receive_buffer_sync(&byte_received, 1, &abort_code, I2C_F_NONE);
   	axis_read |= (byte_received & 0xff) << 8;  //Store Z MSB
#else
	//If no sensor is present just increase current data
    dummy_z++;
	axis_read = (int16_t)dummy_z;
#endif //NO_SENSOR
	return axis_read;
}

/**
 ****************************************************************************************
 * @brief Read all axes acceleration values
 * @param[in] xyz     Pointer to an array which will hold the measurement
 * @return void
 ****************************************************************************************
 */
void ADXL345_read_XYZ(uint8_t* xyz){
    static uint8_t previous[6] __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY//Holds the previous measurement
#ifndef NO_SENSOR
    i2c_abort_t abort_code;     //May be used for error checking
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
#else
    // Send dummy data if no sensor is attached
    dummy_x++;
    dummy_y++;
    dummy_z++;
    
    co_write16p(xyz, dummy_x);
    co_write16p(&xyz[2], dummy_y);
    co_write16p(&xyz[4], dummy_z);
#endif	
}
