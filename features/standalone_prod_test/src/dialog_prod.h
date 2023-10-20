/**
 ****************************************************************************************
 *
 * @file dialog_prod.h
 *
 * @brief Dialog production header file.
 *
 * Copyright (C) 2017-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
 
#ifndef _DIALOG_PROD_H_
#define _DIALOG_PROD_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>
#include "systick.h"
#include "gpio.h"
#include "i2c.h"

#if defined (STANDALONE_MODE) || defined (REMOTE_MODE) || defined (BATCH_REMOTE_MODE)

typedef struct {
    uint16_t opcode;
    union {
        struct {
            uint8_t operation;
            uint8_t channel;
        } unmod_param;

        struct {
            uint8_t data_length;
            uint8_t payload_type;
            uint8_t channel;
        } le_tx_param;

        struct {
            uint8_t channel;
        } le_rx_param;

        struct {
            uint8_t payload_type;
            uint8_t channel;
        } cont_mod_param;

        struct {
            uint8_t sleep_mode;
        } sleep_param;
    } param;
} command_params_t;

typedef struct {
    command_params_t command_on;
    command_params_t command_off;
} standalone_tests_t;

enum {
    STANDALONE_STATE_INIT = 0,
    STANDALONE_STATE_IDLE,
    STANDALONE_STATE_STOP_TEST,
    STANDALONE_STATE_START_TEST,
#ifdef BATCH_REMOTE_MODE
	STANDALONE_STATE_EOT
#endif
};

#endif

#if defined (REMOTE_MODE)
enum 
{
	CHANNEL 		= 0x02,
	PAYLOAD_TYPE,
	OPERATION,
	DATA_LENGTH,
	SLEEP,
};
#endif

extern volatile uint8_t test_state;
extern volatile uint8_t test_freq;
extern volatile uint8_t test_data_pattern;
extern volatile uint16_t test_tx_packet_nr;
extern volatile uint16_t text_tx_nr_of_packets;
extern volatile uint16_t test_rx_irq_cnt;
extern volatile GPIO_PORT WD_port;
extern volatile GPIO_PIN WD_pin;
extern volatile uint8_t WD_volt_rail;

enum
{
    ///NO_TEST_RUNNING
    STATE_IDLE = 0x00,
    ///START_TX_TEST
    STATE_START_TX,            //1
    ///START_RX_TEST
    STATE_START_RX,            //2
    ///DIRECT_TX_TEST
    STATE_DIRECT_TX_TEST,      //3 activated via default hci command
    ///DIRECT_RX_TEST
    STATE_DIRECT_RX_TEST,      //4 activated via default hci command
    ///CONTINUE_TX
    STATE_START_CONTINUE_TX,   //5
    ///UNMODULATED_ON 
    STATE_UNMODULATED_ON,      //6
#if defined (__DA14531__)
    STATE_UNMODULATED_RX_ON,   //7
#endif
};

void user_app_on_init(void);
void otp_read(uint32_t otp_pos, uint8_t *val, uint8_t len);
int otp_write_words(uint32_t otp_pos, uint32_t *val_addr, uint8_t bytes_count);
void set_state_start_continue_tx(void);
void set_state_stop(void);
void set_state_start_tx(void);
void set_state_start_rx(void);
uint8_t check_uart_pins_cmd(uint8_t *ptr_data);
void change_uart_pins_cmd(uint8_t *ptr_data);
void sensor_spi_write_byte(uint8_t address, uint8_t wr_data, GPIO_PORT cs_port, GPIO_PIN cs_pin);
uint8_t sensor_spi_read_byte(uint8_t address, GPIO_PORT cs_port, GPIO_PIN cs_pin);
void sensor_i2c_write_byte(uint32_t address, uint8_t wr_data);
uint8_t sensor_i2c_read_byte(uint8_t address);
uint32_t find_mode_register(uint8_t port_number);
void gpio_wd_timer0_start(void);
void gpio_wd_timer0_stop(void);
#if defined (STANDALONE_MODE) || defined (REMOTE_MODE) || defined (BATCH_REMOTE_MODE)
void app_send_hci_msg(const command_params_t *param);
void app_standalone_switch_test(void);
#endif // STANDALONE_MODE || REMOTE_MODE || BATCH_REMOTE_MODE

#if defined (__DA14531__)
void start_refresh_timer(void);
void stop_refresh_timer(void);
void set_radio_ldo_configuration(bool enable);
void configure_radio_ldo(void);
void apply_radio_arp_tx_configuration(void);
uint32_t adc_531_get_vbat_sample(uint8_t vbat_type);
#endif

#endif // _DIALOG_PROD_H_
