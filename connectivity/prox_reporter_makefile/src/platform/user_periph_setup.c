/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (C) 2012-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration
#include "datasheet.h"
#include "user_periph_setup.h"       // peripheral configuration
#include "system_library.h"
#include "user_profiles_config.h"
#include "gpio.h"
#include "uart.h"                    // UART initialization
#include "app_proxr.h"
#include "user_proxr.h"
#include "syscntl.h"

#if BLE_BATT_SERVER
#include "app_bass.h"
#endif

#if (USE_RANGE_EXT)
#include "range_ext_api.h"
#include "sky66111.h"
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#if defined (CFG_SPI_FLASH_ENABLE)
#include "spi.h"
#include "spi_flash.h"
#endif

#if defined (CFG_I2C_EEPROM_ENABLE)
#include "i2c.h"
#include "i2c_eeprom.h"
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/
    // Push Button
    RESERVE_GPIO(PUSH_BUTTON, GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, PID_GPIO);

#if defined (__DA14531__) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
    // Wake up from hibernation pin
    RESERVE_GPIO(HIB_WAKE_UP, HIB_WAKE_UP_PORT, HIB_WAKE_UP_PIN, PID_GPIO);
#endif

#if BLE_PROX_REPORTER
    // Alert LED
    RESERVE_GPIO(GREEN_LED, GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, PID_GPIO);
#endif

#if (BLE_BATT_SERVER && USE_BAT_LEVEL_ALERT)
    // Battery alert LED
    RESERVE_GPIO(RED_LED, GPIO_BAT_LED_PORT, GPIO_BAT_LED_PIN, PID_GPIO);
#endif

#if defined (CFG_PRINTF_UART2)
    // UART2 Tx pin
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif

#if defined (CFG_SPI_FLASH_ENABLE) && !defined (__DA14586__)
    // SPI Flash
    RESERVE_GPIO(SPI_EN, SPI_EN_PORT, SPI_EN_PIN, PID_SPI_EN);
    RESERVE_GPIO(SPI_CLK, SPI_CLK_PORT, SPI_CLK_PIN, PID_SPI_CLK);
    RESERVE_GPIO(SPI_DO, SPI_DO_PORT, SPI_DO_PIN, PID_SPI_DO);
    RESERVE_GPIO(SPI_DI, SPI_DI_PORT, SPI_DI_PIN, PID_SPI_DI);
#endif

#if defined (CFG_I2C_EEPROM_ENABLE)
    // I2C EEPROM
    RESERVE_GPIO(I2C_SCL, I2C_SCL_PORT, I2C_SCL_PIN, PID_I2C_SCL);
    RESERVE_GPIO(I2C_SDA, I2C_SDA_PORT, I2C_SDA_PIN, PID_I2C_SDA);
#endif

#if (USE_RANGE_EXT)
    // Range extender
    range_ext.init_gpio(NULL);
#endif
}
#endif //DEVELOPMENT_DEBUG

void set_pad_functions(void)        // set gpio port function mode
{
/*
    i.e. to set P0_1 as Generic purpose Output:
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
*/

#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#endif

    // Push Button
    GPIO_ConfigurePin(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, INPUT_PULLUP, PID_GPIO, false);

#if defined (__DA14531__) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
    // Wake up from hibernation pin
    GPIO_ConfigurePin(HIB_WAKE_UP_PORT, HIB_WAKE_UP_PIN, INPUT_PULLUP, PID_GPIO, false);
#endif

#if (BLE_PROX_REPORTER)
    // Alert LED
    GPIO_ConfigurePin(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN, OUTPUT, PID_GPIO, false);
#endif

#if (BLE_BATT_SERVER && USE_BAT_LEVEL_ALERT)
    // Battery alert LED
    GPIO_ConfigurePin(GPIO_BAT_LED_PORT, GPIO_BAT_LED_PIN, OUTPUT, PID_GPIO, false);
#endif

#if defined (CFG_PRINTF_UART2)
    // UART2 Tx pin
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif

#if defined (CFG_SPI_FLASH_ENABLE)
    // SPI Flash
    GPIO_ConfigurePin(SPI_EN_PORT, SPI_EN_PIN, OUTPUT, PID_SPI_EN, true);
    GPIO_ConfigurePin(SPI_CLK_PORT, SPI_CLK_PIN, OUTPUT, PID_SPI_CLK, false);
    GPIO_ConfigurePin(SPI_DO_PORT, SPI_DO_PIN, OUTPUT, PID_SPI_DO, false);
    GPIO_ConfigurePin(SPI_DI_PORT, SPI_DI_PIN, INPUT, PID_SPI_DI, false);
#endif

#if defined (CFG_I2C_EEPROM_ENABLE)
    // I2C EEPROM
    GPIO_ConfigurePin(I2C_SCL_PORT, I2C_SCL_PIN, INPUT, PID_I2C_SCL, false);
    GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, INPUT, PID_I2C_SDA, false);
#endif

#if (USE_RANGE_EXT)
    // Range extender
    range_ext.enable(MAX_POWER, NULL);
#endif
}

#if defined (CFG_PRINTF_UART2)
// Configuration struct for UART2
static const uart_cfg_t uart_cfg = {
    .baud_rate = UART2_BAUDRATE,
    .data_bits = UART2_DATABITS,
    .parity = UART2_PARITY,
    .stop_bits = UART2_STOPBITS,
    .auto_flow_control = UART2_AFCE,
    .use_fifo = UART2_FIFO,
    .tx_fifo_tr_lvl = UART2_TX_FIFO_LEVEL,
    .rx_fifo_tr_lvl = UART2_RX_FIFO_LEVEL,
    .intr_priority = 2,
};
#endif

#if defined (CFG_SPI_FLASH_ENABLE)
// Configuration struct for SPI
static const spi_cfg_t spi_cfg = {
    .spi_ms = SPI_MS_MODE,
    .spi_cp = SPI_CP_MODE,
    .spi_speed = SPI_SPEED_MODE,
    .spi_wsz = SPI_WSZ,
    .spi_cs = SPI_CS,
    .cs_pad.port = SPI_EN_PORT,
    .cs_pad.pin = SPI_EN_PIN,
#if defined (__DA14531__)
    .spi_capture = SPI_EDGE_CAPTURE,
#endif
};

// Configuration struct for SPI FLASH
static const spi_flash_cfg_t spi_flash_cfg = {
    .chip_size = SPI_FLASH_DEV_SIZE,
};
#endif

#if defined (CFG_I2C_EEPROM_ENABLE)
// Configuration struct for I2C
static const i2c_cfg_t i2c_cfg = {
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

// Configuration struct for I2C EEPROM
static const i2c_eeprom_cfg_t i2c_eeprom_cfg = {
    .size = I2C_EEPROM_DEV_SIZE,
    .page_size = I2C_EEPROM_PAGE_SIZE,
    .address_size = I2C_ADDRESS_SIZE,
};
#endif

void periph_init(void)  // set i2c, spi, uart, uart2 serial clks
{
#if defined (__DA14531__)
    // Disable HW Reset functionality of P0_0
    GPIO_Disable_HW_Reset();

    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#else
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    // ROM patch
    patch_func();

    // Initialize peripherals
#if defined (CFG_PRINTF_UART2)
    // Initialize UART2
    uart_initialize(UART2, &uart_cfg);
#endif

#if defined (CFG_SPI_FLASH_ENABLE)
    // Configure SPI Flash environment
    spi_flash_configure_env(&spi_flash_cfg);

    // Initialize SPI
    spi_initialize(&spi_cfg);
#endif

#if defined (CFG_I2C_EEPROM_ENABLE)
    // Configure I2C EEPROM environment
    i2c_eeprom_configure(&i2c_cfg, &i2c_eeprom_cfg);

    // Initialize I2C
    i2c_eeprom_initialize();
#endif

    // Set pad functionality
    set_pad_functions();

#if (BLE_PROX_REPORTER)
    app_proxr_port_reinit(GPIO_ALERT_LED_PORT, GPIO_ALERT_LED_PIN);
    app_button_enable();
#endif

#if BLE_BATT_SERVER
    app_batt_port_reinit();
#endif

    // Enable the pads
    GPIO_set_pad_latch_en(true);

#if (USE_SPI_FLASH_EXTENSIONS)
    spi_flash_enable_power_pin();
#endif
}
