 /*
 ****************************************************************************************
 *
 * @file programmer.c
 *
 * @brief Flash programmer code.
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "arch_system.h"
#include "programmer.h"
#include "spi.h"
#include "spi_flash.h"
#include "i2c_eeprom.h"
#include "user_periph_setup.h"
#include "uart.h"
#include "hw_otpc.h"
#include "version.h"
#include "syscntl.h"
#include "timer0.h"
#include "timer0_2.h"
#if defined (__DA14531__)
#include "adc.h"
#endif

#define GPIO_REG                0x50003000
#define MAX_PACKET_SIZE         8192
#define ACTION_READY            0x5A
#define SPI_IS_EMPTY_ALL        0x01

#define ERR_OK                  0
#define ERR_INVAL               -4

#define NO_ACTION               0
#define ACTION_READ_VERSION     0x10
#define ACTION_RETRIEVE_STATUS  0x20

#define ACTION_UART_BAUD        0x30
#define ACTION_UART_GPIOS       0x31
#define ACTION_GPIOS            0x32

#define ACTION_READ             0x80
#define ACTION_WRITE            0x81

#define ACTION_OTP_READ         0x80
#define ACTION_OTP_WRITE        0x81

#define ACTION_CONTENTS         0x82
#define ACTION_OK               0x83
#define ACTION_ERROR            0x84
#define ACTION_DATA             0x85
#define ACTION_INVALID_COMMAND  0x86
#define ACTION_INVALID_CRC      0x87

#define ACTION_SPI_READ         0x90
#define ACTION_SPI_WRITE        0x91
#define ACTION_SPI_ERASE        0x92
#define ACTION_SPI_ID           0x93
#define ACTION_SPI_ERASE_BLOCK  0x94
#define ACTION_SPI_GPIOS        0x95
#define ACTION_SPI_IS_EMPTY     0x96
#define ACTION_SPI_INIT         0x97

#define ACTION_EEPROM_READ      0xA0
#define ACTION_EEPROM_WRITE     0xA1
#define ACTION_EEPROM_ERASE     0xA2
#define ACTION_I2C_GPIOS        0xA3
#define ACTION_EEPROM_INIT      0xA4

#define ACTION_GPIO_WD          0xB0

#define ACTION_PLATFORM_RESET   0xC0

// Address definitions needed for:
// - data manipulation,
// - storing programmer actions and subsequent responses
#if defined (__DA14531__)
#define TARGET_MEMORY   (void *)0x07FC4000
#define BASE_MEMORY     0x07FC3C00
#define ACTION_OFFSET   0
#define SIZE_OFFSET     2
#define ADDRESS_OFFSET  4
#define RESULT_OFFSET   8
#else
#define TARGET_MEMORY   (void *)0x7FC8000
#define BASE_MEMORY     0x7FC7C00
#define ACTION_OFFSET   0
#define SIZE_OFFSET     2
#define ADDRESS_OFFSET  4
#define RESULT_OFFSET   8
#endif

#define UART_BAUD_9600   0
#define UART_BAUD_19200  1
#define UART_BAUD_57600  2
#define UART_BAUD_115200 3
#define UART_BAUD_1M     4

extern _uart_sel_pins uart_sel_pins;
extern _spi_sel_pins spi_sel_pins;

extern uint32_t crc32(uint32_t crc, const void *buf, size_t size);

#if defined (USE_UART)
    /*
        Data size is limited by the uint16_t length sent via UART
        before the actual data packet for 14585,
        for DA14531 data size is limited to 32k.
    */
    #if defined (__DA14531__)
        #define ALLOWED_DATA 0x08000
    #else
        #define ALLOWED_DATA 0x0FFFF
    #endif
    uint8_t buffer[ALLOWED_DATA];

#else

    #if defined (__DA14531__)
        // Data size 32KB
        #define ALLOWED_DATA 0x08000
    #else
        // Data size 64KB
        #define ALLOWED_DATA 0x10000
    #endif
    uint8_t *buffer = NULL;
#endif

GPIO_PORT WD_port;
GPIO_PIN WD_pin;
uint8_t WD_volt_rail;
static volatile bool wdog_enabled = false;

static int last_status = ERR_OK;

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Configuration struct for I2C
static i2c_cfg_t i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = I2C_SPEED_MODE,
    .mode = I2C_MODE_MASTER,
    .addr_mode = I2C_ADDRESS_MODE,
    .address = I2C_SLAVE_ADDRESS,
    .tx_fifo_level = 1,
    .rx_fifo_level = 1,
};

/// Configuration struct for I2C EEPROM
static i2c_eeprom_cfg_t i2c_eeprom_cfg = {
    .size = I2C_EEPROM_DEV_SIZE,
    .page_size = I2C_EEPROM_PAGE_SIZE,
    .address_size = I2C_ADDRESS_SIZE,
};

static spi_cfg_t spi_cfg_gl = {
    /// SPI master/slave mode
    .spi_ms = SPI_MS_MODE_MASTER,
    /// SPI clock mode (CPOL, CPHA)
    .spi_cp = SPI_CP_MODE_0,
    /// SPI master clock frequency
    .spi_speed = SPI_SPEED_MODE_2MHz,
    /// SPI word size (1, 2, 4 Bytes or 9 bits)
    .spi_wsz = SPI_MODE_8BIT,
    /// SPI master CS mode
    .spi_cs = SPI_CS_0,
    /// SPI Interrupt mode
    .spi_irq = SPI_IRQ_DISABLED,
    /// SPI CS Pad
    .cs_pad = {
        .port = SPI_CS_PORT,
        .pin = SPI_CS_PIN,
    },
#if (SPI_DMA_SUPPORT)
    /// SPI send callback
    .send_cb = ,
    /// SPI receive callback
    .receive_cb = ,
    /// SPI transfer callback
    .transfer_cb = ,
    /// SPI DMA Channel Pair Configuration
    .spi_dma_channel = ,
    /// SPI DMA Priority Configuration
    .spi_dma_priority = ,
#endif // SPI_DMA_SUPPORT
#ifdef __DA14531__
    /// SPI Capture at current or next edge.
    .spi_capture = SPI_MASTER_EDGE_CAPTURE,
#endif
};

/// Configuration struct for SPI Flash
static spi_flash_cfg_t spi_flash_cfg = {
    .dev_index = 0,
    .jedec_id = 0,
    .chip_size = SPI_FLASH_DEV_SIZE,
};

static spi_flash_cfg_t *spi_flash_cfg_p;

/**
 ****************************************************************************************
  @brief GPIO watchdog timer start function.

 ****************************************************************************************
 */
void gpio_wd_timer0_start(void)
{
    tim0_2_clk_div_config_t tim0_2_clk_div_config = {.clk_div = TIM0_2_CLK_DIV_8};

    wdog_enabled = true;

    // Configure watchdog GPIO.
    GPIO_ConfigurePin(WD_port, WD_pin, OUTPUT, PID_PWM0, true);
    GPIO_ConfigurePinPower(WD_port, WD_pin, WD_volt_rail ? GPIO_POWER_RAIL_1V : GPIO_POWER_RAIL_3V);

    // Stop timer to enter settings.
    timer0_stop();

    // Enable 32KHz.
#ifdef __DA14531__
    SetBits16(CLK_RC32K_REG, RC32K_DISABLE, 0);
#else
    SetBits16(CLK_32K_REG, RC32K_ENABLE, 1);
#endif

    // Enable TIMER0 clock.
    timer0_2_clk_enable();

    // Sets TIMER0,TIMER2 clock division factor to 8, so TIM0 Fclk is F = 16MHz/8 = 2Mhz.
    timer0_2_clk_div_set(&tim0_2_clk_div_config);

    // Set timer with 32KHz source clock. Division by 10 is not used in 32KHz.
    timer0_init(TIM0_CLK_32K, PWM_MODE_ONE, TIM0_CLK_DIV_BY_10);

    // Set PWM Timer0 'On', Timer0 'high' and Timer0 'low' reload values.
    timer0_set(1000, 1000, 65535); // 2sec low, 30ms high. f=0.49Hz. 32000/65535 = 0.488Hz.

    // Start Timer0.
    timer0_start();
}

/**
 ****************************************************************************************
  @brief GPIO watchdog timer stop function.

 ****************************************************************************************
 */
void gpio_wd_timer0_stop(void)
{
    if (!wdog_enabled)
    {
        return;
    }
    wdog_enabled = false;

    // Re-configure watchdog GPIO.
    GPIO_ConfigurePin(WD_port, WD_pin, INPUT_PULLDOWN, PID_GPIO, true);

    // Stop timer to enter settings.
    timer0_stop();
}

/**
 ****************************************************************************************
  @brief GPIO watchdog timer start function handler.

 ****************************************************************************************
 */
int gpio_wd_cmd(uint8_t *ptr_data)
{
    int err = ERR_OK;
    GPIO_PORT port;
    GPIO_PIN pin;
    uint8_t volt_rail; /* 0 = 3.3V, 1 = 1.8V. */

    /* Transform the input. */
#if defined(__DA14531__)
    port = GPIO_PORT_0;
    pin = (GPIO_PIN) (ptr_data[0]);
#else
    port = (GPIO_PORT) (ptr_data[0] / 10);
    pin = (GPIO_PIN) (ptr_data[0] % 10);
#endif
    volt_rail = ptr_data[1];

    /* Validate the GPIO voltage rail. 0 = 3.3V, 1 = 1.8V. */
    if (volt_rail > 1)
    {
        err = -1;
    }

    /* Validate the GPIO port/pin. */
    if (!GPIO_is_valid((GPIO_PORT) port, pin))
    {
        err = -1;
    }

    /* Check for error. Send error and return. */
    if (err == -1)
    {
        return -1;
    }

    WD_port = port;
    WD_pin = pin;
    WD_volt_rail = volt_rail;

    gpio_wd_timer0_start();

    return ERR_OK;
}

/**
 ****************************************************************************************
  @brief handler for communication errors

 ****************************************************************************************
 */
int32_t communication_error(void)
{
    return -1;
}

#ifdef USE_UART

/**
 ****************************************************************************************
  @brief Helper funtion that helps to validate UART gpios settings for ACTION_UART_GPIOS

 ****************************************************************************************
 */
static int validate_action_uart_gpios(const uint8_t *const buffer)
{
    int result = ERR_OK;

    if (!GPIO_is_valid((GPIO_PORT) buffer[1], (GPIO_PIN) buffer[2]) ||
        !GPIO_is_valid((GPIO_PORT) buffer[3], (GPIO_PIN) buffer[4]))
    {
        result = -1;
    }

    return result;
}

/**
 ****************************************************************************************
  @brief Helper funtion that helps to validate UART baud settings for ACTION_UART_BAUD

 ****************************************************************************************
 */
static int validate_action_uart_baud(const uint8_t *const buffer)
{
    int result = ERR_OK;

    if ((buffer[1] != UART_BAUD_9600) &&
        (buffer[1] != UART_BAUD_19200) &&
        (buffer[1] != UART_BAUD_57600) &&
        (buffer[1] != UART_BAUD_115200) &&
        (buffer[1] != UART_BAUD_1M))
        result = -1;

    return result;
}

/**
 ****************************************************************************************
 * @brief Helper funtion that maps baudrate select option to driver configuration value.
 ****************************************************************************************
 */
static UART_BAUDRATE get_baudrate(uint8_t baud_sel)
{
    UART_BAUDRATE baud;
    switch (baud_sel)
    {
        case UART_BAUD_9600:
        {
            baud = UART_BAUDRATE_9600;
            break;
        }
        case UART_BAUD_19200:
        {
            baud = UART_BAUDRATE_19200;
            break;
        }
        case UART_BAUD_57600:
        {
            baud = UART_BAUDRATE_57600;
            break;
        }
        case UART_BAUD_115200:
        {
            baud = UART_BAUDRATE_115200;
            break;
        }
        case UART_BAUD_1M:
        {
            baud = UART_BAUDRATE_1000000;
            break;
        }
        // fall back to default baudrate per chip (DA14585/586 or DA14531) as configured in the booter
        default:
        {
        #if defined (__DA14531__)
            baud = UART_BAUDRATE_115200;
        #else
            baud = UART_BAUDRATE_57600;
            break;
        #endif
        }
    }
    return baud;
}

/**
 ****************************************************************************************
 * @brief Set UART pads.
 ****************************************************************************************
 */
static void uart_pads(uint8_t sel)
{
    switch (sel)
    {
        case 0:
            update_uart_pads(GPIO_PORT_0, GPIO_PIN_0, GPIO_PORT_0, GPIO_PIN_1);
            break;
#if !defined (__DA14531__)
        case 2:
            update_uart_pads(GPIO_PORT_0, GPIO_PIN_2, GPIO_PORT_0, GPIO_PIN_3);
            break;
        case 4:
            update_uart_pads(GPIO_PORT_0, GPIO_PIN_4, GPIO_PORT_0, GPIO_PIN_5);
            break;
        case 6:
            update_uart_pads(GPIO_PORT_0, GPIO_PIN_6, GPIO_PORT_0, GPIO_PIN_7);
            break;
#else
        case 3: // SINGLE WIRE @ P0_3
            update_uart_pads(GPIO_PORT_0, GPIO_PIN_3, GPIO_PORT_0, GPIO_PIN_3);
            break;
        case 5: // SINGLE WIRE @ P0_5
            update_uart_pads(GPIO_PORT_0, GPIO_PIN_5, GPIO_PORT_0, GPIO_PIN_5);
            break;
#endif
        default:
            break;
    }

    set_pad_uart();
}

/**
 ****************************************************************************************
 * @brief Configures UART.
 ****************************************************************************************
 */
static void uart_set_config(uint8_t pad_sel, uint8_t baud_sel)
{
    UART_BAUDRATE baud = get_baudrate(baud_sel);
    uart_cfg_t uart_cfg = {.baud_rate = baud, .data_bits = UART_DATABITS_8, .parity = UART_PARITY_NONE,
                           .stop_bits = UART_STOPBITS_1, .auto_flow_control = UART_AFCE_DIS, .use_fifo = UART_FIFO_DIS,
                           .tx_fifo_tr_lvl = UART_TX_FIFO_LEVEL_0, .rx_fifo_tr_lvl = UART_RX_FIFO_LEVEL_0, .intr_priority = 2 };

    uart_initialize(UART1, &uart_cfg);

#if defined (__DA14531__)
    switch (pad_sel) {
    case 0:
        uart_one_wire_disable(UART1);
        break;
    case 3: // SINGLE WIRE @ P0_3
        uart_one_wire_enable(UART1, GPIO_PORT_0, GPIO_PIN_3);
        break;
    case 5: // SINGLE WIRE @ P0_5
        uart_one_wire_enable(UART1, GPIO_PORT_0, GPIO_PIN_5);
        break;
    default:
        break;
    }
#endif

    uart_pads(pad_sel);
}

/**
 ****************************************************************************************
  @brief Put a  byte into transmittion buffer

 ****************************************************************************************
 */
uint8_t *put_uint8(uint8_t value, uint8_t *buffer)
{
    *buffer++ = value;
    return buffer;
}

/**
 ****************************************************************************************
  @brief Put a  short into transmittion buffer

 ****************************************************************************************
 */
uint8_t *put_uint16(uint16_t value, uint8_t *buffer)
{
    buffer = put_uint8((value >> 8) & 0xFF, buffer);
    buffer = put_uint8((value >> 0) & 0xFF, buffer);
    return buffer;
}

/**
 ****************************************************************************************
  @brief Put an integer into transmittion buffer

 ****************************************************************************************
 */
uint8_t *put_uint32(uint32_t value, uint8_t *buffer)
{
    buffer = put_uint16((value >> 16) & 0xFFFF, buffer);
    buffer = put_uint16((value >>  0) & 0xFFFF, buffer);
    return buffer;
}

/**
 ****************************************************************************************
  @brief Transmit a byte to UART

 ****************************************************************************************
 */
void send_byte(char ch)
{
    uart_write_byte(UART1, ch);
}

/**
 ****************************************************************************************
  @brief Transmit a short to UART

 ****************************************************************************************
 */
void send_uint16(uint16_t value)
{
    send_byte((value >> 8) & 0xFF);
    send_byte((value >> 0) & 0xFF);
}

/**
 ****************************************************************************************
  @brief Transmit a string to UART

 ****************************************************************************************
 */
void send_str(char *str)
{
    while (*str)
        send_byte(*str++);
}

/**
 ****************************************************************************************
  @brief Transmit an integer to UART

 ****************************************************************************************
 */
void send_uint32(uint32_t value)
{
    send_uint16((value >> 16) & 0xFFFF);
    send_uint16((value >>  0) & 0xFFFF);
}

/**
 ****************************************************************************************
  @brief Receive a byte from UART

 ****************************************************************************************
*/
uint8_t receive_byte()
{
    return uart_read_byte(UART1);
}

/**
 ****************************************************************************************
  @brief Receive a short from UART

 ****************************************************************************************
*/
uint16_t receive_uint16()
{
    uint8_t b1 = receive_byte();
    uint8_t b0 = receive_byte();
    return (b1 << 8) | b0;
}

/**
 ****************************************************************************************
  @brief Receive an integer from UART

 ****************************************************************************************
*/
uint32_t receive_uint32()
{
    uint16_t w1 = receive_uint16();
    uint16_t w0 = receive_uint16();
    return (w1 << 16) | w0;
}

/**
 ****************************************************************************************
  @brief Extract a short from receive buffer

 ****************************************************************************************
*/
uint16_t get_uint16(uint8_t *buffer)
{
    uint8_t b1 = buffer[0];
    uint8_t b0 = buffer[1];
    return (b1 << 8) | b0;
}

/**
 ****************************************************************************************
  @brief Extract an integer from receive buffer

 ****************************************************************************************
*/
uint32_t get_uint32(uint8_t *buffer)
{
    uint16_t w1 = get_uint16(buffer + 0);
    uint16_t w0 = get_uint16(buffer + 2);
    return (w1 << 16) | w0;
}

/**
 ****************************************************************************************
  @brief Extract a revert short from receive buffer

 ****************************************************************************************
*/
uint16_t get_uint16_rev(uint8_t *buffer)
{
    uint8_t b1 = buffer[0];
    uint8_t b0 = buffer[1];
    return (b0 << 8) | b1;
}

/**
 ****************************************************************************************
  @brief Extract an revert integer from receive buffer

 ****************************************************************************************
*/
uint32_t get_uint32_rev(uint8_t *buffer)
{
    uint16_t w1 = get_uint16_rev(buffer + 0);
    uint16_t w0 = get_uint16_rev(buffer + 2);
    return (w0 << 16) | w1;
}

/**
 ****************************************************************************************
  @brief receive buffer

 ****************************************************************************************
*/
int32_t receive_packet(uint8_t *buffer)
{
    uint16_t length = receive_uint16();
    uint32_t crc_remote = receive_uint32();
    uint32_t crc_local;
    size_t i;

    for (i = 0; i < length; i++)
        buffer[i] = receive_byte();

    crc_local = crc32(0, buffer, length);
    if (crc_local != crc_remote)
        return communication_error();
    return length;
}

/**
 ****************************************************************************************
 * @brief Send pachet to uart
 *
 ****************************************************************************************
 */
void send_packet(uint8_t *buffer, uint16_t length)
{
#if defined (__DA14531__)
    uart_one_wire_tx_en(UART1);
#endif
    uint32_t crc = crc32(0, buffer, length);
    send_uint16(length);
    send_uint32(crc);
    for (size_t i = 0; i < length; i++)
        send_byte(buffer[i]);
#if defined (__DA14531__)
    uart_one_wire_rx_en(UART1);
#endif
    uart_wait_tx_finish(UART1);
}
#endif

/****************************************************************************************
   ****************************** PROTOCOL FUNCTIONS*****************************
 ****************************************************************************************/
void response_retrieve_result(uint8_t *buffer, uint8_t port_sel)
{
#ifdef USE_UART
    uint8_t *p = buffer;

    uart_pads(port_sel);
    p = put_uint8(ACTION_CONTENTS, p);
    p = put_uint32(last_status, p);
    send_packet(buffer, p - buffer);
#else
    *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = ACTION_CONTENTS;
    *(uint32_t *)(BASE_MEMORY + RESULT_OFFSET + 4) = last_status;
#endif
# if defined (__DA14531__)
    enable_hw_reset();
# endif
}

void response_action(uint8_t *buffer, uint32_t result, uint8_t port_sel)
{
    last_status = (result == ERR_OK)?ACTION_OK:ACTION_ERROR;

#ifdef USE_UART
    uint8_t *p = buffer;

    uart_pads(port_sel);
    p = put_uint8(last_status, p);
    send_packet(buffer, p - buffer);
#else
    *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = last_status;
#endif
# if defined (__DA14531__)
    enable_hw_reset();
# endif
}

void response_invalid_crc_result(uint8_t *buffer, uint8_t port_sel)
{
    last_status = ACTION_INVALID_CRC;

#ifdef USE_UART
    uint8_t *p = buffer;

    uart_pads(port_sel);
    p = put_uint8(last_status, p);
    send_packet(buffer, p - buffer);
#else
    *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = last_status;
#endif
}

void response_cfg_action_result(uint8_t *buffer, uint32_t result, uint8_t port_sel)
{
    last_status = (result == ERR_OK)?ACTION_OK:ACTION_ERROR;

#ifdef USE_UART
    uint8_t *p = buffer;

    uart_pads(port_sel);
    p = put_uint8(last_status, p);
#ifndef PLT
    if (result != ERR_OK)
        p = put_uint32(result, p);
#endif
    send_packet(buffer, p - buffer);
#else
    *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = last_status;
    if (result != ERR_OK)
        *(uint32_t *)(BASE_MEMORY + RESULT_OFFSET + 4) = result;
#endif
# if defined (__DA14531__)
    enable_hw_reset();
# endif
}

void response_erase_action_result(uint8_t *buffer, uint32_t result, uint8_t port_sel)
{
    last_status = (result == ERR_OK)?ACTION_OK:ACTION_ERROR;

#ifdef USE_UART
    uint8_t *p = buffer;

    uart_pads(port_sel);
    p = put_uint8(last_status, p);
#ifndef PLT
    if (result != ERR_OK)
        p = put_uint32(result, p);
#endif
    send_packet(buffer, p - buffer);
#else
    *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = last_status;
    if (result != ERR_OK)
        *(uint32_t *)(BASE_MEMORY + RESULT_OFFSET + 4) = result;
#endif
# if defined (__DA14531__)
    enable_hw_reset();
# endif
}

void response_read_action_result(uint8_t *buffer, uint16_t size, uint8_t port_sel)
{
    last_status = ACTION_CONTENTS;

#ifdef USE_UART
    uart_pads(port_sel);
    send_packet(buffer, size);
#else
    *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = last_status;
#endif
# if defined (__DA14531__)
    enable_hw_reset();
# endif
}

void response_action_error(uint8_t *buffer, uint32_t result, uint8_t port_sel)
{
    last_status = (result == ERR_OK)?ACTION_OK:ACTION_ERROR;

#ifdef USE_UART
    uint8_t *p = buffer;

    uart_pads(port_sel);
    p = put_uint8(last_status, p);
#ifndef PLT
    if (result != ERR_OK)
        p = put_uint32(result, p);
#endif
    send_packet(buffer, p - buffer);
    if (result == ERR_OK)
        buffer[0] = 0;
#else
    *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = last_status;
    if (result != ERR_OK)
        *(uint32_t *)(BASE_MEMORY + RESULT_OFFSET + 4) = result;
#endif
# if defined (__DA14531__)
    enable_hw_reset();
# endif
}

void response_write_action_result(uint8_t *buffer, uint32_t result, uint8_t port_sel)
{
    last_status = (result == ERR_OK)?ACTION_OK:ACTION_ERROR;

#ifdef USE_UART
    uint8_t *p = buffer;

    uart_pads(port_sel);
    p = buffer;
    p = put_uint8(last_status, p);
#ifndef PLT
    if (result != ERR_OK) p = put_uint32(result, p);
#endif
    send_packet(buffer, p - buffer);
#else
    *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = last_status;
    if (result != ERR_OK)
        *(uint32_t *)(BASE_MEMORY + RESULT_OFFSET + 4) = result;
#endif
# if defined (__DA14531__)
    enable_hw_reset();
# endif
}

void response_id_action_result(uint8_t *buffer, uint32_t jedec_id , uint8_t port_sel)
{
    last_status = ((jedec_id == 0) || (jedec_id == 0xFFFFFFFF))?ACTION_ERROR:ACTION_CONTENTS;

#ifdef USE_UART
    uint8_t *p = buffer;

    uart_pads(port_sel);
    p = buffer;
    p = put_uint8(last_status, p);
    p = put_uint32((uint32_t)jedec_id, p);
    send_packet(buffer, p - buffer);
#else
    *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = last_status;
    *(uint32_t *)(BASE_MEMORY + RESULT_OFFSET + 4) = (uint32_t)jedec_id;
#endif
# if defined (__DA14531__)
    enable_hw_reset();
# endif
}

uint8_t *get_write_position(uint8_t *buffer)
{
#ifdef USE_UART
    return &buffer[7];      // set pointer to SYSRAM
#else
    return TARGET_MEMORY;
#endif
}

uint8_t *get_read_position(uint8_t *buffer)
{
    uint8_t *p;
#ifdef USE_UART
    p = buffer;
    p = put_uint8(ACTION_CONTENTS, p);
#else
    p = TARGET_MEMORY;
#endif
    return p;
}

/****************************************************************************************
  ****************************************************************************************
  ****************************** FLASH MANIPULATION FUNCTIONS*****************************
  ****************************************************************************************
  ****************************************************************************************/

#define SPI_FLASH_SECTOR_SIZE 4096
SPI_Pad_t spi_FLASH_CS_Pad;
/**
 ****************************************************************************************
 * @brief SPI and SPI flash initialization function
 *
 ****************************************************************************************
 */
static int8_t spi_flash_peripheral_init()
{
    int8_t status = ERR_OK;
    uint8_t dev_id;
    uint32_t jedec_id;

    // Check if there is an external configuration
    if (spi_flash_cfg_p)
    {
        // Initialize SPI block
        spi_initialize(&spi_cfg_gl);

        // Release Flash from power down
        spi_flash_release_from_power_down();

        // Read JEDEC ID
        status = spi_flash_read_jedec_id(&jedec_id);
        if (status != SPI_FLASH_ERR_OK)
        {
            return status;
        }
        if ((jedec_id == 0x00000000) || (jedec_id == 0xFFFFFFFF))
        {
            // SPI flash device not connected
            return SPI_FLASH_ERR_AUTODETECT_ERROR;
        }

        // jedec comparison
        if (jedec_id != spi_flash_cfg_p->jedec_id)
        {
            return SPI_FLASH_ERR_NOT_DETECTED;
        }

        spi_flash_configure_env(spi_flash_cfg_p);

        status = ERR_OK;
    }
    else
    {
        // Initialize SPI driver
        spi_cfg_t spi_cfg = {.spi_ms = SPI_MS_MODE_MASTER, .spi_cp = SPI_CP_MODE_0, .spi_speed = SPI_SPEED_MODE_2MHz, .spi_wsz = SPI_MODE_8BIT,
                             .spi_cs = SPI_CS_0, .cs_pad.port = (GPIO_PORT) spi_sel_pins.spi_cs_port, .cs_pad.pin = (GPIO_PIN) spi_sel_pins.spi_cs_pin};

        status = spi_flash_enable_with_autodetect(&spi_cfg, &dev_id);
        if (status != SPI_FLASH_ERR_OK)
        {
            spi_flash_cfg_t spi_flash_cfg = {.chip_size = SPI_FLASH_DEV_SIZE};
            // The device was not identified. The default parameters are used.
            // Alternatively, an error can be asserted here.
            spi_flash_configure_env(&spi_flash_cfg);
            // Return SPI_FLASH_ERR_OK in order to allow for SPI-Flash-related
            // actions for unknown devices in SmartSnippets Toolbox
            status = SPI_FLASH_ERR_OK;
        }
        if (status == SPI_FLASH_ERR_OK)
        {
            status = ERR_OK;
        }
    }

    // Return known vendor index => 0 on autodetect, error_code < 0 when not detected
    return status;
}

 /**
 ****************************************************************************************
 * @brief Erase a sector
  *
 ****************************************************************************************
 */
static int8_t erase_flash_sectors(uint32_t starting_address, uint32_t size)
{
    int i;
    uint32_t starting_sector;
    uint32_t sector;
    int8_t ret = -1;

    // starting_sector = the starting offset of the sector that contains starting_address
    starting_sector = (starting_address/SPI_FLASH_SECTOR_SIZE) * SPI_FLASH_SECTOR_SIZE;

    sector = (size / SPI_FLASH_SECTOR_SIZE);
    if (size % SPI_FLASH_SECTOR_SIZE)
        sector++;
    if (!sector) sector++;

    for (i = 0; i < sector; i++)
    {
        ret = spi_flash_block_erase(starting_sector, SPI_FLASH_OP_SE);
        starting_sector += SPI_FLASH_SECTOR_SIZE;
        if (ret != ERR_OK) break;
    }
    return ret;
}

int32_t read_data(uint8_t *buffer)
{
#ifdef USE_UART
    return receive_packet(buffer);
#else
    return 0;
#endif
}

uint8_t get_action(uint8_t *buffer)
{
#ifdef USE_UART
    return buffer[0];
#else
    return *(uint8_t *)(BASE_MEMORY + ACTION_OFFSET);
#endif
}

uint32_t get_address(uint8_t *buffer)
{
#ifdef USE_UART
    return (uint32_t) get_uint32(buffer + 1);
#else
    return *(uint32_t *)(BASE_MEMORY + ADDRESS_OFFSET);
#endif
}

uint16_t get_size(uint8_t *buffer)
{
#ifdef USE_UART
    return (uint16_t) get_uint16(buffer + 5);
#else
    return *(uint16_t *)(BASE_MEMORY + SIZE_OFFSET);
#endif
}

/*
 * Those variables will be placed just after the code
 * they can be be initialized by appending 3 bytes to binary image
 */
#pragma arm section zidata = "non_init"
uint8_t port_sel;
#pragma arm section zidata

typedef struct {
    uint8_t port;
    uint8_t pin;
    uint16_t cfg __attribute__((packed));
} gpio_pin_cfg_t;

int main(void)
{
    uint8_t *p;
    uint32_t starting_address;
    char *code;
    int result = -1;
    uint16_t actual_size;
    uint8_t action;
    uint32_t address;
    uint16_t size;
    uint32_t bytes_written = 0;
    uint32_t bytes_read;

    spi_flash_cfg_p = NULL;
    
#ifndef USE_UART
    *(uint8_t *)(BASE_MEMORY + ACTION_OFFSET) = 0;
#endif

    system_init();

#ifdef USE_UART
    #ifdef DEVELOPMENT_MODE // To use the debugger, define it and set the UART_PIN_PAIR according to board configuration (periph_setup.h)
    port_sel = UART_PIN_PAIR;
    #endif

    #if !defined (__DA14531__)
    switch (port_sel)
    {
        case 0:
        {
            uart_set_config(port_sel, UART_BAUD_57600);
            break;
        }
        case 2:
        {
            uart_set_config(port_sel, UART_BAUD_115200);
            break;
        }
        case 4:
        {
            uart_set_config(port_sel, UART_BAUD_57600);
            break;
        }
        case 6:
        {
            uart_set_config(port_sel, UART_BAUD_9600);
            break;
        }
        default:
        {
            uart_set_config(port_sel, UART_BAUD_57600);
            break;
        }
    }
    #else
        uart_set_config(port_sel, UART_BAUD_115200);
    #endif

    /* Some delay for UART to init. */
    for (volatile int k=0; k<8000; k++) {};

#ifndef PLT
    send_str("Hello");
#endif

    memset(buffer, 0, ALLOWED_DATA);
#else
    memset(TARGET_MEMORY, 0, ALLOWED_DATA);
    *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = ACTION_READY;
#endif

    while (1) {

#ifdef USE_UART
        while (read_data(buffer) <= 0) {
            response_invalid_crc_result(buffer, port_sel);
        }
#endif

        action = get_action(buffer);
        address = get_address(buffer);
        size = get_size(buffer);
        starting_address = (uint32_t)address;

#ifndef USE_UART
        if (action)
            *(uint8_t *)(BASE_MEMORY + ACTION_OFFSET) = 0;
#endif
        switch (action)
        {
            case NO_ACTION:
            {
                break;
            }
            case ACTION_GPIO_WD:
            {
                result = gpio_wd_cmd(&buffer[1]);
                response_action(buffer, result, port_sel);
                break;
            }
            case ACTION_READ_VERSION:
            {
                p = get_read_position(buffer);
                memcpy(p, FLASH_PROGRAMMER_SW_VERSION, sizeof(FLASH_PROGRAMMER_SW_VERSION));
#ifndef PLT
                response_read_action_result(buffer, sizeof(FLASH_PROGRAMMER_SW_VERSION) + 1, port_sel);
#else
                memcpy(p + strlen(FLASH_PROGRAMMER_SW_VERSION), PLT_VERSION, sizeof(PLT_VERSION));
                response_read_action_result(buffer, 32, port_sel);
#endif
                break;
            }
            case ACTION_PLATFORM_RESET:
            {
                SetWord16(RESET_FREEZE_REG, FRZ_WDOG);
                SetWord16(WATCHDOG_CTRL_REG, NMI_RST);
                SetWord16(WATCHDOG_REG, 5);
                // Wait for Watchdog to expire
                while (GetBits16(WATCHDOG_REG, WDOG_VAL));
                break;
            }
#ifdef USE_UART
            case ACTION_UART_BAUD:
            {
                result = validate_action_uart_baud(buffer);

                response_action(buffer, result, port_sel);
                uart_wait_tx_finish(UART1);
                if (result == ERR_OK)
                    uart_set_config(port_sel, buffer[1]);
                break;
            }
            case ACTION_UART_GPIOS:
            {
                result = validate_action_uart_gpios(buffer);

                response_action(buffer, result, port_sel);
                uart_wait_tx_finish(UART1);
                if (result == ERR_OK) {
                    port_sel = 10; // something different than 0,2,4,6 the default uart pin pairs.
                    update_uart_pads((GPIO_PORT)buffer[1], (GPIO_PIN)buffer[2], 
                                     (GPIO_PORT)buffer[3], (GPIO_PIN)buffer[4]);
                    set_pad_uart();
                }
                break;
            }
#endif
            case ACTION_SPI_GPIOS:
            {
                #ifdef USE_UART
                // uart
                uint8_t * pin_buffer = &buffer[1];
                #else
                // jtag
                uint8_t * pin_buffer = TARGET_MEMORY;
                #endif

                result = ERR_OK;

                // check that all provided GPIOS are valid
                // note: the action buffer contains 4 port-pin pairs
                for(int idx = 0; idx < 8; idx += 2)
                {
                    if (!GPIO_is_valid((GPIO_PORT) pin_buffer[idx], (GPIO_PIN) pin_buffer[idx + 1]))
                    {
                        result = -1;
                    }
                }

                // "Enable pin" enable.
                if (pin_buffer[8] != 0 && pin_buffer[8] != 1)
                    result = -1;
                bool enable_pin = (bool) pin_buffer[8];

                if (enable_pin && (result == ERR_OK))
                {
                    if (!GPIO_is_valid((GPIO_PORT)pin_buffer[9], (GPIO_PIN)pin_buffer[10]))
                    {
                        result = -1;
                    }
                }

                if (result == ERR_OK) {
                    update_spi_pads(pin_buffer);
                    
                    // Set Enable pin
                    if (enable_pin)
                    {
#if defined (__DA14531__)
                        check_gpio_hw_reset((GPIO_PORT) pin_buffer[9], (GPIO_PIN) pin_buffer[10]);
#endif
                        GPIO_ConfigurePin((GPIO_PORT) pin_buffer[9], (GPIO_PIN) pin_buffer[10], OUTPUT, PID_GPIO, true);
                    }
                }

                response_action(buffer, result, port_sel);
                break;
            }
            case ACTION_I2C_GPIOS:
            {
                #ifdef USE_UART
                // uart
                uint8_t * pin_buffer = &buffer[1];
                #else
                // jtag
                uint8_t * pin_buffer = TARGET_MEMORY;
                #endif

                result = ERR_OK;

                // check that all provided GPIOS are valid
                // note: the action buffer contains 2 port-pin pairs
                for(int idx = 0; idx < 4; idx += 2)
                {
                    if (!GPIO_is_valid((GPIO_PORT) pin_buffer[idx], (GPIO_PIN) pin_buffer[idx + 1]))
                    {
                        result = -1;
                    }
                }

                // "Enable pin" enable.
                if (pin_buffer[4] != 0 && pin_buffer[4] != 1)
                    result = -1;
                bool enable_pin = (bool) pin_buffer[4];

                if (enable_pin && (result == ERR_OK))
                {
                    if (!GPIO_is_valid((GPIO_PORT) pin_buffer[5], (GPIO_PIN) pin_buffer[6]))
                    {
                        result = -1;
                    }
                }

                if (result == ERR_OK) {
                    update_eeprom_pads(pin_buffer);

                    // Set Enable pin
                    if (enable_pin)
                    {
#if defined (__DA14531__)
                        check_gpio_hw_reset((GPIO_PORT) pin_buffer[5], (GPIO_PIN) pin_buffer[6]);
#endif
                        GPIO_ConfigurePin((GPIO_PORT) pin_buffer[5], (GPIO_PIN) pin_buffer[6], OUTPUT, PID_GPIO, true);
                    }
                }

                response_action(buffer, result, port_sel);
                break;
            }
            case ACTION_GPIOS:
            {
                result = ERR_OK;
                #ifdef USE_UART
                // uart
                gpio_pin_cfg_t *pin_cfg = (gpio_pin_cfg_t *)&buffer[2];
                size = buffer[1];
                for (int i = 0; i < size; ++i)
                {
                    // Make sure that UART pins selected by toolbox are not changed
                    if ((pin_cfg[i].port == uart_sel_pins.uart_tx_port &&
                         pin_cfg[i].pin == uart_sel_pins.uart_tx_pin) ||
                        (pin_cfg[i].port == uart_sel_pins.uart_rx_port &&
                         pin_cfg[i].pin == uart_sel_pins.uart_rx_pin))
                    {
                        result = ERR_INVAL;
                        break;
                    }
                }
                #else
                // jtag
                gpio_pin_cfg_t *pin_cfg = TARGET_MEMORY;
                size /= 4;
                #endif
                for (int i = 0; i < size && result == ERR_OK; ++i)
                {
                    if (!GPIO_is_valid((GPIO_PORT) pin_cfg[i].port, (GPIO_PIN) pin_cfg[i].pin))
                    {
                        result = ERR_INVAL;
                    }
                    // For now only GPIO function can be set. This does not require to check if pin function is
                    // already set to other pins.
                    // If other pin functins will be possible in the future other pins should be checked to allow
                    // only on pin with specified funciton.
#if defined (__DA14531__)
                    check_gpio_hw_reset((GPIO_PORT)pin_cfg[i].port, (GPIO_PIN)pin_cfg[i].pin);
#endif
                    GPIO_SetPinFunction((GPIO_PORT)pin_cfg[i].port, (GPIO_PIN)pin_cfg[i].pin, (GPIO_PUPD)(pin_cfg[i].cfg & 0x30), PID_GPIO);
                }

                response_action_error(buffer, result, port_sel);
                break;
            }
            case ACTION_SPI_INIT:
            {
                _spi_cfg_opts *spi_cfg_opts = NULL;

                #ifdef USE_UART
                // uart
                spi_cfg_opts = (_spi_cfg_opts*)&buffer[1];
                #else
                // jtag
                spi_cfg_opts = (_spi_cfg_opts*)TARGET_MEMORY;
                #endif

                // Set parameters in main structs;
                spi_cfg_gl.spi_wsz = spi_cfg_opts->word_len;
                spi_cfg_gl.spi_ms = spi_cfg_opts->mode;
                spi_cfg_gl.spi_cp = (SPI_CP_MODE_CFG)((spi_cfg_opts->idle_pol<<1)|(spi_cfg_opts->sampling_edge^spi_cfg_opts->idle_pol));
                spi_cfg_gl.spi_irq = spi_cfg_opts->mint;
                switch(spi_cfg_opts->clk_div)
                {
                    case SPI_XTAL_DIV_8:    spi_cfg_gl.spi_speed = SPI_SPEED_MODE_2MHz; break;
                    case SPI_XTAL_DIV_4:    spi_cfg_gl.spi_speed = SPI_SPEED_MODE_4MHz; break;
                    case SPI_XTAL_DIV_2:    spi_cfg_gl.spi_speed = SPI_SPEED_MODE_8MHz; break;
#if defined (__DA14531__)
                    case SPI_XTAL_DIV_14:   spi_cfg_gl.spi_speed = SPI_SPEED_MODE_16MHz; break;
#endif
                }

                spi_flash_cfg.jedec_id = spi_cfg_opts->jedec_id,
                spi_flash_cfg.chip_size = spi_cfg_opts->total_size,

                spi_flash_cfg_p = &spi_flash_cfg;

                set_pad_spi();
                if (spi_flash_peripheral_init() != ERR_OK)
                {
                    response_action_error(buffer, (uint32_t)SPI_FLASH_ERR_UNKNOWN_FLASH_TYPE, port_sel);
                    break;
                }

                // Remove memory protection for the entire memory array.
                if (spi_flash_configure_memory_protection(spi_cfg_opts->mem_prot) != ERR_OK)
                    response_action_error(buffer, (uint32_t)SPI_FLASH_ERR_PROTECTED, port_sel);

                response_cfg_action_result(buffer, ERR_OK, port_sel);
                break;
            }
            case ACTION_SPI_ERASE_BLOCK:
            {
                set_pad_spi();
                if (spi_flash_peripheral_init() != ERR_OK)
                {
                    response_action_error(buffer, (uint32_t)SPI_FLASH_ERR_UNKNOWN_FLASH_TYPE, port_sel);
                    break;
                }
                result = (uint32_t) erase_flash_sectors(address, size*SPI_FLASH_SECTOR_SIZE);
                response_erase_action_result(buffer, result, port_sel);
                break;
            }
            case ACTION_SPI_ERASE:
            {
                set_pad_spi();
                if (spi_flash_peripheral_init() != ERR_OK)
                {
                    response_action_error(buffer, (uint32_t)SPI_FLASH_ERR_UNKNOWN_FLASH_TYPE, port_sel);
                    break;
                }
                result = (uint32_t) spi_flash_chip_erase();
                response_erase_action_result(buffer, result, port_sel);
                break;
            }
            case ACTION_SPI_IS_EMPTY:
            {
                set_pad_spi();
                if (spi_flash_peripheral_init() != ERR_OK)
                {
                    response_action_error(buffer, (uint32_t)SPI_FLASH_ERR_UNKNOWN_FLASH_TYPE, port_sel);
                    break;
                }

                starting_address = (uint32_t)address;
                p = get_write_position(buffer);

                if (p[0] == SPI_IS_EMPTY_ALL) {
                    result = (uint32_t) spi_flash_is_empty();
                }
                else
                {
                    starting_address = (starting_address / SPI_FLASH_SECTOR_SIZE) * SPI_FLASH_SECTOR_SIZE;

                    if ((starting_address + size * SPI_FLASH_SECTOR_SIZE) > SPI_FLASH_DEV_SIZE)
                    {
                        size = (uint16_t) ((SPI_FLASH_DEV_SIZE - starting_address) / SPI_FLASH_SECTOR_SIZE);
                    }

                    result = ERR_OK;
                    for (int i = 0; ((i < size) && (result == ERR_OK)); i++)
                    {
                        if (spi_flash_is_sector_empty((uint32_t) (starting_address + i * SPI_FLASH_SECTOR_SIZE)) != SPI_FLASH_ERR_OK)
                        {
                            result = SPI_FLASH_ERR_NOT_ERASED;
                        }
                    }
                }

                response_action(buffer, result, port_sel);
                break;
            }
            case ACTION_SPI_READ:
            {
                set_pad_spi();
                if (spi_flash_peripheral_init() != ERR_OK)
                {
                    response_action_error(buffer, (uint32_t)SPI_FLASH_ERR_UNKNOWN_FLASH_TYPE, port_sel);
                    break;
                }

                starting_address = (uint32_t)address;
                p = get_read_position(buffer);
                result = spi_flash_read_data(p, (uint32_t)starting_address, (uint32_t)size, &actual_size);
                if ((result != SPI_FLASH_ERR_OK) || (actual_size != size))
                {
                    result = SPI_FLASH_ERR_PROG_ERROR;
                    response_action_error(buffer, (uint32_t)result, port_sel);
                }
                else
                {
                    response_read_action_result(buffer, size + 1, port_sel);
                }
                break;
            }
            case ACTION_SPI_WRITE:
            {
                set_pad_spi();
                if (spi_flash_peripheral_init() != ERR_OK)
                {
                    response_action_error(buffer, (uint32_t)SPI_FLASH_ERR_UNKNOWN_FLASH_TYPE, port_sel);
                    break;
                }
                p = get_write_position(buffer);
                result = spi_flash_write_data(p, (uint32_t)starting_address, size, &actual_size);
                if ((result != SPI_FLASH_ERR_OK) || (actual_size != size))
                {
                    result = SPI_FLASH_ERR_PROG_ERROR;
                    response_action_error(buffer, (uint32_t)result, port_sel);
                    break;
                }
                result = ERR_OK;
                response_write_action_result(buffer, (uint32_t)result, port_sel);
                break;
            }
            case ACTION_SPI_ID:
            {
                uint32_t jedec_id;
                set_pad_spi();
                if (spi_flash_peripheral_init() != ERR_OK)
                {
                    response_action_error(buffer, (uint32_t)SPI_FLASH_ERR_UNKNOWN_FLASH_TYPE, port_sel);
                    break;
                }
                result = spi_flash_read_jedec_id(&jedec_id);
                if (result == SPI_FLASH_ERR_OK)
                {
                    response_id_action_result(buffer, jedec_id, port_sel);
                }
                else
                {
                    response_action_error(buffer, (uint32_t)SPI_FLASH_ERR_INVAL, port_sel);
                }
                break;
            }
            case ACTION_EEPROM_INIT:
            {
                _eeprom_cfg_opts *eeprom_cfg_opts = NULL;

                #ifdef USE_UART
                // uart
                eeprom_cfg_opts = (_eeprom_cfg_opts*)&buffer[1];
                #else
                // jtag
                eeprom_cfg_opts = (_eeprom_cfg_opts*)TARGET_MEMORY;
                #endif


                // Set parameters in main structs;
                i2c_cfg.speed = eeprom_cfg_opts->speed_mode;
                i2c_cfg.addr_mode = eeprom_cfg_opts->address_mode;
                i2c_cfg.address = eeprom_cfg_opts->slave_address;

                i2c_eeprom_cfg.address_size = eeprom_cfg_opts->address_byte_count;
                i2c_eeprom_cfg.page_size = eeprom_cfg_opts->page_size;
                i2c_eeprom_cfg.size = eeprom_cfg_opts->total_size;

                // Configure I2C EEPROM environment
                i2c_eeprom_configure(&i2c_cfg, &i2c_eeprom_cfg);

                // Initialize I2C
                i2c_eeprom_initialize();

                // Set pad functionality
                set_pad_eeprom();

                response_cfg_action_result(buffer, ERR_OK, port_sel);

                break;
            }
            case ACTION_EEPROM_READ:
            {
                // Configure I2C EEPROM environment
                i2c_eeprom_configure(&i2c_cfg, &i2c_eeprom_cfg);

                // Initialize I2C
                i2c_eeprom_initialize();

                // Set pad functionality
                set_pad_eeprom();

                starting_address = (uint32_t)address;
                p = get_read_position(buffer);
                result = i2c_eeprom_read_data(p, (uint32_t)starting_address, (uint32_t)size, &bytes_read);
                if (result != I2C_NO_ERROR)
                    response_action_error(buffer, (uint32_t)result, port_sel);
                else
                    response_read_action_result(buffer, size + 1, port_sel);
                break;
            }
            case ACTION_EEPROM_WRITE:
            {
                // Configure I2C EEPROM environment
                i2c_eeprom_configure(&i2c_cfg, &i2c_eeprom_cfg);

                // Initialize I2C
                i2c_eeprom_initialize();

                // Set pad functionality
                set_pad_eeprom();

                starting_address = (uint32_t)address;
                code =  (char*)get_write_position(buffer);
                result = i2c_eeprom_write_data((uint8_t *)code, starting_address, size, &bytes_written);
                if (result != I2C_NO_ERROR)
                    response_action_error(buffer, (uint32_t)result, port_sel);
                else
                    response_write_action_result(buffer, 0, port_sel);
                break;
            }
            case ACTION_OTP_READ:
            {
                hw_otpc_init();
                #if defined (__DA14531__)
                hw_otpc_enter_mode(HW_OTPC_MODE_READ);
                #else
                hw_otpc_manual_read_on(false);
                #endif

    #ifdef USE_UART
                p = buffer;
                p = put_uint8(ACTION_CONTENTS, p);

                memcpy(p, (void *)(MEMORY_OTP_BASE + address), size);
                send_packet(buffer, size + 1);
    #else
                memcpy((void *)TARGET_MEMORY, (void *)(MEMORY_OTP_BASE + address), size);
                *(uint8_t *)(BASE_MEMORY + RESULT_OFFSET) = ACTION_CONTENTS;
    #endif
                hw_otpc_disable();
                break;
            }
            case ACTION_OTP_WRITE:
            {
                uint32_t word2write[HW_OTP_CELL_SIZE / 4];
                volatile size_t i;
                unsigned int target_address = (unsigned int) address;
                result = 0;

                p = get_write_position(buffer);

                for (i = 0; i < size; i += HW_OTP_CELL_SIZE)
                {
                    memcpy(word2write, p + i, sizeof(word2write));

                    /*
                     * OTP cell must be powered on, so we turn on the related clock.
                     * However, the OTP LDO will not be able to sustain an infinite
                     * amount of writes. So we keep our writes limited. Turning off the
                     * LDO at the end of the operation ensures that we restart the
                     * operation appropriately for the next write.
                     */
                    hw_otpc_init();
                    #if defined (__DA14531__)
                    if (word2write[0] != 0xffffffff)
                    {
                        result = hw_otpc_prog_and_verify((word2write), target_address >> 2, 1)? 0 : 1;
                    }
                    #else
                    /* From the manual: The destination address is a word aligned address, that represents the OTP memory
                    address. It is not an AHB bus address. The lower allowed value is 0x0000 and the maximum allowed
                    value is 0x1FFF (8K words space). */
                    result = hw_otpc_fifo_prog((const uint32_t *) (&word2write),
                                               target_address >> 3,
                                               HW_OTPC_WORD_LOW, 2, false) ? 0 : 1;
                    #endif
                    hw_otpc_disable();

                    target_address += HW_OTP_CELL_SIZE;

                    if (result != 0)
                        break;
                }
                response_write_action_result(buffer, result, port_sel);
                break;
            }
            default:
                response_action_error(buffer, ACTION_INVALID_COMMAND, port_sel);
        }
    }
}
