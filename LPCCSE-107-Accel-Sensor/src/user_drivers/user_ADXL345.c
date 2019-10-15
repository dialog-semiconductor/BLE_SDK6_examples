/**
 ****************************************************************************************
 *
 * @file user_ADXL345.c
 *
 * @brief functions to interface with the ADXL345 accelerometer.
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

#include "user_ADXL345.h"
#include "gpio.h"
#include "app_easy_timer.h"
#include "user_accelerometer.h"
#include "user_i2c.h"

void init_ADXL345(){
	#ifdef NO_SENSOR
	return;//If the demo is ran without a sensor return immediately
	#else
	RESERVE_GPIO(,ADXL345_SDA_PORT, ADXL345_SDA_PIN, PID_I2C_SDA);
	RESERVE_GPIO(,ADXL345_SCL_PORT, ADXL345_SCL_PIN, PID_I2C_SCL);
	RESERVE_GPIO(,ADXL345_CS_PORT, ADXL345_CS_PIN, PID_GPIO);
	
	GPIO_ConfigurePin(ADXL345_CS_PORT, ADXL345_CS_PIN, OUTPUT, PID_GPIO, true);
	GPIO_ConfigurePin(ADXL345_SDA_PORT, ADXL345_SDA_PIN, INPUT_PULLUP, PID_I2C_SDA, true);
	GPIO_ConfigurePin(ADXL345_SCL_PORT, ADXL345_SCL_PIN, INPUT_PULLUP, PID_I2C_SCL, true);
	
	user_i2c_init(ADXL345_I2C_ADDRESS, I2C_7BIT_ADDR, I2C_FAST, I2C_1BYTE_REGISTER, I2C_1BYTE_ADDR);
	user_i2c_write_reg(0x2D, 0x8); //Start measuring
	user_i2c_write_reg(0x31, 0xB); //Switch to 16g full res range
	user_i2c_write_reg(0x2C, 0x0A); //0x0A = 100 hz data rate
	#endif //NO_SENSOR
}


int16_t read_ADXL345_X(){
	#ifndef NO_SENSOR
	X_data   =  user_i2c_read_reg(0x32) & 0xff;       //Read X LSB
	X_data  |= (user_i2c_read_reg(0x33) & 0xff) << 8; //Read X MSB
	#else
	//If no sensor is present just increase current data
	X_data++;
	#endif //NO_SENSOR
	return X_data;
}

int16_t read_ADXL345_Y(){
	#ifndef NO_SENSOR
	Y_data   =  user_i2c_read_reg(0x34) & 0xff;       //Read Y LSB
	Y_data  |= (user_i2c_read_reg(0x35) & 0xff) << 8; //Read Y MSB
	#else
	//If no sensor is present just increase current data
	Y_data++;
	#endif //NO_SENSOR
	return Y_data;
}

int16_t read_ADXL345_Z(){
	#ifndef NO_SENSOR
	Z_data   =  user_i2c_read_reg(0x36) & 0xff;       //Read Z LSB
	Z_data  |= (user_i2c_read_reg(0x37) & 0xff) << 8; //Read Z MSB
	#else
	//If no sensor is present just increase current data
	Z_data++;
	#endif //NO_SENSOR
	return Z_data;
}

uint8_t previous[6];

void read_ADXL345_XYZ(uint8_t* xyz){
	user_i2c_multi_byte_read(0x32, xyz, 6);
	for(int i = 0; i < 3; i++){
		int16_t current =   (xyz[i*2+1] & 0xff) << 8 | (xyz[i*2] & 0xff); //Convert the received data to a 16 bit int
		int16_t prev = (previous[i*2+1] & 0xff) << 8 | (previous[i*2] & 0xff);
		current = 0.25 * current + 0.75 * prev; //A very simple smoothing algorithm
		xyz[i*2] = current & 0xff;
		xyz[i*2+1] = (current >> 8) & 0xff;
	}
	
	memcpy(previous, xyz, 6); //Save the current data
}
