/**
 ****************************************************************************************
 *
 * @file user_i2c.c
 *
 * @brief user_i2c source file.
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program includes Confidential,
 * Proprietary Information of Dialog Semiconductor. All Rights Reserved. This software
 * is solely for use on authorized Dialog Semiconductor products and platforms.
 * Recipient shall not transmit any software source code to any third party without
 * Dialog Semiconductor's prior written permission.
 *
 * TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_i2c.h"

/****************************************************************************************
 * MACROS
 ****************************************************************************************/
// I2C helper macros
#define SEND_I2C_COMMAND(X) SetWord16(I2C_DATA_CMD_REG, (X))
#define WAIT_WHILE_I2C_FIFO_IS_FULL() while (!(GetWord16(I2C_STATUS_REG) & TFNF)){}
#define WAIT_UNTIL_I2C_FIFO_IS_EMPTY() while (!(GetWord16(I2C_STATUS_REG) & TFE)){}
#define WAIT_UNTIL_NO_MASTER_ACTIVITY() while (GetWord16(I2C_STATUS_REG) & MST_ACTIVITY)
#define WAIT_FOR_RECEIVED_BYTE() while (!GetWord16(I2C_RXFLR_REG))

struct i2c_cfg_env_t i2c_cfg_env;

static void user_i2c_send_address(uint32_t address_to_send)
{
		for(int8_t i = i2c_cfg_env.register_address_width; i >= 0; i--){
       SEND_I2C_COMMAND((address_to_send >> 8*i) & 0xFF);   // Send the bytes of the register address			
		}
}

void user_i2c_init(uint16_t slave_address, I2C_ADDRESS_MODES i2c_address_width, I2C_SPEED_MODES bit_rate, I2C_REGISTER_WIDTH register_width, I2C_ADDRESS_BYTES_COUNT register_address_width)
{
		// Save the settings in a struct
    i2c_cfg_env.i2c_address_width = i2c_address_width;
    i2c_cfg_env.bit_rate = bit_rate;
    i2c_cfg_env.register_width = register_width;
		i2c_cfg_env.register_address_width = register_address_width;
	
    // Initialize the I2C with the address provided as argument
    SetBits16(CLK_PER_REG, I2C_ENABLE, 1); // Enable  clock for I2C
    SetWord16(I2C_ENABLE_REG, 0x0); // Disable the I2C controller
    SetWord16(I2C_CON_REG,
              I2C_MASTER_MODE | I2C_SLAVE_DISABLE | I2C_RESTART_EN); // Slave is disabled
    SetBits16(I2C_CON_REG, I2C_SPEED,
              i2c_cfg_env.bit_rate); // Set speed. Standard speed = 1, fast = 2
    if (i2c_cfg_env.i2c_address_width == I2C_7BIT_ADDR) {
        SetBits16(I2C_CON_REG, I2C_10BITADDR_MASTER, 0); // Set addressing mode. 7bit = 0
    } else {
        SetBits16(I2C_CON_REG, I2C_10BITADDR_MASTER, 1); // Set addressing mode. 10bit= 1
    }
    SetWord16(I2C_TAR_REG, slave_address & 0x3FF); // Set Slave device address
    SetWord16(I2C_ENABLE_REG, 0x1); // Enable the I2C controller
    while (GetWord16(I2C_STATUS_REG) & 0x20)
        ; // Wait for I2C master FSM to be IDLE
}

void user_i2c_set_register_width(I2C_REGISTER_WIDTH width)
{
	  i2c_cfg_env.register_width = width;
}

void user_i2c_write_reg(uint32_t reg_address, uint32_t wr_data)
{
    user_i2c_send_address(reg_address); //Start the transaction and send the address
	
	  for(int8_t i = i2c_cfg_env.register_width; i >= 0; i--){
       WAIT_WHILE_I2C_FIFO_IS_FULL();              // Wait if I2C Tx FIFO is full
       SEND_I2C_COMMAND((wr_data >> 8*i) & 0xFF);  // Write a byte of data to the FIFO
		}
		
    WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                // Wait until Tx FIFO is empty
    WAIT_UNTIL_NO_MASTER_ACTIVITY();               // wait until no master activity	
}


uint32_t user_i2c_read_reg(uint32_t reg_address)
{
    user_i2c_send_address(reg_address);
	  uint32_t ret = 0;
	  
	  
    WAIT_WHILE_I2C_FIFO_IS_FULL(); // Wait if Tx FIFO is full
	  for(int8_t i = 0; i <= i2c_cfg_env.register_width; i++){
       SEND_I2C_COMMAND(0x0100 & 0x3FF); // Set R/W bit to 1 (read access) MSB
		}
		
    WAIT_UNTIL_I2C_FIFO_IS_EMPTY(); // Wait until I2C Tx FIFO empty
    WAIT_UNTIL_NO_MASTER_ACTIVITY();
		
	  for(int8_t i = i2c_cfg_env.register_width; i >= 0; i--){
       ret |= (0xFF & GetWord16(I2C_DATA_CMD_REG)) << 8*i; // Get received byte
		}
		
    WAIT_UNTIL_I2C_FIFO_IS_EMPTY();  // Wait until Tx FIFO is empty
    WAIT_UNTIL_NO_MASTER_ACTIVITY(); // wait until no master activity	
		return ret;
}

void user_i2c_multi_byte_write(uint32_t reg_address, uint8_t *wr_data, uint32_t num_bytes)
{
    user_i2c_send_address(reg_address);
	  for(uint32_t i = 0; i < num_bytes; i++){
       WAIT_WHILE_I2C_FIFO_IS_FULL();       // Wait if I2C Tx FIFO is full
       SEND_I2C_COMMAND(wr_data[i] & 0xFF); //Write the data to the FIFO
		}		                                    
    WAIT_UNTIL_I2C_FIFO_IS_EMPTY();         //Wait until Tx FIFO is empty
    WAIT_UNTIL_NO_MASTER_ACTIVITY();        //Wait until no master activity	
}


void user_i2c_multi_byte_read(uint32_t reg_address, uint8_t *rd_data, uint32_t num_bytes)
{
    user_i2c_send_address(reg_address);
	  uint32_t index = 0;
	  
	  for(uint32_t i = 0; i < num_bytes;){
       SEND_I2C_COMMAND(0x0100 & 0x3FF); // Set R/W bit to 1 (read access) MSB
			 if(!(++i % 24)){ //FIFO is 32 bits
		      WAIT_UNTIL_I2C_FIFO_IS_EMPTY();                 // Wait until Tx FIFO is empty
          WAIT_UNTIL_NO_MASTER_ACTIVITY();                // wait until no master activity
				  for (uint8_t j = 0; j < 24; j++){
						//Store the FIFO contents in ram
						rd_data[index++] = (0xFF & GetWord16(I2C_DATA_CMD_REG)); // Get received byte
					}
					reg_address += 24/(i2c_cfg_env.register_width + 1);//Calculate the next register to read out 
					user_i2c_send_address(reg_address);                //Restart the read
			 }
		}
		
    WAIT_UNTIL_I2C_FIFO_IS_EMPTY(); // Wait until I2C Tx FIFO empty
    WAIT_UNTIL_NO_MASTER_ACTIVITY();
		
	  while(index < num_bytes){
       rd_data[index++] = (0xFF & GetWord16(I2C_DATA_CMD_REG)); // Get received byte
		}
		
    WAIT_UNTIL_I2C_FIFO_IS_EMPTY();  // Wait until Tx FIFO is empty
    WAIT_UNTIL_NO_MASTER_ACTIVITY(); // wait until no master activity	
}
/// @} APP
