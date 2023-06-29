/**
 ****************************************************************************************
 *
 * @file port_bmi270_config.h
 *
 * @brief Port bmi270 configuration file.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
 */
#ifndef _PORT_BMI270_CONFIG_H_
#define _PORT_BMI270_CONFIG_H_
#include "user_periph_setup.h"

#define PORT_BMI270_INTERFACE_I2C       (0)
#define PORT_BMI270_INTERFACE_SPI       (1)
#define PORT_BMI270_INTERFACE_SPI_3W    (2)


#define PORT_BMI270_USE_BOSCH_DUMMY_BYTE  (1)
/*
* Possible values:
*     BMI270_CX_INTERFACE_I2C   
*     BMI270_CX_INTERFACE_SPI   
*     BMI270_CX_INTERFACE_SPI_3W
*/
#define PORT_BMI270_INTERFACE   PORT_BMI270_INTERFACE_I2C

// BMI270 I2C address
#define PORT_BMI270_I2C_ADDR BMI2_I2C_PRIM_ADDR

// BMI270 SDA pin definition
#define PORT_BMI270_I2C_SDA_PORT GPIO_PORT_0
#define PORT_BMI270_I2C_SDA_PIN  GPIO_PIN_11

// BMI270 SDA pin definition
#define PORT_BMI270_I2C_SCL_PORT GPIO_PORT_0
#define PORT_BMI270_I2C_SCL_PIN  GPIO_PIN_8

// BMI270 CS pin definition
#define PORT_BMI270_CS_PORT      SPI_EN_1_PORT    
#define PORT_BMI270_CS_PIN       SPI_EN_1_PIN

// SDX pin definition (in 3wire spi case this will be used as both MOSI and MISO)
#define PORT_BMI270_SDX_PORT     SPI_DO_PORT
#define PORT_BMI270_SDX_PIN      SPI_DO_PIN 

#if (PORT_BMI270_INTERFACE==PORT_BMI270_INTERFACE_SPI)
// SDI pin definition
#define PORT_BMI270_CX_SDO_PORT  SPI_DI_PORT
#define PORT_BMI270_CX_SDO_PIN   SPI_DI_PIN 
#endif

// SCLK Pin definition
#define PORT_BMI270_SCLK_PORT    SPI_CLK_PORT
#define PORT_BMI270_SCLK_PIN     SPI_CLK_PIN 
#endif // _PORT_BMI270_CONFIG_H_