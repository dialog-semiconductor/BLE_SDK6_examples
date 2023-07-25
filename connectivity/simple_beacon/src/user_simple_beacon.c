/**
 ****************************************************************************************
 *
 * @file user_simple_beacon.c
 *
 * @brief User Simple Beacon source code.
 *
 * Copyright (c) 2012-2021 Renesas Electronics Corporation and/or its affiliates
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

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "gap.h"
#include "app_easy_timer.h"
#include "user_simple_beacon.h"
#include "co_bt.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

timer_hnd app_adv_data_update_timer_used            __attribute__((section(".bss.")));//@RETENTION MEMORY

uint8_t initial_adv_data[ADV_DATA_LEN]              __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t initial_adv_data_len                        __attribute__((section(".bss."))); //@RETENTION MEMORY

uint8_t user_adv_data[ADV_DATA_LEN]                 __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t user_adv_data_len                           __attribute__((section(".bss."))); //@RETENTION MEMORY

uint8_t initial_scan_rsp_data[SCAN_RSP_DATA_LEN]    __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t initial_scan_rsp_data_len                   __attribute__((section(".bss."))); //@RETENTION MEMORY

uint8_t user_scan_rsp_data[SCAN_RSP_DATA_LEN]       __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t user_scan_rsp_data_len                      __attribute__((section(".bss."))); //@RETENTION MEMORY

uint8_t added_adv_data_len                          __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t added_scan_rsp_data_len                     __attribute__((section(".bss."))); //@RETENTION MEMORY

uint8_t adv_data_cursor                             __attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t scan_rsp_data_cursor                        __attribute__((section(".bss.")));//@RETENTION MEMORY

bool 	update_adv_data                             __attribute__((section(".bss."))); //@RETENTION MEMORY

uint8_t user_store_data[USER_DATA_LEN] 				__attribute__((section(".bss.")));  //@RETENTION MEMORY

uint8_t user_store_data_len                       	__attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t user_store_data_counter                   	__attribute__((section(".bss."))); //@RETENTION MEMORY
uint8_t append_data                 			  	__attribute__((section(".bss.")));//@RETENTION MEMORY

bool  	append_data_empty 							__attribute__((section(".bss."))); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize data values kept in global variables.
 * @return void
 ****************************************************************************************
 */
static void data_init()
{
	// Load initial advertising data and length
	user_adv_data_len = initial_adv_data_len;
	memcpy(user_adv_data, initial_adv_data, user_adv_data_len);

	// Load initial scan response data and length
	user_scan_rsp_data_len = initial_scan_rsp_data_len;
	memcpy(user_scan_rsp_data, initial_scan_rsp_data, user_scan_rsp_data_len);

	// Copy the USER_DATA into the RetRAM
	memcpy(user_store_data, USER_DATA ,USER_DATA_LEN );

	// Calculate the length of USER_DATA
	user_store_data_len = ( sizeof( user_store_data )-1 );

	added_adv_data_len = 0;
	added_scan_rsp_data_len = 0;

	adv_data_cursor = initial_adv_data_len;
	scan_rsp_data_cursor = initial_scan_rsp_data_len;

	update_adv_data = true;
}

/**
 ****************************************************************************************
 * @brief Checks if  user_store_data[USER_DATA_LEN] buffer is empty
 * @return true if user_store_data[USER_DATA_LEN] empty , otherwise false
 ****************************************************************************************
 */
static bool is_user_data_empty(void)
{
	if ( user_store_data_counter <= user_store_data_len )	
	{
		append_data = user_store_data[ user_store_data_counter ];
		user_store_data_counter++ ; 
		
		return false;
	}
	
	return true;
}

/**
 ****************************************************************************************
 * @brief Adds a byte to ADV_DATA if there is space.
 * @return true if ADV_DATA is full, otherwise false
 ****************************************************************************************
 */
static bool is_adv_data_full(void)
{
	if ( ( user_adv_data_len < ADV_DATA_LEN ) && update_adv_data )
	{
		added_adv_data_len++;
		adv_data_cursor++;

		user_adv_data[initial_adv_data_len] = added_adv_data_len;

		user_adv_data[adv_data_cursor] = append_data;
		user_adv_data_len = initial_adv_data_len + added_adv_data_len + 1;

		return false;
	}
	
	return true;
}

/**
 ****************************************************************************************
 * @brief Adds a byte to SCAN_RSP_DATA if there is space.
 * @return true if SCAN_RSP_DATA is full, otherwise false
 ****************************************************************************************
 */
static bool is_scan_rsp_data_full(void)
{
	if ( ( user_scan_rsp_data_len < SCAN_RSP_DATA_LEN ) && !update_adv_data )
	{
		added_scan_rsp_data_len++;
		scan_rsp_data_cursor++;

		user_scan_rsp_data[initial_scan_rsp_data_len] = added_scan_rsp_data_len;

		user_scan_rsp_data[scan_rsp_data_cursor] = append_data;
		user_scan_rsp_data_len = initial_scan_rsp_data_len + added_scan_rsp_data_len + 1;
					
		return false;
	}

	return true;
}

/**
****************************************************************************************
* @brief Advertisement data update timer callback function.
* @return void
****************************************************************************************
*/
static void adv_data_update_timer_cb(void)
{
	app_adv_data_update_timer_used = EASY_TIMER_INVALID_TIMER;

	if ( !is_user_data_empty() )			
	{
		if ( is_adv_data_full() && update_adv_data )
		{
			// Ready to switch to SCAN_RSP_DATA
			update_adv_data = false;
			
			/*
			 * Since the ADV_DATA has reached its limit (31-bytes),
			 * we start to fill the SCAN_RSP_DATA
			 */
			
			//Check for Scan Response Data length 
			is_scan_rsp_data_full();
			
			/*
			 * Stop advertising when switching from 
			 * ADV_NONCONN_IND to ADV_SCAN_IND type and vice versa
			 * On the fly update of the ADV_DATA or SCAN_RSP_DATA 
			 * cannot be applied when the advertising type is dynamically changed.	
			 */
			
			app_easy_gap_advertise_stop();
		 
			// Exit here
			return;
		}

		if ( is_scan_rsp_data_full() && !update_adv_data)
		{
			// Ready to switch to ADV_DATA
			update_adv_data = true;
			
			/*
			 * Stop advertising when switching from
			 * ADV_NONCONN_IND to ADV_SCAN_IND type and vice versa
			 * On the fly update of the ADV_DATA or SCAN_RSP_DATA 
			 * cannot be applied when the advertising type is dynamically changed.	
			 */
			
			app_easy_gap_advertise_stop();

			// Exit here
			return;
		}
	}

	// Update advertising data on the fly
	app_easy_gap_update_adv_data(user_adv_data, user_adv_data_len, user_scan_rsp_data, user_scan_rsp_data_len);

	// Restart timer for the next advertising update
	app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);
}


/**
 ****************************************************************************************
 * @brief Non-connectable Advertising function.
 * @return void
 ****************************************************************************************
*/
void user_app_adv_start(void)
{
    // Schedule the next advertising data update
    app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);

    struct gapm_start_advertise_cmd *cmd = app_easy_gap_non_connectable_advertise_get_active();

    // Store initial advertising data and length
    initial_adv_data_len = cmd->info.host.adv_data_len;
    memcpy(initial_adv_data, cmd->info.host.adv_data, initial_adv_data_len);

    // Store initial scan response data and length
    initial_scan_rsp_data_len = cmd->info.host.scan_rsp_data_len;
    memcpy(initial_scan_rsp_data, cmd->info.host.scan_rsp_data, initial_scan_rsp_data_len);

    // Initialize data
    data_init();

    app_easy_gap_non_connectable_advertise_start();
}

/**
 ****************************************************************************************
 * @brief Non-connectable advertising completion function.
 * @param[in] status Command complete event message status
 * @return void
 ****************************************************************************************
*/
void user_app_adv_nonconn_complete(uint8_t status)
{
    // If advertising was canceled then update advertising data and start advertising again
    if (status == GAP_ERR_CANCELED)
    {
        struct gapm_start_advertise_cmd *cmd = app_easy_gap_non_connectable_advertise_get_active();

        // Initialize ADV_DATA and SCAN_RSP_DATA to restart the process
        if (update_adv_data)
        {
            // Initialize data
            data_init();
        }

        // Load advertising data and length
        cmd->info.host.adv_data_len = user_adv_data_len;
        memcpy(cmd->info.host.adv_data, user_adv_data, user_adv_data_len);

        // Load scan response data and length
        cmd->info.host.scan_rsp_data_len = user_scan_rsp_data_len;
        memcpy(cmd->info.host.scan_rsp_data, user_scan_rsp_data, user_scan_rsp_data_len);

        app_easy_gap_non_connectable_advertise_start();

        // Schedule the next advertising data update
        app_adv_data_update_timer_used = app_easy_timer(APP_ADV_DATA_UPDATE_TO, adv_data_update_timer_cb);
    }
}

/// @} APP
