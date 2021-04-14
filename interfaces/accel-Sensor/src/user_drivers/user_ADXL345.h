/**
 ****************************************************************************************
 *
 * @file user_ADXL345.h
 *
 * @brief functions for interfacing with the ADXL345 accelerometer over i2c.
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
