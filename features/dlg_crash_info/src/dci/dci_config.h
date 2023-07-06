#ifndef _DCI_CONFIG_H_
#define _DCI_CONFIG_H_


/*
***********************WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*****************************************
* This should be undefined once used in production.  The purpose of this is to test the module with RAM 
* use ONLY.   This modifies the reset procedure to do a soft reset back into RAM and could potentially
* cause isses iwth the scatterload.
*/
#undef DCI_RAMAP_ADDR0_TEST	

/*
* This is the array size for total trace depth on the stack the larger this is,
* the more call traces made available.
*/
#define DCI_CALL_DEPTH						(6)

/*
* Number of resets stored in RAM.  If user is offloading this flash set this to one, transfer the data
* into flash and clear the data to save RAM space.
*/
#define DCI_NUM_RESETS_STORED			(2)


#if (CFG_RET_DATA_UNINIT_SIZE <= 0)
#error "CFG_RET_DATA_UNINIT_SIZE needs to be greater than zero"
#endif 

#define DCI_MEMORY_CHECK


#ifdef CFG_PRINTF

/*
 * User definable print function to be called with adf_print_verbose.
 * undefine this function if printing is not needed, as verbose printing has significant overhead.
 */
#define DCI_PRINTF(...)                         arch_printf(__VA_ARGS__)
#define DCI_PRINT_VERBOSE


#else 

#define DCI_PRINTF(...) 												()
#undef DCI_PRINT_VERBOSE

#endif 
					

/*
*  Enable the example DCI service for transferring data via BLE to a host.  
*/
#define ENABLE_DCI_SERVICE

#endif //_DCI_CONFIG_H_

