/**
 ****************************************************************************************
 *
 * @file spi_flash.h
 *
 * @brief SPI Flash driver for the Standard SPI Flash header file.
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
 ****************************************************************************************
 */

#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "spi.h"
#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */

#define SPI_FLASH_DRIVER_VERSION (3)
#define SPI_FLASH_DRIVER_SUBVERSION (1)

/*
    Tested SPI FLASH devices
        - W25x10/Windbond
        - W25x20/Winbond
        - AT25DS011/Adesto
        - MX25R2035F/Macronix (embedded in DA14586)
        - P25Q10U
        - GD25WD20
*/

// Global Definitions for standard SPI Flash Devices
#define SPI_FLASH_SECTOR_SIZE                   4096
#define SPI_FLASH_PAGE_SIZE                     256
#define SPI_FLASH_MEM_PROT_NONE                 0
#define SPI_FLASH_MEM_PROT_MASK                 0x3C

// SPI Flash Device-specific definitions

/*
    Winbond SPI Flash Devices
*/

// W25X10CL
#define W25X10CL_MAN_DEV_ID                     0xEF10
#define W25X10CL_JEDEC_ID                       0xEF3011
#define W25X10CL_CHIP_SIZE                      0x20000
#define W25X10CL_MEM_PROT_UPPER_HALF            4
#define W25X10CL_MEM_PROT_LOWER_HALF            36
#define W25X10CL_MEM_PROT_ALL                   8

// W25X20CL
#define W25X20CL_MAN_DEV_ID                     0xEF11
#define W25X20CL_JEDEC_ID                       0xEF3012
#define W25X20CL_CHIP_SIZE                      0x40000
#define W25X20CL_MEM_PROT_UPPER_QUARTER         4
#define W25X20CL_MEM_PROT_UPPER_HALF            8
#define W25X20CL_MEM_PROT_LOWER_QUARTER         36
#define W25X20CL_MEM_PROT_LOWER_HALF            40
#define W25X20CL_MEM_PROT_ALL                   12

/*
    Adesto SPI Flash Devices
*/

// AT25DN011, AT25DF011
#define AT25DX011_JEDEC_ID                      0x1F4200
#define AT25DX011_CHIP_SIZE                     0x20000
#define AT25DX011_MEM_PROT_ALL                  0x04

/*
    Macronix SPI Flash Devices
*/

// MX25V1006E
#define MX25V1006E_MAN_DEV_ID                   0xC210
#define MX25V1006E_JEDEC_ID                     0xC22011
#define MX25V1006E_CHIP_SIZE                    0x20000
#define MX25V1006E_MEM_PROT_ALL                 0x0C

// MX25R1035F
#define MX25R1035F_MAN_DEV_ID                   0xC211
#define MX25R1035F_JEDEC_ID                     0xC22811
#define MX25R1035F_CHIP_SIZE                    0x20000
#define MX25R1035F_MEM_PROT_1_BLOCK             0x04
#define MX25R1035F_MEM_PROT_ALL                 0x0C

// MX25R2035F
#define MX25R2035F_MAN_DEV_ID                   0xC212
#define MX25R2035F_JEDEC_ID                     0xC22812
#define MX25R2035F_CHIP_SIZE                    0x40000
#define MX25R2035F_MEM_PROT_1_BLOCK             0x04
#define MX25R2035F_MEM_PROT_2_BLOCK             0x08
#define MX25R2035F_MEM_PROT_ALL                 0x0C

// MX25R4035F
#define MX25R4035F_MAN_DEV_ID                   0xC213
#define MX25R4035F_JEDEC_ID                     0xC22813
#define MX25R4035F_CHIP_SIZE                    0x400000
#define MX25R4035F_MEM_PROT_1_BLOCK             0x04
#define MX25R4035F_MEM_PROT_2_BLOCK             0x08
#define MX25R4035F_MEM_PROT_4_BLOCK             0x0C
#define MX25R4035F_MEM_PROT_ALL                 0x10

// MX25R8035F
#define MX25R8035F_MAN_DEV_ID                   0xC214
#define MX25R8035F_JEDEC_ID                     0xC22814
#define MX25R8035F_CHIP_SIZE                    0x800000
#define MX25R8035F_MEM_PROT_1_BLOCK             0x04
#define MX25R8035F_MEM_PROT_2_BLOCK             0x08
#define MX25R8035F_MEM_PROT_4_BLOCK             0x0C
#define MX25R8035F_MEM_PROT_8_BLOCK             0x10
#define MX25R8035F_MEM_PROT_ALL                 0x14

// MX25R1635F
#define MX25R1635F_MAN_DEV_ID                   0xC215
#define MX25R1635F_JEDEC_ID                     0xC22815
#define MX25R1635F_CHIP_SIZE                    0x1000000
#define MX25R1635F_MEM_PROT_1_BLOCK             0x04
#define MX25R1635F_MEM_PROT_2_BLOCK             0x08
#define MX25R1635F_MEM_PROT_4_BLOCK             0x0C
#define MX25R1635F_MEM_PROT_8_BLOCK             0x10
#define MX25R1635F_MEM_PROT_16_BLOCK            0x14
#define MX25R1635F_MEM_PROT_ALL                 0x18

#define MX25R_MAN_TYP_ID 0xC228

// MX25V1035F
#define MX25V1035F_MAN_DEV_ID                   0xC211
#define MX25V1035F_JEDEC_ID                     0xC22311
#define MX25V1035F_CHIP_SIZE                    0x20000
#define MX25V1035F_MEM_PROT_1_BLOCK             0x04
#define MX25V1035F_MEM_PROT_ALL                 0x0C

// MX25V2035F
#define MX25V2035F_MAN_DEV_ID                   0xC212
#define MX25V2035F_JEDEC_ID                     0xC22312
#define MX25V2035F_CHIP_SIZE                    0x40000
#define MX25V2035F_MEM_PROT_1_BLOCK             0x04
#define MX25V2035F_MEM_PROT_2_BLOCK             0x08
#define MX25R2035F_MEM_PROT_ALL                 0x0C

// MX25V4035F
#define MX25V4035F_MAN_DEV_ID                   0xC213
#define MX25V4035F_JEDEC_ID                     0xC22313
#define MX25V4035F_CHIP_SIZE                    0x400000
#define MX25V4035F_MEM_PROT_1_BLOCK             0x04
#define MX25V4035F_MEM_PROT_2_BLOCK             0x08
#define MX25V4035F_MEM_PROT_4_BLOCK             0x0C
#define MX25V4035F_MEM_PROT_ALL                 0x10

// MX25V8035F
#define MX25V8035F_MAN_DEV_ID                   0xC214
#define MX25V8035F_JEDEC_ID                     0xC22314
#define MX25V8035F_CHIP_SIZE                    0x800000
#define MX25V8035F_MEM_PROT_1_BLOCK             0x04
#define MX25V8035F_MEM_PROT_2_BLOCK             0x08
#define MX25V8035F_MEM_PROT_4_BLOCK             0x0C
#define MX25V8035F_MEM_PROT_8_BLOCK             0x10
#define MX25V8035F_MEM_PROT_FULL                0x14

// MX25V1635F
#define MX25V1635F_MAN_DEV_ID                   0xC215
#define MX25V1635F_JEDEC_ID                     0xC22315
#define MX25V1635F_CHIP_SIZE                    0x1000000
#define MX25V1635F_MEM_PROT_1_BLOCK             0x04
#define MX25V1635F_MEM_PROT_2_BLOCK             0x08
#define MX25V1635F_MEM_PROT_4_BLOCK             0x0C
#define MX25V1635F_MEM_PROT_8_BLOCK             0x10
#define MX25V1635F_MEM_PROT_16_BLOCK            0x14
#define MX25V1635F_MEM_PROT_ALL                 0x18

#define MX25V_F_MAN_TYP_ID 0xC223

/*
    Puya SPI Flash Devices
*/

// P25Q10U
#define P25Q10U_MAN_DEV_ID                      0x8510
#define P25Q10U_JEDEC_ID                        0x856011
#define P25Q10U_CHIP_SIZE                       0x20000
#define P25Q10U_MEM_PROT_UPPER_HALF             4
#define P25Q10U_MEM_PROT_LOWER_HALF             36
#define P25Q10U_MEM_PROT_ALL                    8

// P25Q40U
#define P25Q40U_MAN_DEV_ID                      0x8512
#define P25Q40U_JEDEC_ID                        0x856012
#define P25Q40U_CHIP_SIZE                       0x40000
#define P25Q40U_MEM_PROT_UPPER_EIGHTH           4
#define P25Q40U_MEM_PROT_UPPER_QUARTER          8
#define P25Q40U_MEM_PROT_UPPER_HALF             12
#define P25Q40U_MEM_PROT_LOWER_EIGHTH           36
#define P25Q40U_MEM_PROT_LOWER_QUARTER          40
#define P25Q40U_MEM_PROT_LOWER_HALF             44
#define P25Q40U_MEM_PROT_ALL                    16

/*
    GigaDevice SPI Flash Devices
*/

// GD25WD10
#define GD25WD10_MAN_DEV_ID                     0xC810
#define GD25WD10_JEDEC_ID                       0xC86411
#define GD25WD10_CHIP_SIZE                      0x20000
#define GD25WD10_MEM_PROT_LOWER_HALF            16
#define GD25WD10_MEM_PROT_ALL                   20

// GD25WD20
#define GD25WD20_MAN_DEV_ID                     0xC811
#define GD25WD20_JEDEC_ID                       0xC86412
#define GD25WD20_CHIP_SIZE                      0x40000
#define GD25WD20_MEM_PROT_LOWER_HALF            20
#define GD25WD20_MEM_PROT_ALL                   24


/// Device index of known SPI Flash devices
typedef enum
{
    W25X10CL_DEV_INDEX = 1,
    W25X20CL_DEV_INDEX,
    AT25DX011_DEV_INDEX,
    MX25V1006E_DEV_INDEX,
    MX25R1035F_DEV_INDEX,
    MX25R2035F_DEV_INDEX,
    MX25R4035F_DEV_INDEX,
    MX25R8035F_DEV_INDEX,
    MX25R1635F_DEV_INDEX,
    MX25V1035F_DEV_INDEX,
    MX25V2035F_DEV_INDEX,
    MX25V4035F_DEV_INDEX,
    MX25V8035F_DEV_INDEX,
    MX25V1635F_DEV_INDEX,
    P25Q10U_DEV_INDEX,
    P25Q40U_DEV_INDEX,
    GD25WD10_DEV_INDEX,
    GD25WD20_DEV_INDEX,
} spi_flash_dev_index_t;

/* Timeout Definitions */
#define SPI_FLASH_WAIT                          2000000

/* Error Codes */
#define SPI_FLASH_ERR_OK                        (0)
#define SPI_FLASH_ERR_TIMEOUT                   (-1)
#define SPI_FLASH_ERR_NOT_ERASED                (-2)
#define SPI_FLASH_ERR_PROTECTED                 (-3)
#define SPI_FLASH_ERR_INVAL                     (-4)
#define SPI_FLASH_ERR_ALIGN                     (-5)
#define SPI_FLASH_ERR_UNKNOWN_FLASH_VENDOR      (-6)
#define SPI_FLASH_ERR_UNKNOWN_FLASH_TYPE        (-7)
#define SPI_FLASH_ERR_PROG_ERROR                (-8)
#define SPI_FLASH_ERR_READ_ERROR                (-9)
#define SPI_FLASH_ERR_NOT_DETECTED              (-10)
#define SPI_FLASH_ERR_AUTODETECT_ERROR          (-11)
#define SPI_FLASH_ERR_WEL_ERROR                 (-12)
#define SPI_FLASH_ERR_ERASE_ERROR               (-13)
#define SPI_FLASH_ERR_BUSY                      (-14)

/* SPI Flash Status Register Bitfields */
#define SPI_FLASH_SR_BUSY                       0x01
#define SPI_FLASH_SR_WEL                        0x02
#define SPI_FLASH_SR_BP0                        0x04
#define SPI_FLASH_SR_BP1                        0x08
#define SPI_FLASH_SR_BP2                        0x10
#define SPI_FLASH_SR_BP3                        0x20
#define SPI_FLASH_SR_BP4                        0x40
#define SPI_FLASH_SR_TB                         0x20
#define SPI_FLASH_SR_SRP                        0x80

/* SPI Flash Security Register Bitfields */
#define SPI_FLASH_SCR_PFAIL                     5
#define SPI_FLASH_SCR_EFAIL                     6

/* SPI Flash Power Modes */
#define SPI_FLASH_PM_LPM                        0x00
#define SPI_FLASH_PM_HPM                        0x02

/* SPI Flash Standard Commands */
/// Write Enable
#define SPI_FLASH_OP_WREN                       0x06
/// Write Enable volatile
#define SPI_FLASH_OP_VWREN                      0x50
/// Write Disable
#define SPI_FLASH_OP_WRDI                       0x04
/// Read Status Register
#define SPI_FLASH_OP_RDSR                       0x05
/// Write Status Register
#define SPI_FLASH_OP_WRSR                       0x01
/// Read Configuration Register
#define SPI_FLASH_OP_RDCFGR                     0x15
/// Read Security Register
#define SPI_FLASH_OP_RDSCUR                     0x2B
/// Power Down
#define SPI_FLASH_OP_DP                         0xB9
/// Release from Power Down
#define SPI_FLASH_OP_RDP                        0xAB
/// Read Manufacturer and Device ID
#define SPI_FLASH_OP_REMS                       0x90
/// Read Unique ID
#define SPI_FLASH_OP_RUID                       0x4B
/// Read JEDEC ID
#define SPI_FLASH_OP_RDID                       0x9F
/// Page Program
#define SPI_FLASH_OP_PP                         0x02
/// Read Data
#define SPI_FLASH_OP_READ                       0x03
/// Page Erase
#define SPI_FLASH_OP_PE                         0x81
/// Chip Erase
#define SPI_FLASH_OP_CE                         0xC7
/// Chip Erase Alternative
#define SPI_FLASH_OP_CE2                        0x60

/// Block/Sector Erase commands
typedef enum
{
    /// Sector Erase
    SPI_FLASH_OP_SE   =                         0x20,

    /// Block32 Erase
    SPI_FLASH_OP_BE32  =                        0x52,

    /// Block64 Erase
    SPI_FLASH_OP_BE64  =                        0xD8,
} spi_flash_op_t;

#if defined (__DA14586__)
    // DA14586 Internal SPI Flash pins
    #define SPI_EN_PORT                         GPIO_PORT_2
    #define SPI_EN_PIN                          GPIO_PIN_3

    #define SPI_CLK_PORT                        GPIO_PORT_2
    #define SPI_CLK_PIN                         GPIO_PIN_0

    #define SPI_DO_PORT                         GPIO_PORT_2
    #define SPI_DO_PIN                          GPIO_PIN_9

    #define SPI_DI_PORT                         GPIO_PORT_2
    #define SPI_DI_PIN                          GPIO_PIN_4

    #define SPI_FLASH_DEV_SIZE                  MX25R2035F_CHIP_SIZE
#endif


// SPI Flash configuration struct
typedef struct
{
    /// Device Index in known devices list
    uint8_t dev_index;

    /// JEDEC ID
    uint32_t jedec_id;

    /// SPI Flash total size in Bytes
    uint32_t chip_size;
} spi_flash_cfg_t;



/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Check if SPI Flash is busy
 * @return Status       SPI_FLASH_ERR_OK if device is ready,
 *                      SPI_FLASH_ERR_BUSY if device is busy or not attached
 ****************************************************************************************
 */
int8_t spi_flash_is_busy(void);

/**
 ****************************************************************************************
 * @brief Wait until SPI Flash is ready
 * @details Reads the busy signal of the Status Register until it is ready. If Status
 *          Register is still busy after SPI_FLASH_WAIT reads it will time out.
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_wait_till_ready(void);

/**
 ****************************************************************************************
 * @brief Initialize the SPI block and release SPI Flash memory from power down
 * @param[in] spi_cfg       Pointer to the configuration struct of the SPI block
 * @param[in] spi_flash_cfg Pointer to the struct with SPI Flash device's parameters
 * @return Error code
 ****************************************************************************************
*/
int8_t spi_flash_enable(const spi_cfg_t *spi_cfg, const spi_flash_cfg_t *spi_flash_cfg);

/**
 ****************************************************************************************
 * @brief Initialize the SPI block and try to auto-detect and enable the attached SPI
 *        Flash
 * @param[in] spi_cfg       Pointer to the configuration struct of the SPI block
 * @param[out] dev_id       Device index found
 * @return Error code
 ****************************************************************************************
*/
int8_t spi_flash_enable_with_autodetect(const spi_cfg_t *spi_cfg, uint8_t *dev_id);

/**
 ****************************************************************************************
 * @brief Configure SPI Flash environment with device's parameters
 * @param[in] spi_flash_cfg Pointer to the struct with SPI Flash device's parameters
 * @return void
 ****************************************************************************************
 */
void spi_flash_configure_env(const spi_flash_cfg_t *spi_flash_cfg);

/**
 ****************************************************************************************
 * @brief Try to auto-detect an SPI Flash device
 * @details Auto-detect is performed based on the JEDEC ID. If the device has been
 *          successfully identified then its parameters (device index and chip size) will
 *          be derived from a known devices look-up table.
 * @param[out] dev_id       Device index found
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_auto_detect(uint8_t *dev_id);

/**
 ****************************************************************************************
 * @brief Sends the Power-Down instruction
 * @note The function spi_flash_release_from_powerdown() is used to enable the IC again
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_power_down(void);

/**
 ****************************************************************************************
 * @brief Sends the Release from Power-Down instruction
 * @details This function is used to restore the IC from power-down mode
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_release_from_power_down(void);

/**
 ****************************************************************************************
 * @brief Read SPI Flash Status Register
 * @return Status Register value
 ****************************************************************************************
 */
uint16_t spi_flash_read_status_reg(void);

/**
 ****************************************************************************************
 * @brief Issue a Write Enable Command
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_set_write_enable(void);

/**
 ****************************************************************************************
 * @brief Issue a Write Enable Volatile Command
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_write_enable_volatile(void);

/**
 ****************************************************************************************
 * @brief Issue a Write Disable Command
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_set_write_disable(void);

/**
 ****************************************************************************************
 * @brief Write SPI Flash Status Register
 * @param[in] data          Value to be written to Status Register
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_write_status_reg(uint8_t data);

/**
 ****************************************************************************************
 * @brief Read Configuration Register
 * @param[out] data         Content of Configuration Register
 * @return void
 ****************************************************************************************
 */
void spi_flash_read_config_reg(uint32_t *data);

/**
 ****************************************************************************************
 * @brief Write Status & Config register
 * @param[in] data          Value to be written to Status and Configuration Register
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_write_status_config_reg(uint32_t data);

/**
 ****************************************************************************************
 * @brief Read Security Register
 * @param[out] data         Content of Configuration Register
 * @return void
 ****************************************************************************************
 */
void spi_flash_read_security_reg(uint32_t *data);

/**
 ****************************************************************************************
 * @brief Get Power Mode
 * @param[out] data         Current power mode
 * @return void
 ****************************************************************************************
 */
void spi_flash_get_power_mode(uint32_t *data);

/**
 ****************************************************************************************
 * @brief Set Power Mode
 * @param[in] data          Low Power Mode (LPM) or High Performance Mode (HPM)
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_set_power_mode(uint8_t data);

/**
 ****************************************************************************************
 * @brief Get SPI Flash Manufacturer/Device ID
 * @param[out] data         Pointer to the Manufacturer/Device ID
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_read_rems_id(uint16_t *data);

/**
 ****************************************************************************************
 * @brief Get SPI Flash Unique ID Number
 * @param[out] data         Pointer to the Unique ID Number
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_read_unique_id(uint64_t *data);

/**
 ****************************************************************************************
 * @brief Get SPI Flash JEDEC ID
 * @param[out] data         Pointer to the JEDEC ID
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_read_jedec_id(uint32_t *data);

/**
 ****************************************************************************************
 * @brief Set the memory protection configuration
 * @param[in] data          Memory protection setting
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_configure_memory_protection(uint8_t data);

/**
 ****************************************************************************************
 * @brief Issue a Page Erase command
 * @param[in] address       Address of the page to erase
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_page_erase(uint32_t page_address);

/**
 ****************************************************************************************
 * @brief Issue a comamnd to erase a given sector or block
 * @param[in] address       Address that belongs to the block64/block32/sector range
 * @param[in] erase_op      Erase operation type (Sector, Block32 or Block64)
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_block_erase(uint32_t address, spi_flash_op_t erase_op);

/**
 ****************************************************************************************
 * @brief Issue a command to erase a given sector or block without waiting for the
 *        operation to complete
 * @param[in] address       Address that belongs to the block64/block32/sector range
 * @param[in] erase_op      Erase operation type (Sector, Block32 or Block64)
 * @return Error code
****************************************************************************************
*/
int8_t spi_flash_block_erase_no_wait(uint32_t address, spi_flash_op_t erase_op);

/**
 ****************************************************************************************
 * @brief Erase chip
 * @note In order for the erase to succeed, all locking options must be disabled.
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_chip_erase(void);

/**
 ****************************************************************************************
 * @brief Erase chip even if some blocks are protected
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_chip_erase_forced(void);

/**
 ****************************************************************************************
 * @brief Check E_fail bit
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_erase_fail(void);

/**
 ****************************************************************************************
 * @brief Program page (up to <SPI Flash page size> bytes) starting at given address
 * @details This operation performs SPI transfers on a byte level using spi_access().
 * It is recommended to use this function when code size is critical to the application.
 * @param[in] wr_data_ptr   Pointer to the data to be written
 * @param[in] address       Starting address of data to be written
 * @param[in] size          Size of the data to be written (should not be larger than
 *                          SPI Flash page size)
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_page_program(uint8_t *wr_data_ptr, uint32_t address, uint16_t size);

/**
 ****************************************************************************************
 * @brief Program page (up to <SPI Flash page size> bytes) starting at given address
 * @details This operation performs SPI transfers on a buffer level using spi_send().
 * It is recommended to use this function when performance is critical to the application.
 * @param[in] wr_data_ptr   Pointer to the data to be written
 * @param[in] address       Starting address of data to be written
 * @param[in] size          Size of the data to be written (should not be larger than
 *                          SPI Flash page size)
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_page_program_buffer(uint8_t *wr_data_ptr, uint32_t address, uint16_t size);

#if defined (CFG_SPI_DMA_SUPPORT)
/**
 ****************************************************************************************
 * @brief Program page (up to <SPI Flash page size> bytes) starting at given address
 * @details This operation performs SPI transfers using the DMA engine.
 * It is recommended to use this function when performance is critical to the application.
 * @param[in] wr_data_ptr   Pointer to the data to be written
 * @param[in] address       Starting address of data to be written
 * @param[in] size          Size of the data to be written (should not be larger than
 *                          SPI Flash page size)
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_page_program_dma(uint8_t *wr_data_ptr, uint32_t address, uint16_t size);
#endif

/**
 ****************************************************************************************
 * @brief Write data to SPI Flash across page boundaries and at any starting address
 * @details This operation performs SPI transfers on a byte level using spi_access().
 * It is recommended to use this function when code size is critical to the application.
 * @param[in] wr_data_ptr   Pointer to the data to be written
 * @param[in] address       Starting address of page to be written (must be a multiple
 *                          of SPI Flash page size)
 * @param[in] size          Size of the data to be written (can be larger than SPI Flash
 *                          page size)
 * @param[out] actual_size  Actual size of written data
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_write_data(uint8_t *wr_data_ptr, uint32_t address,
                            uint16_t size, uint16_t *actual_size);

/**
 ****************************************************************************************
 * @brief Write data to SPI Flash across page boundaries and at any starting address
 * @details This operation performs SPI transfers on a buffer level using spi_send().
 * It is recommended to use this function when performance is critical to the application.
 * @param[in] wr_data_ptr   Pointer to the data to be written
 * @param[in] address       Starting address of page to be written (must be a multiple
 *                          of SPI Flash page size)
 * @param[in] size          Size of the data to be written (can be larger than SPI Flash
 *                          page size)
 * @param[out] actual_size  Actual size of written data
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_write_data_buffer(uint8_t *wr_data_ptr, uint32_t address,
                                   uint16_t size, uint16_t *actual_size);

#if defined (CFG_SPI_DMA_SUPPORT)
/**
 ****************************************************************************************
 * @brief Write data to SPI Flash across page boundaries and at any starting address
 * @details This operation performs SPI transfers using the DMA engine.
 * It is recommended to use this function when performance is critical to the application.
 * @param[in] wr_data_ptr   Pointer to the data to be written
 * @param[in] address       Starting address of page to be written (must be a multiple
 *                          of SPI Flash page size)
 * @param[in] size          Size of the data to be written (can be larger than SPI Flash
 *                          page size)
 * @param[out] actual_size  Actual size of written data
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_write_data_dma(uint8_t *wr_data_ptr, uint32_t address,
                                uint16_t size, uint16_t *actual_size);
#endif

/**
 ****************************************************************************************
 * @brief Fill memory page (up to <SPI Flash page size> bytes) with a given 1-byte value
 *        starting at given address
 * @param[in] data          Value used to fill memory
 * @param[in] address       Starting address
 * @param[in] size          Size of the area to be filled (should not be larger than SPI
 *                          Flash page size)
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_page_fill(uint8_t data, uint32_t address, uint16_t size);

/**
 ****************************************************************************************
 * @brief Fill memory with a 1-byte value, across page boundaries and at any starting
 *        address
 * @param[in] data          The value with which memory will be filled
 * @param[in] address       Starting address of page to be written (must be a multiple of
 *                          SPI Flash page size)
 * @param[in] size          Size of the area to be filled (can be larger than SPI Flash
 *                          page size)
 * @param[out] actual_size  Actual size of written data
 * @return Error code
 ****************************************************************************************
*/
int8_t spi_flash_fill(uint8_t value, uint32_t address,
                      uint16_t size, uint16_t *actual_size);

/**
 ****************************************************************************************
 * @brief Check P_fail bit
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_program_fail(void);

/**
 ****************************************************************************************
 * @brief Read data from a given starting address (up to the end of the flash)
 * @details This operation performs SPI transfers on a byte level using spi_access().
 * It is recommended to use this function when code size is critical to the application.
 * @param[in] rd_data_ptr   Points to the position the read data will be stored
 * @param[in] address       Starting address of data to be read
 * @param[in] size          Size of the data to be read
 * @param[out] actual_size  Actual size of read data
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_read_data(uint8_t *rd_data_ptr, uint32_t address,
                           uint16_t size, uint16_t *actual_size);

/**
 ****************************************************************************************
 * @brief Read data from a given starting address (up to the end of the flash)
 * @details This operation performs SPI transfers on a buffer level using spi_receive().
 * It is recommended to use this function when performance is critical to the application.
 * @param[in] rd_data_ptr   Points to the position the read data will be stored
 * @param[in] address       Starting address of data to be read
 * @param[in] size          Size of the data to be read
 * @param[out] actual_size  Actual size of read data
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_read_data_buffer(uint8_t *rd_data_ptr, uint32_t address,
                                  uint16_t size, uint16_t *actual_size);

#if defined (CFG_SPI_DMA_SUPPORT)
/**
 ****************************************************************************************
 * @brief Read data from a given starting address (up to the end of the flash)
 * @details This operation performs SPI transfers using the DMA engine.
 * It is recommended to use this function when performance is critical to the application.
 * @param[in] rd_data_ptr   Points to the position the read data will be stored
 * @param[in] address       Starting address of data to be read
 * @param[in] size          Size of the data to be read
 * @param[out] actual_size  Actual size of read data
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_read_data_dma(uint8_t *rd_data_ptr, uint32_t address,
                               uint16_t size, uint16_t *actual_size);
#endif

/**
 ****************************************************************************************
 * @brief Check if a page is erased
 * @param[in] address       Starting address of the page
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_is_page_empty(uint32_t page_address);

/**
 ****************************************************************************************
 * @brief Check if a sector is erased
 * @param[in] address       Starting address of the sector
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_is_sector_empty(uint32_t sector_address);

/**
 ****************************************************************************************
 * @brief Check if the whole chip is erased
 * @return Error code
 ****************************************************************************************
 */
int8_t spi_flash_is_empty(void);

#if defined (__DA14586__)
/**
 ****************************************************************************************
 * @brief Power down on-chip Flash. Supported only in DA14586 devices.
 * @return void
 ****************************************************************************************
*/
void power_down_da14586_flash(void);
#endif

#endif //_SPI_FLASH_H_
