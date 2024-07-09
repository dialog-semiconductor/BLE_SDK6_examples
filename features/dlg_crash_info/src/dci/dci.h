/**
 ****************************************************************************************
 *
 * @file dci.h
 *
 * @brief dci project header file.
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

#ifndef _DCI_H_
#define _DCI_H_

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
#include "dci_types.h"

/**
 ****************************************************************************************
 * @brief API for user to provide their own epoch into the data structure
 * @param[out] Epoch
 ****************************************************************************************
 */
__WEAK uint32_t dci_get_epoch(void); 


/**
 ****************************************************************************************
 * @brief Initialize the DLG crash info module.   This should be called after the 
 * reset_indication is called from the SDK.
 * @param[in] reset_stat    RESET_STAT_REG 0x50000304
 * @param[out] Boolean if unitialized RAM needs initialized.  User code can utilize this
 * for intiializing their own sections of unitialized retention RAM.
 ****************************************************************************************
 */

bool dci_init(uint16_t reset_stat);
/**
 ****************************************************************************************
 * @brief Clear RAM DCI data to allow for tracking of more resets.  Should be called
 * after the data is retrieved with dci_get_data
 ****************************************************************************************
 */
void dci_clear_data(void);

/**
 ****************************************************************************************
 * @brief Get relevant crash information.  Should be called prior to fetching and
 * transferring data to host, flash, etc.
 * @param[in] info - crash info as specified in dcy_types.h
 ****************************************************************************************
 */

void dci_get_data(dci_info_t *info);



void HardFault_HandlerC(unsigned long *hardfault_args);
void NMI_HandlerC(unsigned long *nmi_args);

#ifdef DCI_PRINT_VERBOSE

/**
 ****************************************************************************************
 * @brief Helper for printing crash data on startup.  Uses extra memory space and should 
 * be undefined once another transfer medium is in place.
 ****************************************************************************************
 */
void dci_print_verbose(void);

#endif 

/// @} APP

#endif //_DCI_H_


