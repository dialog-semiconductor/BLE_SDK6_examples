/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 *
 * @brief Peripheral project Custom1 Server implementation source code.
 *
 * Copyright (c) 2015-2019 Dialog Semiconductor. All rights reserved.
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
#include "custs1_task.h"
#include "user_custs1_def.h"
#include "user_custs1_impl.h"
#include "user_peripheral.h"
#include "user_periph_setup.h"
#include "HS3001.h"
#include "arch_console.h"

#if defined (CFG_USE_INTERNAL_TEMP_SENSOR) && (__DA14531__)
#include "adc.h"
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

ke_msg_id_t timer_temperature_ind      				__SECTION_ZERO("retention_mem_area0"); 											//@RETENTION MEMORY
ke_msg_id_t timer_HUMIDITY_ind      			   	__SECTION_ZERO("retention_mem_area0"); 											//@RETENTION MEMORY
char temperature_ntf_string[9] 			    			__SECTION_ZERO("retention_mem_area0");                      //@RETENTION MEMORY
char hum_ntf_string[9] 			    			        __SECTION_ZERO("retention_mem_area0");                      //@RETENTION MEMORY

ke_msg_id_t timer_hum_ntf      							  __SECTION_ZERO("retention_mem_area0"); 											//@RETENTION MEMORY
char cc[9] 			    					  	            __SECTION_ZERO("retention_mem_area0");                      //@RETENTION MEMORY

#if defined (CFG_USE_INTERNAL_TEMP_SENSOR) && (__DA14531__)
uint8_t previous_temperature                  __SECTION_ZERO("retention_mem_area0");
#else 
double previous_temperature 								  __SECTION_ZERO("retention_mem_area0"); 
double previous_hum 								          __SECTION_ZERO("retention_mem_area0"); 
#endif
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
 
void user_send_temperature_humidity_ind(void)
{
    uint8_t length_temp;
	  uint8_t length_hum;
		uint8_t buffer[4];
		
    
#if defined (CFG_USE_INTERNAL_TEMP_SENSOR) && (__DA14531__)
    
        adc_config_t temp_config = {
        .input_mode = ADC_INPUT_MODE_SINGLE_ENDED,
        .input = ADC_INPUT_SE_TEMP_SENS,
    };
        
    adc_init(&temp_config);
    
    int8_t temperature = adc_get_temp();
    
    adc_disable();
#else
	 
		Data_Fetch_all (buffer);
		double temperature = HS3001_get_temperature(buffer);
		double hum = HS3001_get_humidity(buffer);
	
#endif
    
	if ((temperature != previous_temperature) || (hum != previous_hum)){
	
		previous_temperature = temperature;
		previous_hum = hum;
		
    length_temp = snprintf(temperature_ntf_string, TEMPERATURE_DATA, SNPRINT_FORMAT, temperature);
		length_hum = snprintf(hum_ntf_string, HUMIDITY_DATA, SNPRINT_FORMAT, hum);
		
		//Allocate a new message
		struct custs1_val_ntf_ind_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																prf_get_task_from_id(TASK_ID_CUSTS1),
																TASK_APP,
																custs1_val_ntf_ind_req,
																length_temp);
																
		req->conidx = 0;                                        //Connection ID to send the data to (this application can only have one connection(0))
		req->notification = false;                               //Data is sent as a notification and not as indication
		req->handle = USER_IDX_TEMPERATURE_VAL_VAL;             //The handle of the characteristic we want to write to
		req->length = length_temp;                          		    	//Data length in bytes
		memcpy(req->value, temperature_ntf_string, length_temp);			//Copy the string to the message
		
		ke_msg_send(req);                                       //Send the message to the task
		


		req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																prf_get_task_from_id(TASK_ID_CUSTS1),
																TASK_APP,
																custs1_val_ntf_ind_req,
																length_hum);
		
		req->conidx = 0;                                           //Connection ID to send the data to (this application can only have one connection(0))
		req->notification = false;                                    //Data is sent as a notification and not as indication
		req->handle = USER_IDX_HUMIDITY_VAL_VAL;                    //The handle of the characteristic we want to write to
		req->length = length_hum;                          		    	//Data length in bytes
		memcpy(req->value, hum_ntf_string, length_hum);			       //Copy the string to the message
		
		ke_msg_send(req);                                       //Send the message to the task
	
	}
    
	timer_temperature_ind = app_easy_timer(INDICATION_DELAY/10, user_send_temperature_humidity_ind); //Set a timer for NOTIFICATION_DELAY ms
	



	
}

 void user_temperature_message_handler(struct custs1_val_write_ind const *param)
{

	if(param->value[0]){
		//If the client subscribed to the notification
		if(timer_temperature_ind == EASY_TIMER_INVALID_TIMER){ 
			//Start the timer if it is not running
			timer_temperature_ind = app_easy_timer(INDICATION_DELAY/10, user_send_temperature_humidity_ind); //Set a timer for NOTIFICATION_DELAY ms
		}
	}
	else{
		//If the client unsubscribed from the notification
		if(timer_temperature_ind != EASY_TIMER_INVALID_TIMER){ 
			//Stop the timer if it is running
			app_easy_timer_cancel(timer_temperature_ind);
			timer_temperature_ind = EASY_TIMER_INVALID_TIMER;
		}
	}

}


 void user_humidity_message_handler(struct custs1_val_write_ind const *param)
{

	if(param->value[0]){
		//If the client subscribed to the notification
		if(timer_HUMIDITY_ind == EASY_TIMER_INVALID_TIMER){ 
			//Start the timer if it is not running
       timer_HUMIDITY_ind = app_easy_timer(INDICATION_DELAY/10, user_send_temperature_humidity_ind); //Set a timer for indication_DELAY ms
		}
	}
	else{
		//If the client unsubscribed from the notification
		if(timer_temperature_ind != EASY_TIMER_INVALID_TIMER){ 
			//Stop the timer if it is running
			app_easy_timer_cancel(timer_temperature_ind);
			timer_temperature_ind = EASY_TIMER_INVALID_TIMER;
		}
	}

}
		

void user_svc1_rest_att_info_req_handler(ke_msg_id_t const msgid,
                                            struct custs1_att_info_req const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    struct custs1_att_info_rsp *rsp = KE_MSG_ALLOC(CUSTS1_ATT_INFO_RSP,
                                                   src_id,
                                                   dest_id,
                                                   custs1_att_info_rsp);
    // Provide the connection index.
    rsp->conidx  = app_env[param->conidx].conidx;
    // Provide the attribute index.
    rsp->att_idx = param->att_idx;
    // Force current length to zero.
    rsp->length  = 0;
    // Provide the ATT error code.
    rsp->status  = ATT_ERR_WRITE_NOT_PERMITTED;

    ke_msg_send(rsp);
}
