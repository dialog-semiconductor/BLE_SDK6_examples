/**
 ****************************************************************************************
 *
 * @file image.h
 *
 * @brief Definition of image header structure.
 *
 * Copyright (c) 2014-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef __IMAGE_H
#define __IMAGE_H

#include <stdint.h>

/* header for single image */
struct image_header {
	uint8_t signature[2];
	uint8_t valid_flag;
	uint8_t image_id;
	uint8_t code_size[4];
	uint8_t CRC[4] ;
	uint8_t version[16];
	uint8_t timestamp[4];
	uint8_t flags;
	uint8_t reserved[31];
};

/* single image header flags */
#define IMG_ENCRYPTED		0x01

/* AN-B-001 header for SPI */
struct an_b_001_spi_header {
	uint8_t preamble[2];
	uint8_t empty[4];
	uint8_t len[2];
};

/* AN-B-001 header for I2C/EEPROM */
struct an_b_001_i2c_header {
	uint8_t preamble[2];
	uint8_t len[2];
	uint8_t crc;
	uint8_t dummy[27];
};

/* product header */
struct product_header {
	uint8_t signature[2];
	uint8_t version[2];
	uint8_t offset1[4];
	uint8_t offset2[4];
	uint8_t bd_address[6];
	uint8_t pad[2];
	uint8_t cfg_offset[4];
};

enum multi_options {
		SPI = 1,
		EEPROM = 2,
		BOOTLOADER = 4,
		CONFIG = 8
};


#endif  /* __IMAGE_H */
