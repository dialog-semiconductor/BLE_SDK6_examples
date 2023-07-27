/**
 ****************************************************************************************
 *
 * @file i2cm.h
 *
 * @brief Wrapper to provide simplified API to SDK I2C master driver.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation and/or its affiliates
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
 ***************************************************************************************
 */

#ifndef _I2CM_H_
#define _I2CM_H_

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
#include <stdbool.h>

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
int i2cm_read(uint8_t dev_addr, uint8_t * const data, uint8_t len, bool add_stop);
int i2cm_write(uint8_t dev_addr, uint8_t const * const data, uint8_t len, bool add_stop);

/// @} APP

#endif // _I2CM_H_