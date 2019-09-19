/**************************************************************************//**
 * @file     system_DA14531.c
 * @brief    CMSIS Cortex-M# Device Peripheral Access Layer Source File for
 *           Device DA14531
 * @version  V5.00
 * @date     10. January 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include "datasheet.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/

#define __XTAL            (16000000UL)    /* Oscillator frequency             */

#define __SYSTEM_CLOCK    (__XTAL)


/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/

uint32_t SystemCoreClock = __SYSTEM_CLOCK;  /* System Clock Frequency (Core Clock)*/



/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/

void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
    SystemCoreClock = __SYSTEM_CLOCK;
}

void SystemInit (void)
{
    SetWord16(TIMER0_CTRL_REG, 0x6);        // stop timer
    NVIC_DisableIRQ(SWTIM_IRQn);            // disable software timer interrupt

    NVIC_DisableIRQ(BLE_GEN_IRQn);
    NVIC_DisableIRQ(BLE_WAKEUP_LP_IRQn);

    // Ensure that XTAL32M will be enabled (switch to XTAL32M will be done later)
    SetBits16(CLK_CTRL_REG, XTAL32M_DISABLE, 0);

    // Set clocks to highest frequency (16MHz)
    SetWord16(CLK_AMBA_REG, 0x00);

    __enable_irq();
}
