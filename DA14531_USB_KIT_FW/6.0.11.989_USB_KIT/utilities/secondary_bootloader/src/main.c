/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Secondary Bootloader application
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

#include <stdio.h>
#include "arch_system.h"
#include "datasheet.h"
#include "syscntl.h"
#include "user_periph_setup.h"
#include "uart_booter.h"
#include "i2c_eeprom.h"
#include "spi_flash.h"
#include "bootloader.h"
#include "gpio.h"
#include "uart.h"
#include "hw_otpc.h"

#if defined (__DA14531__)
#include "adc.h"
#endif

void Start_run_user_application(void);

 /**
****************************************************************************************
* @brief Run the user application after receiving a binany from
*        uart or reading the binary from a non volatile memory booting
****************************************************************************************
*/
void Start_run_user_application(void)
{
    volatile unsigned short tmp;

#ifdef SPI_FLASH_SUPPORTED
    spi_release();

    // reset SPI GPIOs
    GPIO_ConfigurePin(SPI_EN_PORT, SPI_EN_PIN, INPUT_PULLUP, PID_GPIO, true);
    GPIO_ConfigurePin(SPI_CLK_PORT, SPI_CLK_PIN, INPUT_PULLDOWN, PID_GPIO, false);
    GPIO_ConfigurePin(SPI_DO_PORT, SPI_DO_PIN, INPUT_PULLDOWN, PID_GPIO, false);
    GPIO_ConfigurePin(SPI_DI_PORT, SPI_DI_PIN, INPUT_PULLDOWN, PID_GPIO, false);
#endif

#ifdef I2C_EEPROM_SUPPORTED
    i2c_eeprom_release();

    // reset I2C GPIOs
    GPIO_ConfigurePin(I2C_SCL_PORT, I2C_SCL_PIN, INPUT_PULLDOWN, PID_GPIO, false);
    GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, INPUT_PULLDOWN, PID_GPIO, false);
#endif

    if (*(volatile unsigned*)0x07FC0000 & 0x07FC0000)
    {
        tmp = GetWord16(SYS_CTRL_REG);
        //Remap to SysRAM and trigger  SW Reset
        tmp = (tmp & ~REMAP_ADR0) | 0x2;
        tmp |= SW_RESET;
        SetWord16(SYS_CTRL_REG, tmp);
    }
    tmp = GetWord16(SYS_CTRL_REG);
    //  tmp&=~0x0003;
    tmp |= 0x8000;
    SetWord16(SYS_CTRL_REG,tmp);
}

extern int FwDownload(void);
extern void uart_receive_callback(uint16_t length);

#ifdef UART_SUPPORTED
static const uart_cfg_t uart_cfg =
            {
                .baud_rate = UART_BAUDRATE_57600,
                .data_bits = UART_DATABITS_8,
                .parity = UART_PARITY_NONE,
                .stop_bits = UART_STOPBITS_1,
                .auto_flow_control = UART_AFCE_DIS,
                .use_fifo = UART_FIFO_EN,
                .tx_fifo_tr_lvl = UART_TX_FIFO_LEVEL_0,
                .rx_fifo_tr_lvl = UART_RX_FIFO_LEVEL_0,
                .intr_priority = 2,
                .uart_rx_cb = uart_receive_callback,
            };
#endif

int main (void)
{
#if defined(SPI_FLASH_SUPPORTED ) || defined(I2C_EEPROM_SUPPORTED)
    int ret = -1;
#endif

#ifdef UART_SUPPORTED
    int fw_size;
    unsigned int i;
    char *from ;
    char *to;
#endif

    /*
     ************************************************************************************
     * Watchdog start (maximum value)
     ************************************************************************************
     */
    SetBits(WATCHDOG_CTRL_REG, NMI_RST, 1);    // WDOG will generate HW reset upon expiration
    SetWord16(WATCHDOG_REG, 0xFF);             // Reset WDOG
    SetWord16(RESET_FREEZE_REG, FRZ_WDOG);     // Start WDOG

    system_init();

    while (1)
    {
#ifdef UART_SUPPORTED
        if (GPIO_GetPinStatus(UART_GPIO_PORT, UART_RX_PIN))
        {
            uart_initialize(UART1, &uart_cfg);

            //initialize UART pins
            GPIO_ConfigurePin(UART_GPIO_PORT, UART_TX_PIN, OUTPUT, PID_UART1_TX, false);
            GPIO_ConfigurePin(UART_GPIO_PORT, UART_RX_PIN, INPUT, PID_UART1_RX, false);

            fw_size = FwDownload();                     // download FW

            uart_disable(UART1);
            //reset UART pins
            GPIO_ConfigurePin(UART_GPIO_PORT, UART_TX_PIN, INPUT, PID_GPIO, false);
            GPIO_ConfigurePin(UART_GPIO_PORT, UART_RX_PIN, INPUT, PID_GPIO, false);

            if (fw_size > 0)
            {
                from = (char*) (SYSRAM_COPY_BASE_ADDRESS);
                to = (char*) (SYSRAM_BASE_ADDRESS);
                for (i = 0; i < fw_size; i++)
                {
                    to[i] = from[i];
                }

                SetWord16(WATCHDOG_REG, 0xC8);          // 200 * 10.24ms active time for initialization!
                SetWord16(RESET_FREEZE_REG, FRZ_WDOG);  // Start WDOG
                Start_run_user_application();
            }
        }
#endif

#ifdef SPI_FLASH_SUPPORTED
        ret = spi_loadActiveImage();
        if (!ret)
        {
            SetWord16(WATCHDOG_REG, 0xC8);          // 200 * 10.24ms active time for initialization!
            SetWord16(RESET_FREEZE_REG, FRZ_WDOG);  // Start WDOG
            Start_run_user_application();
        }
#endif

#ifdef I2C_EEPROM_SUPPORTED
        ret = eeprom_loadActiveImage();
        if (!ret)
        {
            SetWord16(WATCHDOG_REG, 0xC8);          // 200 * 10.24ms active time for initialization!
            SetWord16(RESET_FREEZE_REG, FRZ_WDOG);  // Start WDOG
            Start_run_user_application();
        }
#endif

    } // while (1)
}

