/**
 ****************************************************************************************
 *
 * @file one_wire.c
 *
 * @brief one_wire source code.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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
#include "one_wire.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "spi.h"
#include "systick.h"
#include "user_uart.h"

bool 			cmp_id_bit 				= 0;
bool 			id_bit 						= 0;
uint32_t 	sensor_index 			= 0;
uint32_t 	read_sensor 			= 0;

struct OneWire_sensor OneWire_sensors[MAX_NO_SENSORS];
uint8_t		bitcount = 0;

// function for storing the bitvalue to the corresponding temperature adress and bitposition
void store_bit_in_address(bool bit, int bit_position)
{
		if (bit)
		{
				if (bit_position >= 32)
				
				{
						OneWire_sensors[(sensor_index)].address_high |= 1 << (bit_position - 32);
				}
				else
				{
						OneWire_sensors[(sensor_index)].address_low |= 1 << bit_position;
				}
		}
		else
		{		
				if (bit_position >= 32)
				{
						OneWire_sensors[(sensor_index)].address_high &= ~(1 << (bit_position - 32));
				}
				else
				{
						OneWire_sensors[(sensor_index)].address_low &= ~(1 << (bit_position));
				}				
		}	
		if(((bit_position + 1) % 4) == 0 && bit_position)
		{
				printf_string(" ");
		}
}

// Interupt handler for reading id
void id_systick_ISR(void)
{
		systick_stop();
		bool inputlevel = GPIO_GetPinStatus(ONEWIRE_RX_PORT, ONEWIRE_RX_PIN);		
		if (inputlevel)
		{
				id_bit = 1;
		}
		else
		{
				id_bit = 0;
		}
}

// Interrupt handler for reading comp_id
void comp_id_systick_ISR(void)
{
		systick_stop();
		bool inputlevel = GPIO_GetPinStatus(ONEWIRE_RX_PORT, ONEWIRE_RX_PIN);		
		if (inputlevel)
		{
				cmp_id_bit = 1;
		}
		else
		{
				cmp_id_bit = 0;
		}
}

// Interrupt handler for reading address 
void address_systick_ISR(void)
{
		systick_stop();
		bool inputlevel = GPIO_GetPinStatus(ONEWIRE_RX_PORT, ONEWIRE_RX_PIN);
		store_bit_in_address(inputlevel, bitcount);
}
// interrupt handler for reading scratchpad
void readScratchpad_systick_ISR(void){
		systick_stop();
		bool inputlevel = GPIO_GetPinStatus(ONEWIRE_RX_PORT, ONEWIRE_RX_PIN);
		if (inputlevel)
		{
				if (bitcount < 16)
				{
						OneWire_sensors[read_sensor].temperature |= 1 << bitcount;	
				}
				else if (bitcount >= 16 &&bitcount < 40)
				{
						OneWire_sensors[read_sensor].scratchpad_low |= 1 << (bitcount - 16);
				}
				else
				{
						OneWire_sensors[read_sensor].scratchpad_high |= 1 << (bitcount - 40);
				}
		}
		else
		{
				if (bitcount < 16)
				{
						OneWire_sensors[read_sensor].temperature &= ~(1 << (bitcount));							
				}
				else if (bitcount >= 16 &&bitcount < 40)
				{
						OneWire_sensors[read_sensor].scratchpad_low &= ~(1 << (bitcount - 16));	
				}
				else
				{
						OneWire_sensors[read_sensor].scratchpad_high &= ~(1 << (bitcount - 40));
				}
		}
}


//
void SPI_write_32bits(uint32_t dataToSend)
{
		SetWord16(SPI_RX_TX_REG1, (uint16_t)(dataToSend >> 16));
		SetWord16(SPI_RX_TX_REG0, (uint16_t)(dataToSend));
    do
    {
    } while (GetBits16(SPI_CTRL_REG, SPI_INT_BIT) == 0);  
		SetWord16(SPI_CLEAR_INT_REG, 0x01);                     // clear pending flag	
}

void SPI_write_16bits(uint16_t dataToSend)
{
		SetWord16(SPI_RX_TX_REG0, (uint16_t)(dataToSend));
    do
    {
    } while (GetBits16(SPI_CTRL_REG, SPI_INT_BIT) == 0);  
		SetWord16(SPI_CLEAR_INT_REG, 0x01);                     // clear pending flag	
}

// Initiate readslot
void OneWire_readSlot(systick_callback_function_t systick_callback)
{
		uint32_t dataToSend = 0x1FFFFFFF;
	
		systick_stop();
		systick_register_callback(systick_callback);
		systick_start(15, 1);
	
		SPI_write_32bits(dataToSend);		
		dataToSend = 0xFFFFFFFF;
		SPI_write_32bits(dataToSend);
		SPI_write_32bits(dataToSend);
		SPI_write_16bits(dataToSend);
}

void OneWire_init(void)
{
		SPI_Pad_t chipselect = {SPI_EN_GPIO_PORT, SPI_EN_GPIO_PIN};
		// 16MHz / DIV_8 = 2MHz => 1 bit = 0.5 µSec
		spi_init(&chipselect, SPI_MODE_32BIT, SPI_ROLE_MASTER, SPI_CLK_IDLE_POL_HIGH, SPI_PHA_MODE_0, SPI_MINT_ENABLE, SPI_XTAL_DIV_8);
		SetBits16(SPI_CTRL_REG, SPI_DO, 1);
}

// Wait for presence pulse after reset state
void OneWire_presence(void)
{
		uint32_t dataToSend = 0xFFFFFFFF;
		for (int loop = 0; loop < 28; loop++)
		{
				SPI_write_32bits(dataToSend);  				
		}
}

// Generate resetpulse
void OneWire_reset(void)
{
		SetBits16(SPI_CTRL_REG, SPI_DO, 0);
		uint32_t dataToSend = 0x00000000;
		for (int loop = 0; loop < 28; loop++)
		{
				SPI_write_32bits(dataToSend);
		}
		SetBits16(SPI_CTRL_REG, SPI_DO, 1);
}

// Master write byte
void OneWire_write_byte(uint8_t byte)
{
		for (int loop = 0; loop < 8; loop++)
		{
			if (byte & 0x01 )
			{
				OneWire_write_1();
			}
			else
			{
				OneWire_write_0();
			}
			byte >>= 1;
		}
}

// Master write "1" slot
void OneWire_write_1(void)
{
		uint32_t dataToSend = 0x0000FFFF;
		SPI_write_32bits(dataToSend);                    
		dataToSend = 0xFFFFFFFF;
		for (int loop = 0; loop < 2; loop++)
		{
				SPI_write_32bits(dataToSend);
		}
		SPI_write_16bits(dataToSend);		
}
// Master write "0" slot
void OneWire_write_0(void)
{
		SetBits16(SPI_CTRL_REG, SPI_DO, 0);
		uint32_t dataToSend = 0x00000000;
		for (int loop = 0; loop < 2; loop++)
		{
				SPI_write_32bits(dataToSend);
		}
		SetBits16(SPI_CTRL_REG, SPI_DO, 1);
		
		dataToSend = 0xFFFFFFFF;
		SPI_write_32bits(dataToSend);// clear pending flag				
		SPI_write_16bits(dataToSend);
}

// Search algorithm
void OneWire_SearchROM(void)
{
		// Search algorithm variables		
		bool 			last_device_flag 	= 0; 		// flag to indicate the search is complete
		bool 			search_direction 	= 0;
		uint32_t 	last_discrepancy 	= 0; 		// index that indicates from which bit the search should start
		uint32_t 	last_zero 				= 0;		// bit position of the last zero written where there was a discrepancy
		uint32_t 	id_bit_number			=	0; 		// Rom bit number value currently being searched
	
		sensor_index = 0;
		while (0 == 0)
		{
				OneWire_reset();
				OneWire_presence();
			
				if (last_device_flag)
				{
						break;
				}
			
				id_bit_number = 1;
				last_zero = 0;
			
				OneWire_write_byte(0xF0);
				
				for (id_bit_number = 1; id_bit_number <= 64; id_bit_number++)
				{

						OneWire_readSlot(id_systick_ISR); 			// read id_bit
						OneWire_readSlot(comp_id_systick_ISR); 	// read comp_id_bit
					
						
						if (id_bit && cmp_id_bit)
						{
								printf_string("\n\rError... Please");
								printf_string(" Restart Search");	
								last_device_flag = 1;
								break;
						}
						else if (id_bit && !cmp_id_bit)							// There are only 1's in all participating ROM numbers
						{
								search_direction = 1;									// Search direction 1
						}
						else if (!id_bit && cmp_id_bit)					// There are only 0's in all participating ROM numbers
						{
								search_direction = 0;									// search direction 0
						}
						else if (!id_bit && !cmp_id_bit)				// Discrepancy
						{
								if (id_bit_number == last_discrepancy)
								{
										search_direction = 1;
								}
								else if (id_bit_number > last_discrepancy)
								{
										search_direction = 0;
										last_zero = id_bit_number;									
								}
								else
								{
										if (id_bit_number > 32)
										{
												if ((OneWire_sensors[(sensor_index - 1)].address_high & (1 << ((id_bit_number - 1) - 32))))
												{
														search_direction = 1;
												}
												else
												{
														search_direction = 0;
														last_zero = id_bit_number;	
												}
										}
										else if (id_bit_number <= 32)
										{
												if ((OneWire_sensors[(sensor_index - 1)].address_low & (1 << (id_bit_number - 1))))
												{
														search_direction = 1;
												}
												else
												{
														search_direction = 0;
														last_zero = id_bit_number;	
												}
										}
								}
						}
						if (search_direction == 1)
						{
								store_bit_in_address(1, (id_bit_number - 1));
								OneWire_write_1();
						}
						else if (search_direction == 0)
						{
								store_bit_in_address(0, (id_bit_number - 1));
								OneWire_write_0();
						}
						
						if (id_bit_number == 64)
						{
								last_discrepancy = last_zero;
								if(last_discrepancy == 0)																// last device condition
								{					
										last_device_flag = 1; // test
								}
								else
								{
										sensor_index++;
								}
						}
				}
		}
}	

// This command can only be used when there is one slave on the bus
// Allows the master to read the slave's 64-bit ROM code without the (more complicated) Search Rom procedure
void OneWire_readRom(void)
{
		OneWire_reset();
		OneWire_presence();
		OneWire_write_byte(0x33);
		for (int loop = 0; loop < 64; loop++)
		{
				bitcount = loop;
				OneWire_readSlot(address_systick_ISR);
		}
		sensor_index = 0;
}

// Command to target a particular slave corresponding the ROM code
void OneWire_matchRom(int sensor_number)
{
		OneWire_reset();
		OneWire_presence();
		OneWire_write_byte(0x55); // Rom command
		OneWire_write_byte((OneWire_sensors[sensor_number].address_low 	>>   0) & 0xFF);
		OneWire_write_byte((OneWire_sensors[sensor_number].address_low 	>>   8) & 0xFF);
		OneWire_write_byte((OneWire_sensors[sensor_number].address_low 	>> 	16) & 0xFF);
		OneWire_write_byte((OneWire_sensors[sensor_number].address_low 	>> 	24) & 0xFF);
		OneWire_write_byte((OneWire_sensors[sensor_number].address_high		>>   0) & 0xFF);
		OneWire_write_byte((OneWire_sensors[sensor_number].address_high  	>>   8) & 0xFF);
		OneWire_write_byte((OneWire_sensors[sensor_number].address_high  	>> 	16) & 0xFF);
		OneWire_write_byte((OneWire_sensors[sensor_number].address_high  	>> 	24) & 0xFF);
}

// Command to read from slaves scratchpad
void OneWire_readScratchpad(void)
{
		for (int loop_i = 0; loop_i <= sensor_index; loop_i++)
		{		
				read_sensor = loop_i;
				OneWire_matchRom(loop_i);
				OneWire_write_byte(0xBE);
				for (int loop = 0; loop < 72; loop++)
				{
						bitcount = loop;
						OneWire_readSlot(readScratchpad_systick_ISR);
				}
		}
}

// Command to initiate temperature conversion
void OneWire_ConvertT(void)
{
		OneWire_write_byte(0xCC);
		OneWire_write_byte(0x44);
}

//
void print_temperature()
{
		printf_string("\n\n\rTemperatures: ");
		for (int loop = 0; loop <= sensor_index; loop++)
		{
				float temp = OneWire_sensors[loop].temperature;
				int sensor_number = (loop + 1);
				temp = (temp/16);
				char s[] = {0};
				char s1[10];
				itoa(sensor_number, s1);
				ftoa(temp, s, 1);
				printf_string("\n\rSensor ");
				printf_string(s1);
				printf_string(".) ");
				printf_string(s);
				printf_string(" DEGREES CELCIUS");
		}
}

// Print Scratchpad
void print_scratchpad()
{
		printf_string("\n\n\r");
		printf_string("Scratchpad");
		printf_string(" contents:");
		for ( int loop = 0; loop <= sensor_index; loop++)
		{
				int temp = (loop + 1);
				char s[] = {0};
				itoa(temp, s);
				printf_string("\n\rSensor ");
				printf_string(s);
				printf_string(".) ");
				printf_byte(OneWire_sensors[loop].temperature);
				printf_byte(OneWire_sensors[loop].temperature >> 8);
				printf_string(" ");
				print_word(OneWire_sensors[loop].scratchpad_low << 8);
				print_word(OneWire_sensors[loop].scratchpad_high);
		}
}

// Print address
void print_address(void)
{
		printf_string("\n\n\r");
		printf_string("Addresses:");
		for ( int loop = 0; loop <= sensor_index; loop++)
		{
				printf_string("\n\r");
				printf_string("Sensor ");
				char s[] = {0};
				itoa(( loop + 1 ), s );
				printf_string(s);
				printf_string(".) ");
				print_word(OneWire_sensors[loop].address_low);
				print_word(OneWire_sensors[loop].address_high);	
		}
}
