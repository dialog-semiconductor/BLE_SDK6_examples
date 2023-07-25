/**
 ****************************************************************************************
 *
 * @file user_ADXL345.h
 *
 * @brief functions for interfacing with the ADXL345 accelerometer over i2c.
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

#ifndef _ADXL345_
#define _ADXL345_

#include "gpio.h"

/*
 * DEFINES
 ****************************************************************************************
 */
 
#undef BLE_TINY_CLICK					//Define this if you are using the BLE Tiny Click

#define ADXL345_I2C_ADDRESS   0x1D      //This is the sensor address without R/W bit (0x1D or 0x53 for ADXL)

#if defined (__DA14531__)

#ifdef BLE_TINY_CLICK

#define ADXL345_SCL_PORT    GPIO_PORT_0
#define ADXL345_SCL_PIN     GPIO_PIN_11

#define ADXL345_SDA_PORT    GPIO_PORT_0
#define ADXL345_SDA_PIN     GPIO_PIN_9

#else
	
#define ADXL345_SCL_PORT    GPIO_PORT_0
#define ADXL345_SCL_PIN     GPIO_PIN_3

#define ADXL345_SDA_PORT    GPIO_PORT_0
#define ADXL345_SDA_PIN     GPIO_PIN_1

#endif

#else

#define ADXL345_SDA_PORT    GPIO_PORT_1
#define ADXL345_SDA_PIN     GPIO_PIN_1
												     
#define ADXL345_SCL_PORT    GPIO_PORT_1
#define ADXL345_SCL_PIN     GPIO_PIN_3
												     
#endif

// ADXL345 Registers

#define ADXL345_REG_POWER_CTL       0x2D //Power-saving features control 
#define ADXL345_REG_DATA_FORMAT     0x31 //Data format control 
#define ADXL345_REG_BW_RATE         0x2C //Data rate and power mode control

#define ADXL345_REG_DATAX0          0x32 //X-Axis Data 0
#define ADXL345_REG_DATAX1          0x33 //X-Axis Data 1
#define ADXL345_REG_DATAY0          0x34 //Y-Axis Data 0
#define ADXL345_REG_DATAY1          0x35 //Y-Axis Data 1
#define ADXL345_REG_DATAZ0          0x36 //Z-Axis Data 0
#define ADXL345_REG_DATAZ1          0x37 //Z-Axis Data 1 

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief ADXL345 initialization function.
 * @return void
 ****************************************************************************************
*/
void ADXL345_init(void);

/**
 ****************************************************************************************
 * @brief Read the data registers of the ADXL345, reads X to a global variable.
 * @return void
 ****************************************************************************************
*/
int16_t ADXL345_read_X(void);

/**
 ****************************************************************************************
 * @brief Read the data registers of the ADXL345, reads Y to a global variable.
 * @return void
 ****************************************************************************************
*/
int16_t ADXL345_read_Y(void);

/**
 ****************************************************************************************
 * @brief Read the data registers of the ADXL345, reads Z to a global variable.
 * @return void
 ****************************************************************************************
*/
int16_t ADXL345_read_Z(void);


void ADXL345_read_XYZ(uint8_t* xyz);
#endif
