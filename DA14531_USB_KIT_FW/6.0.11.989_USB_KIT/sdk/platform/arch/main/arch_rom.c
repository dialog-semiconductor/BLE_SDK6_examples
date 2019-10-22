/**
 ****************************************************************************************
 *
 * @file arch_rom.c
 *
 * @brief ROM initialization functions.
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

/*
 * INCLUDES
 ****************************************************************************************
 */

#include <string.h>
#include "llm.h"
#include "gap_cfg.h"
#include "da1458x_scatter_config.h"
#include "_reg_ble_em_rx_buffer.h"
#include "_reg_ble_em_tx_buffer.h"
#include "em_map_ble.h"
#include "em_map.h"
#include "arch_rom.h"
#include "system_library.h"

#ifdef CFG_DBG_TASK
#include "hci_vs.h"
#endif 

/**
 * @addtogroup DRIVERS
 * @{
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

uint16_t heap_log __SECTION_ZERO("retention_mem_area0");

#if 0 // TODO - to be fixed in ES3 DA14531 ROM
struct custom_msg_handler my_custom_msg_handlers[] = 
{
    { 0, 0, NULL}   //it must end with func = NULL so that loop can terminate correctly
};

extern struct custom_msg_handler *custom_msg_handlers;
#endif

#if defined (__DA14531__)
const uint32_t rom_patched_address1 = 0x7f1c5c4;
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void arch_rom_init(void)
{
    /// Fill BLE EM + ROM data area in SysRAM4 with zeros
    uint32_t *p_retmem = (uint32_t *)__SCT_BLE_BASE;
#if defined (__DA14531__)
    uint32_t nbytes = 0x07FCC000 - __SCT_BLE_BASE;
#else
    uint32_t nbytes = 0x07FD8000 - __SCT_BLE_BASE;
#endif
    for (int i = (nbytes / 4 - 1); i >= 0; i--)
    {
        *(volatile uint32_t *)p_retmem++ = 0;
    }

    // Initialize variables used by ROM patches
    patch_global_vars_init();

#if 0 // TODO - to be fixed in ES3 DA14531 ROM
    custom_msg_handlers = my_custom_msg_handlers;
#endif
    
    // Enable heap log
    heap_log = LOG_HEAP_USAGE;

    // Mandatory HCI root table setup
    memcpy(hci_cmd_desc_root_tab, rom_hci_cmd_desc_root_tab, 48);

#if defined (__DA14531__)
    struct le_features my_llm_local_le_feats = {{ 0xfd, 0, 0, 0, 0, 0, 0, 0 }};
    memcpy(&llm_local_le_feats, &my_llm_local_le_feats, 8);
#endif


#ifdef CFG_DBG_TASK
    // Vendor specific HCI commands initialization
    hci_vs_initialize_commands_tab();
#endif

#ifdef AUTO_DATA_LENGTH_NEGOTIATION_UPON_NEW_CONNECTION
    length_exchange_needed = true;
#endif

    use_h4tl = CFG_USE_H4TL;

    gap_cfg_user = (struct gap_cfg_user_struct *)&gap_cfg_user_var_struct;

    rom_func_addr_table = (uint32_t*)&rom_func_addr_table_var[0];
    rom_cfg_table = &rom_cfg_table_var[0];

    ble_duplicate_filter_max = CFG_BLE_DUPLICATE_FILTER_MAX;
#ifdef CFG_BLE_DUPLICATE_FILTER_FOUND
    ble_duplicate_filter_found = true;
#else
    ble_duplicate_filter_found = false;
#endif
    llm_resolving_list_max = CFG_LLM_RESOLVING_LIST_MAX;

    BLE_TX_DESC_DATA_USER           = __SCT_BLE_TX_DESC_DATA;
    BLE_TX_DESC_CNTL_USER           = __SCT_BLE_TX_DESC_CNTL;

    LLM_LE_ADV_DUMMY_IDX            = __SCT_BLE_TX_DESC_DATA          + __SCT_BLE_TX_DESC_CNTL - 1;
    LLM_LE_SCAN_CON_REQ_ADV_DIR_IDX = LLM_LE_ADV_DUMMY_IDX            + 1;
    LLM_LE_SCAN_RSP_IDX             = LLM_LE_SCAN_CON_REQ_ADV_DIR_IDX + 1;
    LLM_LE_ADV_IDX                  = LLM_LE_SCAN_RSP_IDX             + 1;

    REG_BLE_EM_RX_BUFFER_SIZE       = __SCT_REG_BLE_EM_RX_BUFFER_SIZE;
    REG_BLE_EM_TX_BUFFER_SIZE       = __SCT_REG_BLE_EM_TX_BUFFER_SIZE;

    ASSERT_ERROR(
        (BLE_TX_DESC_DATA == __SCT_BLE_TX_DESC_DATA) &&
        (BLE_TX_DESC_CNTL == __SCT_BLE_TX_DESC_CNTL) &&
        // offsets
        (_EM_BLE_RX_BUFFER_OFFSET == __SCT__EM_BLE_RX_BUFFER_OFFSET) &&
        (EM_BLE_RX_BUFFER_OFFSET  == __SCT_EM_BLE_RX_BUFFER_OFFSET ) &&
        (_EM_BLE_CNXADD_OFFSET    == __SCT__EM_BLE_CNXADD_OFFSET   ) &&
        (EM_BLE_CNXADD_OFFSET     == __SCT_EM_BLE_CNXADD_OFFSET    ) &&
        (_EM_BLE_END              == __SCT__EM_BLE_END             ) &&
        (EM_BLE_END               == __SCT_EM_BLE_END              )
    );

    _ble_base = __SCT_BLE_BASE; // Initialize BLE EM base
}

/// @} DRIVERS
