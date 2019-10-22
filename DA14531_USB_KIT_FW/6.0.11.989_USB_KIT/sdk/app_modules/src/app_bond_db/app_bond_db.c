/**
 *****************************************************************************************
 *
 * @file app_bond_db.c
 *
 * @brief Bond database code file.
 *
 * Copyright (c) 2012-2019 Dialog Semiconductor. All rights reserved.
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
 *****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP_BOND_DB
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_APP_SEC)

#include "rwip.h"
#include "app_bond_db.h"



/*
 * DEFINES
 ****************************************************************************************
 */
#define BOND_DB_VALID_ENTRY             (0xAA)
#define BOND_DB_EMPTY_SLOT              (0)
#define BOND_DB_SLOT_NOT_FOUND          (0xFF)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct bond_db
{
    uint16_t start_hdr;
    uint8_t valid_slot[APP_BOND_DB_MAX_BONDED_PEERS];
    uint32_t timestamp_counter;
    uint32_t timestamp[APP_BOND_DB_MAX_BONDED_PEERS];
    struct app_sec_bond_data_env_tag data[APP_BOND_DB_MAX_BONDED_PEERS];
    uint16_t end_hdr;
};

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

static struct bond_db bdb __SECTION_ZERO("retention_mem_area0"); //@RETENTION MEMORY

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * STATIC FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if defined (USER_CFG_APP_BOND_DB_USE_SPI_FLASH)

static void bond_db_spi_flash_init(void)
{
    uint8_t dev_id;

    // Release Flash from power down
    spi_flash_release_from_power_down();

    // Try to auto-detect the device
    spi_flash_auto_detect(&dev_id);
}

static void bond_db_load_flash(void)
{
    uint16_t actual_size;
    bond_db_spi_flash_init();

    spi_flash_read_data((uint8_t *)&bdb, APP_BOND_DB_DATA_OFFSET, sizeof(struct bond_db),
                        &actual_size);

    // Power down flash
    spi_flash_power_down();
}

/**
 ****************************************************************************************
 * @brief Erase Flash sectors where bond database is stored
 * @param[in] scheduler_en  True: Enable rwip_scheduler while Flash is being erased
 *                          False: Do not enable rwip_scheduler. Blocking mode
 * @return ret              Error code or success (ERR_OK)
 ****************************************************************************************
 */
static int8_t bond_db_erase_flash_sectors(bool scheduler_en)
{
    uint32_t sector_nb;
    uint32_t offset;
    int8_t ret;
    int i;
    uint32_t timeout_cnt;

    // Calculate the starting sector offset
    offset = (APP_BOND_DB_DATA_OFFSET / SPI_FLASH_SECTOR_SIZE) * SPI_FLASH_SECTOR_SIZE;

    // Calculate the numbers of sectors to erase
    sector_nb = (sizeof(bdb) / SPI_FLASH_SECTOR_SIZE);
    if (sizeof(bdb) % SPI_FLASH_SECTOR_SIZE)
        sector_nb++;

    for (i = 0; i < sector_nb; i++)
    {
        if (scheduler_en)
        {
            // Non-Blocking Erase of a Flash sector
            ret = spi_flash_block_erase_no_wait(offset, SPI_FLASH_OP_SE);
            if (ret != SPI_FLASH_ERR_OK)
                break;

            timeout_cnt = 0;

            while ((spi_flash_read_status_reg() & SPI_FLASH_SR_BUSY) != 0)
            {
                // Check if BLE is on and not in deep sleep and call rwip_schedule()
                if ((GetBits16(CLK_RADIO_REG, BLE_ENABLE) == 1) &&
                   (GetBits32(BLE_DEEPSLCNTL_REG, DEEP_SLEEP_STAT) == 0))
                {
                    // Assuming that the WDG is not active, timeout will be reached in case of a Flash erase error.
                    // NOTE: In case the WDG is active, the WDG timer will expire (much) earlier than the timeout
                    // is reached and therefore an NMI will be triggered.
                    if (++timeout_cnt > SPI_FLASH_WAIT)
                    {
                        return SPI_FLASH_ERR_TIMEOUT;
                    }
                    rwip_schedule();
                }
            }
        }
        else
        {
            // Blocking Erase of a Flash sector
            ret = spi_flash_block_erase(offset, SPI_FLASH_OP_SE);
            if (ret != SPI_FLASH_ERR_OK)
                break;
        }
        offset += SPI_FLASH_SECTOR_SIZE;
    }

    return ret;
}

/**
 ****************************************************************************************
 * @brief Store Bond Database to Flash memory
 * @param[in] scheduler_en  True: Enable rwip_scheduler while Flash is being erased
 *                          False: Do not enable rwip_scheduler. Blocking mode
 * @return none
 ****************************************************************************************
 */
static void bond_db_store_flash(bool scheduler_en)
{
    uint16_t actual_size;
    int8_t ret;

    bond_db_spi_flash_init();

    ret = bond_db_erase_flash_sectors(scheduler_en);
    if (ret == SPI_FLASH_ERR_OK)
    {
        spi_flash_write_data((uint8_t *)&bdb, APP_BOND_DB_DATA_OFFSET,
                             sizeof(struct bond_db), &actual_size);
    }

    // Power down flash
    spi_flash_power_down();
}

#elif defined (USER_CFG_APP_BOND_DB_USE_I2C_EEPROM)

static void bond_db_load_eeprom(void)
{
    uint32_t bytes_read;

    // Initialize I2C for Serial EEPROM
    i2c_eeprom_initialize();

    i2c_eeprom_read_data((uint8_t *)&bdb, APP_BOND_DB_DATA_OFFSET, sizeof(struct bond_db), &bytes_read);
    ASSERT_ERROR(bytes_read == sizeof(struct bond_db));

    i2c_eeprom_release();
}

static void bond_db_store_eeprom(void)
{
    uint32_t bytes_written;

    // Initialize I2C for Serial EEPROM
    i2c_eeprom_initialize();

    i2c_eeprom_write_data((uint8_t *)&bdb, APP_BOND_DB_DATA_OFFSET, sizeof(struct bond_db), &bytes_written);
    ASSERT_ERROR(bytes_written == sizeof(struct bond_db));

    i2c_eeprom_release();
}
#endif

/**
 ****************************************************************************************
 * @brief Load Bond Database from external memory
 * @return none
 ****************************************************************************************
 */
static inline void bond_db_load_ext(void)
{
    #if defined (USER_CFG_APP_BOND_DB_USE_SPI_FLASH)
    bond_db_load_flash();
    #elif defined (USER_CFG_APP_BOND_DB_USE_I2C_EEPROM)
    bond_db_load_eeprom();
    #endif
}

/**
 ****************************************************************************************
 * @brief Store Bond Database to external memory
 * @param[in] scheduler_en  Only used if external memory is Flash
                            True: Enable rwip_scheduler while Flash is being erased
 *                          False: Do not enable rwip_scheduler. Blocking mode
 * @return none
 ****************************************************************************************
 */
static inline void bond_db_store_ext(bool scheduler_en)
{
    #if defined (USER_CFG_APP_BOND_DB_USE_SPI_FLASH)
    bond_db_store_flash(scheduler_en);
    #elif defined (USER_CFG_APP_BOND_DB_USE_I2C_EEPROM)
    bond_db_store_eeprom();
    #endif
}

/**
 ****************************************************************************************
 * @brief Store Bond data entry to external memory
 * @param[in] *data  Data to be stored
 * @param[in] *idx   Entry in the database
 * @return none
 ****************************************************************************************
 */
static void bond_db_store_at_idx(struct app_sec_bond_data_env_tag *data, int idx)
{
    bdb.valid_slot[idx] = BOND_DB_VALID_ENTRY;
    // Update the cache
    memcpy(&bdb.data[idx], data, sizeof(struct app_sec_bond_data_env_tag));
    // Store new bond data to external memory
    // In case of Flash (erase then write) enable the scheduler
    bond_db_store_ext(true);
}

/**
 ****************************************************************************************
 * @brief Clear bond data.
 * @return void
 ****************************************************************************************
 */
static void bond_db_clear(bool scheduler_en)
{
    memset((void *)&bdb, 0, sizeof(struct bond_db) ); // zero bond data
    bdb.start_hdr = BOND_DB_HEADER_START;
    bdb.end_hdr = BOND_DB_HEADER_END;
    // Store zero bond data to external memory
    // In case of Flash (erase then write) do not enable the scheduler
    bond_db_store_ext(scheduler_en);
}

/*
 * EXPOSED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void default_app_bdb_init(void)
{
    // Load bond data from the external memory resource
    bond_db_load_ext();

    // Simple check for garbage in memory (this also catches the 0xFF of cleared memory)
    if ((bdb.start_hdr != BOND_DB_HEADER_START) || (bdb.end_hdr != BOND_DB_HEADER_END))
    {
        bond_db_clear(false);
    }
}

uint8_t default_app_bdb_get_size(void)
{
    return APP_BOND_DB_MAX_BONDED_PEERS;
}

void default_app_bdb_add_entry(struct app_sec_bond_data_env_tag *data)
{
    bool first_empty_slot_found = false;
    uint8_t i = 0;
    uint32_t min_timestamp;
    uint8_t slot_to_write = BOND_DB_SLOT_NOT_FOUND;

    for(i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
    {
        // If current slot is not valid (is empty)
        if (bdb.valid_slot[i] != BOND_DB_VALID_ENTRY)
        {
            // Check if an empty slot has already been found
            if (first_empty_slot_found == false)
            {
                // First empty slot has been found
                first_empty_slot_found = true;
                // Store empty slot to write
                slot_to_write = i;
            }
        }
        // If current slot is not empty
        else
        {
            // Check if IRK is present in current slot and in new pairing data
            if ((bdb.data[i].valid_keys & *(&data->valid_keys)) & RIRK_PRESENT)
            {
                // Check if stored IRK matches with new IRK
                if (memcmp(&data->rirk.irk, &bdb.data[i].rirk.irk, sizeof(struct gap_sec_key)) == 0)
                {
                    // IRK matches, store this slot to be replaced and exit
                    slot_to_write = i;
                    break;
                }
            }
            // If IRK is not present
            else
            {
                // Check if stored BD address matches with new BD address
                if (memcmp(&data->peer_bdaddr, &bdb.data[i].peer_bdaddr, sizeof(struct gap_bdaddr)) == 0)
                {
                    // BD address matches, store this slot to be replaced and exit
                    slot_to_write = i;
                    break;
                }
            }
        }
    }

    // If there is no available slot, find the least recently written slot to replace
    if (slot_to_write == BOND_DB_SLOT_NOT_FOUND)
    {
        min_timestamp = bdb.timestamp[0];
        slot_to_write = 0;
        for(i = 1; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
        {
            // Find the slot with the oldest (minimum) timestamp
            if (bdb.timestamp[i] < min_timestamp)
            {
                min_timestamp = bdb.timestamp[i];
                slot_to_write = i;
            }
        }
    }
    bdb.timestamp[slot_to_write] = bdb.timestamp_counter++;
    // Store bond db entry
    data->bdb_slot = slot_to_write;
    bond_db_store_at_idx(data, slot_to_write);
}

void default_app_bdb_remove_entry(enum bdb_search_by_type search_type, enum bdb_remove_type remove_type,
                          void *search_param, uint8_t search_param_length)
{
    uint8_t i = 0;
    uint8_t slot_found = BOND_DB_SLOT_NOT_FOUND;

    if (remove_type == REMOVE_ALL)
    {
        bond_db_clear(true);
        return;
    }

    if (search_type == SEARCH_BY_SLOT_TYPE)
    {
        slot_found = *((uint8_t *)search_param);
    }
    else
    {
        for(i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
        {
            // Check if EDIVs match
            if ((search_type == SEARCH_BY_EDIV_TYPE) &&
                ((memcmp(&bdb.data[i].ltk.ediv, search_param, search_param_length) == 0)))
            {
                slot_found = i;
                break;
            }
            // Check if BD addresses match
            else if ((search_type == SEARCH_BY_BDA_TYPE) &&
                     ((memcmp(&bdb.data[i].peer_bdaddr.addr, search_param, search_param_length) == 0)))
            {
                slot_found = i;
                break;
            }
            // Check if IRKs match
            else if ((search_type == SEARCH_BY_IRK_TYPE) &&
                     (memcmp(&bdb.data[i].rirk, search_param, search_param_length) == 0))
            {
                slot_found = i;
                break;
            }
            else if (search_type == SEARCH_BY_ID_TYPE)
            {
                // Check if bond_db BD address and given ID address match
                if ((memcmp(&bdb.data[i].rirk.addr.addr, search_param, search_param_length) == 0))
                {
                    slot_found = i;
                    break;
                }
            }
        }
    }

    // Check if a valid slot has been found
    if (slot_found < APP_BOND_DB_MAX_BONDED_PEERS)
    {
        if (remove_type == REMOVE_THIS_ENTRY)
        {
            // Remove entry from cache
            memset((void *)&bdb.data[slot_found], 0, sizeof(struct app_sec_bond_data_env_tag));
            bdb.timestamp[slot_found] = 0;
            bdb.valid_slot[slot_found] = BOND_DB_EMPTY_SLOT;
        }
        else
        {
            // If remove_all_but_this is true, remove all other entries from cache
            for(i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
            {
                if (i != slot_found)
                {
                    memset((void *)&bdb.data[i], 0, sizeof(struct app_sec_bond_data_env_tag));
                    bdb.timestamp[i] = 0;
                    bdb.valid_slot[i] = BOND_DB_EMPTY_SLOT;
                }
            }
        }
        // Store the updated cache to the external non volatile memory
        bond_db_store_ext(true);
    }
}

const struct app_sec_bond_data_env_tag* default_app_bdb_search_entry(enum bdb_search_by_type search_type,
                                                             void *search_param,
                                                             uint8_t search_param_length)
{
    struct app_sec_bond_data_env_tag *found_data = NULL;

    for(uint8_t i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
    {
        // Check if EDIVs match
        if ((search_type == SEARCH_BY_EDIV_TYPE) &&
            ((memcmp(&bdb.data[i].ltk.ediv, search_param, search_param_length) == 0)))
        {
            found_data = &bdb.data[i];
            break;
        }
        // Check if BD addresses match
        else if ((search_type == SEARCH_BY_BDA_TYPE) &&
                 ((memcmp(&bdb.data[i].peer_bdaddr.addr, search_param, search_param_length) == 0)))
        {
            found_data = &bdb.data[i];
            break;
        }
        // Check if IRKs match
        else if ((search_type == SEARCH_BY_IRK_TYPE) &&
                 (memcmp(&bdb.data[i].rirk, search_param, search_param_length) == 0))
        {
            found_data = &bdb.data[i];
            break;
        }
        // Check if IDs match
        else if ((search_type == SEARCH_BY_ID_TYPE) &&
                (memcmp(&bdb.data[i].rirk.addr.addr, search_param, search_param_length) == 0))
        {
            found_data = &bdb.data[i];
            break;
        }
    }

    return found_data;
}

uint8_t default_app_bdb_get_number_of_stored_irks(void)
{
    uint8_t nb_key = 0;

    // Search DB to find the entries which contain an IRK
    for(uint8_t i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
    {
        // Check if there is an IRK present in the current slot
        if ((bdb.data[i].valid_keys & RIRK_PRESENT) == RIRK_PRESENT)
            nb_key++;
    }

    return nb_key;
}

uint8_t default_app_bdb_get_stored_irks(struct gap_sec_key *valid_irk_irray)
{
    uint8_t nb_key = 0;

    // Search DB to find the entries which contain an IRK
    for(uint8_t i = 0; i < APP_BOND_DB_MAX_BONDED_PEERS; i++)
    {
        // Check if there is an IRK present in the current slot
        if ((bdb.data[i].valid_keys & RIRK_PRESENT) == RIRK_PRESENT)
        {
            // Copy IRK to the IRK array
            memcpy(&valid_irk_irray[nb_key], &bdb.data[i].rirk.irk, sizeof(struct gap_sec_key));
            nb_key++;
        }
    }

    return nb_key;
}

bool default_app_bdb_get_device_info_from_slot(uint8_t slot, struct gap_ral_dev_info *dev_info)
{
    // Check if the entry is valid and if there is an IRK
    if ((bdb.valid_slot[slot] == BOND_DB_VALID_ENTRY) && ((bdb.data[slot].valid_keys & RIRK_PRESENT) == RIRK_PRESENT))
    {
        memcpy(dev_info->addr, &bdb.data[slot].rirk.addr.addr.addr, BD_ADDR_LEN*sizeof(uint8_t));
        dev_info->addr_type = bdb.data[slot].rirk.addr.addr_type;
        memcpy(dev_info->peer_irk, &bdb.data[slot].rirk.irk, sizeof(struct gap_sec_key));
        return true;
    }
    return false;
}
#endif // (BLE_APP_SEC)

/// @} APP_BOND_DB
