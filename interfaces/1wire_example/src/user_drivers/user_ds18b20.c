/**
 ****************************************************************************************
 *
 * @file ds18b20.c
 *
 * @brief DS18B20 temperature sensor(s) driver - source code.
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

#include "user_ds18b20.h"
#include "gpio.h"
#include "user_periph_setup.h"
#include "arch_console.h"
#include "math.h"
#include "stdlib.h"

extern struct OneWire_sensor OneWire_sensors[MAX_NO_SENSORS]; 	// Contains addresses of all 1-Wire sensors
extern int sensor_index;																				// Value indicating the number of sensors on the bus

int bitcount = 0;
int read_sensor = 0;
int ds18b20_index;

struct temperature_sensor temperature_sensors[MAX_NO_SENSORS];	// Contains all atributes of all DS18B20 sensors

static void ftoa(float n, char *res, int afterpoint);
static int itoa(int n, char s[]);
static void reverse(char s[]);

/**
****************************************************************************************
* @brief 	Interrupt handler for reading the scratchpad contents 
* @return void
****************************************************************************************
*/
void readScratchpad_systick_ISR(void){
		systick_stop();
		bool inputlevel = GPIO_GetPinStatus(ONEWIRE_RX_PORT, ONEWIRE_RX_PIN);
		if (inputlevel)
		{
				if (bitcount < 16)
				{
						temperature_sensors[read_sensor].temperature |= 1 << bitcount;	
				}
				else if (bitcount >= 16 &&bitcount < 40)
				{
						temperature_sensors[read_sensor].scratchpad_low |= 1 << (bitcount - 16);
				}
				else
				{
						temperature_sensors[read_sensor].scratchpad_high |= 1 << (bitcount - 40);
				}
		}
		else
		{
				if (bitcount < 16)
				{
						temperature_sensors[read_sensor].temperature &= ~(1 << (bitcount));							
				}
				else if (bitcount >= 16 &&bitcount < 40)
				{
						temperature_sensors[read_sensor].scratchpad_low &= ~(1 << (bitcount - 16));	
				}
				else
				{
						temperature_sensors[read_sensor].scratchpad_high &= ~(1 << (bitcount - 40));
				}
		}
}

/**
****************************************************************************************
* @brief 	Initiates 1-Wire rom search and stores the DS18B20 least significant 32 bits
					and most significant 32 bits in their corresponding temperature_sensor instance
					and member. 
* @return void
****************************************************************************************
*/
void get_address(void)
{
		OneWire_SearchROM();
		ds18b20_index = -1;
		for (int loop = 0; loop <= sensor_index; loop++)
		{
				// check family code to see if sensor is temperature sensor 
				if ((OneWire_sensors[loop].address_low & 0xFF) == 0x28)
				{
						ds18b20_index++;
						temperature_sensors[loop].address_high = OneWire_sensors[loop].address_high;
						temperature_sensors[loop].address_low = OneWire_sensors[loop].address_low;
				}

		}
}

/**
****************************************************************************************
* @brief 	Issue a match ROM command to target a single DS18B20 sensor.
* @param	the index of the temperature_sensor instance to be targeted
* @return void
****************************************************************************************
*/
void OneWire_matchRom(int sensor_number)
{
		OneWire_reset();
		OneWire_presence();
		OneWire_write_byte(0x55); // Rom command
		OneWire_write_byte((temperature_sensors[sensor_number].address_low 		>>   0) & 0xFF);
		OneWire_write_byte((temperature_sensors[sensor_number].address_low 		>>   8) & 0xFF);
		OneWire_write_byte((temperature_sensors[sensor_number].address_low 		>> 	16) & 0xFF);
		OneWire_write_byte((temperature_sensors[sensor_number].address_low 		>> 	24) & 0xFF);
		OneWire_write_byte((temperature_sensors[sensor_number].address_high		>>   0) & 0xFF);
		OneWire_write_byte((temperature_sensors[sensor_number].address_high  	>>   8) & 0xFF);
		OneWire_write_byte((temperature_sensors[sensor_number].address_high  	>> 	16) & 0xFF);
		OneWire_write_byte((temperature_sensors[sensor_number].address_high  	>> 	24) & 0xFF);
}

/**
****************************************************************************************
* @brief 	Issues a convert temperature command (0x44) to all DS18B20 sensors.
* @return void
****************************************************************************************
*/
void OneWire_readScratchpad(void)
{
		for (int loop_i = 0; loop_i <= ds18b20_index; loop_i++)
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

/**
****************************************************************************************
* @brief 	Issues a convert temperature command (0x44) to all DS18B20 sensors.
* @return void
****************************************************************************************
*/
void OneWire_ConvertT(void)
{
		OneWire_write_byte(0xCC);
		OneWire_write_byte(0x44);
}

/**
****************************************************************************************
* @brief 	Writes all DS18B20 temperature values to serial terminal over UART
* @return void
****************************************************************************************
*/
void print_temperature()
{
		#ifdef CFG_PRINTF
				arch_printf("\n\n\rTemperatures: ");
				for (int loop = 0; loop <= ds18b20_index; loop++)
				{
						float temp = temperature_sensors[loop].temperature;
						temp = (temp/16);
						char s[] = {0};
						ftoa(temp, s, 1);
						arch_printf("\n\rSensor ");
						arch_printf("%d", (loop + 1));
						arch_printf(".) ");
						arch_printf("%s", s);
						arch_printf(" DEGREES CELCIUS");
				}
		#endif
}

/**
****************************************************************************************
* @brief 	Writes all DS18B20 scratchpad contents to serial terminal over UART
* @return void
****************************************************************************************
*/
void print_scratchpad()
{
		#ifdef CFG_PRINTF
				arch_printf("\n\n\rScratchpad contents:");
				for ( int loop = 0; loop <= ds18b20_index; loop++)
				{
						int temp = (loop + 1);
						char s[] = {0};
						itoa(temp, s);
						arch_printf("\n\rSensor %s.) 0x", s);
						arch_printf("%02X", temperature_sensors[loop].temperature							>>	 0	& 0xFF);
						arch_printf("%02X ",(temperature_sensors[loop].temperature 						>> 	 8) & 0xFF);
						arch_printf("%02X", ((temperature_sensors[loop].scratchpad_low <<  8)	>>	 8) & 0xFF);
						arch_printf("%02X", ((temperature_sensors[loop].scratchpad_low <<  8)	>>  16) & 0xFF);
						arch_printf("%02X", ((temperature_sensors[loop].scratchpad_low <<  8) >>  24) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].scratchpad_high 				>>   0) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].scratchpad_high 				>>   8) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].scratchpad_high 				>>  16) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].scratchpad_high 				>>  24) & 0xFF);
				}
		#endif
}

/**
****************************************************************************************
* @brief 	Writes all DS18B20 temperature values to serial terminal over UART
* @return void
****************************************************************************************
*/
void print_address(void)
{
		#ifdef CFG_PRINTF
				arch_printf("\n\n\rAddresses:");
				for ( int loop = 0; loop <= ds18b20_index; loop++)
				{
						arch_printf("\n\rSensor %d.) 0x", (loop + 1));
						arch_printf("%02X", (temperature_sensors[loop].address_low 	>> 	 0) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].address_low	>>	 8) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].address_low	>>  16) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].address_low  >>  24) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].address_high >>   0) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].address_high >>   8) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].address_high >>  16) & 0xFF);
						arch_printf("%02X", (temperature_sensors[loop].address_high >>  24) & 0xFF);						
				}	
		#endif
}

/**
****************************************************************************************
* @brief 	Reverses string for int/float conversion purpose
*	@param	char s[]: String to be reversed
* @return void
****************************************************************************************
*/
static void reverse(char s[])
 {
    int i, j;
    char c;
 
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
 }

/**
****************************************************************************************
* @brief 	Convert int to string for floating point conversion purpose
*	@param	int n		: Integer to be converted 
					char s[]: Resulting string from integer conversion
* @return void
****************************************************************************************
*/
static int itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
		return i;
}

/**
****************************************************************************************
* @brief 	Convert float to string for printing purpose
*	@param	float n  			: Floating point value to be converted 
					char *res			: Resulting string from floating point conversion
					int afterpoint: Specify number of digits after point
* @return void
****************************************************************************************
*/
static void ftoa(float n, char *res, int afterpoint) 
{ 
    int ipart = (int)n; 
  
    float fpart = n - (float)ipart; 
  
    int i = itoa(ipart, res); 
  
    // check for display option after point 
    if (afterpoint != 0) 
    { 
        res[i] = '.';  // add dot 
        fpart = abs(fpart * pow(10, afterpoint)); 
        itoa((int)fpart, res + i + 1); 
    } 
} 

