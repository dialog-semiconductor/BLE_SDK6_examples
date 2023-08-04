/**
 ****************************************************************************************
 *
 * @file dci.c
 *
 * @brief DLG Crash info c source.
 *
 * Copyright (c) 2012-2023 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
 
 /*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "cmsis_armclang.h"
#include "compiler.h"
#include "arch_wdg.h"
#include "syscntl.h"
#include "arch_system.h"
#include "arch_console.h"

#include "dci_config.h"
#include "dci_types.h"
#include "dci.h"


#define SYS_RAM_MAGIC_NUM               (0x01C28122)
#define RET_RAM_NEEDS_INITIALIZED(mn)   (mn != SYS_RAM_MAGIC_NUM)

#define LAST_FRAME_MN                   (0xBADCOFEE)
#define ADF_MN                          (0xBAADBEBE)
#define FOUR_BYTE_PADDING_MASK          (0x100)

#define CODE_AREA_START          				(0x07fc0000 + 0xC0 + 80)
#define CODE_AREA_END										(0x07FCBFFF)

#define ROM_CODE_START									(0x07f0200)
#define ROM_CODE_END										(0x07F24000)

#define IS_IN_RAM(x)                    (x > CODE_AREA_START && x < CODE_AREA_END)
#define IS_IN_ROM(x)                  	(x>ROM_CODE_START && x<ROM_CODE_END)
#define IS_VALID_ADDRESS(x)             (x & 0x01) && ( IS_IN_RAM(x) || IS_IN_ROM(x) )
#define GET_STACK_ALIGN_BYTES(xPSR)     (xPSR & FOUR_BYTE_PADDING_MASK ? 4 : 0)



uint32_t						s_mn									__SECTION_ZERO("retention_mem_area_uninit");
dci_reset_reason_t	s_last_reset_reason		__SECTION_ZERO("retention_mem_area_uninit");
dci_data_t					s_dci_data						__SECTION_ZERO("retention_mem_area_uninit");
/*
 * Get the reset reason as indicated by the RESET_STAT_REG
 */

static dci_reset_reason_t get_reset_reason(uint32_t reset_stat_reg)
{
        dci_reset_reason_t reason = RESET_LAST;
        uint8_t mask;

        for(uint8_t i = 0; i < RESET_LAST; i++)
        {
                mask = 1 << i;
                if(reset_stat_reg & mask)
                {
                        reason = (dci_reset_reason_t)i;
                        break;
                }

        }

        return reason;

}

void dci_get_data(dci_info_t *info)
{
	info->header = DCI_HEADER;
	info->length = sizeof(s_dci_data);
	info->data = &s_dci_data;
}


#ifdef DCI_PRINT_VERBOSE

char reset_reason_map[][10] = {
		"RESET_POR",
		"RESET_HW",
		"RESET_SW",
		"RESET_WDOG",
};

char last_fault_type_map[][15] =
{
		"LF_HARDFAULT",
		"LF_NMI",
};

void dci_print_verbose(void)
{
	
	uint8_t i;
	
	DCI_PRINTF("\r\n|| *******************DCI_DATA**********************||\r\n");
	DCI_PRINTF("LAST_RESET_REASON: %s\r\n", reset_reason_map[s_dci_data.last_reset_reason]);
	DCI_PRINTF("TOTAL_RESETS: %d\r\n", s_dci_data.num_resets);
	
	#ifdef DCI_MEMORY_CHECK
	uint16_t dci_size = sizeof(s_mn) + sizeof(s_last_reset_reason) + sizeof(s_dci_data);
	
	if(dci_size > CFG_RET_DATA_UNINIT_SIZE)
	{
		DCI_PRINTF("WARNING:  CFG_RET_DATA_UNINIT_SIZE is undersized.  Currently at %d, but dci data needs %d \r\n", 
						CFG_RET_DATA_UNINIT_SIZE, dci_size);
	}
	

#endif

	
	for(i = 0; i < DCI_NUM_RESETS_STORED; i++)
	{
		if(s_dci_data.fault_data[i].data_valid)
		{
			DCI_PRINTF("FAULT_DATA #%d\r\n", i);
			DCI_PRINTF("\t EPOCH: %d\r\n", s_dci_data.fault_data[i].epoch);
			DCI_PRINTF("\t FAULT_TYPE: %s\r\n",  last_fault_type_map[s_dci_data.fault_data[i].fault_handler]);
			DCI_PRINTF("\t Last Stack Frame:\r\n");
			DCI_PRINTF("\t\t r0: 0x%08x\r\n", (unsigned int)s_dci_data.fault_data[i].stack_frame.r0);
			DCI_PRINTF("\t\t r1: 0x%08x\r\n", (unsigned int)s_dci_data.fault_data[i].stack_frame.r1);
			DCI_PRINTF("\t\t r2: 0x%08x\r\n", (unsigned int)s_dci_data.fault_data[i].stack_frame.r2);
			DCI_PRINTF("\t\t r3: 0x%08x\r\n", (unsigned int)s_dci_data.fault_data[i].stack_frame.r3);
			DCI_PRINTF("\t\t r12: 0x%08x\r\n", (unsigned int)s_dci_data.fault_data[i].stack_frame.r12);
			DCI_PRINTF("\t\t LR: 0x%08x\r\n", (unsigned int)s_dci_data.fault_data[i].stack_frame.LR);
			DCI_PRINTF("\t\t ReturnAddress: 0x%08x\r\n", (unsigned int)s_dci_data.fault_data[i].stack_frame.ReturnAddress);
			DCI_PRINTF("\t\t xPSR: 0x%08x\r\n", (unsigned int)s_dci_data.fault_data[i].stack_frame.xPSR);
			
			DCI_PRINTF("\t Call Trace: [");
			
			for(uint8_t j = 0; j < s_dci_data.fault_data[i].num_of_call_vals; j++)
			{
					DCI_PRINTF("0x%08x", s_dci_data.fault_data[i].call_trace[j]);
					if((j+1) < s_dci_data.fault_data[i].num_of_call_vals)
					{
						DCI_PRINTF(",");
					}
			}
			
			DCI_PRINTF("]\r\n");
		}	
	}
	
	DCI_PRINTF("\r\n|| *******************DCI_DATA_END**********************||\r\n");
	
}

#else

void dci_print_verbose(void){}
#endif 

/****************************************************************************************
 * @brief Clear the reset data
 * @return none
 ****************************************************************************************
*/
void dci_clear_data(void)
{
		memset(&s_dci_data, 0, sizeof(s_dci_data));
}

/*
 * Start Tracking reset info
 */
__USED bool dci_init(uint16_t reset_stat)
{
		bool ret_ram_needs_init = false; 

		dci_reset_reason_t temp_reason = get_reset_reason(reset_stat);

		/*
		 * If we have a POR or the sys_ram_magic_num is corrupted, then reinitialize.
		 * The magic number is placed in a linker location, that if you rebuild new firmware, this will move
		 * the number, and not trigger reset data for a new firwmare load (New firmware load is a HW_RESET, not POR).
		 */

		if(temp_reason == RESET_POR ||
						RET_RAM_NEEDS_INITIALIZED(s_mn))
		{
				memset(&s_dci_data, 0, sizeof(s_dci_data));
				ret_ram_needs_init = true;
		}

		s_dci_data.last_reset_reason = temp_reason;
		s_mn = SYS_RAM_MAGIC_NUM;

		return ret_ram_needs_init;
}

__STATIC_INLINE void set_dci_fault_info(void *exception_args)
{
	uint8_t stack_val_num = 0;
	uintptr_t stack_ptr;
	uint8_t		fault_index = s_dci_data.num_resets-1;

	//First word in the IVT will have the initial stack pointer.  Check if we mapped SYSRAM1 or SYSRAM3 to address 0x0 and get the intitial stack pointer
	uintptr_t end_of_stack = (GetBits16(SYS_CTRL_REG, REMAP_ADR0) & 1) ? (*(uintptr_t*)0x07FC7000) : (*(uintptr_t*)0x07fc0000);
	
	//Pop off the the last stack frame 
	// on NMI and HF a FAULT causes the M0+ to push a stack frame, manually pop it off to find the MSP
	stack_ptr = (uintptr_t) exception_args + sizeof(cortex_m0_stack_frame_t);
	
	
	memcpy(&s_dci_data.fault_data[fault_index].stack_frame, exception_args, sizeof(cortex_m0_stack_frame_t));
	
	for(uintptr_t i = (uintptr_t)stack_ptr; i < (uintptr_t)end_of_stack; i +=sizeof(uintptr_t))
	{
					uint32_t stack_val = *(uint32_t *)i;
					if( IS_VALID_ADDRESS(stack_val) ) //Check to make sure its a linked address in the stack.
					{
									/*Linked Addresses are Odd numbers on teh stack. But thumb and ARM is even for retrieving the disassembly adddress. 
										Subtract 1 to indicate the actual address linked**/
									s_dci_data.fault_data[fault_index].call_trace[stack_val_num] = stack_val-1;
									stack_val_num++;
					}

					/*Copy don't copy a value out of bounds
					 */

					if(stack_val_num >= DCI_CALL_DEPTH)
					{
									break;
					}

	}

	s_dci_data.fault_data[fault_index].num_of_call_vals = stack_val_num;
	s_dci_data.fault_data[fault_index].epoch = dci_get_epoch();
	
}

__STATIC_INLINE bool init_dci_fault_info(dci_last_fault_hdlr_t fault_hdlr)
{
		if(s_dci_data.num_resets >= DCI_NUM_RESETS_STORED)
		{
			//Can't store anymore reset data, return to avoid buffer overflow
			return false;
		}
		s_dci_data.fault_data[s_dci_data.num_resets].fault_handler = fault_hdlr;
		s_dci_data.fault_data[s_dci_data.num_resets].data_valid = 1;
		s_dci_data.num_resets++;
		
		return true;
}



/**
 ****************************************************************************************
 *
 * @file hardfault_handler.c
 *
 * @brief Hardfault handler source code file.
 *
 * Copyright (C) 2012-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */


/*
 * This is the base address in Retention RAM where the stacked information will be copied.
 */




void HardFault_HandlerC(unsigned long *hardfault_args);



void HardFault_HandlerC(unsigned long *hardfault_args)
{

    if (DEVELOPMENT_DEBUG)
    {
#if defined (__DA14531__) && (!USE_POWER_MODE_BYPASS)

        // In Boost mode enable the DCDC converter to supply VBAT_HIGH for debugger pins
        if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
        {
            syscntl_dcdc_set_level(SYSCNTL_DCDC_LEVEL_3V0);
            SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 1);
        }
#endif
        syscntl_load_debugger_cfg();        // load debugger configuration to be able to re-attach the debugger

        if (USE_WDOG)
            wdg_freeze();            // Stop WDOG

        if ((GetWord16(SYS_STAT_REG) & DBG_IS_UP) == DBG_IS_UP)
            __BKPT(0);
        else
            while(1);
    }
    else // DEVELOPMENT_DEBUG
    {

				if(init_dci_fault_info(LF_HARDFAULT))
				{
					//set reset data only if there is room.
					set_dci_fault_info(hardfault_args);
				}
				
#ifdef DCI_RAMAP_ADDR0_TEST
        SetWord16(SYS_CTRL_REG, GetWord16(SYS_CTRL_REG)  | SW_RESET );
#else
				SetWord16(SYS_CTRL_REG, (GetWord16(SYS_CTRL_REG) & ~REMAP_ADR0) | SW_RESET );
#endif 
    }

}

void NMI_HandlerC(unsigned long *nmi_args);

void NMI_HandlerC(unsigned long *nmi_args)
{
    // Reached this point due to a WDOG timeout

    SetBits16(PMU_CTRL_REG, RADIO_SLEEP, 1);        // turn off radio PD
#if !defined (__DA14531__)
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 1);       // turn off peripheral power domain
#endif
    SetBits16(CLK_RADIO_REG, BLE_LP_RESET, 1);      // reset the BLE LP timer
    NVIC_ClearPendingIRQ(BLE_WAKEUP_LP_IRQn);       // clear any pending LP IRQs

    if (DEVELOPMENT_DEBUG)
    {
        wdg_freeze();            // Stop WDOG
#if defined (__DA14531__) && (!USE_POWER_MODE_BYPASS)

        // In Boost mode enable the DCDC converter to supply VBAT_HIGH for debugger pins
        if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED))
        {
            syscntl_dcdc_set_level(SYSCNTL_DCDC_LEVEL_3V0);
            SetBits16(DCDC_CTRL_REG, DCDC_ENABLE, 1);
        }
#endif
        syscntl_load_debugger_cfg();        // load debugger configuration to be able to re-attach the debugger


        if ((GetWord16(SYS_STAT_REG) & DBG_IS_UP) == DBG_IS_UP)
            __BKPT(0);
        else
        {
            while(1);
        }
    }
    else // DEVELOPMENT_DEBUG
    {
				if(init_dci_fault_info(LF_NMI))
				{
					//set reset data only if there is room.
					set_dci_fault_info(nmi_args);
				}
    }

#ifdef DCI_RAMAP_ADDR0_TEST
        SetWord16(SYS_CTRL_REG, GetWord16(SYS_CTRL_REG)  | SW_RESET );
#else
				SetWord16(SYS_CTRL_REG, (GetWord16(SYS_CTRL_REG) & ~REMAP_ADR0) | SW_RESET );
#endif 

}


/// @} APP

