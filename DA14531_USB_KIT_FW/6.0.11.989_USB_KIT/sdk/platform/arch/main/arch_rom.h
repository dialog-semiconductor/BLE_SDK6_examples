/**
 ****************************************************************************************
 *
 * @file arch_rom.h
 *
 * @brief ROM initialization functions and declarations.
 *
 * Copyright (c) 2016-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _ARCH_ROM_H_
#define _ARCH_ROM_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include "co_hci.h"
#include "gap_cfg.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum packet header size (considering unpacked structure sizes)
#define MAX_HEADER_SIZE       HCI_ACL_HDR_LEN

///H4TL RX states
enum H4TL_STATE_RX
{
    ///H4TL RX Start State - receive message type
    H4TL_STATE_RX_START,
    ///H4TL RX Header State - receive message header
    H4TL_STATE_RX_HDR,
    ///H4TL RX Header State - receive (rest of) message payload
    H4TL_STATE_RX_PAYL,
    ///H4TL RX Out Of Sync state - receive message type
    H4TL_STATE_RX_OUT_OF_SYNC
};

///H4TL TX states
enum H4TL
{
     ///H4TL TX Start State - when packet is ready to be sent
     H4TL_STATE_TX_ONGOING,
     ///H4TL TX Done State - TX ended with no error
     H4TL_STATE_TX_IDLE,
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern uint8_t hci_cmd_desc_root_tab[];                          // ROM variable
extern uint8_t rom_hci_cmd_desc_root_tab[];                      // ROM variable (init data for hci_cmd_desc_root_tab)
extern char use_h4tl;                                            // ROM variable
extern const struct gap_cfg_user_struct gap_cfg_user_var_struct; // forward declaration
extern uint32_t *rom_func_addr_table;                            // ROM variable
extern uint32_t *rom_cfg_table;                                  // ROM variable
extern void * const rom_func_addr_table_var[];                   // defined in jump_table.c
extern uint32_t rom_cfg_table_var[];                             // defined in jump_table.c
extern bool length_exchange_needed;                              // ROM variable

extern struct h4tl_env_tag h4tl_env;

/// H4TL out of synchronization recovery variables
struct h4tl_out_of_sync_tag
{
    uint8_t byte;
    uint8_t index;
};

/// H4TL Environment context structure
struct h4tl_env_tag
{
    /// pointer to External interface api
    const struct rwip_eif_api* ext_if;

    ///Latest received message header, 4 byte buffer for eother configuration.
    uint8_t curr_hdr_buff[MAX_HEADER_SIZE];
    ///Pointer to space reserved for received payload.
    uint8_t *curr_payl_buff;

    /// HCI synchronization error parameters
    struct h4tl_out_of_sync_tag out_of_sync;

    ///TX callback for indicating the end of transfer
    void (*tx_callback)(void);
    ///Rx state - can be receiving message type, header, payload or error
    uint8_t  rx_state;
    ///Tx state - either transmitting, done or error.
    uint8_t  tx_state;
    ///Latest received message type: CMD/EVT/ACL.
    uint8_t  rx_type;
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize ROM variables.
 * @return void
 ****************************************************************************************
 */
void arch_rom_init(void);

/// @} DRIVERS

#endif // _ARCH_ROM_H_
