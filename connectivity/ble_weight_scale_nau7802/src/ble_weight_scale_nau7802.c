/**
 ****************************************************************************************
 *
 * @file ble_weight_scale_nau7802.c
 *
 * @brief BLE weight scale project source code.
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 Dialog Semiconductor
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
# OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
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

#include "app_api.h"
#include "ble_weight_scale_nau7802.h"
#include "app_wsss.h"
#include "nau7802.h"
#include "custs1_task.h"
#include "user_custs1_def.h"
#include "arch_console.h"

/*
 * VARIABLE DEFINITIONS
 ****************************************************************************************
 */
uint8_t conidx																				__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
timer_hnd timer_id																		__SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY
struct wss_wt_meas* meas;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
		meas = (struct wss_wt_meas* ) ke_malloc(sizeof(struct wss_wt_meas),KE_MEM_ATT_DB);
    memset(meas, 0, sizeof(struct wss_wt_meas));
	
		#ifdef CFG_PRINTF
		arch_puts("Connection\n\r");
		#endif
		conidx = connection_idx;
		app_wsss_enable(connection_idx);
    default_app_on_connection(connection_idx, param);
	
		nau7802_init();
		nau7802_enable_adc();
	
		timer_id = app_easy_timer(WEIGHT_SCALE_UPDATE_INTERVAL,update_weight);
		
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
		app_easy_timer_cancel(timer_id);
    default_app_on_disconnect(param);
}

void user_app_on_init( void )
{
		#ifdef CFG_PRINTF
		arch_puts("User app init\n\r");
		#endif
		app_wsss_init();
		app_wsss_create_db();
}

void update_weight( void )
{
		uint32_t load;
		(void)nau7802_get_ug_load(&load);
		meas->weight = ((float)load)/10000000; // convert 10 mg to kg
		app_wsss_send_measurement(conidx, meas);
		app_easy_timer(WEIGHT_SCALE_UPDATE_INTERVAL,update_weight);
	
		#ifdef CFG_PRINTF
		arch_printf("Load: %d\n\r", (int)(meas->weight));
		#endif
}

uint32_t grams_byte_array_to_10milligrams(const uint8_t data[4])
{	
		uint32_t dat = (uint32_t)((((uint32_t)(data[3])) << 24) |
														  (((uint32_t)(data[2])) << 16)  |
														  (((uint32_t)(data[1])) << 8)  |
															 ((uint32_t)(data[0])));
		return dat*10000;
}

void calib_scale(ke_msg_id_t const msgid, struct custs1_val_write_ind const *param, 
								 ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
		uint32_t calibmicrograms = grams_byte_array_to_10milligrams(param->value);
		nau7802_calib_scale(calibmicrograms);
}

void user_catch_rest_hndl(ke_msg_id_t const msgid, void const *param, 
													ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    switch(msgid)
    {
        case CUSTS1_VAL_WRITE_IND:
        {
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

            switch (msg_param->handle)
            {

                case SVC1_IDX_ZERO_VAL:
										nau7802_zero_scale();
                    break;

                case SVC1_IDX_CALIB_VAL:
										calib_scale(msgid, msg_param, dest_id, src_id);
                    break;

                default:
                    break;
            }
        } break;

        default:
            break;
    }
}


/// @} APP