/**
 ****************************************************************************************
 *
 * @file host_hci.c
 *
 * @brief Connection Manager HCI library.
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

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <windows.h>
#include <conio.h>
#include <stdint.h>         

#include "host_hci.h"
#include "uart.h"
#include "queue.h"

/* HCI TEST MODE */

void send_hci_command(hci_cmd_t *cmd)
{
#ifdef DEVELOPMENT_MESSAGES
	int kk;

	// log command in stderr
	fprintf(stderr, "==== Tx ====> \n");
	fprintf(stderr, "opcode   : 0x%04x\n", cmd->opcode);
	fprintf(stderr, "length   : 0x%02x\n", cmd->length);
	fprintf(stderr, "Payload  : ");
	for (kk = 0; kk < (cmd->length + 3/*sizeof(hci_cmd_header_t)*/); kk++)
	{
		fprintf(stderr, "%02x ", *((unsigned char *)cmd+ kk));
	}
	fprintf(stderr, "\n");
#endif //DEVELOPMENT_MESSAGES

	UARTSend(0x01, cmd->length + 3/*sizeof(hci_cmd_header_t)*/, (unsigned char *) cmd);

	free(cmd);
}

void *alloc_hci_command(unsigned short opcode, unsigned char length)
{
    hci_cmd_t *cmd = (hci_cmd_t *) malloc(sizeof(hci_cmd_t) + length - sizeof (unsigned char));

    cmd->opcode = opcode;
	cmd->length = length;

	if (length)
		memset(cmd->parameters, 0, length);

    return cmd;
}

hci_evt_t *hci_recv_event_wait(unsigned int millis)
{	
	QueueElement *qe;
	DWORD dw;
	hci_evt_t *evt;
	
	dw = WaitForSingleObject(QueueHasAvailableData, millis); // wait until elements are available
	if (dw != WAIT_OBJECT_0)	
	{
		return 0;
	}

	WaitForSingleObject(UARTRxQueueSem, INFINITE);
	qe = (QueueElement *) DeQueue(&UARTRxQueue); 
	ReleaseMutex(UARTRxQueueSem);

	evt = (hci_evt_t *) qe->payload;

	free(qe);

	return evt;
};

void handle_hci_event( hci_evt_t * evt)
{
#ifdef DEVELOPMENT_MESSAGES
	unsigned int kk;

	// log event in stderr
	fprintf(stderr, "<==== Rx ===== \n");	
	fprintf(stderr, "event     : 0x%02X \n", evt->event);
	fprintf(stderr, "length    : 0x%02X \n", evt->length);
	fprintf(stderr, "parameters: ");
	for (kk = 0; kk < evt->length; kk++)
	{
		fprintf(stderr, "0x%02X ", evt->parameters[kk]);
	}
	fprintf(stderr, "\n");

	// log payload
	fprintf(stderr, "Payload   : ");
	for (kk = 0; kk < (evt->length + sizeof(hci_evt_header_t)); kk++)
	{
		fprintf(stderr, "%02X ",*((unsigned char *)evt + kk));
	}
	fprintf(stderr, "\n");
#endif // DEVELOPMENT_MESSAGES
	
}

/*
 * HCI TEST MODE
 */

bool __stdcall hci_reset()
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (0x0C03, 0);

    send_hci_command((void *)cmd);

    return(true);
}

bool __stdcall hci_tx_test(uint8_t frequency, uint8_t length, uint8_t payload)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (0x201E, 3);

    cmd->parameters[0] = frequency;
    cmd->parameters[1] = length;
    cmd->parameters[2] = payload;

    send_hci_command((void *)cmd);

    return(true);
}

bool __stdcall hci_rx_test(uint8_t frequency)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (0x201D, 1);

    cmd->parameters[0] = frequency;

    send_hci_command((void *)cmd);

    return(true);
}

bool __stdcall hci_test_end()
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (0x201F, 0);
    
    send_hci_command((void *)cmd);
    
    return(true);
}

bool __stdcall hci_dialog_tx_test(uint8_t frequency, uint8_t length, uint8_t payload, uint16_t number_of_packets)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_TX_TEST_CMD_OPCODE, 5); // same with default tx test but with 5 parameters

    cmd->parameters[0] = frequency;
    cmd->parameters[1] = length;
    cmd->parameters[2] = payload;
    cmd->parameters[3] = number_of_packets & 0xFF;
    cmd->parameters[4] = number_of_packets >> 8;

    send_hci_command((void *)cmd);

    return(true);
}

bool __stdcall hci_dialog_rx_readback_test(uint8_t frequency)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_START_PROD_RX_TEST_CMD_OPCODE, 1);

    cmd->parameters[0] = frequency;

    send_hci_command((void *)cmd);

    return(true);
}

bool __stdcall hci_dialog_rx_readback_test_end()
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_END_PROD_RX_TEST_CMD_OPCODE, 0);

    send_hci_command((void *)cmd);

    return(true);
}

bool __stdcall hci_dialog_unmodulated_rx_tx(uint8_t mode, uint8_t frequency)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_UNMODULATED_ON_CMD_OPCODE, 2);

    cmd->parameters[0] = mode;
    cmd->parameters[1] = frequency;

    send_hci_command((void *)cmd);

    return(true);
}

bool __stdcall hci_dialog_tx_continuous_start(uint8_t frequency, uint8_t payload_type)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_TX_START_CONTINUE_TEST_CMD_OPCODE, 2);

    cmd->parameters[0] = frequency;
    cmd->parameters[1] = payload_type;

    send_hci_command((void *)cmd);

    return(true);
}

bool __stdcall hci_dialog_tx_continuous_end()
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_TX_END_CONTINUE_TEST_CMD_OPCODE, 0);

    send_hci_command((void *)cmd);

    return(true);
}

bool __stdcall hci_dialog_sleep(uint8_t sleep_type, uint8_t minutes, uint8_t seconds)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_SLEEP_TEST_CMD_OPCODE, 3);

    cmd->parameters[0] = sleep_type;
    cmd->parameters[1] = minutes;
    cmd->parameters[2] = seconds;

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_xtal_trimming(uint8_t operation, uint16_t trim_value_or_delta)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_XTAL_TRIM_CMD_OPCODE, 3);

    cmd->parameters[0] = operation;
    cmd->parameters[1] = (trim_value_or_delta ) & 0xFF;
    cmd->parameters[2] = (trim_value_or_delta >> 8 ) & 0xFF;

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_otp_rd_xtrim(void)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_OTP_RW_CMD_OPCODE, 7);

    cmd->parameters[0] = CMD__OTP_OP_RD_XTRIM;
    cmd->parameters[1] = 0;
    cmd->parameters[2] = 0;
    cmd->parameters[3] = 0;
    cmd->parameters[4] = 0;
    cmd->parameters[5] = 0;
    cmd->parameters[6] = 0;

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_otp_wr_xtrim (uint16_t trim_value)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_OTP_RW_CMD_OPCODE, 7);

    cmd->parameters[0] = CMD__OTP_OP_WR_XTRIM ;
    cmd->parameters[1] = trim_value & 0xFF;
    cmd->parameters[2] = (trim_value >> 8 ) & 0xFF;
    cmd->parameters[3] = 0;
    cmd->parameters[4] = 0;
    cmd->parameters[5] = 0;
    cmd->parameters[6] = 0;

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_otp_re_xtrim (void)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_OTP_RW_CMD_OPCODE, 7);

    cmd->parameters[0] = CMD__OTP_OP_RE_XTRIM ;
    cmd->parameters[1] = 0;
    cmd->parameters[2] = 0;
    cmd->parameters[3] = 0;
    cmd->parameters[4] = 0;
    cmd->parameters[5] = 0;
    cmd->parameters[6] = 0;

    send_hci_command(cmd);

    return(true);
}
bool __stdcall hci_dialog_otp_we_xtrim (void)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_OTP_RW_CMD_OPCODE, 7);

    cmd->parameters[0] = CMD__OTP_OP_WE_XTRIM ;
    cmd->parameters[1] = 0x10;
    cmd->parameters[2] = 0;
    cmd->parameters[3] = 0;
    cmd->parameters[4] = 0;
    cmd->parameters[5] = 0;
    cmd->parameters[6] = 0;

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_otp_rd_bdaddr(void)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_OTP_RW_CMD_OPCODE, 7);

    cmd->parameters[0] = CMD__OTP_OP_RD_BDADDR;
    cmd->parameters[1] = 0;
    cmd->parameters[2] = 0;
    cmd->parameters[3] = 0;
    cmd->parameters[4] = 0;
    cmd->parameters[5] = 0;
    cmd->parameters[6] = 0;

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_otp_wr_bdaddr(uint8_t bdaddr[6])
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_OTP_RW_CMD_OPCODE, 7);

    cmd->parameters[0] = CMD__OTP_OP_WR_BDADDR;
    cmd->parameters[1] = bdaddr[0]; // LSB first
    cmd->parameters[2] = bdaddr[1];
    cmd->parameters[3] = bdaddr[2];
    cmd->parameters[4] = bdaddr[3];
    cmd->parameters[5] = bdaddr[4];
    cmd->parameters[6] = bdaddr[5];

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_otp_read(uint16_t otp_address, uint8_t word_count)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_OTP_READ_CMD_OPCODE, 3);

    cmd->parameters[0] = otp_address & 0xFF;
    cmd->parameters[1] = otp_address >> 8;
    cmd->parameters[2] = word_count;

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_otp_write(uint16_t otp_address, uint32_t *words, uint8_t word_count)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_OTP_WRITE_CMD_OPCODE, 3 + 4 * word_count);
    int pos = 0, i = 0;

    cmd->parameters[0] = otp_address & 0xFF;
    cmd->parameters[1] = otp_address >> 8;
    cmd->parameters[2] = word_count;

    for (pos = 3, i = 0; i < word_count; ++i, pos += 4)
    {
        cmd->parameters[pos + 0] = (words[i]      ) & 0xFF;
        cmd->parameters[pos + 1] = (words[i] >>  8) & 0xFF;
        cmd->parameters[pos + 2] = (words[i] >> 16) & 0xFF;
        cmd->parameters[pos + 3] = (words[i] >> 24) & 0xFF;
    }

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_read_reg32(uint32_t reg_addr)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_REGISTER_RW_CMD_OPCODE, 9);

    cmd->parameters[0] = CMD__REGISTER_RW_OP_READ_REG32;
    cmd->parameters[1] = reg_addr       & 0xFF; // LSB first
    cmd->parameters[2] = reg_addr >> 8  & 0xFF;
    cmd->parameters[3] = reg_addr >> 16 & 0xFF;
    cmd->parameters[4] = reg_addr >> 24 & 0xFF;
    cmd->parameters[5] = 0;
    cmd->parameters[6] = 0;
    cmd->parameters[7] = 0;
    cmd->parameters[8] = 0;

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_write_reg32(uint32_t reg_addr, uint32_t value)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_REGISTER_RW_CMD_OPCODE, 9);

    cmd->parameters[0] = CMD__REGISTER_RW_OP_WRITE_REG32;
    cmd->parameters[1] = reg_addr       & 0xFF; // LSB first
    cmd->parameters[2] = reg_addr >> 8  & 0xFF;
    cmd->parameters[3] = reg_addr >> 16 & 0xFF;
    cmd->parameters[4] = reg_addr >> 24 & 0xFF;
    cmd->parameters[5] = value       & 0xFF; // LSB first
    cmd->parameters[6] = value >> 8  & 0xFF;
    cmd->parameters[7] = value >> 16 & 0xFF;
    cmd->parameters[8] = value >> 24 & 0xFF;

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_read_reg16(uint32_t reg_addr)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_REGISTER_RW_CMD_OPCODE, 9);

    cmd->parameters[0] = CMD__REGISTER_RW_OP_READ_REG16;
    cmd->parameters[1] = reg_addr       & 0xFF; // LSB first
    cmd->parameters[2] = reg_addr >> 8  & 0xFF;
    cmd->parameters[3] = reg_addr >> 16 & 0xFF;
    cmd->parameters[4] = reg_addr >> 24 & 0xFF;
    cmd->parameters[5] = 0;
    cmd->parameters[6] = 0;
    cmd->parameters[7] = 0;
    cmd->parameters[8] = 0;

    send_hci_command(cmd);

    return(true);
}

bool __stdcall hci_dialog_write_reg16(uint32_t reg_addr, uint16_t value)
{
    hci_cmd_t *cmd = (hci_cmd_t *) alloc_hci_command (HCI_REGISTER_RW_CMD_OPCODE, 9);

    cmd->parameters[0] = CMD__REGISTER_RW_OP_WRITE_REG16;
    cmd->parameters[1] = reg_addr       & 0xFF; // LSB first
    cmd->parameters[2] = reg_addr >> 8  & 0xFF;
    cmd->parameters[3] = reg_addr >> 16 & 0xFF;
    cmd->parameters[4] = reg_addr >> 24 & 0xFF;
    cmd->parameters[5] = value       & 0xFF; // LSB first
    cmd->parameters[6] = value >> 8  & 0xFF;
    cmd->parameters[7] = 0;
    cmd->parameters[8] = 0;

    send_hci_command(cmd);

    return(true);
}
