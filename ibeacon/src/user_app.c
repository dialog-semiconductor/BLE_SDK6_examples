/**
 ****************************************************************************************
 *
 * @file user_app.c
 *
 * @brief iBeacon user application source code.
 *
 * Copyright (c) 2012-2020 Dialog Semiconductor. All rights reserved.
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
#include <stdint.h>

#include "rf_531.h"
#include "spi_flash.h"
#include "user_app.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/* Select output power */
#undef  TX_POWER_2d5Bm

/* Advertising payload field sizes */
#define FLAGS_LEN										3
#define COMPANY_ID_LEN							2
#define BEACON_TYPE_LEN							2
#define UUID_LEN										16

/* Fixed advertising fields */
#define FLAG_0											0x02
#define FLAG_1											0x01
#define FLAG_2											0x06
#define LENGTH											0x1A
#define TYPE												0xFF
#define COMPANY_ID_0								0x4C
#define COMPANY_ID_1								0x00
#define BEACON_TYPE_0								0x02
#define BEACON_TYPE_1								0x15

#if defined (TX_POWER_2d5Bm)
#define MEASURED_POWER                          0xC7
#if !defined (__DA14531__)
    #error "Config error: Can not define TX_POWER_2d5Bm when device selected is DA14585 or DA14586."
#endif
#else    
/* Output power 0dBm */
#define MEASURED_POWER							0xC5
#endif

/* User defined advertising fields */
#define UUID_STR										"d4070339-6da4-4e50-a375-bade13be6daa"
#define MAJOR												0x0100
#define MINOR												0x0000

/* Set the advertising rate */
#define ADV_INTERVAL_ms							100

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/* Apple Proximity Beacon Specification - Release R1  */
typedef struct __attribute__ ((__packed__)) {
		uint8_t  flags[FLAGS_LEN];
	  uint8_t  length;
		uint8_t  type;
		uint8_t  company_id[COMPANY_ID_LEN];
	  uint8_t  beacon_type[BEACON_TYPE_LEN];
		uint8_t  uuid[UUID_LEN];
		uint16_t major;
		uint16_t minor;
		uint8_t  measured_power;
} ibeacon_adv_payload_t;

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

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
		
		#if defined (__DA14531__) && defined (TX_POWER_2d5Bm)
				/* Set output power to maximum value (+2.5dBm) */
				rf_pa_pwr_set(RF_TX_PWR_LVL_PLUS_2d5);
		#endif
	
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
		ibeacon_adv_payload_t adv_payload;
		struct gapm_start_advertise_cmd *cmd = app_easy_gap_non_connectable_advertise_get_active();	
	
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
		
		adv_payload.major = MAJOR;
		adv_payload.minor = MINOR;
		adv_payload.measured_power = MEASURED_POWER;
	
		memcpy(cmd->info.host.adv_data, &adv_payload, sizeof(ibeacon_adv_payload_t));
		cmd->info.host.adv_data_len = sizeof(ibeacon_adv_payload_t);
	
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
