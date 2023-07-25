/**
 ****************************************************************************************
 *
 * @file user_app.h
 *
 * @brief iBeacon user application header file.
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
