
/**
 ****************************************************************************************
 *
 * @file HS3001.h
 *
 * @brief HS3001 module.
 *
 * Copyright (C) 2012-2023 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
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