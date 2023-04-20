/**
 ****************************************************************************************
 *
 * @file user_app.h
 *
 * @brief iBeacon user application header file.
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

#ifndef _USER_APP_H_
#define _USER_APP_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief 
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>

#include "rf_531.h"
//#include "spi_flash.h"
#include "battery.h"
#include "llc_util.h"
#include "lld_util.h"  
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
 
/* Advertising payload field sizes */
#define FLAGS_LEN										3
#define COMPANY_ID_LEN							2
#define BEACON_TYPE_LEN							2
#define UUID_LEN										16
#define major_LEN                   2
#define minor_LEN                   2

#define MAJOR_0												0x00
#define MAJOR_1												0x01

#define MINOR_0												0x00
#define MINOR_1												0x00

/* Fixed advertising fields */
#define FLAG_0											0x02
#define FLAG_1											0x01
#define FLAG_2											0x06

#define MAJOR_0_new									0x03
#define MAJOR_1_new									0x02
#define MINOR_0_new									0x07
#define MINOR_1_new									0x06

#define LENGTH											0x1A
#define TYPE												0xFF
#define COMPANY_ID_0								0x4C
#define COMPANY_ID_1								0x00
#define BEACON_TYPE_0								0x02
#define BEACON_TYPE_1								0x15

#define ADC_BATT_INTERVAL 8640   //24 * 60 * 60 / 16 = 8640 

/* Apple Proximity Beacon Specification - Release R1  */
typedef struct __attribute__ ((__packed__)) {
		uint8_t  flags[FLAGS_LEN];
	  uint8_t  length;
		uint8_t  type;
		uint8_t  company_id[COMPANY_ID_LEN];
	  uint8_t  beacon_type[BEACON_TYPE_LEN];
		uint8_t  uuid[UUID_LEN];
		uint8_t  major[major_LEN];
		uint8_t  minor[minor_LEN];
		uint8_t  measured_power;
} ibeacon_adv_payload_t;

typedef struct __attribute__ ((__packed__)) {
		uint8_t  flags[FLAGS_LEN];
	  uint8_t  length;
		uint8_t  type;
		uint8_t  company_id[COMPANY_ID_LEN];
	  uint8_t  beacon_type[BEACON_TYPE_LEN];
		uint8_t  uuid[UUID_LEN];
		uint8_t  major[major_LEN];
		uint8_t  minor[minor_LEN];
		uint8_t  measured_power;
} ibeacon_adv_payload_t_new;

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void user_app_adv_start(void);
void user_app_on_init(void);
void user_app_adv_start_new(void);
/// @} APP

#endif // _USER_APP_H_
