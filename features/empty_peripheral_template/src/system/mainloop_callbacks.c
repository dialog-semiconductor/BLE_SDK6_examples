/**
 ****************************************************************************************
 *
 * @file mainloop_callbacks.h
 *
 * @brief Implementation of mainloop callbacks used by the production test application. 
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "arch.h"
#include "llm.h"
#include "gpio.h"
#include "uart.h"
#include "arch_api.h"
#include "dialog_prod.h"
#include "dialog_commands.h"
#include "hci.h"
#include "system_library.h"
#include "mainloop_callbacks.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

arch_main_loop_callback_ret_t app_on_ble_powered(void)
{
#if defined (STANDALONE_MODE) || defined (BATCH_REMOTE_MODE)
    void app_standalone_switch_test(void);
    app_standalone_switch_test();
#endif
    
    arch_main_loop_callback_ret_t ret = GOTO_SLEEP;
    static int cnt2sleep=100;

    do {
        if(test_state == STATE_START_TX)
        {
            if(llm_le_env.elt == NULL)
            {
                test_state=STATE_IDLE;
                set_state_start_tx();
            }
        }

        if((test_tx_packet_nr >= text_tx_nr_of_packets) &&  (test_state == STATE_START_TX) )
        {
            test_tx_packet_nr=0;
            send_tx_cmd_cmp_event();
            set_state_stop();
            //btw.state is STATE_IDLE after set_state_stop()
        }

        rwip_schedule();
    } while(0);

    if(arch_get_sleep_mode() == 0)
    {
        ret = KEEP_POWERED;
        cnt2sleep = 100;
    }
    else
    {
        if(cnt2sleep-- > 0)
        {
            ret = KEEP_POWERED;
        }
    }

    return ret;
}


arch_main_loop_callback_ret_t app_on_full_power(void)
{
    return GOTO_SLEEP;
}


void app_going_to_sleep(sleep_mode_t sleep_mode)
{
}


void app_resume_from_sleep(void)
{
#if !defined(STANDALONE_MODE) && !defined(STANDALONE_GO_TO_SLEEP_TEST)
    arch_disable_sleep();
#endif
}

void send_tx_cmd_cmp_event(void)
{
    void *event = ke_msg_alloc(HCI_CMD_CMP_EVENT , 0, HCI_TX_TEST_CMD_OPCODE, 0);
    hci_send_2_host(event);
}
