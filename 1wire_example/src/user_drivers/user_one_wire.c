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
#include "user_one_wire.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "spi.h"
#include "systick.h"
#include "user_uart.h"

bool 			cmp_id_bit 				= 0;
bool 			id_bit 						= 0;
uint32_t 	sensor_index 			= 0;

struct OneWire_sensor OneWire_sensors[MAX_NO_SENSORS];
struct OneWire_sensor empty[MAX_NO_SENSORS];

static void store_bit_in_address(bool bit, int bit_position);
static void id_systick_ISR(void);
static void comp_id_systick_ISR(void);
static void SPI_write_32bits(uint32_t dataToSend);
static void SPI_write_16bits(uint16_t dataToSend);

/**
****************************************************************************************
* @brief Interrupt handler for reading id bit in the Search algorithm
* @return void
****************************************************************************************
*/
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

/**
****************************************************************************************
* @brief Interrupt handler for reading complementary id bit in the Search algorithm
* @return void
****************************************************************************************
*/
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

/**
****************************************************************************************
* @brief 			Write 32 bits of data to MOSI (P0_6) from the 
	@dataToSend Parameter containing 32 bits of data to send
* @return 		void
****************************************************************************************
*/
void SPI_write_32bits(uint32_t dataToSend)
{
		SetWord16(SPI_RX_TX_REG1, (uint16_t)(dataToSend >> 16));
		SetWord16(SPI_RX_TX_REG0, (uint16_t)(dataToSend));
    do
    {
    } while (GetBits16(SPI_CTRL_REG, SPI_INT_BIT) == 0);  
		SetWord16(SPI_CLEAR_INT_REG, 0x01);                     // clear pending flag	
}

/**
****************************************************************************************
* @brief 			Write 16 bits of data to MOSI (P0_6) from the 
	@param[in] 	Parameter containing 16 bits of data to send
* @return 		void
****************************************************************************************
*/
void SPI_write_16bits(uint16_t dataToSend)
{
		SetWord16(SPI_RX_TX_REG0, (uint16_t)(dataToSend));
    do
    {
    } while (GetBits16(SPI_CTRL_REG, SPI_INT_BIT) == 0);  
		SetWord16(SPI_CLEAR_INT_REG, 0x01);                     // clear pending flag	
}

/**
****************************************************************************************
* @brief 			Transmits 1.5 µSec pulse initiating master readslot (>70 µSec in total)
* @return 		void
****************************************************************************************
*/
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

/**
****************************************************************************************
* @brief 			Initiates the OneWirebus. Since data is transmitted using the SPI-
*							controller, it is initialized accordingly. 
* @return 		void
****************************************************************************************
*/
void OneWire_init(void)
{
		SPI_Pad_t chipselect = {SPI_EN_GPIO_PORT, SPI_EN_GPIO_PIN};
		// 16MHz / DIV_8 = 2MHz => 1 bit = 0.5 µSec
		spi_init(&chipselect, SPI_MODE_32BIT, SPI_ROLE_MASTER, SPI_CLK_IDLE_POL_HIGH, SPI_PHA_MODE_0, SPI_MINT_ENABLE, SPI_XTAL_DIV_8);
		SetBits16(SPI_CTRL_REG, SPI_DO, 1);
}

/**
****************************************************************************************
* @brief 			Let master wait for (approximately 500µSec) to detect presence pulse. 
* @return 		void
****************************************************************************************
*/
void OneWire_presence(void)
{
		uint32_t dataToSend = 0xFFFFFFFF;
		for (int loop = 0; loop < 28; loop++)
		{
				SPI_write_32bits(dataToSend);  				
		}
}

/**
****************************************************************************************
* @brief 			Initiates a resetpulse of approximately 500 µSec.
* @return 		void
****************************************************************************************
*/
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

/**
****************************************************************************************
* @brief 			Transmit a byte value on the bus (LSB first)
*
*	@param			Byte to be transmitted
*
* @return 		void
****************************************************************************************
*/
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

/**
****************************************************************************************
* @brief 			Transmit a high level bit to the bus (Pulse < 15 µSec) using SPI.
* @return 		void
****************************************************************************************
*/
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

/**
****************************************************************************************
* @brief 			Transmit a low level bit to the bus (Pulse > 15 µSec) using SPI.
* @return 		void
****************************************************************************************
*/
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

/**
****************************************************************************************
* @brief 	1-Wire search algorithm. Tree search finding all sensor addresses (least
					significant 32 bits and most significant 32 bits) and storing them in 
					OneWire_sensors array. 
* @return void
****************************************************************************************
*/
void OneWire_SearchROM(void)
{
		// Search algorithm variables		
		bool 			last_device_flag 	= 0; 		// flag to indicate the search is complete
		bool 			search_direction 	= 0;
		uint32_t 	last_discrepancy 	= 0; 		// index that indicates from which bit the search should start
		uint32_t 	last_zero 				= 0;		// bit position of the last zero written where there was a discrepancy
		uint32_t 	id_bit_number			=	0; 		// Rom bit number value currently being searched
	
		for (int loop = 0; loop < MAX_NO_SENSORS; loop++)
		{
			OneWire_sensors[loop] = empty[loop];
		}
	
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

/**
****************************************************************************************
* @brief Store the bitvalue in the corresponding sensor address and bitposition
* @return void
****************************************************************************************
*/
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
}
