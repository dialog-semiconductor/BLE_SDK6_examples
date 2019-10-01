/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
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

#include "custs1.h"
#include "custs1_task.h"
#include "gpio.h"
#include "user_custs1_def.h"
#include "user_periph_setup.h"
#include "ble_notify_button_wakeup.h"
#include "wkupct_quadec.h"

/*
 * DEFINITIONS
 ****************************************************************************************
 */

#define LEGO_SEND_CMD_TO					(700)	//7s timeout
#define LEGO_SEND_CMD_TO_RET			(50)	//0.5s timeout

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
timer_hnd lego_tmr								__attribute__((section("retention_mem_area0"), zero_init));
timer_hnd lego_tmr_ret						__attribute__((section("retention_mem_area0"), zero_init)); 
 
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
*/ 

// Interrupt initialization function declarations
void GPIO_1_UP_ISR(void);
void GPIO_1_STOP_ISR(void);
void GPIO_1_BACK_ISR(void);
 
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
/* Send the second command */
static void lego_send_ret_cmd_timer_cb()
{
		uint8_t lego_up_ret_cmd[] = {0x05, 0x00, 0x45, 0x00, 0x00};
		uint8_t lego_up_ret_cmd_len = sizeof(lego_up_ret_cmd)/sizeof(lego_up_ret_cmd[0]);
	
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_CTRL_POINT_CHAR_LEN);
		
    req->handle = SVC1_IDX_CONTROL_POINT_VAL;
    req->length = lego_up_ret_cmd_len;
    req->notification = true;
    memcpy(req->value, &lego_up_ret_cmd, lego_up_ret_cmd_len);
	
    ke_msg_send(req);
}

static void lego_send_stop_ret_cmd_timer_cb()
{
		uint8_t lego_up_ret_cmd[] = {0x05, 0x00, 0x45, 0x00, 0x00};
		uint8_t lego_up_ret_cmd_len = sizeof(lego_up_ret_cmd)/sizeof(lego_up_ret_cmd[0]);
	
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_CTRL_POINT_CHAR_LEN);
		
    req->handle = SVC1_IDX_CONTROL_POINT_VAL;
    req->length = lego_up_ret_cmd_len;
    req->notification = true;
    memcpy(req->value, &lego_up_ret_cmd, lego_up_ret_cmd_len);
	
    ke_msg_send(req);
}

static void lego_send_back_ret_cmd_timer_cb()
{
		uint8_t lego_back_ret_cmd[] = {0x05, 0x00, 0x45, 0x00, 0x00};
		uint8_t lego_back_ret_cmd_len = sizeof(lego_back_ret_cmd)/sizeof(lego_back_ret_cmd[0]);
	
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_CTRL_POINT_CHAR_LEN);
		
    req->handle = SVC1_IDX_CONTROL_POINT_VAL;
    req->length = lego_back_ret_cmd_len;
    req->notification = true;
    memcpy(req->value, &lego_back_ret_cmd, lego_back_ret_cmd_len);
	
    ke_msg_send(req);
}

void app_wakeup_press_cb(void)
{
		#if !defined (__DA14531__)
				if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))
		#endif
				{
						periph_init();
				}
						
        arch_ble_force_wakeup();	
				
				// If wakeup is generated by button P0_6
				if(!GPIO_GetPinStatus(GPIO_SW3_PORT, GPIO_SW3_PIN))						
				{
					app_easy_wakeup_set(GPIO_1_UP_ISR);
				}
				// If wakeup is generated by button P1_1
				else if (!GPIO_GetPinStatus(GPIO_SW2_PORT, GPIO_SW2_PIN))
				{
					app_easy_wakeup_set(GPIO_1_STOP_ISR);
				}
				else if (!GPIO_GetPinStatus(GPIO_SW1_PORT, GPIO_SW1_PIN))
				{
					app_easy_wakeup_set(GPIO_1_BACK_ISR);
				}
				wkupct_enable_irq((WKUPCT_PIN_SELECT(GPIO_SW2_PORT, GPIO_SW2_PIN) | WKUPCT_PIN_SELECT(GPIO_SW3_PORT, GPIO_SW3_PIN) | WKUPCT_PIN_SELECT(GPIO_SW1_PORT, GPIO_SW1_PIN)), 						// When enabling more than one interruptsource use OR bitoperation. WKUPCT_PIN_SELECT will make sure the appropriate bit in the register is set. 
							(WKUPCT_PIN_POLARITY(GPIO_SW2_PORT, GPIO_SW2_PIN, WKUPCT_PIN_POLARITY_LOW) | WKUPCT_PIN_POLARITY(GPIO_SW3_PORT, GPIO_SW3_PIN, WKUPCT_PIN_POLARITY_LOW) | WKUPCT_PIN_POLARITY(GPIO_SW1_PORT, GPIO_SW1_PIN, WKUPCT_PIN_POLARITY_LOW) ),	// When enabling more than one interruptsource use OR bitoperation. WKUPCT_PIN_POLARITY will make sure the appriopriate bit in the register is set.
							1,																																																													// Number of events needed to trigger interrupt
							30);
				
        app_easy_wakeup();

}

void GPIO_1_UP_ISR(void)
{
		uint8_t lego_up_cmd[] = {0x05, 0x00, 0x45, 0x00, 0x01};
		uint8_t lego_up_cmd_len = sizeof(lego_up_cmd)/sizeof(lego_up_cmd[0]);
	
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_CTRL_POINT_CHAR_LEN);
		
    req->handle = SVC1_IDX_CONTROL_POINT_VAL;
    req->length = lego_up_cmd_len;
    req->notification = true;
    memcpy(req->value, &lego_up_cmd, lego_up_cmd_len);

  	lego_tmr = app_easy_timer(LEGO_SEND_CMD_TO_RET, lego_send_ret_cmd_timer_cb);
		
    ke_msg_send(req);
}

void GPIO_1_STOP_ISR(void)
{
		uint8_t lego_up_cmd[] = {0x05, 0x00, 0x45, 0x00, 0x7f};
		uint8_t lego_up_cmd_len = sizeof(lego_up_cmd)/sizeof(lego_up_cmd[0]);
	
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_CTRL_POINT_CHAR_LEN);
		
    req->handle = SVC1_IDX_CONTROL_POINT_VAL;
    req->length = lego_up_cmd_len;
    req->notification = true;
    memcpy(req->value, &lego_up_cmd, lego_up_cmd_len);

  	lego_tmr = app_easy_timer(LEGO_SEND_CMD_TO_RET, lego_send_stop_ret_cmd_timer_cb);
		
    ke_msg_send(req);
}

void GPIO_1_BACK_ISR(void)
{
		uint8_t lego_back_cmd[] = {0x05, 0x00, 0x45, 0x00, 0xff};
		uint8_t lego_back_cmd_len = sizeof(lego_back_cmd)/sizeof(lego_back_cmd[0]);
	
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_CTRL_POINT_CHAR_LEN);
		
    req->handle = SVC1_IDX_CONTROL_POINT_VAL;
    req->length = lego_back_cmd_len;
    req->notification = true;
    memcpy(req->value, &lego_back_cmd, lego_back_cmd_len);

  	lego_tmr = app_easy_timer(LEGO_SEND_CMD_TO_RET, lego_send_back_ret_cmd_timer_cb);
		
    ke_msg_send(req);
}

/* User defined behavior for handling client's requests */
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
	uint8_t	lego_brick_wr_1[] = {0x04, 0x00, 0x08, 0x06};
	uint8_t lego_brick_wr_2[] = {0x06, 0x00, 0x22, 0x00, 0x00, 0x01};
	uint8_t lego_brick_wr_3[] = {0x06, 0x00, 0x22, 0x00, 0x00, 0x02};
	uint8_t lego_brick_wr_4[] = {0x06, 0x00, 0x22, 0x00, 0x00, 0x03};
	uint8_t lego_brick_wr_5[] = {0x06, 0x00, 0x22, 0x00, 0x00, 0x05};
	uint8_t lego_brick_wr_6[] = {0x06, 0x00, 0x22, 0x00, 0x00, 0x80};

	uint8_t lego_15_cmd_1[] = {0x0f, 0x00, 0x04, 0x00, 0x01, 0x37, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10};

	uint8_t lego_14_cmd_1[] = {0x0e, 0x00, 0x44, 0x00, 0x00, 0x01, 0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x80, 0x3f};
	uint8_t lego_14_cmd_2[] = {0x0e, 0x00, 0x44, 0x00, 0x00, 0x02, 0x00, 0x00, 0xc8, 0xc2, 0x00, 0x00, 0xc8, 0x42};
	uint8_t lego_14_cmd_3[] = {0x0e, 0x00, 0x44, 0x00, 0x00, 0x03, 0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x80, 0x3f};

	uint8_t lego_8_cmd_1[] = {0x08, 0x00, 0x44, 0x00, 0x00, 0x05, 0x18, 0x00};

	uint8_t lego_10_cmd_1[] = {0x0a, 0x00, 0x44, 0x00, 0x00, 0x80, 0x01, 0x00, 0x02, 0x00};


	switch(msgid)
	{
			case CUSTS1_VAL_WRITE_IND:
			{
					struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

					switch (msg_param->handle)
					{
							case SVC1_IDX_CONTROL_POINT_VAL:
									if(!memcmp(&lego_brick_wr_1[0], &msg_param->value[0], msg_param->length)) {
										//uint8_t lego_15_cmd_1[] = {0x0f, 0x00, 0x04, 0x00, 0x01, 0x37, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10};
										uint8_t lego_15_cmd_len = sizeof(lego_15_cmd_1)/sizeof(lego_15_cmd_1[0]);

										struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																																					prf_get_task_from_id(TASK_ID_CUSTS1),
																																					TASK_APP,
																																					custs1_val_ntf_ind_req,
																																					DEF_SVC1_CTRL_POINT_CHAR_LEN);
										// First message
										req->handle = SVC1_IDX_CONTROL_POINT_VAL;
										req->length = lego_15_cmd_len;
										req->notification = true;
										memcpy(req->value, &lego_15_cmd_1, lego_15_cmd_len);
										ke_msg_send(req);											
									}
									else if(!memcmp(&lego_brick_wr_2[0], &msg_param->value[0], msg_param->length)) {
										uint8_t lego_14_cmd_len = sizeof(lego_14_cmd_1)/sizeof(lego_14_cmd_1[0]);
										struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																																					prf_get_task_from_id(TASK_ID_CUSTS1),
																																					TASK_APP,
																																					custs1_val_ntf_ind_req,
																																					DEF_SVC1_CTRL_POINT_CHAR_LEN);
										// Second message
										req->handle = SVC1_IDX_CONTROL_POINT_VAL;
										req->length = lego_14_cmd_len;
										req->notification = true;
										memcpy(req->value, &lego_14_cmd_1, lego_14_cmd_len);
										ke_msg_send(req);											
									}
									else if(!memcmp(&lego_brick_wr_3[0], &msg_param->value[0], msg_param->length)) {
										uint8_t lego_14_cmd_len = sizeof(lego_14_cmd_2)/sizeof(lego_14_cmd_2[0]);
										struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																																					prf_get_task_from_id(TASK_ID_CUSTS1),
																																					TASK_APP,
																																					custs1_val_ntf_ind_req,
																																					DEF_SVC1_CTRL_POINT_CHAR_LEN);
										// Third message
										req->handle = SVC1_IDX_CONTROL_POINT_VAL;
										req->length = lego_14_cmd_len;
										req->notification = true;
										memcpy(req->value, &lego_14_cmd_2, lego_14_cmd_len);
										ke_msg_send(req);											
									}
									else if(!memcmp(&lego_brick_wr_4[0], &msg_param->value[0], msg_param->length)) {
										uint8_t lego_14_cmd_len = sizeof(lego_14_cmd_3)/sizeof(lego_14_cmd_3[0]);
										struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																																					prf_get_task_from_id(TASK_ID_CUSTS1),
																																					TASK_APP,
																																					custs1_val_ntf_ind_req,
																																					DEF_SVC1_CTRL_POINT_CHAR_LEN);
										// Fourth message
										req->handle = SVC1_IDX_CONTROL_POINT_VAL;
										req->length = lego_14_cmd_len;
										req->notification = true;
										memcpy(req->value, &lego_14_cmd_3, lego_14_cmd_len);
										ke_msg_send(req);											
									}
									else if(!memcmp(&lego_brick_wr_5[0], &msg_param->value[0], msg_param->length)) {
										uint8_t lego_8_cmd_len = sizeof(lego_8_cmd_1)/sizeof(lego_8_cmd_1[0]);
										struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																																					prf_get_task_from_id(TASK_ID_CUSTS1),
																																					TASK_APP,
																																					custs1_val_ntf_ind_req,
																																					DEF_SVC1_CTRL_POINT_CHAR_LEN);
										// Fifth message
										req->handle = SVC1_IDX_CONTROL_POINT_VAL;
										req->length = lego_8_cmd_len;
										req->notification = true;
										memcpy(req->value, &lego_8_cmd_1, lego_8_cmd_len);
										ke_msg_send(req);											
									}
									else if(!memcmp(&lego_brick_wr_6[0], &msg_param->value[0], msg_param->length)) {
										uint8_t lego_10_cmd_len = sizeof(lego_10_cmd_1)/sizeof(lego_10_cmd_1[0]);
										struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
																																					prf_get_task_from_id(TASK_ID_CUSTS1),
																																					TASK_APP,
																																					custs1_val_ntf_ind_req,
																																					DEF_SVC1_CTRL_POINT_CHAR_LEN);
										// Sixth message
										req->handle = SVC1_IDX_CONTROL_POINT_VAL;
										req->length = lego_10_cmd_len;
										req->notification = true;
										memcpy(req->value, &lego_10_cmd_1, lego_10_cmd_len);
										ke_msg_send(req);											
									}										
									break;

							default:
									break;
					}
			} break;
			
			default:
					break;
	}
}

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    //app_easy_wakeup_set(app_wakeup_cb);

		wkupct_enable_irq((WKUPCT_PIN_SELECT(GPIO_SW2_PORT, GPIO_SW2_PIN) | WKUPCT_PIN_SELECT(GPIO_SW3_PORT, GPIO_SW3_PIN) | WKUPCT_PIN_SELECT(GPIO_SW1_PORT, GPIO_SW1_PIN)), 						// When enabling more than one interruptsource use OR bitoperation. WKUPCT_PIN_SELECT will make sure the appropriate bit in the register is set. 
									(WKUPCT_PIN_POLARITY(GPIO_SW2_PORT, GPIO_SW2_PIN, WKUPCT_PIN_POLARITY_LOW) | WKUPCT_PIN_POLARITY(GPIO_SW3_PORT, GPIO_SW3_PIN, WKUPCT_PIN_POLARITY_LOW)
										| WKUPCT_PIN_POLARITY(GPIO_SW1_PORT, GPIO_SW1_PIN, WKUPCT_PIN_POLARITY_LOW)),	// When enabling more than one interruptsource use OR bitoperation. WKUPCT_PIN_POLARITY will make sure the appriopriate bit in the register is set.
									1,																																																													// Number of events needed to trigger interrupt
									30);																																																												// Debounce time ranging from 0 to 63
	
		wkupct_register_callback(app_wakeup_press_cb);	// sets this function as wake-up interrupt callback	

    default_app_on_connection(connection_idx, param);

}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}
 

/// @} APP
