/**
 ****************************************************************************************
 *
 * @file user_custs1_def.h
 *
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
 */

#ifndef _USER_CUSTS1_DEF_H_
#define _USER_CUSTS1_DEF_H_

/**
 ****************************************************************************************
 * @defgroup USER_CONFIG
 * @ingroup USER
 * @brief Custom Server 1 (CUSTS1) profile database definitions.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "attm_db_128.h"

/*
 * DEFINES
 ****************************************************************************************
 */

// Service 1 of the custom server 1
#define DEF_SVC1_UUID_128            			{0xb7, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}

#define DEF_CUST1_SERVER_TX_UUID_128      {0xb8, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}
#define DEF_CUST1_SERVER_RX_UUID_128      {0xba, 0x5c, 0x49, 0xd2, 0x04, 0xa3, 0x40, 0x71, 0xa0, 0xb5, 0x35, 0x85, 0x3e, 0xb0, 0x83, 0x07}

//length = MTU - 3, change it when increasing MTU or use DLE
#define DEF_CUST1_SERVER_TX_CHAR_LEN      (247 - 3)
#define DEF_CUST1_SERVER_RX_CHAR_LEN      (247 - 3)

#define CUST1_SERVER_TX_USER_DESC     "Server TX Data"
#define CUST1_SERVER_RX_USER_DESC     "Server RX Data"

/// Custom1 Service Data Base Characteristic enum
enum
{
    // Custom Service 1
    SVC1_IDX_SVC = 0,

    CUST1_IDX_SERVER_RX_CHAR,
    CUST1_IDX_SERVER_RX_VAL,
    CUST1_IDX_SERVER_RX_USER_DESC,
	
	  CUST1_IDX_SERVER_TX_CHAR,
    CUST1_IDX_SERVER_TX_VAL,
    CUST1_IDX_SERVER_TX_NTF_CFG,
    CUST1_IDX_SERVER_TX_USER_DESC,

    CUSTS1_IDX_NB
};

/// @} USER_CONFIG

#endif // _USER_CUSTS1_DEF_H_