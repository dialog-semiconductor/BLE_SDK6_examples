
/**
 ****************************************************************************************
 *
 * @file HS3001.h
 *
 * @brief HS3001 module.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#define _HS3001_H_


#define DF_command 0xff

#include "rwip_config.h"             // SW configuration
#include "arch_console.h"
#include "gpio.h"
#include "user_periph_setup.h"
#include "i2c.h"
#include "math.h"















/*****************************************Prototypes***************************************************************************/
void HS3001_wakeup(void);
void Data_Fetch_all (uint8_t* buffer);
double HS3001_get_temperature (uint8_t *buffer);
double HS3001_get_humidity (uint8_t *buffer);