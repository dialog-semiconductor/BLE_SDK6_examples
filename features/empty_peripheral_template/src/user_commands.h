/**
 ****************************************************************************************
 *
 * @file user_commands.h
 *
 * @brief User_commands header file.
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef USER_COMMANDS_H_
#define USER_COMMANDS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "ke_task.h"
#include "hci_int.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define HCI_VS_FIRST_USER_CMD_OPCODE    0xFD00

//User HCI commands range from 0xFD00 to 0XFDFF
enum
{
    HCI_VS1_USER_CMD_OPCODE = HCI_VS_FIRST_USER_CMD_OPCODE,
    HCI_VS2_USER_CMD_OPCODE,               //= 0xFD01,
    HCI_VS_LAST_USER_CMD_OPCODE           // DO NOT MOVE. Must always be last and opcodes linear (00,01,02 ...)
};

// HCI user vs1 command parameters - vendor specific
struct hci_vs1_user_cmd
{
    //VS specif data
    uint8_t    inputData;
};

// HCI user vs1 complete event parameters - vendor specific
struct hci_vs1_user_cmd_cmp_evt
{
    //Status of the command reception
    uint8_t status;
    //type
    uint8_t returnData;
};

// HCI user vs2 command parameters - vendor specific
struct hci_vs2_user_cmd
{
    //VS specif data
    uint8_t    inputData;
};

// HCI user vs2 complete event parameters - vendor specific
struct hci_vs2_user_cmd_cmp_evt
{
    //Status of the command reception
    uint8_t status;
    //type
    uint8_t returnData;
    //mycode
    uint8_t mycode;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

// Number of user handlers
extern const uint8_t user_commands_handler_num;
// Number of Dialog commands
extern const uint8_t user_commands_num;

// User HCI commands
extern const struct hci_cmd_desc_tag hci_cmd_desc_tab_user_vs[];
// User HCI commands handlers
extern const struct ke_msg_handler user_commands_handler_tab[];

#endif // USER_COMMANDS_H_
