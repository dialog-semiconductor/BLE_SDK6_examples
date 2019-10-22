/**
 ****************************************************************************************
 *
 * @file commands.h
 *
 * @brief Commands header file.
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

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

/* status codes */
#define SC_NO_ERROR                                 0
#define SC_MISSING_COMMAND                          1
#define SC_INVALID_COMMAND                          2
#define SC_WRONG_NUMBER_OF_ARGUMENTS                3
#define SC_INVALID_COM_PORT_NUMBER                  4
#define SC_INVALID_FREQUENCY_ARG                    5
#define SC_INVALID_DATA_LENGTH_ARG                  6
#define SC_INVALID_PAYLOAD_TYPE_ARG                 7
#define SC_COM_PORT_INIT_ERROR                      8
#define SC_RX_TIMEOUT                               9
#define SC_UNEXPECTED_EVENT                        10
#define SC_INVALID_NUMBER_OF_PACKETS_ARG           11
#define SC_INVALID_UNMODULATED_CMD_MODE_ARG        12
#define SC_COM_PORT_NOT_SPECIFIED                  13
#define SC_INVALID_SLEEP_CMD_MODE_ARG              14
#define SC_INVALID_SLEEP_CMD_MINUTES_ARG           15
#define SC_INVALID_SLEEP_CMD_SECONDS_ARG           16
#define SC_INVALID_XTAL_TRIMMING_CMD_OPERATION_ARG 17
#define SC_INVALID_XTAL_TRIMMING_CMD_TRIM_VALUE_ARG 18
#define SC_INVALID_OTP_CMD_OPERATION_ARG            19
#define SC_INVALID_OTP_CMD_TRIM_VALUE_ARG           20
#define SC_INVALID_OTP_CMD_BDADDR_ARG               21
#define SC_INVALID_OTP_ADDRESS_ARG                  22
#define SC_INVALID_NUMBER_OF_OTP_WORDS_ARG          23
#define SC_INVALID_WORD_VALUE_ARG                   24
#define SC_INVALID_GPIO_ARG                         25
#define SC_XTAL_TRIMMING_CAL_OUT_OF_RANGE_ERROR     26
#define SC_XTAL_TRIMMING_CAL_FREQ_NOT_CONNECTED     27
#define SC_INVALID_REGISTER_ADDRESS_ARG             28
#define SC_INVALID_REGISTER_VALUE_ARG               29

#define SC_HCI_STANDARD_ERROR_CODE_BASE           1000


/* timeout while waiting for events*/
#define RX_TIMEOUT_MILLIS 10000 

/* Maximum number of words that can be read or written by a command at once */
#define MAX_READ_WRITE_OTP_WORDS 60

/* command handlers */
int starttest_tx_param_len_3_handler(int argc, char **argv);
int starttest_tx_param_len_5_handler(int argc, char **argv);
int starttest_rx_default_handler(int argc, char **argv);
int starttest_rx_readback_values_handler(int argc, char **argv);
int stoptest_rx_readback_values_handler(int argc, char **argv);
int stoptest_handler(int argc, char **argv);
int starttest_unmodulated_handler(int argc, char **argv);
int starttest_tx_continue_handler(int argc, char **argv);
int stoptest_tx_continue_handler(int argc, char **argv);
int reset_handler(int argc, char **argv);
int sleep_cmd_handler(int argc, char **argv);
int xtal_trimming_cmd_handler(int argc, char **argv);
int otp_cmd_handler(int argc, char **argv);
int otp_read_cmd_handler(int argc, char **argv);
int otp_write_cmd_handler(int argc, char **argv);
int read_reg32_cmd_handler(int argc, char **argv);
int write_reg32_cmd_handler(int argc, char **argv);
int read_reg16_cmd_handler(int argc, char **argv);
int write_reg16_cmd_handler(int argc, char **argv);

/* utils*/
long parse_number(int *return_status, const char * str);

#endif /* _COMMANDS_H_ */
