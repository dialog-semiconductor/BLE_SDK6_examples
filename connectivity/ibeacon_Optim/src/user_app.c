/**
 ****************************************************************************************
 *
 * @file user_app.c
 *
 * @brief iBeacon user application source code.
 *
 * Copyright (c) 2012-2021 Dialog Semiconductor
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_app.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/* Select output power */
/* Select output power */

/* User defined advertising fields */
#define UUID_STR										"c3b8d257-493e-49da-87ba-9294ee8acf94"


/* Set the advertising rate */
#define ADV_INTERVAL_ms							10000

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
ibeacon_adv_payload_t adv_payload;
ibeacon_adv_payload_t_new adv_payload_new;
/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
extern uint8_t change_bd_addr_flag;
extern uint8_t seed_region;
uint8_t batt_voltage = 0;
//daily timer variable, each iteration through the main while loop increments the interval_count
uint32_t interval_count = ADC_BATT_INTERVAL;
			
uint32_t  MajorMinor = 0;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/ 
static uint8_t hex2byte(char *hex);
static void uuid2hex(char *uuid, uint8_t *output);

/**
 ****************************************************************************************
 * @brief System start callback - gets called following a reset (but NOT on wakeup).
 * @return void
 ****************************************************************************************
*/
void user_app_on_init(void)
{

		/* If we have booted from flash then it will still be in active mode, power down to 
			 reduce current consumption. */
		(void)spi_flash_power_down();	

		default_app_on_init();

}

/**
 ****************************************************************************************
 * @brief Advertising start callback - called by the stack to allow the user to start 
 *        advertising.
 * @return void
 ****************************************************************************************
*/
void user_app_adv_start(void)
{
		struct gapm_start_advertise_cmd *cmd = app_easy_gap_non_connectable_advertise_get_active();	
	
    uint16_t Major_Addr = (uint16_t)((MajorMinor & 0xffff0000) >>16);
    uint16_t Minor_Addr = (uint16_t)((MajorMinor & 0x0000ffff));
	
		/* Setup the iBeacon advertising payload */
		adv_payload.flags[0]       = FLAG_0;
		adv_payload.flags[1]       = FLAG_1;
		adv_payload.flags[2]       = FLAG_2;
		adv_payload.length         = LENGTH;
		adv_payload.type           = TYPE;
		adv_payload.company_id[0]  = COMPANY_ID_0;
		adv_payload.company_id[1]  = COMPANY_ID_1;
		adv_payload.beacon_type[0] = BEACON_TYPE_0;
		adv_payload.beacon_type[1] = BEACON_TYPE_1;
	
		/* Convert ASCII UUID to hex format */
		uuid2hex((char *)UUID_STR, adv_payload.uuid);


	  //first byte of UUID is incremented upon each beacon in a sequence of ten (change_bd_addr_flag) beacons, simulating 10 unique, virtual beacons
	  adv_payload.uuid[0] = seed_region + change_bd_addr_flag;


		adv_payload.major[0] = MAJOR_0;
	  adv_payload.major[1] = MAJOR_1;
		
	  adv_payload.minor[0] = MINOR_0;
	  adv_payload.minor[1] = MINOR_1;
		
		
					
      //ADC read
      if(interval_count == ADC_BATT_INTERVAL) 
	    {
	       batt_voltage = battery_get_lvl(BATT_CR2032);
	       batt_voltage = (batt_voltage - 1800)/80;
	  
	       //ensure lowest (non-brownout value) is 1 to distinguish reset = 0;
	       if(batt_voltage == 0) batt_voltage = 1;
	  
	      // reserve batt_voltage of 0 to be reset condition
	      Major_Addr &= 0x0FFF;
	      Major_Addr |= ((batt_voltage & 0x000F) << 12);

	     interval_count = 0;
	    }

      interval_count++;
      
      //major
      adv_payload.major[0] = (uint8_t)((Major_Addr >> 8) & 0x00FF);
      adv_payload.major[1] = (uint8_t)(Major_Addr & 0x00FF);
      
      //minor
      adv_payload.minor[0] = (uint8_t)((Minor_Addr >> 8) & 0x00FF);
      adv_payload.minor[1] = (uint8_t)(Minor_Addr & 0x00FF);
		
		/* Set the advertising interval */
		cmd->intv_min = MS_TO_BLESLOTS(ADV_INTERVAL_ms);
		cmd->intv_max = MS_TO_BLESLOTS(ADV_INTERVAL_ms);
	
		/* Request advertising is started.. */
		app_easy_gap_non_connectable_advertise_start();

}

void user_app_adv_start_new(void)
{
		struct gapm_start_advertise_cmd *cmd = app_easy_gap_non_connectable_advertise_get_active();	
	  
	  uint16_t Major_Addr = (uint16_t)((MajorMinor & 0xffff0000) >>16);
    uint16_t Minor_Addr = (uint16_t)((MajorMinor & 0x0000ffff));
	
		/* Setup the iBeacon advertising payload */
		adv_payload_new.flags[0]       = FLAG_0;
		adv_payload_new.flags[1]       = FLAG_1;
		adv_payload_new.flags[2]       = FLAG_2;
		adv_payload_new.length         = LENGTH;
		adv_payload_new.type           = TYPE;
		adv_payload_new.company_id[0]  = COMPANY_ID_0;
		adv_payload_new.company_id[1]  = COMPANY_ID_1;
		adv_payload_new.beacon_type[0] = BEACON_TYPE_0;
		adv_payload_new.beacon_type[1] = BEACON_TYPE_1;
	
		/* Convert ASCII UUID to hex format */
		uuid2hex((char *)UUID_STR, adv_payload_new.uuid);
	

	  //first byte of UUID is incremented upon each beacon in a sequence of ten (change_bd_addr_flag) beacons, simulating 10 unique, virtual beacons
	  adv_payload_new.uuid[0] = seed_region + change_bd_addr_flag;

		
		adv_payload_new.major[0] = MAJOR_0_new;
	  adv_payload_new.major[1] = MAJOR_1_new;
		
	  adv_payload_new.minor[0] = MINOR_0_new;
	  adv_payload_new.minor[1] = MINOR_1_new;
		
		
							
      //ADC read
      if(interval_count == ADC_BATT_INTERVAL) 
	    {
	       batt_voltage = battery_get_lvl(BATT_CR2032);
	       batt_voltage = (batt_voltage - 1800)/80;
	  
	       //ensure lowest (non-brownout value) is 1 to distinguish reset = 0;
	       if(batt_voltage == 0) batt_voltage = 1;
	  
	      // reserve batt_voltage of 0 to be reset condition
	      Major_Addr &= 0x0FFF;
	      Major_Addr |= ((batt_voltage & 0x000F) << 12);

	     interval_count = 0;
	    }

      interval_count++;
      
      //major
      adv_payload_new.major[0] = (uint8_t)((Major_Addr >> 8) & 0x00FF);
      adv_payload_new.major[1] = (uint8_t)(Major_Addr & 0x00FF);
      
      //minor
      adv_payload_new.minor[0] = (uint8_t)((Minor_Addr >> 8) & 0x00FF);
      adv_payload_new.minor[1] = (uint8_t)(Minor_Addr & 0x00FF);
			
		/* Set the advertising interval */
		cmd->intv_min = MS_TO_BLESLOTS(ADV_INTERVAL_ms);
		cmd->intv_max = MS_TO_BLESLOTS(ADV_INTERVAL_ms);
	
		/* Request advertising is started.. */
		app_easy_gap_non_connectable_advertise_start();

}


/**
 ****************************************************************************************
 * @brief Converts UUID ASCII string to hex equivalent.
 * @return void
 ****************************************************************************************
*/
static void uuid2hex(char *uuid, uint8_t *output)
{
		uint8_t in_ix;
		uint8_t out_ix;
		for (out_ix = 0, in_ix = 0; out_ix < UUID_LEN; out_ix++, in_ix += 2) {
				if (uuid[in_ix] == '-') {
					 in_ix++;
				}
				output[out_ix] = hex2byte(&uuid[in_ix]);
		}	
}

/**
 ****************************************************************************************
 * @brief Converts ASCII character to hex equivalent.
 * @return void
 ****************************************************************************************
*/
static uint8_t hex2byte(char *hex)
{
		uint8_t byte = 0;
		uint8_t digits = 2;
	
		while (digits--) {
				uint8_t c = *hex++;	
				if (c >= '0' && c <= '9') c = c - '0';
				else if (c >= 'a' && c <= 'f') c = c - 'a' + 10;
				else if (c >= 'A' && c <= 'F') c = c - 'A' + 10;
				else c = 0;
				byte = byte << 4;
				byte = byte | (c & 0x0F);
		}
		return byte;
}

/// @} APP
