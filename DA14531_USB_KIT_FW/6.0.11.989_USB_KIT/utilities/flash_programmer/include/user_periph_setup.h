/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
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

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "gpio.h"
#include "i2c_eeprom.h"
#include "spi.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#undef DEVELOPMENT_MODE

// EEPROM Configuration Wizard
//*** <<< Use Configuration Wizard in Context Menu >>> ***

//  <h> EEPROM Configuration Wizard
//          <o0> Memory Selection   <0=> Custom Memory
//                                  <1=> Microchip 24LC02
//                                  <2=> Microchip 24LC64
//                                  <3=> ST M24M01
#define EEPROM_CONFIG 0
//      <h> Custom Memory Settings (Used only if Custom Memory is selected)
//          <o1.0..18> I2C_EEPROM_DEV_SIZE (B) <0x0-0x40000>
//          <o2> I2C_EEPROM_PAGE_SIZE (B) <0-256>
//          <o3.0..15> I2C_SLAVE_ADDRESS <0x0-0xFFFF>
//          <o4>  I2C_SPEED_MODE        <1=> I2C_STANDARD <2=> I2C_FAST
//          <o5>  I2C_ADDRESS_MODE     <0=> 7-bit addressing <2=> 10-bit addressing
//          <o6>  I2C_ADDRESS_SIZE     <0=> 8-bit memory address <1=> 16-bit memory address <2=> 24-bit memory address
//      </h>
//  </h>

#if EEPROM_CONFIG == 0                          // Custom Memory
#define I2C_EEPROM_DEV_SIZE   0x40000               // EEPROM size in bytes
#define I2C_EEPROM_PAGE_SIZE   256                   // EEPROM page size in bytes
#define I2C_SLAVE_ADDRESS 0x50                  // Set slave device address
#define I2C_SPEED_MODE    I2C_SPEED_FAST        // Speed mode: I2C_SPEED_STANDARD (100 kbits/s), I2C_SPEED_FAST (400 kbits/s)
#define I2C_ADDRESS_MODE  I2C_ADDRESSING_7B     // Addressing mode: {I2C_ADDRESSING_7B, I2C_ADDRESSING_10B}
#define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR       // Address width: {I2C_1BYTE_ADDR, I2C_2BYTES_ADDR, I2C_3BYTES_ADDR}


#elif EEPROM_CONFIG == 1                        // Microchip 24LC02 (2 kBit - 8 byte page)
#define I2C_EEPROM_DEV_SIZE   0x100                 // EEPROM size in bytes
#define I2C_EEPROM_PAGE_SIZE   8                     // EEPROM page size in bytes
#define I2C_SLAVE_ADDRESS 0x50                  // Set slave device address
#define I2C_SPEED_MODE    I2C_SPEED_FAST        // Speed mode: I2C_SPEED_STANDARD (100 kbits/s), I2C_SPEED_FAST (400 kbits/s)
#define I2C_ADDRESS_MODE  I2C_ADDRESSING_7B     // Addressing mode: {I2C_ADDRESSING_7B, I2C_ADDRESSING_10B}
#define I2C_ADDRESS_SIZE  I2C_1BYTE_ADDR        // Address width: {I2C_1BYTE_ADDR, I2C_2BYTES_ADDR, I2C_3BYTES_ADDR}

#elif EEPROM_CONFIG == 2                        // Microchip 24LC64 (64 kBit - 32 byte page)
#define I2C_EEPROM_DEV_SIZE   0x2000                // EEPROM size in bytes
#define I2C_EEPROM_PAGE_SIZE   32                    // EEPROM page size in bytes
#define I2C_SLAVE_ADDRESS 0x50                  // Set slave device address
#define I2C_SPEED_MODE    I2C_SPEED_FAST        // Speed mode: I2C_SPEED_STANDARD (100 kbits/s), I2C_SPEED_FAST (400 kbits/s)
#define I2C_ADDRESS_MODE  I2C_ADDRESSING_7B     // Addressing mode: {I2C_ADDRESSING_7B, I2C_ADDRESSING_10B}
#define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR       // Address width: {I2C_1BYTE_ADDR, I2C_2BYTES_ADDR, I2C_3BYTES_ADDR}

#elif EEPROM_CONFIG == 3                        // ST M24M01 (1 MBit - 256 byte page)
#define I2C_EEPROM_DEV_SIZE   0x20000               // EEPROM size in bytes
#define I2C_EEPROM_PAGE_SIZE   256                   // EEPROM page size in bytes
#define I2C_SLAVE_ADDRESS 0x50                  // Set slave device address
#define I2C_SPEED_MODE    I2C_SPEED_FAST        // Speed mode: I2C_SPEED_STANDARD (100 kbits/s), I2C_SPEED_FAST (400 kbits/s)
#define I2C_ADDRESS_MODE  I2C_ADDRESSING_7B     // Addressing mode: {I2C_ADDRESSING_7B, I2C_ADDRESSING_10B}
#define I2C_ADDRESS_SIZE  I2C_2BYTES_ADDR       // Address width: {I2C_1BYTE_ADDR, I2C_2BYTES_ADDR, I2C_3BYTES_ADDR}

#endif
//*** <<< end of configuration section >>>    ***

// SPI Flash memory and page sizes
#define SPI_FLASH_DEV_SIZE 262144     // SPI Flash memory size in bytes
#define SPI_FLASH_PAGE_SIZE 256        // SPI Flash memory page size in bytes

// UART
// init values
#if defined (__DA14531__)
    #define UART_PIN_PAIR   0 // (0:(P0_0 , P0_1)
    #define UART_GPIO_PORT  GPIO_PORT_0
    #define UART_TX_PIN     GPIO_PIN_0
    #define UART_RX_PIN     GPIO_PIN_1
#else
    #define UART_PIN_PAIR   4 // (0:(P0_0 , P0_1),2:(P0_2 , P0_3),4:(P0_4 , P0_5),6:(P0_6 , P0_7))
    #define UART_GPIO_PORT  GPIO_PORT_0
    #define UART_TX_PIN     GPIO_PIN_4
    #define UART_RX_PIN     GPIO_PIN_5
#endif

typedef struct __uart_sel_pins
{
    uint8_t uart_tx_port;
    uint8_t uart_tx_pin;
    uint8_t uart_rx_port;
    uint8_t uart_rx_pin;
}_uart_sel_pins;

// SPI
// init values
#if defined (__DA14531__)
    #define SPI_CS_PORT     GPIO_PORT_0
    #define SPI_CS_PIN      GPIO_PIN_1
    #define SPI_CLK_PORT    GPIO_PORT_0
    #define SPI_CLK_PIN     GPIO_PIN_4
    #define SPI_DO_PORT     GPIO_PORT_0
    #define SPI_DO_PIN      GPIO_PIN_0
    #define SPI_DI_PORT     GPIO_PORT_0
    #define SPI_DI_PIN      GPIO_PIN_3
#else
    #define SPI_CS_PORT     GPIO_PORT_0
    #define SPI_CS_PIN      GPIO_PIN_3
    #define SPI_CLK_PORT    GPIO_PORT_0
    #define SPI_CLK_PIN     GPIO_PIN_0
    #define SPI_DO_PORT     GPIO_PORT_0
    #define SPI_DO_PIN      GPIO_PIN_6
    #define SPI_DI_PORT     GPIO_PORT_0
    #define SPI_DI_PIN      GPIO_PIN_5
#endif

typedef struct __spi_sel_pins
{
    uint8_t spi_cs_port;
    uint8_t spi_cs_pin;
    uint8_t spi_clk_port;
    uint8_t spi_clk_pin;
    uint8_t spi_do_port;
    uint8_t spi_do_pin;
    uint8_t spi_di_port;
    uint8_t spi_di_pin;
}_spi_sel_pins;

typedef enum SPI_POL_MODES {
    SPI_CLK_IDLE_POL_LOW,
    SPI_CLK_IDLE_POL_HIGH,
 }SPI_Polarity_Mode_t;

typedef enum SPI_PHA_MODES {
    SPI_PHA_MODE_0,
    SPI_PHA_MODE_1,
} SPI_PHA_Mode_t;

typedef enum SPI_FREQ_MODES {
    SPI_XTAL_DIV_8,
    SPI_XTAL_DIV_4,
    SPI_XTAL_DIV_2,
#ifdef __DA14531__
    SPI_XTAL_DIV_14,
#endif
} SPI_XTAL_Freq_t;

typedef struct __attribute__((packed)) __spi_cfg_opts {
    SPI_WSZ_MODE_CFG        word_len:8;
    SPI_MS_MODE_CFG         mode:8;
    SPI_Polarity_Mode_t     idle_pol:8;
    SPI_PHA_Mode_t          sampling_edge:8;
    SPI_IRQ_CFG             mint:8;
    SPI_XTAL_Freq_t         clk_div:8;

    uint32_t                total_size;
    uint32_t                page_size;
    uint32_t                jedec_id;
    uint32_t                jedec_id_mask;
    uint8_t                 mem_prot;
} _spi_cfg_opts;

// I2C/EEPROM
// init values
#if defined (__DA14531__)
    #define I2C_SCL_PORT    GPIO_PORT_0
    #define I2C_SCL_PIN     GPIO_PIN_4
    #define I2C_SDA_PORT    GPIO_PORT_0
    #define I2C_SDA_PIN     GPIO_PIN_3
#else
#define I2C_SCL_PORT    GPIO_PORT_0
#define I2C_SCL_PIN     GPIO_PIN_2
#define I2C_SDA_PORT    GPIO_PORT_0
#define I2C_SDA_PIN     GPIO_PIN_3
#endif

typedef struct __i2c_sel_pins
{
    uint8_t i2c_scl_port;
    uint8_t i2c_scl_pin;
    uint8_t i2c_sda_port;
    uint8_t i2c_sda_pin;
}_i2c_sel_pins;

typedef struct __attribute__((packed)) __eeprom_cfg_opts {
    uint16_t                        slave_address;
    i2c_speed_t                     speed_mode:8;
    i2c_addressing_t                address_mode:8;
    enum I2C_ADDRESS_BYTES_COUNT    address_byte_count:8;
    uint32_t                        total_size;
    uint32_t                        page_size;
} _eeprom_cfg_opts;


#ifdef USE_UART
void update_uart_pads(GPIO_PORT tx_port, GPIO_PIN tx_pin, GPIO_PORT rx_port, GPIO_PIN rx_pin);
void set_pad_uart(void);
#endif

#if defined (__DA14531__)
// Define CFG_HW_RESET_P00 to enable hw_reset operation in P00
#define CFG_HW_RESET_P00

void enable_hw_reset(void);
void check_gpio_hw_reset(GPIO_PORT port, GPIO_PIN pin);
#endif

void set_pad_spi(void);
void update_spi_pads(uint8_t *pin_buffer);
void set_pad_eeprom(void);
void update_eeprom_pads(uint8_t *pin_buffer);

void periph_init(void);

#endif // _USER_PERIPH_SETUP_H_


