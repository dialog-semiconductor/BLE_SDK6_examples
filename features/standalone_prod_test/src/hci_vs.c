/**
 ****************************************************************************************
 *
 * @file hci_vs.c
 *
 * @brief HCI vendor specific command handlers project source code.
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "co_utils.h"
#include "hci_vs.h"
#include "hci_int.h"
#include "hci.h"
#include "dialog_commands.h"
#include "user_commands.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define HCI_DIALOG_CMD_NUM (HCI_VS_LAST_DIALOG_CMD_OPCODE - HCI_VS_FIRST_DIALOG_CMD_OPCODE)
#define HCI_USER_CMD_NUM   (HCI_VS_LAST_USER_CMD_OPCODE - HCI_VS_FIRST_USER_CMD_OPCODE)
#define TOTAL_HCI_COMMANDS (HCI_RW_CMD_NUM + HCI_DIALOG_CMD_NUM + HCI_USER_CMD_NUM)

/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */

// ROM variable
extern const struct hci_cmd_desc_tab_ref rom_hci_cmd_desc_root_tab[];
//ROM init data for hci_cmd_desc_root_tab
extern struct hci_cmd_desc_tab_ref hci_cmd_desc_root_tab[];
// RW HCI commands
extern const struct hci_cmd_desc_tag hci_cmd_desc_tab_vs[HCI_RW_CMD_NUM];

/*
 * LOCAL VARIABLES DECLARATION
 ****************************************************************************************
 */

struct hci_cmd_desc_tag hci_ram_cmd_desc_tab_vs[TOTAL_HCI_COMMANDS] __SECTION_ZERO("retention_mem_area0");

/**
 ****************************************************************************************
 * @brief Handles the reception of the vs hci commands.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
*/
int hci_vs_handle_rest_commands(ke_task_id_t const dest_id, 
                                void const *param, 
                                ke_task_id_t const src_id)
{
    int return_status = KE_MSG_CONSUMED;
    
    for(int i = 0; i < user_commands_handler_num; i++)
    {
        // Check if opcode matches
        if(user_commands_handler_tab[i].id == src_id)
        {
            // Check if there is a handler function
            if(user_commands_handler_tab[i].func != NULL)
            {
                // Call handler for user command
                return_status = user_commands_handler_tab[i].func(src_id, param, dest_id, src_id);
                return return_status;
            }
            break;
        }
    }

    for(int i = 0; i < dialog_commands_handler_num; i++)
    {
        // Check if opcode matches
        if(dialog_commands_handler_tab[i].id == src_id)
        {
            // Check if there is a handler function
            if(dialog_commands_handler_tab[i].func != NULL)
            {
                // Call handler for Dialog command
                return_status = dialog_commands_handler_tab[i].func(src_id, param, dest_id, src_id);
                return return_status;
            }
            break;
        }
    }

    return return_status;
}

void hci_vs_initialize_commands_tab(void)
{
    //copy all hci vs rom commands to hci_ram_cmd_desc_tab_vs table
    memcpy(hci_ram_cmd_desc_tab_vs, hci_cmd_desc_tab_vs, sizeof(hci_cmd_desc_tab_vs));    

    //copy all hci vs user commands to hci_ram_cmd_desc_tab_vs table
    for (int i = 0; i < user_commands_num; i++)
    {
        hci_ram_cmd_desc_tab_vs[ARRAY_LEN(hci_cmd_desc_tab_vs) + i] = hci_cmd_desc_tab_user_vs[i];    
    }

    //copy all hci vs dialog commands to hci_ram_cmd_desc_tab_vs table
    for (int i = 0; i < dialog_commands_num; i++)
    {      
        hci_ram_cmd_desc_tab_vs[ARRAY_LEN(hci_cmd_desc_tab_vs) + user_commands_num + i] = hci_cmd_desc_tab_dialog_vs[i];
    }

    //change pointer points to hci_ram_cmd_desc_tab_vs table
    hci_cmd_desc_root_tab[5].cmd_desc_tab = hci_ram_cmd_desc_tab_vs;
    hci_cmd_desc_root_tab[5].nb_cmds      = ARRAY_LEN(hci_ram_cmd_desc_tab_vs);
}
