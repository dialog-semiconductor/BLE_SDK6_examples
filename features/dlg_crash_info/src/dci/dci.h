#ifndef _DCI_H_
#define _DCI_H_

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

#ifdef DCI_PRINT_VERBOSE

/**
 ****************************************************************************************
 * @brief Helper for printing crash data on startup.  Uses extra memory space and should 
 * be undefined once another transfer medium is in place.
 ****************************************************************************************
 */
void dci_print_verbose(void);

#endif 

#endif //_DCI_H_


