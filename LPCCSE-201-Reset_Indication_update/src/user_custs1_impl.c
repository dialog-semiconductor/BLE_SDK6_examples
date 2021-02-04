/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 *
 * @brief Reset Mechanism SW Example Custom1 Server implementation source code.
 *
 * Copyright (c) 2015-2020 Dialog Semiconductor. All rights reserved.
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
            const char wdg_nmi_msg[] = "The device will reset by the Watchdog Reset no NMI will occur";
            /*Reload the value of the watchdog so we have time to print a mesage*/
            wdg_reload(WATCHDOG_DEFAULT_PERIOD);
            uart_send(UART2, (uint8_t*)wdg_nmi_msg, sizeof(wdg_nmi_msg), UART_OP_BLOCKING);
            /*Triggering the WDOG timer to generates a WDOG (SYS) reset at value 0 and can not be frozen by Software.*/	
            SetBits16( WATCHDOG_CTRL_REG , NMI_RST , 1);
            while(1);
        }
            break;
        case TRIGGER_HARDFAULT:
        {
            const char hrd_fault_msg[] = "The device will hit the Hardfault handler";
            uart_send(UART2, (uint8_t*)hrd_fault_msg, sizeof(hrd_fault_msg), UART_OP_BLOCKING);
            *(uint32_t *)0x01 = 0x90;
        }
            break;
        case TRIGGER_WDOG_NMI:
        {
            const char wdg_rst_msg[] = "The device will reset by a SW reset, NMI will occur";
            /*Reload the value of the watchdog so we have time to print a mesage*/
            wdg_reload(WATCHDOG_DEFAULT_PERIOD);
            uart_send(UART2, (uint8_t*)wdg_rst_msg, sizeof(wdg_rst_msg), UART_OP_BLOCKING);
            /*Set the watchdog to trigger an NMI instead of a reset*/	
            SetBits16( WATCHDOG_CTRL_REG , NMI_RST , 0);
            while(1);   // 
        }
            break;
        case TRIGGER_SW_RESET:
        {
            const char sw_rst_msg[] = "The device will reset by a SW reset";
            uart_send(UART2, (uint8_t*)sw_rst_msg, sizeof(sw_rst_msg), UART_OP_BLOCKING);
            usDelay(25);    // Give some time for the UART to transmit the last character before reset
            /*Generating a Software Reset.*/	
            SetBits16(SYS_CTRL_REG, SW_RESET, 1);
        }
            break;
        case TRIGGER_HW_RESET:
        {
            const char hw_rst_msg[] = "The device will reset by a HW reset";
            uart_send(UART2, (uint8_t*)hw_rst_msg, sizeof(hw_rst_msg), UART_OP_BLOCKING);
            usDelay(25);    // Give some time for the UART to transmit the last character before reset
            /* 
             * For generating a HW reset with no WDOG we will set the P00 high. The system won't allow while reset 
             * function is enabled to use P00, thus we achieve that by disabling the reset, charge the line and 
             * enable the reset function on P00 again.
             */
            GPIO_Disable_HW_Reset();                                                // Disable the reset function from P00
            GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_0, OUTPUT, PID_GPIO, true);     // Set P00 to active to charge the line
            GPIO_Enable_HW_Reset();                                                 // Activate reset again
        }
            break;
        case TRIGGER_POR_RESET:
        {
            const char por_rst_msg[] = "The device will reset by a POR reset";
            uart_send(UART2, (uint8_t*)por_rst_msg, sizeof(por_rst_msg), UART_OP_BLOCKING);
            /* Enabling the P0_5 as Power-On Reset (POR) source. A high polarity signal will cause a POR on P0_5. */
            GPIO_EnablePorPin( GPIO_POR_PORT, GPIO_POR_PIN, GPIO_POR_PIN_POLARITY_HIGH , POR_TIME_VAL );
            /* Activate the pull up on the pin to force the POR reset */
            GPIO_ConfigurePin( GPIO_POR_PORT, GPIO_POR_PIN, INPUT_PULLUP, PID_GPIO, false );
            break;
        }
        default:
        {
            const char no_rst_msg[] = "Not a valid message to trigger a reset";
            uart_send(UART2, (uint8_t*)no_rst_msg, sizeof(no_rst_msg), UART_OP_BLOCKING);
        }
            break;
    }
}
