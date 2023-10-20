/**
 ****************************************************************************************
 *
 * @file prod_test_rmt.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
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

#include "app_api.h"
#include "custs1_task.h"
#include "user_custs1_def.h"
#include "prod_test_rmt.h"

#include "dialog_commands.h"
#include "hci.h"
#include "dialog_prod.h"

/*
 * MACROS
 ****************************************************************************************
 */
#if defined (REMOTE_MODE)
#define VALID_OPERATION(x)		((x == UNMODULATED_CMD_MODE_TX) || \
								(x == UNMODULATED_CMD_MODE_RX) || \
								(x == UNMODULATED_CMD_MODE_OFF) ) ? VALID_PARAM : INVALID_OPERATION

#define VALID_CHANNEL(x)		((x > 39) || (x < 0)) ? INVALID_CHANNEL : VALID_PARAM
#define VALID_PAYLOAD(x)		((x > 7) || (x < 0)) ? INVALID_PAYLOAD_TYPE : VALID_PARAM
#define VALID_DATA_LEN(x)		((x > 255) || (x < 1)) ? INVALID_DATA_LEN : VALID_PARAM
#define VALID_SLEEP(x)			((x > 2) || (x < 0)) ? INVALID_SLEEP_MODE : VALID_PARAM
#endif // REMOTE_MODE

/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */
#ifdef BATCH_REMOTE_MODE
extern uint8_t standalone_state;
#endif


/*
 * LOCAL VARIABLES DECLARATION
 ****************************************************************************************
 */
#ifdef REMOTE_MODE
standalone_tests_t	pending_test;

const uint16_t supported_cmds[] = {
	HCI_UNMODULATED_ON_CMD_OPCODE,
	HCI_TX_START_CONTINUE_TEST_CMD_OPCODE,
	HCI_TX_END_CONTINUE_TEST_CMD_OPCODE,
	HCI_LE_TX_TEST_CMD_OPCODE,
	HCI_LE_TEST_END_CMD_OPCODE,
	HCI_START_PROD_RX_TEST_CMD_OPCODE,
	HCI_END_PROD_RX_TEST_CMD_OPCODE,
	HCI_SLEEP_TEST_CMD_OPCODE,
};

enum {
	VALID_PARAM,
	INVALID_OPCODE 			= 0x01,
	INVALID_CHANNEL 		= 0x02,
	INVALID_PAYLOAD_TYPE	= 0x04,
	INVALID_OPERATION		= 0x08,
	INVALID_DATA_LEN		= 0x10,
	INVALID_SLEEP_MODE		= 0x20,
	INVALID_PARAM			= 0x40,
};

#endif

timer_hnd DTM_timer_hnd				__SECTION_ZERO("retention_mem_area0");

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

#ifdef BATCH_REMOTE_MODE
static void next_command_exe_hnd(void);
#endif

#ifdef REMOTE_MODE
static void stop_command_exe_hnd(void);
static uint8_t validate_DTM_command(standalone_tests_t *pend);
#endif

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
	/* Start the test if we terminated the connection localy */
	if (param->reason == CO_ERROR_CON_TERM_BY_LOCAL_HOST)
	{
		
#if defined (BATCH_REMOTE_MODE)
		/* Start count down for advertising again */
		next_command_exe_hnd();
		/* Start executing the test commands */
		app_standalone_switch_test();
#endif // REMOTE_MODE
		
#if defined (REMOTE_MODE)	
		if(pending_test.command_on.opcode != 0x00)
			app_send_hci_msg(&pending_test.command_on);
		
		/* Delete the command just send for processing */
		memset(&pending_test.command_on, 0, sizeof(command_params_t));
		DTM_timer_hnd = app_easy_timer(END_OF_TEST_TIME, stop_command_exe_hnd);
#endif // REMOTE_MODE
	}
	else
	{
		default_advertise_operation();
	}
}

void user_app_restart(void)
{
	/* reset from RAM */
	SetBits16(SYS_CTRL_REG, SW_RESET, 1);
}
	
#if defined (BATCH_REMOTE_MODE)
static void next_command_exe_hnd(void)
{
	if(standalone_state == STANDALONE_STATE_EOT)
	{
		user_app_restart();
	}
	else
	{
		if(standalone_state == STANDALONE_STATE_INIT)
			standalone_state = STANDALONE_STATE_START_TEST;
		else
			standalone_state = STANDALONE_STATE_STOP_TEST;
		
		DTM_timer_hnd = app_easy_timer(END_OF_TEST_TIME, next_command_exe_hnd);
	}
}
#endif

#if defined (REMOTE_MODE)

static void stop_command_exe_hnd(void)
{

	if(pending_test.command_on.opcode == 0x00)
		app_send_hci_msg(&pending_test.command_off);
	
	ke_msg_id_t start_adv = app_easy_msg_set(user_app_restart);
	ke_msg_send_basic(start_adv, TASK_APP, TASK_APP);
}

static uint8_t validate_DTM_command(standalone_tests_t *pend)
{
	uint8_t ret = VALID_PARAM; 
	
	for(uint8_t i=0; i < sizeof(supported_cmds)/sizeof(uint16_t); i++)
	{
		if(pend->command_on.opcode == supported_cmds[i])
			break;
		else if (i == (sizeof(supported_cmds)/sizeof(uint16_t)) - 1)
			return INVALID_OPCODE;
	}
	
	switch(pend->command_on.opcode)
	{
		case HCI_UNMODULATED_ON_CMD_OPCODE:
			ret = VALID_CHANNEL(pend->command_on.param.unmod_param.channel);
			ret |= VALID_OPERATION(pend->command_on.param.unmod_param.operation);
			break;
		case HCI_LE_TX_TEST_CMD_OPCODE:
			ret = VALID_CHANNEL(pend->command_on.param.le_tx_param.channel); 
			ret |= VALID_DATA_LEN(pend->command_on.param.le_tx_param.data_length);
			ret |= VALID_PAYLOAD(pend->command_on.param.le_tx_param.payload_type);
			break;
		case HCI_TX_START_CONTINUE_TEST_CMD_OPCODE:
			ret = VALID_CHANNEL(pend->command_on.param.cont_mod_param.channel);
			ret |= VALID_PAYLOAD(pend->command_on.param.cont_mod_param.payload_type);
			break;
		case HCI_START_PROD_RX_TEST_CMD_OPCODE:
			ret = VALID_CHANNEL(pend->command_on.param.le_rx_param.channel);
			break;
		case HCI_LE_TEST_END_CMD_OPCODE:
		case HCI_END_PROD_RX_TEST_CMD_OPCODE:
		case HCI_TX_END_CONTINUE_TEST_CMD_OPCODE:
			ret = VALID_PARAM;
			break;
		case HCI_SLEEP_TEST_CMD_OPCODE:
			ret = VALID_SLEEP(pend->command_on.param.sleep_param.sleep_mode);
			break;
	}
	
	return ret;
}

#endif // REMOTE_MODE

void user_svc1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                      struct custs1_val_write_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
#if defined REMOTE_MODE
	/* Process the command */
	uint16_t opcode = (param->value[0] << 8) | param->value[1];
	
	pending_test.command_on.opcode = opcode;
	
	switch(opcode) {
	case HCI_UNMODULATED_ON_CMD_OPCODE:
		pending_test.command_on.param.unmod_param.operation = param->value[OPERATION];
		pending_test.command_on.param.unmod_param.channel = param->value[CHANNEL];
		pending_test.command_off.opcode = opcode;
		pending_test.command_off.param.unmod_param.operation = UNMODULATED_CMD_MODE_OFF;
		break;
	case HCI_LE_TX_TEST_CMD_OPCODE:
		pending_test.command_on.param.le_tx_param.channel = param->value[CHANNEL];
		pending_test.command_on.param.le_tx_param.data_length = param->value[DATA_LENGTH];
		pending_test.command_off.opcode = HCI_LE_TEST_END_CMD_OPCODE;
		pending_test.command_on.param.le_tx_param.payload_type = param->value[PAYLOAD_TYPE];
		break;
	case HCI_TX_START_CONTINUE_TEST_CMD_OPCODE:
		pending_test.command_on.param.cont_mod_param.channel = param->value[CHANNEL];
		pending_test.command_on.param.cont_mod_param.payload_type = param->value[PAYLOAD_TYPE];
		pending_test.command_off.opcode = HCI_TX_END_CONTINUE_TEST_CMD_OPCODE;
		break;
	case HCI_START_PROD_RX_TEST_CMD_OPCODE:
		pending_test.command_on.param.le_rx_param.channel = param->value[CHANNEL];
		pending_test.command_off.opcode = HCI_END_PROD_RX_TEST_CMD_OPCODE;
		break;
	case HCI_LE_TEST_END_CMD_OPCODE:
	case HCI_END_PROD_RX_TEST_CMD_OPCODE:
	case HCI_TX_END_CONTINUE_TEST_CMD_OPCODE:
		break;
	case HCI_SLEEP_TEST_CMD_OPCODE:
		pending_test.command_on.param.sleep_param.sleep_mode = CMD__SLEEP_MODE_EXTENDED;
		pending_test.command_off.param.sleep_param.sleep_mode = CMD__SLEEP_MODE_ACTIVE;
		break;
	}
	
	uint8_t ret = validate_DTM_command(&pending_test);
	
	if (ret != VALID_PARAM)
	{
		struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
													  prf_get_task_from_id(TASK_ID_CUSTS1),
													  TASK_APP,
													  custs1_val_ntf_ind_req,
													  DEF_SVC1_RESPONSE_CHAR_LEN);
		req->handle = SVC1_IDX_RESPONSE_VAL;
		req->length = DEF_SVC1_RESPONSE_CHAR_LEN;
		req->notification = true;
		memcpy(req->value, &ret, DEF_SVC1_RESPONSE_CHAR_LEN);

		ke_msg_send(req);
	}
	else
	{
#endif // REMOTE_MODE
	
#if defined (BATCH_REMOTE_MODE)
	uint8_t start_batch_exe = param->value[0];
	if (start_batch_exe != 0x00)
	{
#endif
		app_easy_gap_disconnect(param->conidx);
	}	
}

void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
    switch(msgid)
    {
        case CUSTS1_VAL_WRITE_IND:
        {
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

            switch (msg_param->handle)
            {
                case SVC1_IDX_CONTROL_POINT_VAL:
                    user_svc1_ctrl_wr_ind_handler(msgid, msg_param, dest_id, src_id);
                    break;

                default:
                    break;
            }
        } break;

        case GAPC_PARAM_UPDATED_IND:
        {
            // Cast the "param" pointer to the appropriate message structure
            struct gapc_param_updated_ind const *msg_param = (struct gapc_param_updated_ind const *)(param);

            // Check if updated Conn Params filled to preferred ones
            if ((msg_param->con_interval >= user_connection_param_conf.intv_min) &&
                (msg_param->con_interval <= user_connection_param_conf.intv_max) &&
                (msg_param->con_latency == user_connection_param_conf.latency) &&
                (msg_param->sup_to == user_connection_param_conf.time_out))
            {
            }
        } break;

        default:
            break;
    }
}
 

/// @} APP
