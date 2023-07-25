/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 *
 * @brief Reset Mechanism SW Example Custom1 Server implementation source code.
 *
 * Copyright (c) 2015-2021 Dialog Semiconductor
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.E.
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
#include "user_reset_mechanism.h"
#include "user_periph_setup.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void user_svc1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                      struct custs1_val_write_ind const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
	uint8_t val = 0;
    memcpy(&val, &param->value[0], param->length);
    
    switch(val)
    {
        case TRIGGER_WDOG_RESET:
        {
#if defined (CFG_PRINTF)
            const char wdg_nmi_msg[] = "The device will reset by the Watchdog Reset no NMI will occur\n\r";
            /*Reload the value of the watchdog to make sure the message will be printed*/
            wdg_reload(WATCHDOG_DEFAULT_PERIOD);
            uart_send(UART2, (uint8_t*)wdg_nmi_msg, sizeof(wdg_nmi_msg), UART_OP_BLOCKING);
#endif // CFG_PRINTF
            /*Triggering the WDOG timer to generates a WDOG (SYS) reset at value 0 and can not be frozen by Software.*/	
            SetBits16( WATCHDOG_CTRL_REG , NMI_RST , 1);
            while(1);
        }
            break;
        case TRIGGER_HARDFAULT:
        {
#if defined (CFG_PRINTF)
            const char hrd_fault_msg[] = "The device will hit the Hardfault handler\n\r";
            uart_send(UART2, (uint8_t*)hrd_fault_msg, sizeof(hrd_fault_msg), UART_OP_BLOCKING);
#endif
            *(uint32_t *)0x01 = 0x90;   // Force Hardfault by unaligned access in memory
        }
            break;
        case TRIGGER_WDOG_NMI:
        {
#if defined (CFG_PRINTF)
            const char wdg_rst_msg[] = "The device will reset by a SW reset, NMI will occur\n\r";
            /*Reload the value of the watchdog so we have time to print a message*/
            wdg_reload(WATCHDOG_DEFAULT_PERIOD);
            uart_send(UART2, (uint8_t*)wdg_rst_msg, sizeof(wdg_rst_msg), UART_OP_BLOCKING);
#endif
            /*Set the watchdog to trigger an NMI instead of a reset*/	
            SetBits16( WATCHDOG_CTRL_REG , NMI_RST , 0);
            while(1);   // 
        }
            break;
        case TRIGGER_SW_RESET:
        {
#if defined (CFG_PRINTF)
            const char sw_rst_msg[] = "The device will reset by a SW reset\n\r";
            uart_send(UART2, (uint8_t*)sw_rst_msg, sizeof(sw_rst_msg), UART_OP_BLOCKING);
            usDelay(100);    // Give some time for the UART to transmit the last character before reset
#endif
            /*Generating a Software Reset.*/	
            SetBits16(SYS_CTRL_REG, SW_RESET, 1);
        }
            break;
        case TRIGGER_HW_RESET:
        {
#if defined (CFG_PRINTF)
            const char hw_rst_msg[] = "The device will reset by a HW reset (via the RESET_ON_WAKEUP)\n\r";
            uart_send(UART2, (uint8_t*)hw_rst_msg, sizeof(hw_rst_msg), UART_OP_BLOCKING);
#endif
            // Make sure that sleep is enabled
            arch_set_extended_sleep(false);
            // Perform HW reset on wake-up
            SetBits16(PMU_CTRL_REG, RESET_ON_WAKEUP, 1);
        }
            break;

        case TRIGGER_POR_RESET:
        {
#if defined (CFG_PRINTF)
            const char por_rst_msg[] = "The device will reset by a POR reset\n\r";
            uart_send(UART2, (uint8_t*)por_rst_msg, sizeof(por_rst_msg), UART_OP_BLOCKING);
#endif
            /* Enabling the P0_5 as Power-On Reset (POR) source. A high polarity signal will cause a POR on P0_5. */
            GPIO_EnablePorPin( GPIO_POR_PORT, GPIO_POR_PIN, GPIO_POR_PIN_POLARITY_HIGH , POR_TIME_VAL );
            /* Activate the pull up on the pin to force the POR reset */
            GPIO_ConfigurePin( GPIO_POR_PORT, GPIO_POR_PIN, INPUT_PULLUP, PID_GPIO, false );
            /* Make sure that sleep is enabled so that set_pad_function() won't run again resetting the pin's state */
            arch_disable_sleep();
            break;
        }
        default:
        {
#if defined (CFG_PRINTF)
            const char no_rst_msg[] = "Not a valid message to trigger a reset\n\r";
            uart_send(UART2, (uint8_t*)no_rst_msg, sizeof(no_rst_msg), UART_OP_BLOCKING);
#endif
        }
            break;
    }
}
