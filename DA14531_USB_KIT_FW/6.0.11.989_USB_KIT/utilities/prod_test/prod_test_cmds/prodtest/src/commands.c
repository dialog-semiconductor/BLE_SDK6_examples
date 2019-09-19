/**
 ****************************************************************************************
 *
 * @file commands.c
 *
 * @brief Commands source code file.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include "uart.h"
#include "queue.h"
#include "host_hci.h"
#include "commands.h"

extern int g_com_port_number;

long parse_number(int *return_status, const char * str)
{
	long result;
	char *endptr;
	
	*return_status = 0;

	result = strtol(str, &endptr, 10);
	
	if (endptr[0])
		*return_status = 1;
		
	if (errno)
		*return_status = 1;

	return result;
}

static uint8_t parse_uint8(int *error_code, const char * str)
{
    long result = 0;

    result = parse_number(error_code, str);

    if (*error_code)
        return 0;

    // check: a number between 0 and 255
    if (result < 0 || 255 < result  )
        *error_code = 1;

    return (result & 0xFF );
}

static uint16_t parse_uint16(int *error_code, const char * str)
{
    long result = 0;

    result = parse_number(error_code, str);

    if (*error_code)
        return 0;

    // check: a number between 0 and 255
    if (result < 0 || 0xFFFF < result  )
        *error_code = 1;

    return (result & 0xFFFF );
}

static uint32_t parse_hex_uint32(int *error_code, const char *str)
{
    int rc;
    uint32_t tmp;

    rc = sscanf(str, "%x" , &tmp);

    if (rc != 1) {
        *error_code = -1;
    }

    return tmp;
}

static int parse_frequency(int *return_status, const char * str)
{
	long result;
	char *endptr;
	
	*return_status = 0;

	result = strtol(str, &endptr, 10);
	
	if (endptr[0])
		*return_status = 1;
		
	if (errno)
		*return_status = 1;
	
	// check: an even number between 2402 and 2480
	if (result < 2402  || 2480 < result || (result % 2) )
		*return_status = 1;

	result = (result - 2402) / 2;

	return result;
}

static int parse_data_length(int *return_status, const char * str)
{
	long result;
	char *endptr;
	
	*return_status = 0;

	result = strtol(str, &endptr, 10);
	
	if (endptr[0])
		*return_status = 1;

	if (errno)
		*return_status = 1;

	// check: an number between 0 and 255
	if (result < 0 || 255 < result )
		*return_status = 1;

	return result;
}

static int parse_payload_type(int *return_status, const char * str)
{
	long result;
	char *endptr;
	
	*return_status = 0;

	result = strtol(str, &endptr, 10);

	if (endptr[0])
		*return_status = 1;
	
	if (errno)
		*return_status = 1;
	
	// check: an number between 0 and 7 
	if (result < 0 || 7 < result  )
		*return_status = 1;

	return result;
}

static int parse_number_of_packets(int *error_code, const char * str)
{
	long result = 0;
	
	result = parse_number(error_code, str);

	if (*error_code)
		return 0;
	
	// check: an number between 1 and 65535
	if (result < 1 || 65535 < result  )
		*error_code = 1;

	return result;
}

static int parse_bd_addr(uint8_t bd_addr[6], const char *str)
{
    int rc;
    uint32_t bdaddr[6];

    rc = sscanf(str, "%02X:%02X:%02X:%02X:%02X:%02X" ,  &bdaddr[5], &bdaddr[4], &bdaddr[3], &bdaddr[2], &bdaddr[1], &bdaddr[0] );

    if (rc != 6) {
        return -1; // invalid BD address
    }

    bd_addr[5] = bdaddr[5];
    bd_addr[4] = bdaddr[4];
    bd_addr[3] = bdaddr[3];
    bd_addr[2] = bdaddr[2];
    bd_addr[1] = bdaddr[1];
    bd_addr[0] = bdaddr[0];

    return 0;
}

// GPIO's
enum
{
    INVALID_GPIO = 0xFF,

    P0_0 = 00,
    P0_1 = 01,
    P0_2 = 02,
    P0_3 = 03,
    P0_4 = 04,
    P0_5 = 05,
    P0_6 = 06,
    P0_7 = 07,

    P1_0 = 10,
    P1_1 = 11,
    P1_2 = 12,
    P1_3 = 13,
    P1_4 = 14,
    P1_5 = 15,

    P2_0 = 20,
    P2_1 = 21,
    P2_2 = 22,
    P2_3 = 23,
    P2_4 = 24,
    P2_5 = 25,
    P2_6 = 26,
    P2_7 = 27,
    P2_8 = 28,
    P2_9 = 29,
    
    P3_0 = 30,
    P3_1 = 31,
    P3_2 = 32,
    P3_3 = 33,
    P3_4 = 34,
    P3_5 = 35,
    P3_6 = 36,
    P3_7 = 37
};

typedef struct {
    char *name;
    uint8_t code;
} gpio_name_t;

gpio_name_t gpio_names[] = {
    {"P0_0", P0_0 },
    {"P0_1", P0_1 },
    {"P0_2", P0_2 },
    {"P0_3", P0_3 },
    {"P0_4", P0_4 },
    {"P0_5", P0_5 },
    {"P0_6", P0_6 },
    {"P0_7", P0_7 },

    {"P1_0", P1_0 },
    {"P1_1", P1_1 },
    {"P1_2", P1_2 },
    {"P1_3", P1_3 },
    {"P1_4", P1_4 },
    {"P1_5", P1_5 },

    {"P2_0", P2_0 },
    {"P2_1", P2_1 },
    {"P2_2", P2_2 },
    {"P2_3", P2_3 },
    {"P2_4", P2_4 },
    {"P2_5", P2_5 },
    {"P2_6", P2_6 },
    {"P2_7", P2_7 },
    {"P2_8", P2_8 },
    {"P2_9", P2_9 },
    
    {"P3_0", P3_0 },
    {"P3_1", P3_1 },
    {"P3_2", P3_2 },
    {"P3_3", P3_3 },
    {"P3_4", P3_4 },
    {"P3_5", P3_5 },
    {"P3_6", P3_6 },
    {"P3_7", P3_7 },

    {0, 0} // end marker
};


/**
 ****************************************************************************************
 * @brief Parse string GPIO name and return a GPIO code.
 *
 *  @param[in] argc  GPIO name
 *
 * @return GPIO code
 ****************************************************************************************
 */
uint8_t parse_gpio(const char *s)
{
    int kk;
    uint8_t ret = INVALID_GPIO;

    for( kk = 0; gpio_names[kk].name != 0; kk++) 
    {
        if ( 0 == strcmp(gpio_names[kk].name,s) )
        {
            ret = gpio_names[kk].code;
            break;
        }
    } 
    return ret;
}



#define UNMODULATED_CMD_MODE_OFF 0x4F
#define UNMODULATED_CMD_MODE_TX  0x54
#define UNMODULATED_CMD_MODE_RX  0x52

uint8_t parse_unmodulated_mode(int *return_status, const char *str)
{
	uint8_t result = 0;
	
	*return_status = 0;

	if ( 0 == strcmp(str, "OFF") || 0 == strcmp(str, "off"))
	{
		result = UNMODULATED_CMD_MODE_OFF;
	}
	else if ( 0 == strcmp(str, "TX") || 0 == strcmp(str, "tx"))
	{
		result = UNMODULATED_CMD_MODE_TX;
	}
	else if ( 0 == strcmp(str, "RX") || 0 == strcmp(str, "rx") )
	{
		result = UNMODULATED_CMD_MODE_RX;
	}
	else
	{
		*return_status = 1;
	}

	return result;
}

/*
 ****************************************************************************************
 * @brief Command handler for "Start testmode TX with param length is 3 (default)"
 *
 * command line: prodtest starttest_tx_param_len_3 <FREQUENCY> <DATA_LENGTH> <PAYLOAD_TYPE>
 *
 *  @param[in] argc		Command line argument count.
 *  @param[in] argv		Command line arguments.
 *
 * @return error code on failure / 0 on success.
 ****************************************************************************************
*/
int starttest_tx_param_len_3_handler(int argc, char **argv)
{
	uint8_t frequency, data_length, payload_type;
	int return_status = 0;
	hci_evt_t *evt = NULL;

	// HCI event return parameters:
	uint8_t status; // see: BT spec 4.0 vol 2- part D - error codes

	if (argc != 4)
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}
	
	// parse FREQUENCY
	frequency = parse_frequency(&return_status, argv[1]);

	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_FREQUENCY_ARG;
		goto exit_command_handler;
	}

	// parse DATA_LENGTH 
	data_length = parse_data_length(&return_status, argv[2]);
	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_DATA_LENGTH_ARG; 
		goto exit_command_handler;
	}
	
	// parse PAYLOAD_TYPE 
	payload_type = parse_payload_type(&return_status, argv[3]);
	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_PAYLOAD_TYPE_ARG;
		goto exit_command_handler;
	}

	//
	// execute ..
	//

	// open COM port, initialize rx thread  and queue
	if (!InitUART(g_com_port_number, 115200))  
		InitTasks();
	else
	{
		return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
		goto exit_command_handler;
	}

	// send HCI command
	hci_tx_test(/*uint8_t*/ frequency, /*uint8_t*/  data_length, /*uint8_t*/ payload_type);

	// receive reply event
	evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

	if (evt == NULL)
	{
		return_status = SC_RX_TIMEOUT; // rx timeout 
		goto exit_command_handler;
	}
	
	handle_hci_event(evt); ////////////////////////////////////////////// print evt

	if ( !( evt->event == 0x0E 
		&& evt->length == 4
		&& evt->parameters[1] == 0x1E
		&& evt->parameters[2] == 0x20)
	)
	{
		return_status = SC_UNEXPECTED_EVENT; // unexpected event
		goto exit_command_handler;
	}

	status = evt->parameters[3];

	// check command completion event status
	if (status != 0 )
	{
		return_status = SC_HCI_STANDARD_ERROR_CODE_BASE + status;
	}
	
exit_command_handler:
	if(evt)
		free(evt);

	printf("status = %d\n", return_status);
	
	return return_status;
};

/*
 ****************************************************************************************
 * @brief Command handler for "Start testmode TX with param length is 5"
 *
 *  @param[in] argc		Command line argument count.
 *  @param[in] argv		Command line arguments.
 *
 * @return error code on failure / 0 on success.
 ****************************************************************************************
*/
int starttest_tx_param_len_5_handler(int argc, char **argv)
{
	uint8_t frequency, data_length, payload_type;
	uint16_t number_of_packets;
	int return_status = 0;
        hci_evt_t *evt = NULL;


	//check number of arguments
	if (argc != 5)
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}
	// parse FREQUENCY
	frequency = parse_frequency(&return_status, argv[1]);
	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_FREQUENCY_ARG;
		goto exit_command_handler;
	}
	// parse DATA_LENGTH 
	data_length = parse_data_length(&return_status, argv[2]);
	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_DATA_LENGTH_ARG; 
		goto exit_command_handler;
	}
	// parse PAYLOAD_TYPE 
	payload_type = parse_payload_type(&return_status, argv[3]);
	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_PAYLOAD_TYPE_ARG;
		goto exit_command_handler;
	}
	// parse number_of_packets argument
	number_of_packets = parse_number_of_packets(&return_status, argv[4]);
	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_NUMBER_OF_PACKETS_ARG;
		goto exit_command_handler;
	}

	//
	// execute ..
	//

	// open COM port, initialize rx thread  and queue
	if (!InitUART(g_com_port_number, 115200))  
		InitTasks();
	else
	{
		return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
		goto exit_command_handler;
	}

	// send HCI command
	hci_dialog_tx_test(/*uint8_t*/ frequency, /*uint8_t*/  data_length, /*uint8_t*/ payload_type, number_of_packets);

	// receive command completion event
	evt = hci_recv_event_wait(60000); // wait for 60 seconds

	if (evt == NULL)
	{
		return_status = SC_RX_TIMEOUT; // rx timeout 
		goto exit_command_handler;
	}
	
	handle_hci_event(evt); ////////////////////////////////////////////// print evt

	if ( !( evt->event == 0x0E
		&& evt->length == 3
                && (evt->parameters[1] == (HCI_TX_TEST_CMD_OPCODE & 0x00FF))
                && (evt->parameters[2] == HCI_TX_TEST_CMD_OPCODE>>8))
	)
	{
		return_status = SC_UNEXPECTED_EVENT; // unexpected event
		goto exit_command_handler;
	}

	// there are no return parameters
	
exit_command_handler:
	if(evt)
		free(evt);

	printf("status = %d\n", return_status);
	
	return return_status;
}

/*
 ****************************************************************************************
 * @brief Command handler for "Start testmode RX (default)"
 *
 *  @param[in] argc		Command line argument count.
 *  @param[in] argv		Command line arguments.
 *
 * @return error code on failure / 0 on success.
 ****************************************************************************************
*/
int starttest_rx_default_handler(int argc, char **argv)
{
	uint8_t frequency;
	int return_status = 0;
	hci_evt_t *evt = NULL;

	uint8_t status; // see: BT spec 4.0 vol 2- part D - error codes

	// check number of args
	if (argc != 2)
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}
	// parse FREQUENCY
	frequency = parse_frequency(&return_status, argv[1]);
	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_FREQUENCY_ARG;
		goto exit_command_handler;
	}

	//
	// execute ..
	//

	// open COM port, initialize rx thread  and queue
	if (!InitUART(g_com_port_number, 115200))  
		InitTasks();
	else
	{
		return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
		goto exit_command_handler;
	}

	// send HCI command
	hci_rx_test(/*uint8_t*/ frequency);

	// receive reply event
	evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

	if (evt == NULL)
	{
		return_status = SC_RX_TIMEOUT; // rx timeout 
		goto exit_command_handler;
	}
	
	handle_hci_event(evt); ////////////////////////////////////////////// print evt

	if ( !( evt->event == 0x0E 
		&& evt->length == 4
		&& evt->parameters[1] == 0x1D
		&& evt->parameters[2] == 0x20)
	)
	{
		return_status = SC_UNEXPECTED_EVENT; // unexpected event
		goto exit_command_handler;
	}

	status = evt->parameters[3];

	// check command completion event status
	if (status != 0 )
	{
		return_status = SC_HCI_STANDARD_ERROR_CODE_BASE + status;
	}
	
exit_command_handler:
	if(evt)
		free(evt);

	printf("status = %d\n", return_status);
	
	return return_status;
};

/*
 ****************************************************************************************
 * @brief Command handler for "Start Prod testmode RX with ReadBack values"
 *
 *  @param[in] argc		Command line argument count.
 *  @param[in] argv		Command line arguments.
 *
 * @return error code on failure / 0 on success.
 ****************************************************************************************
*/
int starttest_rx_readback_values_handler(int argc, char **argv)
{
	uint8_t frequency;
	int return_status = 0;
	hci_evt_t *evt = NULL;


	// check number of args
	if (argc != 2)
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}
	// parse FREQUENCY
	frequency = parse_frequency(&return_status, argv[1]);
	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_FREQUENCY_ARG;
		goto exit_command_handler;
	}

	//
	// execute ..
	//

	// open COM port, initialize rx thread  and queue
	if (!InitUART(g_com_port_number, 115200))  
		InitTasks();
	else
	{
		return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
		goto exit_command_handler;
	}

	// send HCI command
	hci_dialog_rx_readback_test(/*uint8_t*/ frequency);

	// receive reply event
	evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

	if (evt == NULL)
	{
		return_status = SC_RX_TIMEOUT; // rx timeout 
		goto exit_command_handler;
	}
	
	handle_hci_event(evt); ////////////////////////////////////////////// print evt

	if ( !( evt->event == 0x0E 
		&& evt->length == 3
	        && (evt->parameters[1] == (HCI_START_PROD_RX_TEST_CMD_OPCODE & 0x00FF))
	        && (evt->parameters[2] == HCI_START_PROD_RX_TEST_CMD_OPCODE>>8))
	)
	{
		return_status = SC_UNEXPECTED_EVENT; // unexpected event
		goto exit_command_handler;
	}

	// event has no return parameters

	
exit_command_handler:
	if(evt)
		free(evt);

	printf("status = %d\n", return_status);
	
	return return_status;
};

/*
 ****************************************************************************************
 * @brief Command handler for "End Prod testmode RX with ReadBack values"
 *
 *  @param[in] argc		Command line argument count.
 *  @param[in] argv		Command line arguments.
 *
 * @return error code on failure / 0 on success.
 ****************************************************************************************
*/
int stoptest_rx_readback_values_handler(int argc, char **argv)
{
	int return_status = 0;
	hci_evt_t *evt = NULL;
	// command complete event return parameters
	uint16_t nb_packets_received_correctly = 0;
	uint16_t nb_packets_with_syncerror = 0;
	uint16_t nb_packets_received_with_crcerr = 0;
	uint16_t rssi = 0;
	float dBm = 0;


	// check number of args
	if (argc != 1)
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}

	//
	// execute ..
	//

	// open COM port, initialize rx thread  and queue
	if (!InitUART(g_com_port_number, 115200))  
		InitTasks();
	else
	{
		return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
		goto exit_command_handler;
	}

	// send HCI command
	hci_dialog_rx_readback_test_end();

	// receive reply event
	evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

	if (evt == NULL)
	{
		return_status = SC_RX_TIMEOUT; // rx timeout 
		goto exit_command_handler;
	}
	
	handle_hci_event(evt); ////////////////////////////////////////////// print evt

	if ( !( evt->event == 0x0E
		&& evt->length == 11
                && (evt->parameters[1] == (HCI_END_PROD_RX_TEST_CMD_OPCODE & 0x00FF))
                && (evt->parameters[2] == HCI_END_PROD_RX_TEST_CMD_OPCODE>>8))
        )
	{
		return_status = SC_UNEXPECTED_EVENT; // unexpected event
		goto exit_command_handler;
	}

	// parse return parameters
	nb_packets_received_correctly   = evt->parameters[3] + 256 * evt->parameters[4];
	nb_packets_with_syncerror       = evt->parameters[5] + 256 * evt->parameters[6];
	nb_packets_received_with_crcerr = evt->parameters[7] + 256 * evt->parameters[8];
	rssi                            = evt->parameters[9] + 256 * evt->parameters[10];
	
	// convert rssi value to dBm
	dBm = (0.474f * rssi) - 112.4f;

exit_command_handler:
	if(evt)
		free(evt);


	printf("status = %d\n", return_status);
	printf("nb_packets_received_correctly   = %d\n", nb_packets_received_correctly);
	printf("nb_packets_with_syncerror       = %d\n", nb_packets_with_syncerror);
	printf("nb_packets_received_with_crcerr = %d\n", nb_packets_received_with_crcerr);
	printf("rssi                            = %.2f\n", dBm);
	
	return return_status;
};

/*
 ****************************************************************************************
 * @brief Command handler for "End Test Mode Command(default)"
 *
 *  @param[in] argc		Command line argument count.
 *  @param[in] argv		Command line arguments.
 *
 * @return error code on failure / 0 on success.
 ****************************************************************************************
*/
int stoptest_handler(int argc, char **argv)
{
	int return_status = 0;
	hci_evt_t *evt = NULL;

	uint8_t status; // see: BT spec 4.0 vol 2- part D - error codes
	uint16_t number_of_packets = 0;

	// check number of args
	if (argc != 1)
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}

	//
	// execute ..
	//

	// open COM port, initialize rx thread  and queue
	if (!InitUART(g_com_port_number, 115200))  
		InitTasks();
	else
	{
		return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
		goto exit_command_handler;
	}

	// send HCI command
	hci_test_end();

	// receive reply event
	evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

	if (evt == NULL)
	{
		return_status = SC_RX_TIMEOUT; // rx timeout 
		goto exit_command_handler;
	}
	
	handle_hci_event(evt); ////////////////////////////////////////////// print evt

	if ( !( evt->event == 0x0E 
		&& evt->length == 6
		&& evt->parameters[1] == 0x1F
		&& evt->parameters[2] == 0x20)
	)
	{
		return_status = SC_UNEXPECTED_EVENT; // unexpected event
		goto exit_command_handler;
	}

	status = evt->parameters[3];
	number_of_packets = evt->parameters[4] + 256 * evt->parameters[5];

	// check command completion event status
	if (status != 0 )
	{
		return_status = SC_HCI_STANDARD_ERROR_CODE_BASE + status;
	}
	
exit_command_handler:
	if(evt)
		free(evt);


	printf("status = %d\n", return_status);
	printf("number_of_packets = %d\n", number_of_packets);
	
	return return_status;
};

/*
 ****************************************************************************************
 * @brief Command handler for "Unmodulated RX/TX"
 *
 * command line:
 *   prodtest  starttest_unmodulated  OFF
 *   prodtest  starttest_unmodulated  TX 2404
 *   prodtest  starttest_unmodulated  RX 2404
 *
 *  @param[in] argc		Command line argument count.
 *  @param[in] argv		Command line arguments.
 *
 * @return error code on failure / 0 on success.
 ****************************************************************************************
*/
int starttest_unmodulated_handler(int argc, char **argv)
{
	uint8_t mode;
	uint8_t frequency;
	int frequency_arg_required = 0;
	int return_status = 0;
	hci_evt_t *evt = NULL;


	// we need at least 1 arg: the MODE
	if (argc < 2 )
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}

	// parse MODE - argv[1]
	mode = parse_unmodulated_mode(&return_status, argv[1]);
	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_UNMODULATED_CMD_MODE_ARG;
		goto exit_command_handler;
	}

	switch (mode) {
		case UNMODULATED_CMD_MODE_OFF: frequency_arg_required = 0; break;
		case UNMODULATED_CMD_MODE_TX:  frequency_arg_required = 1; break;
		case UNMODULATED_CMD_MODE_RX:  frequency_arg_required = 1; break;
	}

	if (argc != (2 + frequency_arg_required) )
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}

	if ( frequency_arg_required == 1 )
	{
		// parse FREQUENCY
		frequency = parse_frequency(&return_status, argv[2]);
		if (return_status != 0 ) 
		{
			return_status = SC_INVALID_FREQUENCY_ARG;
			goto exit_command_handler;\
		}
	}
	else
	{
		frequency = 0;
	}

	//
	// execute ..
	//

	// open COM port, initialize rx thread  and queue
	if (!InitUART(g_com_port_number, 115200))  
		InitTasks();
	else
	{
		return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
		goto exit_command_handler;
	}

	// send HCI command
	hci_dialog_unmodulated_rx_tx(mode, frequency);

	// receive reply event
	evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

	if (evt == NULL)
	{
		return_status = SC_RX_TIMEOUT; // rx timeout 
		goto exit_command_handler;
	}
	
	handle_hci_event(evt); ////////////////////////////////////////////// print evt

	if ( !( evt->event == 0x0E 
		&& evt->length == 3
	        && (evt->parameters[1] == (HCI_UNMODULATED_ON_CMD_OPCODE & 0x00FF))
	        && (evt->parameters[2] == HCI_UNMODULATED_ON_CMD_OPCODE>>8))
	)
	{
		return_status = SC_UNEXPECTED_EVENT; // unexpected event
		goto exit_command_handler;
	}

	// event has no return parameters

	
exit_command_handler:
	if(evt)
		free(evt);

	printf("status = %d\n", return_status);
	
	return return_status;
};

/*
 ****************************************************************************************
 * @brief Command handler for "Start TX continue test"
 *
 *  Command line:
 *    prodtest starttest_tx_continue <FREQUENCY> <PAYLOAD_TYPE>
 *
 *  @param[in] argc		Command line argument count.
 *  @param[in] argv		Command line arguments.
 *
 * @return error code on failure / 0 on success.
 ****************************************************************************************
*/
int starttest_tx_continue_handler(int argc, char **argv)
{
	uint8_t frequency, payload_type;
	int return_status = 0;
	hci_evt_t *evt = NULL;

	// check number of arguments
	if (argc != 3)
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}
	// parse FREQUENCY
	frequency = parse_frequency(&return_status, argv[1]);

	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_FREQUENCY_ARG;
		goto exit_command_handler;
	}	
	// parse PAYLOAD_TYPE 
	payload_type = parse_payload_type(&return_status, argv[2]);
	if (return_status != 0 ) 
	{
		return_status = SC_INVALID_PAYLOAD_TYPE_ARG;
		goto exit_command_handler;
	}

	//
	// execute ..
	//

	// open COM port, initialize rx thread  and queue
	if (!InitUART(g_com_port_number, 115200))  
		InitTasks();
	else
	{
		return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
		goto exit_command_handler;
	}

	// send HCI command
	hci_dialog_tx_continuous_start(/*uint8_t*/ frequency, /*uint8_t*/ payload_type);

	// receive reply event
	evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

	if (evt == NULL)
	{
		return_status = SC_RX_TIMEOUT; // rx timeout 
		goto exit_command_handler;
	}
	
	handle_hci_event(evt); ////////////////////////////////////////////// print evt

	if ( !( evt->event == 0x0E 
	    && evt->length == 3
	    && (evt->parameters[1] == (HCI_TX_START_CONTINUE_TEST_CMD_OPCODE & 0x00FF))
	    && (evt->parameters[2] == HCI_TX_START_CONTINUE_TEST_CMD_OPCODE>>8))
	)
	{
		return_status = SC_UNEXPECTED_EVENT; // unexpected event
		goto exit_command_handler;
	}

	// no return parameters

exit_command_handler:
	if(evt)
		free(evt);

	printf("status = %d\n", return_status);
	
	return return_status;
};

/*
 ****************************************************************************************
 * @brief Command handler for "End TX continue test"
 *
 *  @param[in] argc		Command line argument count.
 *  @param[in] argv		Command line arguments.
 *
 * @return error code on failure / 0 on success.
 ****************************************************************************************
*/
int stoptest_tx_continue_handler(int argc, char **argv)
{
	int return_status = 0;
	hci_evt_t *evt = NULL;


	// check number of args
	if (argc != 1)
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}

	//
	// execute ..
	//

	// open COM port, initialize rx thread  and queue
	if (!InitUART(g_com_port_number, 115200))  
		InitTasks();
	else
	{
		return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
		goto exit_command_handler;
	}

	// send HCI command
	hci_dialog_tx_continuous_end();

	// receive reply event
	evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

	if (evt == NULL)
	{
		return_status = SC_RX_TIMEOUT; // rx timeout 
		goto exit_command_handler;
	}
	
	handle_hci_event(evt); ////////////////////////////////////////////// print evt

	if ( !( evt->event == 0x0E 
		&& evt->length == 3
	        && (evt->parameters[1] == (HCI_TX_END_CONTINUE_TEST_CMD_OPCODE & 0x00FF))
	        && (evt->parameters[2] == HCI_TX_END_CONTINUE_TEST_CMD_OPCODE>>8))
	)
	{
		return_status = SC_UNEXPECTED_EVENT; // unexpected event
		goto exit_command_handler;
	}
	
exit_command_handler:
	if(evt)
		free(evt);

	printf("status = %d\n", return_status);
	
	return return_status;
};

/*
 ****************************************************************************************
 * @brief Command handler for "Reset Command"
 *
 *  @param[in] argc		Command line argument count.
 *  @param[in] argv		Command line arguments.
 *
 * @return error code on failure / 0 on success.
 ****************************************************************************************
*/
int reset_handler(int argc, char **argv)
{
	int return_status = 0;
	hci_evt_t *evt = NULL;

	uint8_t status; // see: BT spec 4.0 vol 2- part D - error codes

	// check number of args
	if (argc != 1)
	{
		return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
		goto exit_command_handler;
	}

	//
	// execute ..
	//

	// open COM port, initialize rx thread  and queue
	if (!InitUART(g_com_port_number, 115200))  
		InitTasks();
	else
	{
		return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
		goto exit_command_handler;
	}

	// send HCI command
	hci_reset();

	// receive reply event
	evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

	if (evt == NULL)
	{
		return_status = SC_RX_TIMEOUT; // rx timeout 
		goto exit_command_handler;
	}
	
	handle_hci_event(evt); ////////////////////////////////////////////// print evt

	if ( !( evt->event == 0x0E 
		&& evt->length == 4
		&& evt->parameters[1] == 0x03
		&& evt->parameters[2] == 0x0C)
	)
	{
		return_status = SC_UNEXPECTED_EVENT; // unexpected event
		goto exit_command_handler;
	}

	status = evt->parameters[3];

	// check command completion event status
	if (status != 0 )
	{
		return_status = SC_HCI_STANDARD_ERROR_CODE_BASE + status;
	}
	
exit_command_handler:
	if(evt)
		free(evt);

	printf("status = %d\n", return_status);
	
	return return_status;
};

#define CMD__SLEEP_MODE_ACTIVE   0x00
#define CMD__SLEEP_MODE_EXTENDED 0x01
#define CMD__SLEEP_MODE_DEEP     0x02

#define CMD__SLEEP_MODE_ACTIVE_STR   "none"
#define CMD__SLEEP_MODE_EXTENDED_STR "extended"
#define CMD__SLEEP_MODE_DEEP_STR     "deep"

static uint8_t parse_sleep_mode(int *return_status, const char *str)
{
	uint8_t result = 0;
	
	*return_status = 0;

	if ( 0 == strcmp(str, CMD__SLEEP_MODE_ACTIVE_STR) )
	{
		result = CMD__SLEEP_MODE_ACTIVE;
	}
	else if ( 0 == strcmp(str, CMD__SLEEP_MODE_EXTENDED_STR) )
	{
		result = CMD__SLEEP_MODE_EXTENDED;
	}
	else if ( 0 == strcmp(str, CMD__SLEEP_MODE_DEEP_STR) )
	{
		result = CMD__SLEEP_MODE_DEEP;
	}
	else
	{
		*return_status = 1;
	}

	return result;
}

int sleep_cmd_handler(int argc, char **argv)
{
    uint8_t sleep_type;
    uint8_t minutes;
    uint8_t seconds;
    int return_status = 0;
    hci_evt_t *evt = NULL;

    // check number of arguments
    if (argc != 4)
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // parse sleep_type
    sleep_type = parse_sleep_mode(&return_status, argv[1]);
    if (return_status != 0 ) 
    {
        return_status = SC_INVALID_SLEEP_CMD_MODE_ARG;
        goto exit_command_handler;
    }

    // parse minutes
    minutes = parse_uint8(&return_status, argv[2]);
    if (return_status != 0 ) 
    {
        return_status = SC_INVALID_SLEEP_CMD_MINUTES_ARG;
        goto exit_command_handler;
    }

    // parse seconds
    seconds = parse_uint8(&return_status, argv[3]);
    if (return_status != 0 ) 
    {
        return_status = SC_INVALID_SLEEP_CMD_SECONDS_ARG;
        goto exit_command_handler;
    }

    //
    // execute ..
    //

    // open COM port, initialize rx thread  and queue
    if (!InitUART(g_com_port_number, 115200))  
        InitTasks();
    else
    {
        return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
        goto exit_command_handler;
    }

    // send HCI command
    hci_dialog_sleep(sleep_type, minutes, seconds);

    // receive reply status_event
    evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS);

    if (evt == NULL)
    {
        return_status = SC_RX_TIMEOUT; // rx timeout 
        goto exit_command_handler;
    }

    handle_hci_event(evt); ////////////////////////////////////////////// print statut evt

    // check response
    if ( !( evt->event == 0x0F
        && evt->length == 4
        && evt->parameters [0] == 0
        && (evt->parameters[2] == (HCI_SLEEP_TEST_CMD_OPCODE & 0x00FF))
        && (evt->parameters[3] == HCI_SLEEP_TEST_CMD_OPCODE>>8))
    )
    {
        return_status = SC_UNEXPECTED_EVENT; // unexpected event
        goto exit_command_handler;
    }

    // no return parameters

exit_command_handler:
    if(evt)
        free(evt);

    printf("status = %d\n", return_status);

    return return_status;
}

#define CMD__XTRIM_OP_RD  0x00
#define CMD__XTRIM_OP_WR  0x01
#define CMD__XTRIM_OP_EN  0x02
#define CMD__XTRIM_OP_INC 0x03
#define CMD__XTRIM_OP_DEC 0x04
#define CMD__XTRIM_OP_DIS 0x05
#define CMD__XTRIM_OP_CALTEST 0x06
#define CMD__XTRIM_OP_CAL     0x07

#define CMD__XTRIM_OP_RD_STR  "rd"
#define CMD__XTRIM_OP_WR_STR  "wr"
#define CMD__XTRIM_OP_EN_STR  "en"
#define CMD__XTRIM_OP_INC_STR "inc"
#define CMD__XTRIM_OP_DEC_STR "dec"
#define CMD__XTRIM_OP_DIS_STR "dis"
#define CMD__XTRIM_OP_CALTEST_STR "caltest"
#define CMD__XTRIM_OP_CAL_STR     "cal"

static uint8_t parse_xtrim_operation(int *return_status, const char *str)
{
    uint8_t result = 0;
    *return_status = 0;

    if      ( 0 == strcmp(str, CMD__XTRIM_OP_RD_STR)  ) { result = CMD__XTRIM_OP_RD; }
    else if ( 0 == strcmp(str, CMD__XTRIM_OP_WR_STR)  ) { result = CMD__XTRIM_OP_WR; }
    else if ( 0 == strcmp(str, CMD__XTRIM_OP_EN_STR)  ) { result = CMD__XTRIM_OP_EN; }
    else if ( 0 == strcmp(str, CMD__XTRIM_OP_INC_STR) ) { result = CMD__XTRIM_OP_INC; }
    else if ( 0 == strcmp(str, CMD__XTRIM_OP_DEC_STR) ) { result = CMD__XTRIM_OP_DEC; }
    else if ( 0 == strcmp(str, CMD__XTRIM_OP_DIS_STR) ) { result = CMD__XTRIM_OP_DIS; }
    else if ( 0 == strcmp(str, CMD__XTRIM_OP_CALTEST_STR) ) { result = CMD__XTRIM_OP_CALTEST; }
    else if ( 0 == strcmp(str, CMD__XTRIM_OP_CAL_STR    ) ) { result = CMD__XTRIM_OP_CAL; }
    else
    {
        *return_status = 1;
    }

    return result;
}

int xtal_trimming_cmd_handler(int argc, char **argv)
{
    uint8_t operation = 0;
    uint16_t required_number_of_args = 0;
    uint16_t trim_value_or_delta = 0;
    uint8_t freq_gen_gpio = 0;
    uint16_t returned_trim_value = 0;
    int return_status = 0;
    hci_evt_t *evt = NULL;
    unsigned int timeout = RX_TIMEOUT_MILLIS;

    // check number of arguments
    if ( !(argc == 2  || argc == 3) )
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // parse operation
    operation = parse_xtrim_operation(&return_status, argv[1]);
    if (return_status != 0 ) 
    {
        return_status = SC_INVALID_XTAL_TRIMMING_CMD_OPERATION_ARG;
        goto exit_command_handler;
    }

    switch(operation)
    {
        case CMD__XTRIM_OP_RD:
        case CMD__XTRIM_OP_EN:
        case CMD__XTRIM_OP_DIS:
            required_number_of_args = 2;
            break;
        case CMD__XTRIM_OP_WR:
        case CMD__XTRIM_OP_INC:
        case CMD__XTRIM_OP_DEC:
            required_number_of_args = 3;
            break;
            
        case CMD__XTRIM_OP_CALTEST:
        case CMD__XTRIM_OP_CAL:
            required_number_of_args = 3;
            break;
    }

    // check number of arguments
    if ( !(argc == required_number_of_args) )
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // parse args if any
    switch(operation)
    {
        case CMD__XTRIM_OP_RD:
        case CMD__XTRIM_OP_EN:
        case CMD__XTRIM_OP_DIS:
            break;
        case CMD__XTRIM_OP_WR:
        case CMD__XTRIM_OP_INC:
        case CMD__XTRIM_OP_DEC:
            // parse trim_value_or_delta
            trim_value_or_delta = parse_uint16(&return_status, argv[2]);
            if (return_status != 0 ) 
            {
                return_status = SC_INVALID_XTAL_TRIMMING_CMD_TRIM_VALUE_ARG;
                goto exit_command_handler;
            }
            break;
            
        case CMD__XTRIM_OP_CALTEST:
        case CMD__XTRIM_OP_CAL:
            freq_gen_gpio = parse_gpio(argv[2]);
            if (freq_gen_gpio == INVALID_GPIO )
            {
                return_status = SC_INVALID_GPIO_ARG;
                goto exit_command_handler;
            }
            trim_value_or_delta = freq_gen_gpio;
            timeout = 15 * 1000; // 15 seconds
            break;
    }

    //
    // execute ..
    //

    // open COM port, initialize rx thread  and queue
    if (!InitUART(g_com_port_number, 115200))  
        InitTasks();
    else
    {
        return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
        goto exit_command_handler;
    }

    // send HCI command
    hci_dialog_xtal_trimming(operation, trim_value_or_delta);

    // receive reply event
    evt = hci_recv_event_wait(timeout); // timeout depends on operation

    if (evt == NULL)
    {
        return_status = SC_RX_TIMEOUT; // rx timeout 
        goto exit_command_handler;
    }

    handle_hci_event(evt); ////////////////////////////////////////////// print evt

    // check response 
    if ( !( evt->event == 0x0E 
        && evt->length == 5
        && (evt->parameters[1] == (HCI_XTAL_TRIM_CMD_OPCODE & 0x00FF))
        && (evt->parameters[2] == HCI_XTAL_TRIM_CMD_OPCODE >> 8))
    )
    {
        return_status = SC_UNEXPECTED_EVENT; // unexpected event
        goto exit_command_handler;
    }

    // no return parameters
    returned_trim_value = evt->parameters[3] + 256 * evt->parameters[4];

    switch(operation)
    {
        case CMD__XTRIM_OP_CALTEST:
        case CMD__XTRIM_OP_CAL:
            if (returned_trim_value == 0x01)
                return_status = SC_XTAL_TRIMMING_CAL_OUT_OF_RANGE_ERROR;
            else if (returned_trim_value == 0x02)
                return_status = SC_XTAL_TRIMMING_CAL_FREQ_NOT_CONNECTED;
            break;
        default:
            break;
    }

exit_command_handler:
    if(evt)
        free(evt);

    printf("status     = %d\n", return_status);
    if(operation == CMD__XTRIM_OP_RD)
    {
        printf("trim_value = %d\n", returned_trim_value);
    }

    return return_status;
} 

#define CMD__OTP_OP_RE_XTRIM_STR  "re_xtrim"
#define CMD__OTP_OP_WE_XTRIM_STR  "we_xtrim"
#define CMD__OTP_OP_RD_XTRIM_STR  "rd_xtrim"
#define CMD__OTP_OP_WR_XTRIM_STR  "wr_xtrim"
#define CMD__OTP_OP_RD_BDADDR_STR "rd_bdaddr"
#define CMD__OTP_OP_WR_BDADDR_STR "wr_bdaddr"
 
 
static uint8_t parse_otp_operation(int *return_status, const char *str)
{
    uint8_t result = 0;
    *return_status = 0;

    if      ( 0 == strcmp(str, CMD__OTP_OP_RD_XTRIM_STR)  ) { result = CMD__OTP_OP_RD_XTRIM; }
    else if ( 0 == strcmp(str, CMD__OTP_OP_WR_XTRIM_STR)  ) { result = CMD__OTP_OP_WR_XTRIM; }
    else if ( 0 == strcmp(str, CMD__OTP_OP_RD_BDADDR_STR) ) { result = CMD__OTP_OP_RD_BDADDR; }
    else if ( 0 == strcmp(str, CMD__OTP_OP_WR_BDADDR_STR) ) { result = CMD__OTP_OP_WR_BDADDR; }
	else if ( 0 == strcmp(str, CMD__OTP_OP_RE_XTRIM_STR)  ) { result = CMD__OTP_OP_RE_XTRIM; }
	else if ( 0 == strcmp(str, CMD__OTP_OP_WE_XTRIM_STR)  ) { result = CMD__OTP_OP_WE_XTRIM; }
    else
    {
        *return_status = 1;
    }

    return result;
}

int otp_cmd_handler(int argc, char **argv)
{
    uint8_t operation = 0xFF;
    uint16_t required_number_of_args = 0;
    uint16_t trim_value = 0;
    uint16_t returned_trim_value = 0;
    uint8_t bd_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t returned_bd_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t returned_xtrim_enable[4] = {0x00, 0x00, 0x00, 0x00};
    int return_status = 0;
    hci_evt_t *evt = NULL;

    // check number of arguments
    if ( !(argc == 2  || argc == 3) )
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // parse operation
    operation = parse_otp_operation(&return_status, argv[1]);
    if (return_status != 0 ) 
    {
        return_status = SC_INVALID_OTP_CMD_OPERATION_ARG;
        goto exit_command_handler;
    }

    switch(operation)
    {
        case CMD__OTP_OP_RD_XTRIM:  required_number_of_args = 2; break;
        case CMD__OTP_OP_WR_XTRIM:  required_number_of_args = 3; break;
        case CMD__OTP_OP_RD_BDADDR: required_number_of_args = 2; break;
        case CMD__OTP_OP_WR_BDADDR: required_number_of_args = 3; break;
		case CMD__OTP_OP_RE_XTRIM:  required_number_of_args = 2; break;
		case CMD__OTP_OP_WE_XTRIM:  required_number_of_args = 2; break;
    }

    // check number of arguments
    if ( !(argc == required_number_of_args) )
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // if the operation requires an arg
    switch(operation)
    {
        case CMD__OTP_OP_RD_XTRIM:
		case CMD__OTP_OP_RE_XTRIM:
		case CMD__OTP_OP_WE_XTRIM:
            break;

        case CMD__OTP_OP_WR_XTRIM:
            {
                // parse trim value 
                trim_value = parse_uint16(&return_status, argv[2]);
                if (return_status != 0 ) 
                {
                    return_status = SC_INVALID_OTP_CMD_TRIM_VALUE_ARG;
                    goto exit_command_handler;
                }
            }
            break;

        case CMD__OTP_OP_RD_BDADDR: 
            break;

        case CMD__OTP_OP_WR_BDADDR: 
            {
                // parse BD address
                return_status = parse_bd_addr(bd_addr, argv[2]);
                if (return_status != 0 ) 
                {
                    return_status = SC_INVALID_OTP_CMD_BDADDR_ARG;
                    goto exit_command_handler;
                }
            }
            break;
		 
    }

    //
    // execute ..
    //

    // open COM port, initialize rx thread  and queue
    if (!InitUART(g_com_port_number, 115200))  
        InitTasks();
    else
    {
        return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
        goto exit_command_handler;
    }

    // send HCI command
    switch(operation)
    {
        case CMD__OTP_OP_RD_XTRIM:
            hci_dialog_otp_rd_xtrim();
            break;
        case CMD__OTP_OP_WR_XTRIM:
            hci_dialog_otp_wr_xtrim (trim_value);
            break;
        case CMD__OTP_OP_RD_BDADDR:
            hci_dialog_otp_rd_bdaddr();
            break;
        case CMD__OTP_OP_WR_BDADDR:
            hci_dialog_otp_wr_bdaddr(bd_addr);
            break;
		case CMD__OTP_OP_RE_XTRIM:
            hci_dialog_otp_re_xtrim ();
            break;
		case CMD__OTP_OP_WE_XTRIM:
            hci_dialog_otp_we_xtrim ();
            break;
    }

    // receive reply event
    evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

    if (evt == NULL)
    {
        return_status = SC_RX_TIMEOUT; // rx timeout 
        goto exit_command_handler;
    }

    handle_hci_event(evt); ////////////////////////////////////////////// print evt

    // check response 
    if ( !( evt->event == 0x0E 
        && evt->length == 10
        && (evt->parameters[1] == (HCI_OTP_RW_CMD_OPCODE & 0x00FF))
        && (evt->parameters[2] == HCI_OTP_RW_CMD_OPCODE>>8))
    )
    {
        return_status = SC_UNEXPECTED_EVENT; // unexpected event
        goto exit_command_handler;
    }

    // return parameters
    switch(operation)
    {
        case CMD__OTP_OP_RD_XTRIM:
            returned_trim_value = evt->parameters[4] + 256 * evt->parameters[5];
            break;

        case CMD__OTP_OP_WR_XTRIM:
            break;
        case CMD__OTP_OP_RD_BDADDR:
            returned_bd_addr[0] = evt->parameters[4];
            returned_bd_addr[1] = evt->parameters[5];
            returned_bd_addr[2] = evt->parameters[6];
            returned_bd_addr[3] = evt->parameters[7];
            returned_bd_addr[4] = evt->parameters[8];
            returned_bd_addr[5] = evt->parameters[9];
            break;
        case CMD__OTP_OP_WR_BDADDR:
            break;
        case CMD__OTP_OP_RE_XTRIM:
	    returned_xtrim_enable[0]= evt->parameters[4];
	    break;
	case CMD__OTP_OP_WE_XTRIM:
             break;
	}

exit_command_handler:
    if(evt)
        free(evt);

    printf("status     = %d\n", return_status);

    switch(operation)
    {
        case CMD__OTP_OP_RD_XTRIM:
            printf("otp_xtrim_value = %d\n", returned_trim_value );
            break;
        case CMD__OTP_OP_WR_XTRIM:
            break;
        case CMD__OTP_OP_RD_BDADDR:
            printf("otp_bd_addr = %02X:%02X:%02X:%02X:%02X:%02X" , returned_bd_addr[5],
                                                                   returned_bd_addr[4],
                                                                   returned_bd_addr[3],
                                                                   returned_bd_addr[2],
                                                                   returned_bd_addr[1],
                                                                   returned_bd_addr[0] 
                                                                   );
            break;
        case CMD__OTP_OP_WR_BDADDR:
            break;
		case CMD__OTP_OP_RE_XTRIM:
 			if (returned_xtrim_enable[0] & 0x10) 
                printf("otp_xtrim_enabled = 1 \n");
			else
                printf("otp_xtrim_enabled = 0 \n");
             break;
		case CMD__OTP_OP_WE_XTRIM:
             break;
    }

    return return_status;
}


int otp_read_cmd_handler(int argc, char **argv)
{
    uint16_t otp_address = 0x0000;
    uint8_t word_count = 0 ;
    uint8_t returned_word_count = 0;
    uint32_t returned_words[MAX_READ_WRITE_OTP_WORDS];
    int return_status = 0;
    hci_evt_t *evt = NULL;
    int kk = 0;

    // check number of arguments
    if ( !(argc == 3) )
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // parse otp_address
    otp_address = parse_hex_uint32(&return_status, argv[1]);
    if (return_status != 0 
        || (otp_address % 4 != 0)
        || (otp_address >= 0xFFFF) )
    {
        return_status = SC_INVALID_OTP_ADDRESS_ARG;
        goto exit_command_handler;
    }

    // parse word_count
    word_count = (uint8_t) parse_number(&return_status, argv[2]);
    if (return_status != 0
        || word_count == 0
        || word_count > MAX_READ_WRITE_OTP_WORDS
        || (otp_address + word_count * 4 > 0xFFFF) )
    {
        return_status = SC_INVALID_NUMBER_OF_OTP_WORDS_ARG;
        goto exit_command_handler;
    }

    //
    // execute ..
    //

    // open COM port, initialize rx thread  and queue
    if (!InitUART(g_com_port_number, 115200))  
        InitTasks();
    else
    {
        return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
        goto exit_command_handler;
    }

    // send HCI command
    hci_dialog_otp_read(otp_address, word_count);

    // receive reply event
    evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

    if (evt == NULL)
    {
        return_status = SC_RX_TIMEOUT; // rx timeout 
        goto exit_command_handler;
    }

    handle_hci_event(evt); ////////////////////////////////////////////// print evt

    // check response 
    if ( !( evt->event == 0x0E 
        && evt->length == (5 + 4 * word_count)
        && (evt->parameters[1] == (HCI_OTP_READ_CMD_OPCODE & 0x00FF))
        && (evt->parameters[2] == HCI_OTP_READ_CMD_OPCODE>>8))
    )
    {
        return_status = SC_UNEXPECTED_EVENT; // unexpected event
        goto exit_command_handler;
    }

    // return parameters
    returned_word_count = evt->parameters[4];
    {
        int pos = 5;
        for (kk = 0 ; kk < returned_word_count; ++kk, pos +=4) 
        {
            returned_words[kk] = evt->parameters[pos]
                              | (evt->parameters[pos + 1] <<  8)
                              | (evt->parameters[pos + 2] << 16)
                              | (evt->parameters[pos + 3] << 24) ;
        }
    }

exit_command_handler:
    if(evt)
        free(evt);

    printf("status = %d\n", return_status);
    for (kk = 0 ; kk < returned_word_count; ++kk) 
    {
        printf("[%04X] = %08X \n", otp_address+ 4* kk, returned_words[kk]);
    }

    return return_status;
}


int otp_write_cmd_handler(int argc, char **argv)
{
    uint16_t otp_address = 0x0000;
    uint8_t word_count = 0 ;
    uint32_t words[MAX_READ_WRITE_OTP_WORDS];
    int return_status = 0;
    hci_evt_t *evt = NULL;
    int kk = 0;

    // check number of arguments
    if ( !(argc >= 3) )
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // parse otp_address
    otp_address = parse_hex_uint32(&return_status, argv[1]);
    if (return_status != 0 
        || (otp_address % 4 != 0)
        || (otp_address >= 0xFFFF) )
    {
        return_status = SC_INVALID_OTP_ADDRESS_ARG;
        goto exit_command_handler;
    }

    // the rest arguments are the words to be written in OTP
    word_count = argc - 2;

    if ( word_count == 0
        || word_count > MAX_READ_WRITE_OTP_WORDS 
        || (otp_address + word_count * 4 > 0xFFFF) )
    {
        return_status = SC_INVALID_NUMBER_OF_OTP_WORDS_ARG;
        goto exit_command_handler;
    }

    for( kk = 0; kk < word_count; ++kk)
    {
        words[kk] = parse_hex_uint32(&return_status, argv[2+kk]);
        if (return_status != 0)
        {
            return_status = SC_INVALID_WORD_VALUE_ARG;
            goto exit_command_handler;
        }
    }

    //
    // execute ..
    //

    // open COM port, initialize rx thread  and queue
    if (!InitUART(g_com_port_number, 115200))  
        InitTasks();
    else
    {
        return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
        goto exit_command_handler;
    }

    // send HCI command
    hci_dialog_otp_write(otp_address, words, word_count);

    // receive reply event
    evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

    if (evt == NULL)
    {
        return_status = SC_RX_TIMEOUT; // rx timeout 
        goto exit_command_handler;
    }

    handle_hci_event(evt); ////////////////////////////////////////////// print evt

    // check response 
    if ( !( evt->event == 0x0E 
        && evt->length == 5
        && (evt->parameters[1] == (HCI_OTP_WRITE_CMD_OPCODE & 0x00FF))
        && (evt->parameters[2] == HCI_OTP_WRITE_CMD_OPCODE>>8))
    )
    {
        return_status = SC_UNEXPECTED_EVENT; // unexpected event
        goto exit_command_handler;
    }

    // return parameters
    // none

exit_command_handler:
    if(evt)
        free(evt);

    printf("status = %d\n", return_status);

    return return_status;
}

int read_reg32_cmd_handler(int argc, char **argv)
{
    uint32_t register_address = 0;
    uint32_t returned_value = 0;
    int return_status = 0;
    hci_evt_t *evt = NULL;

    // check number of arguments
    if ( !(argc == 2) )
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // parse register address
    register_address = parse_hex_uint32(&return_status, argv[1]);
    if (return_status != 0
        || (register_address % 4 !=0) // address must be word aligned
        )
    {
        return_status = SC_INVALID_REGISTER_ADDRESS_ARG;
        goto exit_command_handler;
    }

    //
    // execute ..
    //

    // open COM port, initialize rx thread  and queue
    if (!InitUART(g_com_port_number, 115200))  
        InitTasks();
    else
    {
        return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
        goto exit_command_handler;
    }

    // send HCI command
    hci_dialog_read_reg32(register_address);

    // receive reply event
    evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

    if (evt == NULL)
    {
        return_status = SC_RX_TIMEOUT; // rx timeout 
        goto exit_command_handler;
    }

    handle_hci_event(evt); ////////////////////////////////////////////// print evt

    // check response 
    if ( !( evt->event == 0x0E 
        && evt->length == 9
        && (evt->parameters[1] == (HCI_REGISTER_RW_CMD_OPCODE & 0x00FF))
        && (evt->parameters[2] == HCI_REGISTER_RW_CMD_OPCODE>>8))
    )
    {
        return_status = SC_UNEXPECTED_EVENT; // unexpected event
        goto exit_command_handler;
    }

    // return parameters
    returned_value = evt->parameters[5]
                   | (evt->parameters[6] <<  8)
                   | (evt->parameters[7] << 16)
                   | (evt->parameters[8] << 24) ;

exit_command_handler:
    if(evt)
        free(evt);

    printf("status = %d\n", return_status);
    printf("value  = %08X \n", returned_value);

    return return_status;
}

int write_reg32_cmd_handler(int argc, char **argv)
{
    uint32_t register_address = 0;
    uint32_t value = 0;
    int return_status = 0;
    hci_evt_t *evt = NULL;

    // check number of arguments
    if ( !(argc == 3) )
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // parse register address
    register_address = parse_hex_uint32(&return_status, argv[1]);
    if (return_status != 0
        || (register_address % 4 !=0) // address must be word aligned
        )
    {
        return_status = SC_INVALID_REGISTER_ADDRESS_ARG;
        goto exit_command_handler;
    }

    // parse value to be written
    value = parse_hex_uint32(&return_status, argv[2]);
    if (return_status != 0)
    {
        return_status = SC_INVALID_REGISTER_VALUE_ARG;
        goto exit_command_handler;
    }

    //
    // execute ..
    //

    // open COM port, initialize rx thread  and queue
    if (!InitUART(g_com_port_number, 115200))  
        InitTasks();
    else
    {
        return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
        goto exit_command_handler;
    }

    // send HCI command
    hci_dialog_write_reg32(register_address, value);

    // receive reply event
    evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

    if (evt == NULL)
    {
        return_status = SC_RX_TIMEOUT; // rx timeout 
        goto exit_command_handler;
    }

    handle_hci_event(evt); ////////////////////////////////////////////// print evt

    // check response 
    if ( !( evt->event == 0x0E 
        && evt->length == 9
        && (evt->parameters[1] == (HCI_REGISTER_RW_CMD_OPCODE & 0x00FF))
        && (evt->parameters[2] == HCI_REGISTER_RW_CMD_OPCODE>>8))
    )
    {
        return_status = SC_UNEXPECTED_EVENT; // unexpected event
        goto exit_command_handler;
    }

    // return parameters
    // none

exit_command_handler:
    if(evt)
        free(evt);

    printf("status = %d\n", return_status);

    return return_status;
}

int read_reg16_cmd_handler(int argc, char **argv)
{
    uint32_t register_address = 0;
    uint16_t returned_value = 0;
    int return_status = 0;
    hci_evt_t *evt = NULL;

    // check number of arguments
    if ( !(argc == 2) )
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // parse register address
    register_address = parse_hex_uint32(&return_status, argv[1]);
    if (return_status != 0
        || (register_address % 2 !=0) // address must be word aligned
        )
    {
        return_status = SC_INVALID_REGISTER_ADDRESS_ARG;
        goto exit_command_handler;
    }

    //
    // execute ..
    //

    // open COM port, initialize rx thread  and queue
    if (!InitUART(g_com_port_number, 115200))  
        InitTasks();
    else
    {
        return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
        goto exit_command_handler;
    }

    // send HCI command
    hci_dialog_read_reg16(register_address);

    // receive reply event
    evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

    if (evt == NULL)
    {
        return_status = SC_RX_TIMEOUT; // rx timeout 
        goto exit_command_handler;
    }

    handle_hci_event(evt); ////////////////////////////////////////////// print evt

    // check response 
    if ( !( evt->event == 0x0E 
        && evt->length == 9
        && (evt->parameters[1] == (HCI_REGISTER_RW_CMD_OPCODE & 0x00FF))
        && (evt->parameters[2] == HCI_REGISTER_RW_CMD_OPCODE>>8))
    )
    {
        return_status = SC_UNEXPECTED_EVENT; // unexpected event
        goto exit_command_handler;
    }

    // return parameters
    returned_value = evt->parameters[5]
                   | (evt->parameters[6] <<  8);

exit_command_handler:
    if(evt)
        free(evt);

    printf("status = %d\n", return_status);
    printf("value  = %04X \n", returned_value);

    return return_status;
}

int write_reg16_cmd_handler(int argc, char **argv)
{
    uint32_t register_address = 0;
    uint16_t value = 0;
    int return_status = 0;
    hci_evt_t *evt = NULL;

    // check number of arguments
    if ( !(argc == 3) )
    {
        return_status = SC_WRONG_NUMBER_OF_ARGUMENTS;
        goto exit_command_handler;
    }

    // parse register address
    register_address = parse_hex_uint32(&return_status, argv[1]);
    if (return_status != 0
        || (register_address % 2 !=0) // address must be word aligned
        )
    {
        return_status = SC_INVALID_REGISTER_ADDRESS_ARG;
        goto exit_command_handler;
    }

    // parse value to be written
    value = parse_hex_uint32(&return_status, argv[2]);
    if (return_status != 0)
    {
        return_status = SC_INVALID_REGISTER_VALUE_ARG;
        goto exit_command_handler;
    }

    //
    // execute ..
    //

    // open COM port, initialize rx thread  and queue
    if (!InitUART(g_com_port_number, 115200))  
        InitTasks();
    else
    {
        return_status = SC_COM_PORT_INIT_ERROR; // InitUART failed
        goto exit_command_handler;
    }

    // send HCI command
    hci_dialog_write_reg16(register_address, value);

    // receive reply event
    evt = hci_recv_event_wait(RX_TIMEOUT_MILLIS); // wait for RX_TIMEOUT_MILLIS milliseconds

    if (evt == NULL)
    {
        return_status = SC_RX_TIMEOUT; // rx timeout 
        goto exit_command_handler;
    }

    handle_hci_event(evt); ////////////////////////////////////////////// print evt

    // check response 
    if ( !( evt->event == 0x0E 
        && evt->length == 9
        && (evt->parameters[1] == (HCI_REGISTER_RW_CMD_OPCODE & 0x00FF))
        && (evt->parameters[2] == HCI_REGISTER_RW_CMD_OPCODE>>8))
    )
    {
        return_status = SC_UNEXPECTED_EVENT; // unexpected event
        goto exit_command_handler;
    }

    // return parameters
    // none

exit_command_handler:
    if(evt)
        free(evt);

    printf("status = %d\n", return_status);

    return return_status;
}
