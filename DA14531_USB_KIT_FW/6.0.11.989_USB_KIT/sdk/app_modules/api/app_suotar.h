/**
 ****************************************************************************************
 *
 * @file app_suotar.h
 *
 * @brief SUOTA Receiver Application header file
 *
 * Copyright (c) 2013-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef APP_SUOTAR_H_
#define APP_SUOTAR_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief SUOTA Receiver Application header file.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_profiles_config.h"

#if BLE_SUOTA_RECEIVER

// #include <stdint.h>          // standard integer definition
// #include <co_bt.h>

#include "gpio.h"
#include "spi.h"
#include "spi_flash.h"
#include "i2c.h"
#include "i2c_eeprom.h"
#include "suotar.h"
// #include "suotar_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

#if defined (CFG_SPI_FLASH_ENABLE)
    #define SUOTAR_SPI_DISABLE                  0
    #define SUOTAR_I2C_DISABLE                  1
    #define SUOTAR_SPI_DELATE_SECTOR_ERASE      1
#elif defined (CFG_I2C_EEPROM_ENABLE)
    #define SUOTAR_SPI_DISABLE                  1
    #define SUOTAR_I2C_DISABLE                  0
#else
    #define SUOTAR_SPI_DISABLE                  1
    #define SUOTAR_I2C_DISABLE                  1
#endif

#if defined (CFG_SUOTAR_UPDATE_DISABLE)
    #define SUOTAR_UPDATE_DISABLE               1
#else
    #define SUOTAR_UPDATE_DISABLE               0
#endif

#if ((SUOTAR_SPI_DISABLE && SUOTAR_I2C_DISABLE) && !SUOTAR_UPDATE_DISABLE)
    #error "SUOTA application is enabled but both I2C EEPROM and SPI Flash are disabled"
#endif

// SUOTAR status indications
#define SUOTAR_START    1
#define SUOTAR_END      0

 // SUOTAR application types
#define SUOTAR_PATCH            0   // Software patch over the air (SUOTA)
#define SUOTAR_IMAGE            1   // Software image update over the air (SUOTA)

// Checks the application type (SUOTA or SUOTA)
#define SUOTAR_IS_FOR_PATCH(mem_dev) ( (((mem_dev) & 0x10) >> 4 ) == SUOTAR_PATCH)
#define SUOTAR_IS_FOR_IMAGE(mem_dev) ( (((mem_dev) & 0x10) >> 4 ) == SUOTAR_IMAGE)

#define SUOTAR_IS_VALID_MEM_DEV(mem_dev) ( (mem_dev) < SUOTAR_MEM_INVAL_DEV )

// FLAGS
#define SUOTAR_READ_MEM_DEV_TYPE    0xFF000000
#define SUOTAR_READ_MEM_BASE_ADD    0x0000FFFF
#define SUOTAR_READ_NUM_OF_PATCHES  0xFFFF0000
#define SUOTAR_READ_MEM_PATCH_SIZE  0x0000FFFF

// Example FLASH/EEPROM base address and GPIO mapping
#define  SUOTAR_FLASH_PATCH_BASE_ADDR   0x03002800
#define  SUOTAR_FLASH_GPIO_MAP          0x05060300
#define  SUOTAR_EEPROM_PATCH_BASE_ADDR  0x02000800
#define  SUOTAR_EEPROM_GPIO_MAP         0x00500203

// SUOTAR defines for image manipulation
#define PRODUCT_HEADER_POSITION     0x38000
#define PRODUCT_HEADER_SIGNATURE1   0x70
#define PRODUCT_HEADER_SIGNATURE2   0x52
#define IMAGE_HEADER_SIGNATURE1     0x70
#define IMAGE_HEADER_SIGNATURE2     0x51
#define CODE_OFFSET                 64
#define ADDITINAL_CRC_SIZE          1
#define IMAGE_HEADER_VERSION_SIZE   16
#define STATUS_VALID_IMAGE          0xAA
#define IMAGE_HEADER_OK             0
#define IMAGE_HEADER_INVALID        1
#define IMAGE_HEADER_SAME_VERSION   2
#define ANY_IMAGE_BANK              0
#define FISRT_IMAGE_BANK            1
#define SECOND_IMAGE_BANK           2
#define IMAGE_ID_0                  0
#define IMAGE_ID_1                  1
#define IMAGE_ID_2                  2

// Holds the retainable variables of SUOTAR app
typedef struct
{
    uint8_t     mem_dev;
    uint32_t    mem_base_add;
    uint32_t    gpio_map;
    uint32_t    new_patch_len;
    uint8_t     suota_pd_idx;
    uint8_t     suota_image_bank;
    uint32_t    suota_block_idx;
    uint32_t    suota_img_idx;
    uint8_t     crc_calc;
    uint32_t    suota_image_len;
    void (*status_ind_func) (const uint8_t);
    uint8_t     reboot_requested;
}app_suota_state;


#if (!SUOTAR_SPI_DISABLE)
// Defines the SPI GPIO type
typedef struct
{
    SPI_Pad_t cs;
    SPI_Pad_t mosi;
    SPI_Pad_t miso;
    SPI_Pad_t clk;
}spi_gpio_config_t;
#endif


typedef struct
{
	GPIO_PORT port;
	GPIO_PIN pin;
}I2C_Pad_t;

// Defines the i2c GPIO type
typedef struct
{
    I2C_Pad_t scl;
    I2C_Pad_t sda;
    uint32_t  slave_addr;
}i2c_gpio_config_t;

// Product header for SUOTA
typedef struct
{
    uint8_t signature[2];
    uint8_t version[2];
    uint32_t offset1; // Start address of first image header
    uint32_t offset2; // Start address of second image header
}product_header_t;

// Image header for SUOTA
typedef struct
{
    uint8_t signature[2];
    uint8_t validflag;      // Set to STATUS_VALID_IMAGE at the end of the image update
    uint8_t imageid;        // it is used to determine which image is the newest
    uint32_t code_size;     // Image size
    uint32_t CRC ;          // Image CRC
    uint8_t version[16];    // Vertion of the image
    uint32_t timestamp;
    uint8_t encryption;
    uint8_t reserved[31];
}image_header_t;

// SUOTAR/SUOTAR Physical memory device selection and commands
enum
{
    // SUOTA is used to send patches
    SUOTAR_MEM_INT_SYSRAM = 0x00,
    SUOTAR_MEM_INT_RETRAM = 0x01,
    SUOTAR_MEM_I2C_EEPROM = 0x02,
    SUOTAR_MEM_SPI_FLASH  = 0x03,

    // SUOTA is used to send entire image
    SUOTAR_IMG_INT_SYSRAM = 0x10,
    SUOTAR_IMG_INT_RETRAM = 0x11,
    SUOTAR_IMG_I2C_EEPROM = 0x12,
    SUOTAR_IMG_SPI_FLASH  = 0x13,

    SUOTAR_MEM_INVAL_DEV  = 0x14, // DO NOT move. Must be before commands

    /* SUOTAR/SUOTAR commands
    */
    SUOTAR_REBOOT         = 0xFD,
    SUOTAR_IMG_END        = 0xFE,
    // When initiator selects 0xff, it wants to exit SUOTAR service.
    // This is used in case of unexplained failures. If SUOTAR process
    // finishes correctly it will exit automatically.
    SUOTAR_MEM_SERVICE_EXIT   = 0xFF,
};

struct app_suotar_cb
{
    void (*on_suotar_status_change)(const uint8_t suotar_event);
};

/*
 * EXTERNAL DEFINITIONS
 ****************************************************************************************
 */

extern app_suota_state suota_state;
extern uint8_t suota_all_pd[SUOTA_OVERALL_PD_SIZE];


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes SUOTAR Apllication.
 *
 * @return      void
 ****************************************************************************************
 */
void app_suotar_init( void );

/**
 ****************************************************************************************
 * @brief Resets SUOTAR Apllication.
 *
 * @param[in]   void
 *
 * @return      void
 ****************************************************************************************
 */
void app_suotar_reset(void);

/**
 ****************************************************************************************
 * @brief Creates SUOTAR service database.
 *
 * @param[in]   void
 *
 * @return      void
 ****************************************************************************************
 */
void app_suotar_create_db(void);

/**
 ****************************************************************************************
 * @brief Reads memory device and writes memory info.
 *
 * @param[in]   MSbyte holds the Memory device type, rest is the base address.
 * @param[in]   16MSbits hold number of patches, 16LSbits hold overall mem len.
 *
 * @return      void
 ****************************************************************************************
 */
void app_suotar_read_mem(uint32_t mem_dev_type, uint32_t* mem_info);

/**
 ****************************************************************************************
 * @brief Starts SUOTAR serivce and disables sleep.
 *
 * @param[in]   void
 *
 * @return      void
 ****************************************************************************************
 */
void app_suotar_start(void);

/**
 ****************************************************************************************
 * @brief Stops SUOTAR service and resets application
 *
 * @param[in]   void
 *
 * @return      void
 ****************************************************************************************
 */
void app_suotar_stop(void);

/**
 ****************************************************************************************
 * @brief Handles patch execution. Should be called at system start up and after deep sleep.
 *
 * @param[in]   void
 *
 * @return      void
 *
 ****************************************************************************************
 */
void app_suotar_exec_patch(void);

/**
 ****************************************************************************************
 * @brief Updates SUOTAR status characteristic.
 *
 * @param[in]   SUOTAR application status.
 *
 * @return      void
 ****************************************************************************************
 */
void suotar_send_status_update_req( uint8_t status);

/**
 ****************************************************************************************
 * @brief Updates SUOTAR memory info characteristic.
 *
 * @param[in]   Patch memory info. Number of patches and overall patch length.
 *
 * @return      void.
 ****************************************************************************************
 */
void suotar_send_mem_info_update_req( uint32_t mem_info);

/**
 ****************************************************************************************
 * @brief SUOTA image block handler. Validates image block and stores it to
 *        external memory device.
 *
 * @param[in]   void
 *
 * @return      void
 *
 ****************************************************************************************
 */
void app_suotar_img_hdlr(void);

#endif //BLE_SUOTA_RECEIVER

/// @} APP

#endif // APP_H_
