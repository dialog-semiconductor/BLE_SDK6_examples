/**
 ****************************************************************************************
 *
 * @file user_i2c.h
 *
 * @brief user_i2c header file.
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

#ifndef _USER_I2C_H_
#define _USER_I2C_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"

/*
 * DEFINES
 ****************************************************************************************
 */

typedef enum 
{
    I2C_7BIT_ADDR,
    I2C_10BIT_ADDR,
}I2C_ADDRESS_MODES;

typedef enum 
{
    I2C_1BYTE_ADDR,
    I2C_2BYTES_ADDR,
    I2C_3BYTES_ADDR,
}I2C_ADDRESS_BYTES_COUNT;

typedef enum 
{
    I2C_STANDARD = 1,
    I2C_FAST,
}I2C_SPEED_MODES;

typedef enum 
{
    I2C_1BYTE_REGISTER,
    I2C_2BYTES_REGISTER,
    I2C_3BYTES_REGISTER,
    I2C_4BYTES_REGISTER,
}I2C_REGISTER_WIDTH;

/**
 * @brief  I2C struct
 */
struct i2c_cfg_env_t
{
    uint8_t bit_rate;              //Standard mode or fast mode
    uint8_t register_width;        //Slave register width 8 or 16 bit
    uint8_t i2c_address_width;     //Slave addressing width 10 or 7 bit
		uint8_t register_address_width;//Slave register addressing width 8 or 16 bit
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief      Initializes the I2C interface with the provide slave address
 *
 * @param[in]  slave_address          , The I2C slave address to communicate with
 * @param[in]  i2c_address_width      , The width of the I2C address(7 or 10 bit)
 * @param[in]  bit_rate               , The I2C bus frequency (standard or fast mode)
 * @param[in]  register_width         , The I2C slave register width (1-4 bytes)
 * @param[in]  register_address_width , The I2C slave register address width (1-3 bytes)
 *
 * @return     void
 *****************************************************************************************
 */
void user_i2c_init(uint16_t slave_address, I2C_ADDRESS_MODES i2c_address_width, I2C_SPEED_MODES bit_rate, I2C_REGISTER_WIDTH register_width, I2C_ADDRESS_BYTES_COUNT register_address_width);

/**
 ****************************************************************************************
 * @brief      Sets the register width of the slave device
 *
 * @param[in]  register_width, The I2C slave register width (1-4 bytes)
 *
 * @return     void
 *****************************************************************************************
 */
void user_i2c_set_register_width(I2C_REGISTER_WIDTH width);
	
/**
 ****************************************************************************************
 * @brief      Reads data from an I2C slave register
 *
 * @param[in]  reg_address, Register address to be read
 *
 * @return     Contents of the register
 *****************************************************************************************
 */
uint32_t user_i2c_read_reg(uint32_t reg_address);

/**
 ****************************************************************************************
 * @brief      Writes data to a register address of an I2C slave
 *
 * @param[in]  reg_address, Register address to write to
 * @param[in]  wr_data, Data to write
 *
 * @return     void
 *****************************************************************************************
 */
void user_i2c_write_reg(uint32_t reg_address, uint32_t wr_data);

/**
 ****************************************************************************************
 * @brief      Writes data to a register address of an I2C slave
 *
 * @param[in]  reg_address, Register address to write to
 * @param[in]  wr_data, Data to write
 * @param[in]  num_bytes, Number of bytes to write
 *
 * @return     void
 *****************************************************************************************
 */
void user_i2c_multi_byte_write(uint32_t reg_address, uint8_t *wr_data, uint32_t num_bytes);

/**
 ****************************************************************************************
 * @brief      Writes data to a register address of an I2C slave
 *
 * @param[in]  reg_address, Register address to write to
 * @param[in]  rd_data, Array where the data will be read to
 * @param[in]  num_bytes, Number of bytes to read
 *
 * @return     void
 *****************************************************************************************
 */
void user_i2c_multi_byte_read(uint32_t reg_address, uint8_t *rd_data, uint32_t num_bytes);

#endif //_USER_I2C_H_
/// @} APP
