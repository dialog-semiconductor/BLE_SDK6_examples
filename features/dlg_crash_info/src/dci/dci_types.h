/**
 ****************************************************************************************
 *
 * @file dci_types.h
 *
 * @brief DCI Types project header file.
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

#ifndef _DCI_TYPES_H
#define _DCI_TYPES_H

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief 
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */


#include "stdint.h"
#include "cmsis_armclang.h"

#include "dci_config.h"

#define DCI_HEADER			(0xAA)

typedef __PACKED_STRUCT
{

        uint32_t        r0;
        uint32_t        r1;
        uint32_t        r2;
        uint32_t        r3;
        uint32_t        r12;
        uint32_t        LR;
        uint32_t        ReturnAddress;
        uint32_t        xPSR;

}cortex_m0_stack_frame_t;


typedef enum
{
	RESET_POR,
	RESET_HW,
	RESET_SW,
	RESET_WDOG,
	RESET_LAST,
}dci_reset_reason_t;

typedef enum
{
	LF_HARDFAULT,
	LF_NMI,
	LF_PLATFORM_RESET,
}dci_last_fault_hdlr_t;

typedef __PACKED_STRUCT
{
	uint8_t 									data_valid;
	uint32_t									epoch;
	dci_last_fault_hdlr_t			fault_handler;
	cortex_m0_stack_frame_t		stack_frame;
	uint8_t										num_of_call_vals;
	uint32_t									call_trace[DCI_CALL_DEPTH];
	
}dci_fault_info_t;

typedef __PACKED_STRUCT
{
	dci_reset_reason_t		last_reset_reason;
	uint8_t								num_resets;
	dci_fault_info_t			fault_data[DCI_NUM_RESETS_STORED];
	
}dci_data_t;

typedef __PACKED_STRUCT
{
	uint8_t			header;
	uint16_t 		length;
	dci_data_t	*data;		
	
}dci_info_t;


/// @} APP


#endif //_DCI_TYPES_H


