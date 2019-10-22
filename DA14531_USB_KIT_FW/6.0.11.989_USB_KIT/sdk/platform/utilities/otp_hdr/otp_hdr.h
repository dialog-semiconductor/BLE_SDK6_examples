/**
 ****************************************************************************************
 *
 * @file otp_hdr.h
 *
 * @brief Definition of OTP header field offsets and addresses.
 *
 * Copyright (c) 2016-2019 Dialog Semiconductor. All rights reserved.
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

#ifndef _OTP_HDR_H_
#define _OTP_HDR_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "datasheet.h"
#include "arch.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#if !defined (__DA14531__)

/*
 * DA14585/DA14586: OTP header field offsets from OTP memory starting address
 */
#define OTP_HDR_BASE_OFFSET                 (0xFE00)

#define OTP_HDR_SWD_ENABLE_OFFSET           (0xFFF8) // SWD enable flag
#define OTP_HDR_OTP_DMA_LENGTH_OFFSET       (0xFFF0) // OTP DMA length
#define OTP_HDR_RESERVED1_OFFSET            (0xFFE8) // Reserved
#define OTP_HDR_RESERVED3_OFFSET            (0xFFB8) // Reserved
#define OTP_HDR_COMPANY_NUMBER_OFFSET       (0xFFB0) // Company number
#define OTP_HDR_BDADDR_OFFSET               (0xFFA8) // Bluetooth Device Address
#define OTP_HDR_SIGN_ALG_SEL_OFFSET         (0xFFA0) // Signature Algorithm Selector
#define OTP_HDR_SIGN_CUST_CODE_OFFSET       (0xFF28) // Signature of Customer Code
#define OTP_HDR_TRIM_VCO_OFFSET             (0xFF20) // Trim value for the VCO
#define OTP_HDR_TRIM_XTAL16M_OFFSET         (0xFF18) // Trim value for the XTAL16M oscillator
#define OTP_HDR_TRIM_RC16M_OFFSET           (0xFF10) // Trim value for the RC16M oscillator
#define OTP_HDR_TRIM_BANDGAP_OFFSET         (0xFF08) // Trim value for the BandGap
#define OTP_HDR_TRIM_RFIO_CAP_OFFSET        (0xFF00) // Trim value for the RFIO capacitance
#define OTP_HDR_TRIM_LNA_OFFSET             (0xFEF8) // Trim value for the LNA
#define OTP_HDR_CALIBRATION_FLAGS_OFFSET    (0xFEF0) // Calibration Flags
#define OTP_HDR_SLEEP_CLK_SOURCE_OFFSET     (0xFEE8) // Sleep Clock Source Flag
#define OTP_HDR_PACKAGE_OFFSET              (0xFEE0) // Package Flag
#define OTP_HDR_RESERVED2_OFFSET            (0xFEC8) // Reserved
#define OTP_HDR_BOOT_MAPPING_OFFSET         (0xFEC0) // Boot specific mapping
#define OTP_HDR_UART_STX_TO_OFFSET          (0xFEB8) // UART STX Timeout
#define OTP_HDR_OTP_CONTROL_OFFSET          (0xFEB0) // OTP control value
#define OTP_HDR_CUSTOMER_FIELD_OFFSET       (0xFE20) // Customer Specific Field (21 64-bit words)
#define OTP_HDR_CRC_TRIM_CAL_OFFSET         (0xFE18) // CRC for Trim and Calibration values
#define OTP_HDR_IQ_TRIM_VALUE_OFFSET        (0xFE10) // IQ Trim value
#define OTP_HDR_APP_PROG_FLAG2_OFFSET       (0xFE08) // Application Programmed Flag #2
#define OTP_HDR_APP_PROG_FLAG1_OFFSET       (0xFE00) // Application Programmed Flag #1

/*
 * DA14585/DA14586: OTP header field memory addresses
 */
#define OTP_HDR_BASE_ADDR               (MEMORY_OTP_BASE + OTP_HDR_BASE_OFFSET)

#define OTP_HDR_SWD_ENABLE_ADDR         (MEMORY_OTP_BASE + OTP_HDR_SWD_ENABLE_OFFSET)
#define OTP_HDR_OTP_DMA_LENGTH_ADDR     (MEMORY_OTP_BASE + OTP_HDR_OTP_DMA_LENGTH_OFFSET)
#define OTP_HDR_RESERVED1_ADDR          (MEMORY_OTP_BASE + OTP_HDR_RESERVED1_OFFSET)
#define OTP_HDR_RESERVED3_ADDR          (MEMORY_OTP_BASE + OTP_HDR_RESERVED3_OFFSET)
#define OTP_HDR_COMPANY_NUMBER_ADDR     (MEMORY_OTP_BASE + OTP_HDR_COMPANY_NUMBER_OFFSET)
#define OTP_HDR_BDADDR_ADDR             (MEMORY_OTP_BASE + OTP_HDR_BDADDR_OFFSET)
#define OTP_HDR_SIGN_ALG_SEL_ADDR       (MEMORY_OTP_BASE + OTP_HDR_SIGN_ALG_SEL_OFFSET)
#define OTP_HDR_SIGN_CUST_CODE_ADDR     (MEMORY_OTP_BASE + OTP_HDR_SIGN_CUST_CODE_OFFSET)
#define OTP_HDR_TRIM_VCO_ADDR           (MEMORY_OTP_BASE + OTP_HDR_TRIM_VCO_OFFSET)
#define OTP_HDR_TRIM_XTAL16M_ADDR       (MEMORY_OTP_BASE + OTP_HDR_TRIM_XTAL16M_OFFSET)
#define OTP_HDR_TRIM_RC16M_ADDR         (MEMORY_OTP_BASE + OTP_HDR_TRIM_RC16M_OFFSET)
#define OTP_HDR_TRIM_BANDGAP_ADDR       (MEMORY_OTP_BASE + OTP_HDR_TRIM_BANDGAP_OFFSET)
#define OTP_HDR_TRIM_RFIO_CAP_ADDR      (MEMORY_OTP_BASE + OTP_HDR_TRIM_RFIO_CAP_OFFSET)
#define OTP_HDR_TRIM_LNA_ADDR           (MEMORY_OTP_BASE + OTP_HDR_TRIM_LNA_OFFSET)
#define OTP_HDR_CALIBRATION_FLAGS_ADDR  (MEMORY_OTP_BASE + OTP_HDR_CALIBRATION_FLAGS_OFFSET)
#define OTP_HDR_SLEEP_CLK_SOURCE_ADDR   (MEMORY_OTP_BASE + OTP_HDR_SLEEP_CLK_SOURCE_OFFSET)
#define OTP_HDR_PACKAGE_ADDR            (MEMORY_OTP_BASE + OTP_HDR_PACKAGE_OFFSET)
#define OTP_HDR_RESERVED2_ADDR          (MEMORY_OTP_BASE + OTP_HDR_RESERVED2_OFFSET)
#define OTP_HDR_BOOT_MAPPING_ADDR       (MEMORY_OTP_BASE + OTP_HDR_BOOT_MAPPING_OFFSET)
#define OTP_HDR_UART_STX_TO_ADDR        (MEMORY_OTP_BASE + OTP_HDR_UART_STX_TO_OFFSET)
#define OTP_HDR_OTP_CONTROL_ADDR        (MEMORY_OTP_BASE + OTP_HDR_OTP_CONTROL_OFFSET)
#define OTP_HDR_CUSTOMER_FIELD_ADDR     (MEMORY_OTP_BASE + OTP_HDR_CUSTOMER_FIELD_OFFSET)
#define OTP_HDR_CRC_TRIM_CAL_ADDR       (MEMORY_OTP_BASE + OTP_HDR_CRC_TRIM_CAL_OFFSET)
#define OTP_HDR_IQ_TRIM_VALUE_ADDR      (MEMORY_OTP_BASE + OTP_HDR_IQ_TRIM_VALUE_OFFSET)
#define OTP_HDR_APP_PROG_FLAG2_ADDR     (MEMORY_OTP_BASE + OTP_HDR_APP_PROG_FLAG2_OFFSET)
#define OTP_HDR_APP_PROG_FLAG1_ADDR     (MEMORY_OTP_BASE + OTP_HDR_APP_PROG_FLAG1_OFFSET)

#else

/*
 * DA14531: OTP header field offsets from OTP memory starting address
 */
#define OTP_HDR_BASE_OFFSET                 (0x7FC0)

#define OTP_HDR_RESERVED_OFFSET             (0x7FEC) // Reserved
#define OTP_HDR_TIMESTAMP_OFFSET            (0x7FE8) // Timestamp
#define OTP_HDR_TESTER_OFFSET               (0x7FE4) // Tester
#define OTP_HDR_POSITION_OFFSET             (0x7FE0) // Position
#define OTP_HDR_OTP_DMA_LENGTH_OFFSET       (0x7FDC) // OTP DMA length
#define OTP_HDR_BDADDR_OFFSET               (0x7FD4) // Bluetooth Device Address
#define OTP_HDR_PACKAGE_OFFSET              (0x7FD0) // Device and Package Flag
#define OTP_HDR_BOOT_MAPPING_OFFSET         (0x7FCC) // Boot specific mapping
#define OTP_HDR_BOOT_CONFIG_OFFSET          (0x7FC8) // Boot specific configuration
#define OTP_HDR_APP_PROG_FLAG2_OFFSET       (0x7FC4) // Application Programmed Flag #2
#define OTP_HDR_APP_PROG_FLAG1_OFFSET       (0x7FC0) // Application Programmed Flag #1

/*
 * DA14531: OTP header field memory addresses
 */
#define OTP_HDR_BASE_ADDR               (MEMORY_OTP_BASE + OTP_HDR_BASE_OFFSET)

#define OTP_HDR_RESERVED_ADDR           (MEMORY_OTP_BASE + OTP_HDR_RESERVED_OFFSET)
#define OTP_HDR_TIMESTAMP_ADDR          (MEMORY_OTP_BASE + OTP_HDR_TIMESTAMP_OFFSET)
#define OTP_HDR_TESTER_ADDR             (MEMORY_OTP_BASE + OTP_HDR_TESTER_OFFSET)
#define OTP_HDR_POSITION_ADDR           (MEMORY_OTP_BASE + OTP_HDR_POSITION_OFFSET)
#define OTP_HDR_OTP_DMA_LENGTH_ADDR     (MEMORY_OTP_BASE + OTP_HDR_OTP_DMA_LENGTH_OFFSET)
#define OTP_HDR_BDADDR_ADDR             (MEMORY_OTP_BASE + OTP_HDR_BDADDR_OFFSET)
#define OTP_HDR_PACKAGE_ADDR            (MEMORY_OTP_BASE + OTP_HDR_PACKAGE_OFFSET)
#define OTP_HDR_BOOT_MAPPING_ADDR       (MEMORY_OTP_BASE + OTP_HDR_BOOT_MAPPING_OFFSET)
#define OTP_HDR_BOOT_CONFIG_ADDR        (MEMORY_OTP_BASE + OTP_HDR_BOOT_CONFIG_OFFSET)
#define OTP_HDR_APP_PROG_FLAG2_ADDR     (MEMORY_OTP_BASE + OTP_HDR_APP_PROG_FLAG2_OFFSET)
#define OTP_HDR_APP_PROG_FLAG1_ADDR     (MEMORY_OTP_BASE + OTP_HDR_APP_PROG_FLAG1_OFFSET)

#endif

/**
 ****************************************************************************************
 * @brief Get the BD address from OTP Header.
 * @param[out] bd_addr       The BD address value returned.
 * @return void
 ****************************************************************************************
 */
void otp_hdr_get_bd_address(uint8_t *bd_addr);

#endif //OTP_HDR_H
