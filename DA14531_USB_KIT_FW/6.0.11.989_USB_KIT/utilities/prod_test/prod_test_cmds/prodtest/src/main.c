/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Main source code file.
 *
 * Copyright (c) 2013-2018 Dialog Semiconductor. All rights reserved.
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

#include <stdlib.h>
#include <stdio.h>

#include <windows.h>

#include "uart.h"
#include "queue.h"
#include "commands.h"
#include "getopt.h"
#include "sdk_version.h"

#define CMD__STARTTEST_TX_PARAM_LEN_3     "cont_pkt_tx"  //"starttest_tx_param_len_3"
#define CMD__STARTTEST_TX_PARAM_LEN_5     "pkt_tx"       //"starttest_tx_param_len_5"
#define CMD__STARTTEST_RX_DEFAULT         "start_pkt_rx" //"starttest_rx_default"
#define CMD__STARTTEST_RX_READBACK_VALUES "start_pkt_rx_stats" // "starttest_rx_readback_values"
#define CMD__STOPTEST_RX_READBACK_VALUES  "stop_pkt_rx_stats"  // "stoptest_rx_readback_values"
#define CMD__STOPTEST                     "stoptest"
#define CMD__STARTTEST_UNMODULATED        "unmodulated" // "starttest_unmodulated"
#define CMD__STARTTEST_TX_CONTINUE        "start_cont_tx" // "starttest_tx_continue"
#define CMD__STOPTEST_TX_CONTINUE         "stop_cont_tx" // "stoptest_tx_continue"
#define CMD__RESET                        "reset"
#define CMD__SLEEP                        "sleep"
#define CMD__XTAL_TRIMMING                "xtrim"
#define CMD__OTP                          "otp"
#define CMD__OTP_READ                     "otp_read"
#define CMD__OTP_WRITE                    "otp_write"
#define CMD__READ_REG32                   "read_reg32"
#define CMD__WRITE_REG32                  "write_reg32"
#define CMD__READ_REG16                   "read_reg16"
#define CMD__WRITE_REG16                  "write_reg16"

typedef int (*cmd_handler_t) (int argc, char **argv);

typedef struct {
	char cmd_name[64];
	cmd_handler_t cmd_handler;
} cmd_t;


int default_cmd_handler(int argc, char **argv)
{
	return 0;
};

cmd_t cmd_table[] = {
    { CMD__STARTTEST_TX_PARAM_LEN_3     , starttest_tx_param_len_3_handler},
    { CMD__STARTTEST_TX_PARAM_LEN_5     , starttest_tx_param_len_5_handler},
    { CMD__STARTTEST_RX_DEFAULT         , starttest_rx_default_handler},
    { CMD__STARTTEST_RX_READBACK_VALUES , starttest_rx_readback_values_handler},
    { CMD__STOPTEST_RX_READBACK_VALUES  , stoptest_rx_readback_values_handler},
    { CMD__STOPTEST                     , stoptest_handler},
    { CMD__STARTTEST_UNMODULATED        , starttest_unmodulated_handler},
    { CMD__STARTTEST_TX_CONTINUE        , starttest_tx_continue_handler},
    { CMD__STOPTEST_TX_CONTINUE         , stoptest_tx_continue_handler},
    { CMD__RESET                        , reset_handler},
    { CMD__SLEEP                        , sleep_cmd_handler},
    { CMD__XTAL_TRIMMING                , xtal_trimming_cmd_handler},
    { CMD__OTP                          , otp_cmd_handler},
    { CMD__OTP_READ                     , otp_read_cmd_handler},
    { CMD__OTP_WRITE                    , otp_write_cmd_handler},
    { CMD__READ_REG32                   , read_reg32_cmd_handler},
    { CMD__WRITE_REG32                  , write_reg32_cmd_handler},
    { CMD__READ_REG16                   , read_reg16_cmd_handler},
    { CMD__WRITE_REG16                  , write_reg16_cmd_handler},

    { "",0}
};


int g_com_port_number;

void print_usage(void);

int main(int argc, char **argv)
{
	int help_option = 0;
	int com_port_option = 0;
	int kk = 0;
	cmd_t *cmd = NULL;
	int rc;
	int opt;
	int cmd_argc;
	char ** cmd_argv;

	__progname = argv[0]; // used by getopt

	// parse command line switches
	while( ( opt = getopt( argc, argv, "hvp:" ) )!= -1 )  
 	{
		switch( opt ) 
		{
			case 'h':
				print_usage();
				exit(SC_NO_ERROR);
				break;
			case 'p':
				{
					int return_status;
					long com_port_number = 0;

					com_port_number = parse_number(&return_status, optarg);
					if(return_status !=0 )
					{
						fprintf(stderr, "Illegal com port number in -p option \n");
						exit(SC_INVALID_COM_PORT_NUMBER);
					}
					
					com_port_option = 1;
					g_com_port_number = com_port_number;
				}
				break;
			case 'v':
				printf("%s\n", SDK_VERSION);
				exit(SC_NO_ERROR);
				break;


		}
	}

	if (help_option)
	{
		print_usage();
		exit(SC_NO_ERROR);
	}

	// remaining arguments are interpreted as
	//   <cmd> <cmd_arg_1> <cmd_arg_2>  ... <cmd_arg_n>
	// and the following will hold:
	//    cmd_argc = 1 + n
	//    cmd_argv[0] = <cmd> 
	//    cmd_argv[1] = <cmd_arg_1>
	//    cmd_argv[2] = <cmd_arg_2>
	//    ...
	//    cmd_argv[n] = <cmd_arg_n>

	cmd_argc = argc - optind;
	cmd_argv = argv + optind;

	//
	// check if a command was specified 
	//
	if (cmd_argc == 0 )
	{
		fprintf(stderr, "No command specified. \n");
		print_usage();
		exit(SC_MISSING_COMMAND);
	}

	// search for command handler - cmd_argv[0] contains the command name 
	for (kk = 0; cmd_table[kk].cmd_name[0] != 0; kk++)
	{
		if ( 0 == strcmp(cmd_argv[0], cmd_table[kk].cmd_name) )
		{
			cmd = &cmd_table[kk];
			break;
		}
	}
	
	// if no command handler was found
	if (cmd == NULL)
	{
		fprintf(stderr, "Invalid command: \"%s\"\n", cmd_argv[0]);
		exit(SC_INVALID_COMMAND);
	}

	// all commands require a COM port
	if (!com_port_option) 
	{
		fprintf(stderr, "Option -p is required. \n");
		print_usage();
		exit(SC_COM_PORT_NOT_SPECIFIED);
	}

	//
	// execute command
	//
	rc = cmd->cmd_handler(cmd_argc, cmd_argv);

	return rc;

}

void print_usage(void)
{
    printf("Usage: \n\n");

    printf("prodtest -h \n");

    printf("prodtest -p <COM port number> cont_pkt_tx <FREQUENCY> <DATA_LENGTH> <PAYLOAD_TYPE> \n");
    printf("prodtest -p <COM port number> pkt_tx <FREQUENCY> <DATA_LENGTH> <PAYLOAD_TYPE> <NUMBER_OF_PACKETS>  \n");
    printf("prodtest -p <COM port number> start_pkt_rx <FREQUENCY> \n");
    printf("prodtest -p <COM port number> start_pkt_rx_stats <FREQUENCY> \n");
    printf("prodtest -p <COM port number> stop_pkt_rx_stats \n");
    printf("prodtest -p <COM port number> stoptest \n");
    printf("prodtest -p <COM port number> unmodulated OFF \n");
    printf("prodtest -p <COM port number> unmodulated TX <FREQUENCY> \n");
    printf("prodtest -p <COM port number> unmodulated RX <FREQUENCY> \n");
    printf("prodtest -p <COM port number> start_cont_tx <FREQUENCY> <PAYLOAD_TYPE> \n");
    printf("prodtest -p <COM port number> stop_cont_tx \n");
    printf("prodtest -p <COM port number> reset \n");

    printf("prodtest -p <COM port number> sleep none     <minutes> <seconds> \n");
    printf("prodtest -p <COM port number> sleep extended <minutes> <seconds> \n");
    printf("prodtest -p <COM port number> sleep deep     <minutes> <seconds> \n");

    printf("prodtest -p <COM port number> xtrim rd \n");
    printf("prodtest -p <COM port number> xtrim wr <trim_value> \n");
    printf("prodtest -p <COM port number> xtrim en \n");
    printf("prodtest -p <COM port number> xtrim dis \n");
    printf("prodtest -p <COM port number> xtrim inc <delta> \n");
    printf("prodtest -p <COM port number> xtrim dec <delta> \n");
    printf("prodtest -p <COM port number> xtrim caltest <gpio> \n");
    printf("prodtest -p <COM port number> xtrim cal     <gpio> \n");

    printf("prodtest -p <COM port number> otp wr_xtrim <decimal trim value> \n");
    printf("prodtest -p <COM port number> otp rd_xtrim                      \n");
    printf("prodtest -p <COM port number> otp wr_bdaddr <BD address>        \n");
    printf("prodtest -p <COM port number> otp rd_bdaddr                     \n");
    printf("prodtest -p <COM port number> otp re_xtrim                     \n");
    printf("prodtest -p <COM port number> otp we_xtrim                     \n");

    printf("prodtest -p <COM port number> otp_read  <otp address in hex> <word_count> \n");
    printf("prodtest -p <COM port number> otp_write <otp address in hex> <word 1> ... <word n>\n");

    printf("prodtest -p <COM port number> read_reg32  <address of 32 bit reg. in hex>                       \n");
    printf("prodtest -p <COM port number> write_reg32 <address of 32 bit reg. in hex> <32 bit value in hex> \n");
    printf("prodtest -p <COM port number> read_reg16  <address of 16 bit reg. in hex>                       \n");
    printf("prodtest -p <COM port number> write_reg16 <address of 16 bit reg. in hex> <16 bit value in hex> \n");

    printf("prodtest -v \n");
}
