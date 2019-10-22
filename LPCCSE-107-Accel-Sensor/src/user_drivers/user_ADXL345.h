/**
 ****************************************************************************************
 *
 * @file user_ADXL345.h
 *
 * @brief functions for interfacing with the ADXL345 accelerometer over i2c.
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs included in this package ("Software") include confidential, proprietary information of Dialog Semiconductor. All Rights Reserved.
 * 
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’) AND MAY ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND.  THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS.  RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG’S PRIOR WRITTEN PERMISSION.
 * 
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED BY THE FCC OR OTHER LIKE AGENCIES.
 * 
 * IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
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

#define ADXL345_I2C_ADDRESS   0x1D //This is the sensor address without R/W bit (0x1D or 0x53 for ADXL)

//#define NO_SENSOR //Uncomment this line when no sensor is connected

#define ADXL345_SDA_PORT     GPIO_PORT_1
#define ADXL345_SDA_PIN      GPIO_PIN_2
												     
#define ADXL345_SCL_PORT     GPIO_PORT_1
#define ADXL345_SCL_PIN      GPIO_PIN_0
												     
#define ADXL345_CS_PORT      GPIO_PORT_0
#define ADXL345_CS_PIN       GPIO_PIN_2

#define NOTIFICATION_DELAY 50 //The time interval between sent notifications

// ADXL345 Registers

#define ADXL345_REG_POWER_CTL       0x2D //Power-saving features control 
#define ADXL345_REG_DATA_FORMAT     0x31 //Data format control 
#define ADXL345_REG_BW_RATE         0x2C //Data rate and power mode control 

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
