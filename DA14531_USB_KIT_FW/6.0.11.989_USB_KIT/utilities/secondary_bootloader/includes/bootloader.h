/**
 ****************************************************************************************
 *
 * @file downloader.h
 *
 * @brief firmware download protocol settings
 *
 * Copyright (c) 2012-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _DOWNLOADER_H
#define _DOWNLOADER_H

#include <stdint.h>

/************** Encrypted Image Support section**************/
#define AES_ENCRYPTED_IMAGE_SUPPORTED 1

/************** UART Booter section**************/
#undef UART_SUPPORTED

/************** Flash memory section  **************/
#define SPI_FLASH_SUPPORTED
#undef I2C_EEPROM_SUPPORTED

#if  defined (SPI_FLASH_SUPPORTED ) && defined (I2C_EEPROM_SUPPORTED)
    #error "Both SPI_FLASH_SUPPORTED and I2C_EEPROM_SUPPORTED configuration flags are defined."
#endif 

/************** AN-B-001 based image /fast bootloader section **************/
// Define it if
// if AN-B-001 based image is burned in SPI flash and the bootloader is running from the OTP
#undef SUPPORT_AN_B_001  //Enable it for the secondary bootloader

// Define ALLOW_NO_HEADER to enable loading 0x7f00 bytes from memory offset 0
// in case no valid AN-B-001 header is found.
#define ALLOW_NO_HEADER

#if  defined (SUPPORT_AN_B_001) && AES_ENCRYPTED_IMAGE_SUPPORTED == 1
     #warning "Encryption is not supported by the secondary bootloader"
#endif

// boot header flags
#define IMAGE_BOOT_SIGNATURE1               0x70
#define IMAGE_BOOT_SIGNATURE2               0x50

#define AN001_SPI_HEADER_SIZE               8
#define AN001_SPI_STARTCODE_POSITION        8

// 58x boot header: supports image sizes > 64kB
typedef struct __bootHeader58x_spi {
    uint8_t preamble[2]; // 0x70 0x50
    uint8_t empty[3];
    uint8_t len[3];
} s_bootHeader58x_spi;

#define AN001_EEPROM_HEADER_SIZE            8
#define AN001_EEPROM_STARTCODE_POSITION     32

// 58x: AN-B-001 header for I2C/EEPROM (sizes < 64kB)
typedef struct __bootHeader580_i2c {
    uint8_t preamble[2]; // 0x70 0x50
    uint8_t len[2];
    uint8_t crc;
    uint8_t dummy[27];
} s_bootHeader580_i2c;

// 585 boot header: supports image sizes > 64kB
typedef struct __bootHeader585_i2c{
    uint8_t preamble[2]; // 0x70 0x50
    uint8_t reserved[2]; // 0x0, 0x0
    uint8_t len[3];
    uint8_t crc;
    uint8_t dummy[24];
} s_bootHeader585_i2c;

/************** Dual Image bootloader section**************/
 
// product header flags 
#define PRODUCT_HEADER_POSITION     0x38000
#define PRODUCT_HEADER_SIGNATURE1   0x70
#define PRODUCT_HEADER_SIGNATURE2   0x52
// image header flags 
#define IMAGE_HEADER_SIGNATURE1     0x70
#define IMAGE_HEADER_SIGNATURE2     0x51
#define CODE_OFFSET                 64
#define STATUS_INVALID_IMAGE        0x0
#define STATUS_VALID_IMAGE          0xAA
//product header structure
typedef struct __productHeader {
    uint8_t signature[2];
    uint8_t version[2];
    uint32_t offset1;
    uint32_t offset2;
}s_productHeader;
//image header structure
typedef struct __imageHeader {
    uint8_t signature[2];
    uint8_t validflag;
    uint8_t imageid;
    uint32_t code_size;
    uint32_t CRC;
    uint8_t version[16];
    uint32_t timestamp;
    uint8_t encryption;
    uint8_t reserved[31];
}s_imageHeader;

// Functions Prototype
/**
****************************************************************************************
* @brief Load the active from a SPI flash
* @return Success (0) or Error Code.
* 
****************************************************************************************
**/
int spi_loadActiveImage(void);

/**
****************************************************************************************
* @brief Load the active from an EEPROM flash
* @return Success (0) or Error Code.
* 
****************************************************************************************
**/
int eeprom_loadActiveImage(void);

#endif
