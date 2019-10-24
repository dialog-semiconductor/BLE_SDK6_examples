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
