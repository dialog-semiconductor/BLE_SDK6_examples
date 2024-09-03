/**
 ****************************************************************************************
 *
 * @file dci_config.h
 *
 * @brief DCI configuration project header file.
 *
 * Copyright (c) 2012-2023 Renesas Electronics Corporation and/or its affiliates
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

#ifndef _DCI_CONFIG_H_
#define _DCI_CONFIG_H_
#include "da1458x_config_advanced.h"
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
***********************WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*****************************************
* This should be undefined once used in production.  The purpose of this is to test the module with RAM 
* use ONLY.   This modifies the reset procedure to do a soft reset back into RAM and could potentially
* cause isses with the scatterload.
*/
#define DCI_RAMAP_ADDR0_TEST	

/*
* This is the array size for total trace depth on the stack the larger this is,
* the more call traces made available.
*/
#define DCI_CALL_DEPTH						(6)

/*
* Number of resets stored in RAM.  If user is offloading this flash set this to one, transfer the data
* into flash and clear the data to save RAM space.
*/
#define DCI_NUM_RESETS_STORED			(2)


#if (CFG_RET_DATA_UNINIT_SIZE <= 0)
#error "CFG_RET_DATA_UNINIT_SIZE needs to be greater than zero"
#endif 

#define DCI_MEMORY_CHECK


#ifdef CFG_PRINTF

/*
 * User definable print function to be called with adf_print_verbose.
 * undefine this function if printing is not needed, as verbose printing has significant overhead.
 */
#define DCI_PRINTF(...)                         arch_printf(__VA_ARGS__)
#define DCI_PRINT_VERBOSE


#else 

#define DCI_PRINTF(...) 												()
#undef DCI_PRINT_VERBOSE

#endif 
					

/*
*  Enable the example DCI service for transferring data via BLE to a host.  
*/
#define ENABLE_DCI_SERVICE

/// @} APP

#endif //_DCI_CONFIG_H_

