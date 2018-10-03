/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 *
 * @brief Peripheral project Custom1 Server implementation source code.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gpio.h"
#include "app_api.h"
#include "app.h"
#include "prf_utils.h"
#include "custs1.h"
#include "user_custs1_def.h"
#include "user_custs1_impl.h"
#include "user_peripheral.h"
#include "user_periph_setup.h"
#include "systick.h"
#include "one_wire.h"
#include "user_uart.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
extern struct OneWire_sensor OneWire_sensors[10];
extern uint32_t sensor_index;

ke_msg_id_t timer_used      __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY
uint16_t indication_counter __attribute__((section("retention_mem_area0"), zero_init)); //@RETENTION MEMORY

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
 
void user_svc1_onewire_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                     struct custs1_val_write_ind const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    uint8_t val = 0;
    memcpy(&val, &param->value[0], param->length);

    if (val == SEARCH_ROM)
		{
				OneWire_SearchROM();
				print_address();
				sent_address();
		}
		else if (val == CONVERT_T)
		{
        OneWire_ConvertT();
				printf_string("\n\n\rCONVERTING");
				printf_string("TEMPERATURE...");
				systick_wait(750000); // maximum temperature conversion time
				printf_string("DONE");				
		}
		else if (val == READ_SCRATCHPAD)
		{
        OneWire_readScratchpad();
				print_scratchpad();
				sent_scratchpad();
		}
		else if (val == PRINT_TEMPERATURE)
		{
        print_temperature();
				sent_temperatures();
		}
}

void sent_address(void)
{
		for (int loop = 0; loop <= sensor_index; loop ++)
		{
				struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																															prf_get_task_from_id(TASK_ID_CUSTS1),
																															TASK_APP,
																															custs1_val_ntf_ind_req,
																															DEF_SVC1_ONEWIRE_DATA_CHAR_LEN);
					
				//req->conhdl = app_env->conhdl;
				req->handle = SVC1_IDX_ONEWIRE_DATA_VAL;
				req->length = sizeof(OneWire_sensors[loop].address_low);
				req->notification = true;
				
				memcpy(req->value, &OneWire_sensors[loop].address_low, sizeof(OneWire_sensors[loop].address_low));
				//memcpy(req->value, &high, DEF_SVC1_ONEWIRE_DATA_CHAR_LEN);
				ke_msg_send(req);
				
						struct custs1_val_ntf_ind_req *req1 = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																															prf_get_task_from_id(TASK_ID_CUSTS1),
																															TASK_APP,
																															custs1_val_ntf_ind_req,
																															DEF_SVC1_ONEWIRE_DATA_CHAR_LEN);
					
				//req->conhdl = app_env->conhdl;
				req1->handle = SVC1_IDX_ONEWIRE_DATA_VAL;
				req1->length = sizeof(OneWire_sensors[loop].address_low);
				req1->notification = true;
						
				memcpy(req1->value, &OneWire_sensors[loop].address_high, sizeof(OneWire_sensors[loop].address_high));
				//memcpy(req->value, &high, DEF_SVC1_ONEWIRE_DATA_CHAR_LEN);
				ke_msg_send(req1);			
				
		}
}

void sent_temperatures(void)
{
		for (int loop = 0; loop <= sensor_index; loop ++)
		{
				struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																															prf_get_task_from_id(TASK_ID_CUSTS1),
																															TASK_APP,
																															custs1_val_ntf_ind_req,
																															DEF_SVC1_ONEWIRE_DATA_CHAR_LEN);
					
				//req->conhdl = app_env->conhdl;
				req->handle = SVC1_IDX_ONEWIRE_DATA_VAL;
				req->length = sizeof(OneWire_sensors[loop].temperature);
				req->notification = true;
				
				memcpy(req->value, &OneWire_sensors[loop].temperature, sizeof(OneWire_sensors[loop].temperature));
				//memcpy(req->value, &high, DEF_SVC1_ONEWIRE_DATA_CHAR_LEN);
				ke_msg_send(req);		
		}
}

void sent_scratchpad(void)
{
		for (int loop = 0; loop <= sensor_index; loop ++)
		{
				struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																															prf_get_task_from_id(TASK_ID_CUSTS1),
																															TASK_APP,
																															custs1_val_ntf_ind_req,
																															DEF_SVC1_ONEWIRE_DATA_CHAR_LEN);
					
				//req->conhdl = app_env->conhdl;
				req->handle = SVC1_IDX_ONEWIRE_DATA_VAL;
				req->length = sizeof(OneWire_sensors[loop].scratchpad_low);
				req->notification = true;
				
				memcpy(req->value, &OneWire_sensors[loop].scratchpad_low, sizeof(OneWire_sensors[loop].scratchpad_low));
				//memcpy(req->value, &high, DEF_SVC1_ONEWIRE_DATA_CHAR_LEN);
				ke_msg_send(req);
				
						struct custs1_val_ntf_ind_req *req1 = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																															prf_get_task_from_id(TASK_ID_CUSTS1),
																															TASK_APP,
																															custs1_val_ntf_ind_req,
																															DEF_SVC1_ONEWIRE_DATA_CHAR_LEN);
					
				//req->conhdl = app_env->conhdl;
				req1->handle = SVC1_IDX_ONEWIRE_DATA_VAL;
				req1->length = sizeof(OneWire_sensors[loop].scratchpad_high);
				req1->notification = true;
						
				memcpy(req1->value, &OneWire_sensors[loop].scratchpad_high, sizeof(OneWire_sensors[loop].scratchpad_high));
				//memcpy(req->value, &high, DEF_SVC1_ONEWIRE_DATA_CHAR_LEN);
				ke_msg_send(req1);			
				
		}
}
