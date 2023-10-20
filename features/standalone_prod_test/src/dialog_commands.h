/**
 ****************************************************************************************
 *
 * @file dialog_commands.h
 *
 * @brief Dialog_commands header file.
 *
 * Copyright (C) 2017-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef DIALOG_COMMANDS_H_
#define DIALOG_COMMANDS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "hci_int.h"
#include "ke_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define HCI_VS_FIRST_DIALOG_CMD_OPCODE    0xFE00

enum
{
    HCI_CUSTOM_ACTION_CMD_OPCODE = HCI_VS_FIRST_DIALOG_CMD_OPCODE,
    HCI_SLEEP_TEST_CMD_OPCODE,
    HCI_XTAL_TRIM_CMD_OPCODE,
    HCI_OTP_RW_CMD_OPCODE,
    HCI_OTP_READ_CMD_OPCODE,
    HCI_OTP_WRITE_CMD_OPCODE,
    HCI_REGISTER_RW_CMD_OPCODE,
    HCI_AUDIO_TEST_CMD_OPCODE,
    HCI_FIRMWARE_VERSION_GET_CMD_OPCODE,
    HCI_CHANGE_UART_PINS_ACTION_CMD_OPCODE,
    HCI_RDTESTER_CMD_OPCODE,
    HCI_TX_TEST_CMD_OPCODE,
    HCI_START_PROD_RX_TEST_CMD_OPCODE,
    HCI_END_PROD_RX_TEST_CMD_OPCODE,
    HCI_UNMODULATED_ON_CMD_OPCODE,
    HCI_TX_START_CONTINUE_TEST_CMD_OPCODE,
    HCI_TX_END_CONTINUE_TEST_CMD_OPCODE,
    HCI_SENSOR_TEST_CMD_OPCODE,
    HCI_GPIO_SET_CMD_OPCODE,
    HCI_GPIO_READ_CMD_OPCODE,
    HCI_UART_LOOP_CMD_OPCODE,
    HCI_UART_BAUD_CMD_OPCODE,
    HCI_EXT32KHz_TEST_CMD_OPCODE,                                   /* 0xFE16. Reserved by PLT. */
    HCI_GPIO_WD_CMD_OPCODE,                                         /* 0xFE17. Reserved by PLT. */
    HCI_SLEEP_CLK_SEL_CMD_OPCODE,                                   /* 0xFE18. Reserved by PLT. */
    HCI_RANGE_EXT_EN_CMD_OPCODE,                                    /* 0xFE19. Reserved by PLT. */
    HCI_ADC_VBAT_CMD_OPCODE,                                        /* 0xFE1A */
    HCI_SET_TX_POWER_CMD_OPCODE,                                    /* 0xFE1B */
    HCI_CONFIGURE_TEST_MODE_CMD_OPCODE,                             /* 0xFE1C */
    HCI_PLATFORM_RESET_CMD_OPCODE,                                  /* 0xFE1D */
    HCI_RESET_MODE_CMD_OPCODE,                                      /* 0xFE1E */
    HCI_VS_LAST_DIALOG_CMD_OPCODE           // DO NOT MOVE. Must always be last and opcodes linear (00,01,02 ...)
};

// HCI dialog ADC read VBAT command parameters - vendor specific
struct hci_adc_vbat_dialog_cmd
{
    uint8_t vbat_type; // Buck: 0x0=VBAT_HIGH, 0x1=VBAT_LOW. Boost: 0x0=VBAT_LOW, 0x1=VBAT_HIGH
};

// HCI dialog ADC read VBAT complete event parameters - vendor specific
struct hci_adc_vbat_dialog_cmd_cmp_evt
{
    uint16_t vbat_lvl;
};

// HCI dialog custom_action command parameters - vendor specific
struct hci_custom_action_dialog_cmd
{
    //VS specific data
    uint8_t inputData;
};

// HCI dialog custom_action complete event parameters - vendor specific
struct hci_custom_action_dialog_cmd_cmp_evt
{
    //type
    uint32_t returnData;
};

// HCI firmware_version complete event parameters - vendor specific
struct hci_firmware_version_get_dialog_cmd_cmp_evt
{
    //Status of the command reception
    uint8_t ble_ver_len;
    uint8_t app_ver_len;
    char ble_ver[32];
    char app_ver[32];
};

// HCI dialog change_uart_pins_action command parameters - vendor specific
struct hci_change_uart_pins_action_dialog_cmd
{
    uint8_t tx_port;
    uint8_t tx_pin;
    uint8_t rx_port;
    uint8_t rx_pin;
};

// HCI dialog change_uart_pins_action complete event parameters - vendor specific
struct hci_change_uart_pins_action_dialog_cmd_cmp_evt
{
    uint16_t status;
};

// HCI dialog register_rw command parameters - vendor specific
struct hci_register_rw_dialog_cmd
{
    uint8_t operation;
    uint32_t addr;
    uint32_t data;
};

// HCI dialog register_rw complete event parameters - vendor specific
struct hci_register_rw_dialog_cmd_cmp_evt
{
    uint8_t operation;
    uint8_t reserved;
    uint32_t data;
};

enum 
{
    CMD__REGISTER_RW_OP_READ_REG32,  // read_reg32
    CMD__REGISTER_RW_OP_WRITE_REG32, // write_reg32
    CMD__REGISTER_RW_OP_READ_REG16,  // read_reg16
    CMD__REGISTER_RW_OP_WRITE_REG16  // write_reg16
};

// HCI dialog tx_start_continue_test command parameters - vendor specific
struct hci_tx_start_continue_test_dialog_cmd
{
    uint8_t frequency;
    uint8_t payload_type;
};

// HCI dialog otp_rw command parameters - vendor specific
struct hci_otp_rw_dialog_cmd
{
    uint8_t operation;
    uint8_t data[6];
};

// HCI dialog otp_rw complete event parameters - vendor specific
struct hci_otp_rw_dialog_cmd_cmp_evt
{
    uint8_t operation;
    uint8_t data[6];
};

enum 
{
    CMD__OTP_OP_RD_XTRIM,  // read XTAL16M
    CMD__OTP_OP_WR_XTRIM,  // write XTAL16M
    CMD__OTP_OP_RD_BDADDR, // read BDADDR
    CMD__OTP_OP_WR_BDADDR, // write BDADDR
    CMD__OTP_OP_RE_XTRIM,  // read enable bit XTAL16M
    CMD__OTP_OP_WE_XTRIM   // write enable bit XTAL16M
};

// HCI dialog xtal_trim command parameters - vendor specific
struct hci_xtal_trim_dialog_cmd
{
    uint8_t operation;
    uint8_t trim_LSB;
    uint8_t trim_MSB;
};

// HCI dialog xtal_trim complete event parameters - vendor specific
struct hci_xtal_trim_dialog_cmd_cmp_evt
{
    uint16_t trim_value;
};

enum 
{
    CMD__XTRIM_OP_RD,  // read XTRIM
    CMD__XTRIM_OP_WR,  // write XTRIM
    CMD__XTRIM_OP_EN,  
    CMD__XTRIM_OP_INC,
    CMD__XTRIM_OP_DEC,
    CMD__XTRIM_OP_DIS,
    CMD__XTRIM_OP_CALTEST,
    CMD__XTRIM_OP_CAL
};

// HCI dialog start_prod_rx command parameters - vendor specific
struct hci_start_prod_rx_dialog_cmd
{
    uint8_t frequency;
};

// HCI dialog end_prod_rx complete event parameters - vendor specific
struct hci_end_prod_rx_dialog_cmd_cmp_evt
{
    uint16_t rp;
    uint16_t rp_SYNC_error;
    uint16_t rp_CRC_error;
    uint16_t RSSI;
};

// HCI dialog unmodulated_on command parameters - vendor specific
struct hci_unmodulated_on_dialog_cmd
{
    uint8_t operation;
    uint8_t frequency;
};

enum
{
    UNMODULATED_CMD_MODE_OFF = 0x4F,
    UNMODULATED_CMD_MODE_TX  = 0x54,
    UNMODULATED_CMD_MODE_RX  = 0x52
};

// HCI dialog tx_test command parameters - vendor specific
struct hci_tx_test_dialog_cmd
{
    uint8_t frequency;
    uint8_t data_length;
    uint8_t payload_type;
    uint8_t NpacketsLSB;
    uint8_t NpacketsMSB;
};

// HCI dialog sleep_test command parameters - vendor specific
struct hci_sleep_test_dialog_cmd
{
    uint8_t sleep_mode;
    uint8_t mins_to_sleep;
    uint8_t seconds_to_sleep;
};

enum
{
    CMD__SLEEP_MODE_ACTIVE,
    CMD__SLEEP_MODE_EXTENDED,
    CMD__SLEEP_MODE_DEEP
};

// HCI dialog rdtest command parameters - vendor specific
struct hci_rdtest_dialog_cmd
{
    uint8_t operation;
    uint16_t data;
};

enum
{
    RDTESTER_INIT,
    RDTESTER_UART_CONNECT,
    RDTESTER_UART_LOOPBACK,
    RDTESTER_VBAT_CNTRL,
    RDTESTER_VPP_CNTRL,
    RDTESTER_RST_PULSE,
    RDTESTER_UART_PULSE,
    RDTESTER_XTAL_PULSE,
    RDTESTER_PULSE_WIDTH
};

// HCI dialog otp_read command parameters - vendor specific
struct hci_otp_read_dialog_cmd
{
    uint16_t address;
    uint8_t num_of_words;
};

// HCI dialog otp_read complete event parameters - vendor specific
struct hci_otp_read_dialog_cmd_cmp_evt
{
    uint8_t status;
    uint8_t num_of_words; 
    uint8_t wordbyte[240]; //4*60 = 240 bytes max length
};

// HCI dialog otp_write command parameters - vendor specific
struct hci_otp_write_dialog_cmd
{
    uint16_t start_addr;
    uint8_t num_of_words; 
    uint8_t wordbyte[240]; //4*60 = 240 bytes max length
};

// HCI dialog otp_write complete event parameters - vendor specific
struct hci_otp_write_dialog_cmd_cmp_evt
{
    uint8_t status;
    uint8_t num_of_words; 
};

// HCI dialog sensor_test command parameters - vendor specific
struct hci_sensor_test_dialog_cmd
{
    uint8_t interface;
    uint8_t rw;
    uint8_t spi_clk_or_i2c_port;
    uint8_t spi_clk_or_i2c_pin;
    uint8_t spi_di_or_i2c_sda_port;
    uint8_t spi_di_or_i2c_sda_pin;
    uint8_t spi_do_port;
    uint8_t spi_do_pin;
    uint8_t spi_cs_port;
    uint8_t spi_cs_pin;
    uint8_t register_address;
    uint8_t register_data_write;
    uint8_t i2c_slave_address;
    uint8_t int_gpio_check;
    uint8_t int_port;
    uint8_t int_pin;
    uint8_t pins_voltage_level;
};

// HCI dialog sensor_test complete event parameters - vendor specific
struct hci_sensor_test_dialog_cmd_cmp_evt
{
    uint16_t data;
};

// HCI dialog gpio_set command parameters - vendor specific
struct hci_gpio_set_dialog_cmd
{
    uint8_t gpio_pad;
    uint32_t mode;
    uint8_t gpio_lvl;
    uint8_t val;
    uint8_t pwm;        /* 0=No PWM, Other=PWM frequency */
    uint8_t duty_cycle; /* If PWM, PWM duty cycle. */
};

// HCI dialog gpio_set complete event parameters - vendor specific
struct hci_gpio_set_dialog_cmd_cmp_evt
{
    uint8_t error;
};

// HCI dialog gpio_read command parameters - vendor specific
struct hci_gpio_read_dialog_cmd
{
    uint8_t gpio_pad;
};

// HCI dialog gpio_read complete event parameters - vendor specific
struct hci_gpio_read_dialog_cmd_cmp_evt
{
    uint8_t data;
};

// HCI dialog uart_loop command parameters - vendor specific
struct hci_uart_loop_dialog_cmd
{
    uint8_t data[100];
};

// HCI dialog uart loop complete event parameters - vendor specific
struct hci_uart_loop_dialog_cmd_cmp_evt
{ 
    uint8_t data[100]; 
};

// HCI dialog uart_baud command parameters - vendor specific
struct hci_uart_baud_dialog_cmd
{
    uint8_t data;
};

// HCI dialog uart_baud complete event parameters - vendor specific
struct hci_uart_baud_dialog_cmd_cmp_evt
{
    uint8_t error;
};

// HCI dialog gpio_wd command parameters - vendor specific
struct hci_gpio_wd_dialog_cmd
{
    uint8_t gpio_pad;   /* The watchdog GPIO to apply PWM. */
    uint8_t gpio_lvl;   /* 0 = 3.3V, 1 = 1.8V. */    
};

// HCI dialog gpio_wd complete event parameters - vendor specific
struct hci_gpio_wd_dialog_cmd_cmp_evt
{
    uint8_t error;      /* 0xFF=error, 0x00=OK. */
};

// HCI dialog ext32khz_test complete event parameters - vendor specific
struct hci_ext32khz_test_dialog_cmd_cmp_evt
{
    uint8_t error;      /* 0xFF=error, 0x00=OK. */
};

// HCI dialog sleep_clk_sel command parameters - vendor specific
struct hci_sleep_clk_sel_dialog_cmd
{
    uint8_t sleep_clk;   /* The sleep clock selected. 0x1=LP_CLK_XTAL32, 0x0=LP_CLK_RCX20. */
};

// HCI dialog sleep_clk_sel complete event parameters - vendor specific
struct hci_sleep_clk_sel_dialog_cmd_cmp_evt
{
    uint8_t error;      /* 0xFF=error, 0x00=OK. */
};

// HCI dialog platform_reset complete event parameters - vendor specific
struct hci_platform_reset_cmd_cmp_evt
{
    uint8_t error;
};

#if defined (__DA14531__)
// HCI dialog set_tx_power command parameters - vendor specific
struct hci_set_tx_power_dialog_cmd
{
    uint8_t data;
};

// HCI dialog set_tx_power complete event parameters - vendor specific
struct hci_set_tx_power_dialog_cmd_cmp_evt
{
    uint8_t error;
};

// HCI dialog configure_test_mode command parameters - vendor specific
struct hci_configure_test_mode_dialog_cmd
{
    uint8_t data;
};

// HCI dialog configure_test_mode complete event parameters - vendor specific
struct hci_configure_test_mode_dialog_cmd_cmp_evt
{
    uint8_t error;
};

// HCI dialog set_reset_mode command parameters - vendor specific
struct hci_set_reset_mode_dialog_cmd
{
    uint8_t reset_mode;
};

// HCI dialog set_reset_mode complete event parameters - vendor specific
struct hci_set_reset_mode_dialog_cmd_cmp_evt
{
    uint8_t error;
};
#endif

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
extern volatile uint8_t test_data_len;
// Number of Dialog handlers
extern const uint8_t dialog_commands_handler_num;
// Number of Dialog commands
extern const uint8_t dialog_commands_num;
// Dialog HCI commands
extern const struct hci_cmd_desc_tag hci_cmd_desc_tab_dialog_vs[];
// Dialog HCI commands handlers
extern const struct ke_msg_handler dialog_commands_handler_tab[];

#endif // DIALOG_COMMANDS_H_
