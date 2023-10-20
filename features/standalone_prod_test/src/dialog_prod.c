/**
 ****************************************************************************************
 *
 * @file dialog_prod.c
 *
 * @brief Dialog production source code.
 *
 * Copyright (C) 2017-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

 /*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>
#include "app.h"
#include "rwip_config.h"
#include "hw_otpc.h"
#include "dialog_prod.h"
#include "user_periph_setup.h"
#include "gpio.h"
#include "llm.h"
#include "lld.h"
#include "lld_evt.h"
#include "timer0.h"
#include "timer0_2.h"
#include "spi.h"
#include "adc.h"

#if defined (STANDALONE_MODE) || defined (REMOTE_MODE) || defined (BATCH_REMOTE_MODE)
#include "dialog_commands.h"
#include "hci.h"
#endif

#ifdef STANDALONE_MODE
#include "wkupct_quadec.h"
#include "hci_int.h"
#endif

#if defined (__DA14531__)
#include "adc_531.h"
#include "rf_531.h"
#endif

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
volatile uint8_t test_state;
volatile uint16_t test_rx_irq_cnt;
volatile uint8_t test_data_len;
volatile uint16_t text_tx_nr_of_packets;
volatile uint16_t test_tx_packet_nr;
volatile GPIO_PORT WD_port;
volatile GPIO_PIN WD_pin;
volatile uint8_t WD_volt_rail;
static volatile bool wdog_enabled = false;
bool radio_cont_mode __SECTION_ZERO("retention_mem_area0");

//#define ENABLE_BLE_DIAGNOSTICS

#if defined (STANDALONE_MODE) || defined (BATCH_REMOTE_MODE)

uint8_t standalone_mode     __SECTION_ZERO("retention_mem_area0");
uint8_t standalone_state    __SECTION_ZERO("retention_mem_area0");

const standalone_tests_t standalone_tests[]  =  {
#ifdef STANDALONE_UNMOD_TX_TEST
    // Unmodulated Tx
    { .command_on =  {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_TX,  .channel = LOW_CHANNEL}  },
      .command_off = {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_OFF} }
    },
    { .command_on =  {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_TX,  .channel = MID_CHANNEL}  },
      .command_off = {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_OFF} }
    },
    { .command_on =  {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_TX,  .channel = HIGH_CHANNEL} },
      .command_off = {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_OFF} }
    },
#endif
#ifdef STANDALONE_UNMOD_RX_TEST
    // Unmodulated Rx
    { .command_on =  {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_RX,  .channel = LOW_CHANNEL}  },
      .command_off = {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_OFF} }
    },
    { .command_on =  {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_RX,  .channel = MID_CHANNEL}  },
      .command_off = {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_OFF} }
    },
    { .command_on =  {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_RX,  .channel = HIGH_CHANNEL} },
      .command_off = {.opcode = HCI_UNMODULATED_ON_CMD_OPCODE, .param.unmod_param = {.operation = UNMODULATED_CMD_MODE_OFF} }
    },
#endif
#ifdef STANDALONE_LE_TX_TEST
    // LE Tx
    { .command_on =  {.opcode = HCI_LE_TX_TEST_CMD_OPCODE, .param.le_tx_param = {.channel = LOW_CHANNEL,  .data_length = PAYLOAD_LENGTH, .payload_type = PAYLOAD_TYPE} },
      .command_off = {.opcode = HCI_LE_TEST_END_CMD_OPCODE } 
    },
    { .command_on =  {.opcode = HCI_LE_TX_TEST_CMD_OPCODE, .param.le_tx_param = {.channel = MID_CHANNEL,  .data_length = PAYLOAD_LENGTH, .payload_type = PAYLOAD_TYPE} },
      .command_off = {.opcode = HCI_LE_TEST_END_CMD_OPCODE } 
    },
    { .command_on =  {.opcode = HCI_LE_TX_TEST_CMD_OPCODE, .param.le_tx_param = {.channel = HIGH_CHANNEL, .data_length = PAYLOAD_LENGTH, .payload_type = PAYLOAD_TYPE} },
      .command_off = {.opcode = HCI_LE_TEST_END_CMD_OPCODE } 
    },
#endif
#ifdef STANDALONE_LE_RX_TEST
    // LE Rx
    { .command_on =  {.opcode = HCI_START_PROD_RX_TEST_CMD_OPCODE, .param.le_rx_param = {.channel = LOW_CHANNEL} },
      .command_off = {.opcode = HCI_END_PROD_RX_TEST_CMD_OPCODE} 
    },
    { .command_on =  {.opcode = HCI_START_PROD_RX_TEST_CMD_OPCODE, .param.le_rx_param = {.channel = MID_CHANNEL} },
      .command_off = {.opcode = HCI_END_PROD_RX_TEST_CMD_OPCODE} 
    },
    { .command_on =  {.opcode = HCI_START_PROD_RX_TEST_CMD_OPCODE, .param.le_rx_param = {.channel = HIGH_CHANNEL} },
      .command_off = {.opcode = HCI_END_PROD_RX_TEST_CMD_OPCODE} 
    },
#endif
#ifdef STANDALONE_CONT_MOD_TX_TEST
    // Continuous modulated Tx
    { .command_on =  {.opcode = HCI_TX_START_CONTINUE_TEST_CMD_OPCODE, .param.cont_mod_param = {.channel = LOW_CHANNEL,  .payload_type = PAYLOAD_TYPE} },
      .command_off = {.opcode = HCI_TX_END_CONTINUE_TEST_CMD_OPCODE} 
    },
    { .command_on =  {.opcode = HCI_TX_START_CONTINUE_TEST_CMD_OPCODE, .param.cont_mod_param = {.channel = MID_CHANNEL,  .payload_type = PAYLOAD_TYPE} },
      .command_off = {.opcode = HCI_TX_END_CONTINUE_TEST_CMD_OPCODE} 
    },
    { .command_on =  {.opcode = HCI_TX_START_CONTINUE_TEST_CMD_OPCODE, .param.cont_mod_param = {.channel = HIGH_CHANNEL, .payload_type = PAYLOAD_TYPE} },
      .command_off = {.opcode = HCI_TX_END_CONTINUE_TEST_CMD_OPCODE} 
    },
#endif
#ifdef STANDALONE_GO_TO_SLEEP_TEST
    // Sleep mode
    { .command_on =  {.opcode = HCI_SLEEP_TEST_CMD_OPCODE, .param.sleep_param = {.sleep_mode = CMD__SLEEP_MODE_EXTENDED} },
     
      .command_off = {.opcode = HCI_SLEEP_TEST_CMD_OPCODE, .param.sleep_param = {.sleep_mode = CMD__SLEEP_MODE_ACTIVE} }
    },
#endif
};

#endif // STANDALONE || BATCH_REMOTE_MODE

#if defined (STANDALONE_MODE)

static void app_enable_button(void)
{
    wkupct_enable_irq(WKUPCT_PIN_SELECT(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN), // Select pin
                      WKUPCT_PIN_POLARITY(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, GPIO_BUTTON_POLARITY), // Polarity low
                      1, 63);
}

#endif // STANDALONE_MODE

#if defined (STANDALONE_MODE) || defined (REMOTE_MODE) || defined (BATCH_REMOTE_MODE)

extern uint8_t arch_rwble_last_event;

void app_send_hci_msg(const command_params_t *param)
{
    uint8_t length;
    uint8_t task;
    
    switch(param->opcode) {
    case HCI_UNMODULATED_ON_CMD_OPCODE:
    case HCI_TX_START_CONTINUE_TEST_CMD_OPCODE:
        length = 2;
        task = TASK_DBG;
        break;
    case HCI_LE_TX_TEST_CMD_OPCODE:
        length = 3;
        task = TASK_LLM;
        arch_rwble_last_event = 0xFF;
        break;
    case HCI_START_PROD_RX_TEST_CMD_OPCODE:
        length = 1;
        task = TASK_DBG;
        arch_rwble_last_event = 0xFF;
        break;
    case HCI_END_PROD_RX_TEST_CMD_OPCODE:
    case HCI_TX_END_CONTINUE_TEST_CMD_OPCODE:
        length = 0;
        task = TASK_DBG;
        break;
    case HCI_LE_TEST_END_CMD_OPCODE:
        length = 0;
        task = TASK_LLM;
        break;
    case HCI_SLEEP_TEST_CMD_OPCODE:
        length = 3;
        task = TASK_DBG;
        break;
    }
        
    uint8_t* cmd = ke_msg_alloc(HCI_COMMAND, task, param->opcode, length);

    if(cmd != NULL) {
        switch(param->opcode) {
        case HCI_UNMODULATED_ON_CMD_OPCODE:
            cmd[0] = param->param.unmod_param.operation;
            cmd[1] = param->param.unmod_param.channel;
            break;
        case HCI_LE_TX_TEST_CMD_OPCODE:
            cmd[0] = param->param.le_tx_param.channel;
            cmd[1] = param->param.le_tx_param.data_length;
            cmd[2] = param->param.le_tx_param.payload_type;
            break;
        case HCI_TX_START_CONTINUE_TEST_CMD_OPCODE:
            cmd[0] = param->param.cont_mod_param.channel;
            cmd[1] = param->param.cont_mod_param.payload_type;
            break;
        case HCI_START_PROD_RX_TEST_CMD_OPCODE:
            cmd[0] = param->param.le_rx_param.channel;
            break;
        case HCI_LE_TEST_END_CMD_OPCODE:
        case HCI_END_PROD_RX_TEST_CMD_OPCODE:
        case HCI_TX_END_CONTINUE_TEST_CMD_OPCODE:
            break;
        case HCI_SLEEP_TEST_CMD_OPCODE:
            cmd[0] = param->param.sleep_param.sleep_mode;
            cmd[1] = 0;
            cmd[2] = 0;
            break;
        }
    }
    ke_msg_send(cmd);
}

#endif

#if defined (STANDALONE_MODE) || defined (BATCH_REMOTE_MODE)

void app_standalone_switch_test(void)
{
    switch(standalone_state) {
    case STANDALONE_STATE_INIT:
    case STANDALONE_STATE_IDLE:
        break;
    case STANDALONE_STATE_STOP_TEST:
        // Stop the previous test
        app_send_hci_msg(&standalone_tests[standalone_mode].command_off);
        standalone_mode++;
#ifdef STANDALONE_MODE
        standalone_mode %= sizeof(standalone_tests) / sizeof(standalone_tests_t);
#endif
		standalone_state = STANDALONE_STATE_START_TEST;
#ifdef BATCH_REMOTE_MODE
		if(standalone_mode == sizeof(standalone_tests) / sizeof(standalone_tests_t))
			standalone_state = STANDALONE_STATE_EOT;
#endif
        
        break;
    case STANDALONE_STATE_START_TEST:
        // Wait for BLE end event
        if (arch_rwble_last_event != 0xFF) {
            app_send_hci_msg(&standalone_tests[standalone_mode].command_on);
            standalone_state = STANDALONE_STATE_IDLE;
        }
        break;
    }
}

#endif // STANDALONE_MODE || BATCH_REMOTE_MODE

#ifdef STANDALONE_MODE

static void app_button_press_cb(void)
{
    arch_ble_force_wakeup();

    // reenable the button interrupt
    app_enable_button();
    if(standalone_state == STANDALONE_STATE_INIT) {
#ifdef STANDALONE_GO_TO_SLEEP_TEST
        arch_disable_sleep();
#endif
        standalone_state = STANDALONE_STATE_START_TEST;
    }
    else {
        standalone_state = STANDALONE_STATE_STOP_TEST;
    }
}
#endif // STANDALONE_MODE

extern void rf_reinit(void);

void user_app_on_init(void)
{
#if defined(STANDALONE_MODE) && defined(STANDALONE_GO_TO_SLEEP_TEST) && !defined(STANDALONE_START_IMMEDIATELY)
    // sleep forever
    arch_set_extended_sleep(false);
    rwip_env.ext_wakeup_enable = 2;
#else
    arch_disable_sleep();
#endif

    test_state = STATE_IDLE;
    test_data_pattern = 2;
    test_freq = 0;
    test_data_len = 37;
    text_tx_nr_of_packets = 50;
    test_tx_packet_nr = 0;
#ifdef CFG_BLE_METRICS
    metrics.rx_err = 0;
    metrics.rx_err_sync = 0;
    metrics.rx_err_crc = 0;
    metrics.rx_pkt = 0;
    metrics.rx_rssi = 0;
#endif
    SetBits32(BLE_CNTL2_REG, BLE_RSSI_SEL, 1);      //SELECT CURRENT AVERAGE RSSI.

#if !defined (__DA14531__)
    init_TXEN_RXEN_irqs();
#else
    // Apply trim value immediately when CLK_FREQ_TRIM_REG is written
    SetBits16(TRIM_CTRL_REG, XTAL_TRIM_SELECT, 2);
#endif

#if defined (__DA14531__) && defined (ENABLE_BLE_DIAGNOSTICS)
    SetBits32(BLE_DIAGCNTL_REG , DIAG3, 0x3b);  // diag3 gpio P0_03/P0_11
    SetBits32(BLE_DIAGCNTL3_REG, DIAG3_BIT, 6); // tx_en rx_en ARP output
    SetBits32(BLE_DIAGCNTL_REG , DIAG3_EN, 1);
    SetWord16(P011_MODE_REG,0x300+18);
#endif

#ifdef STANDALONE_MODE
    #ifdef STANDALONE_START_IMMEDIATELY
    standalone_state = STANDALONE_STATE_START_TEST;
    #endif
    wkupct_register_callback(app_button_press_cb);
    app_enable_button();
#endif

#if defined (__DA14531__)
    rf_pa_pwr_set(RADIO_INIT_TX_POWER);
    rf_reinit();
#endif

#if defined (BATCH_REMOTE_MODE) || defined(REMOTE_MODE)
	default_app_on_init();
#endif

}

int otp_write_words(uint32_t otp_pos, uint32_t *val_addr, uint8_t words_count)
{
#if defined (__DA14531__)
    bool result;

    for (uint16_t i = 0; i < words_count; i++)
    {
        hw_otpc_init();
        result = hw_otpc_prog_and_verify(val_addr + i, otp_pos >> 2, 1);
        hw_otpc_disable();

        otp_pos += HW_OTP_CELL_SIZE;

        if (result == false)
        {
            break;
        }
    }
#else
    int result;
    uint32_t otp_addr;

    for(uint16_t i = 0; i < words_count; i += 2)
    {
        otp_addr = otp_pos;
        //Initialize OTP Controller
        hw_otpc_init();
        //write the OTP
        result = hw_otpc_fifo_prog((const uint32_t *) (val_addr), otp_addr >> 3, HW_OTPC_WORD_LOW, 2, false) ? 0 : 1;

        hw_otpc_disable();
        otp_pos += 8;

        if (result != 0)
            break;
    }

    //Close the OTP Controller
    hw_otpc_close();
#endif // defined (__DA14531__)

    return 0;
}

void otp_read(uint32_t otp_pos, uint8_t *val, uint8_t len)
{
    uint8_t *otp_val = (uint8_t *)otp_pos;   //where in OTP header to read

    //Initialize OTP Controller
    hw_otpc_init();
    // set OTP in read mode

#if !defined (__DA14531__)
    hw_otpc_manual_read_on(false);
#else
    hw_otpc_enter_mode(HW_OTPC_MODE_READ);
#endif

    memcpy(val, otp_val, len);

    hw_otpc_disable();
}

void set_state_start_continue_tx(void)
{
    if(test_state == STATE_IDLE)
    {
        struct hci_le_tx_test_cmd  tx_con_test;
        tx_con_test.tx_freq = test_freq;
        tx_con_test.test_data_len = 37; //select a valid value.
        tx_con_test.pk_payload_type = test_data_pattern;

#if defined (__DA14531__)
        configure_radio_ldo();
#endif
        
        llm_test_mode_start_tx( (struct hci_le_tx_test_cmd const *)&tx_con_test);

        SetBits32(BLE_RFTESTCNTL_REG,INFINITETX,1);
        SetBits32(BLE_RFTESTCNTL_REG,TXLENGTHSRC,0);
        SetBits32(BLE_RFTESTCNTL_REG,TXPLDSRC,0);
        SetBits32(BLE_RFTESTCNTL_REG,TXLENGTH,0);
    
        test_state = STATE_START_CONTINUE_TX;
        NVIC_DisableIRQ(BLE_RF_DIAG_IRQn);
        
#if defined (__DA14531__)
        start_refresh_timer();
#endif
    }
}

void set_state_stop(void)
{
#if !defined (__DA14531__)
    if( (test_state==STATE_START_TX) || (test_state==STATE_START_RX) || (test_state==STATE_START_CONTINUE_TX) )// in case of default
                                                                                                             //direct TX or RX stack handles the end of these tasks
#else
    if( (test_state==STATE_START_TX) || (test_state==STATE_START_RX) || 
         (test_state==STATE_START_CONTINUE_TX) || (test_state==STATE_UNMODULATED_RX_ON) )// in case of default
#endif
    {

         lld_test_stop(llm_le_env.elt);
         // Set the state to stopping
         ke_state_set(TASK_LLM, LLM_IDLE);
    }
    test_state = STATE_IDLE;
}


void set_state_start_tx(void)
{
    if(test_state==STATE_IDLE)
    {
        test_tx_packet_nr=0;
        struct hci_le_tx_test_cmd  tx_test;
        tx_test.tx_freq = test_freq;
        tx_test.test_data_len = test_data_len; //37;
        tx_test.pk_payload_type = test_data_pattern;

        llm_test_mode_start_tx( (struct hci_le_tx_test_cmd const *)&tx_test);
        while(llm_le_env.elt == NULL) 
            if(llm_test_mode_start_tx( (struct hci_le_tx_test_cmd const *)&tx_test) != CO_ERROR_NO_ERROR)
                while(1);

        test_state=STATE_START_TX;
    }
}

void set_state_start_rx(void)
{
    if(test_state==STATE_IDLE)
    {
#ifdef CFG_BLE_METRICS
        metrics.rx_err = 0;
        metrics.rx_err_sync = 0;
        metrics.rx_err_crc = 0;
        metrics.rx_pkt = 0;
        metrics.rx_rssi = 0;
#endif
        struct hci_le_rx_test_cmd  rx_test;
        rx_test.rx_freq = test_freq;
        
        llm_test_mode_start_rx( (struct hci_le_rx_test_cmd const *)&rx_test);
        test_state=STATE_START_RX;
    }
}

uint8_t check_uart_pins_cmd(uint8_t *ptr_data)
{
    uint8_t result = 0;

    if(!GPIO_is_valid((GPIO_PORT)ptr_data[0], (GPIO_PIN)ptr_data[1]))
        result = 1;

    if(!GPIO_is_valid((GPIO_PORT)ptr_data[2], (GPIO_PIN)ptr_data[3]))
        result = 1;

    return result;
}

void change_uart_pins_cmd(uint8_t *ptr_data)
{
    for (volatile int i=0; i<2000; i++){}
    // Return previous UART GPIOs to default settings
    GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_port_tx, (GPIO_PIN) uart_sel_pins.uart_tx_pin, INPUT, PID_GPIO, false);
    GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_port_rx, (GPIO_PIN) uart_sel_pins.uart_rx_pin, INPUT, PID_GPIO, false);
    #ifndef GPIO_DRV_PIN_ALLOC_MON_DISABLED
        RESERVE_GPIO(UART1_TX, (GPIO_PORT) ptr_data[0], (GPIO_PIN) ptr_data[1], PID_UART1_TX);
        RESERVE_GPIO(UART1_TX, (GPIO_PORT) ptr_data[2], (GPIO_PIN) ptr_data[3], PID_UART1_RX);
    #endif // GPIO_DRV_PIN_ALLOC_MON_DISABLED
    update_uart_pads ((GPIO_PORT) ptr_data[0],(GPIO_PIN) ptr_data[1],(GPIO_PORT) ptr_data[2], (GPIO_PIN) ptr_data[3]);
    set_pad_uart();
}

uint8_t sensor_i2c_read_byte(uint8_t address)
{
    while(!i2c_is_tx_fifo_empty());                 // Wait in case Tx FIFO has data.
    i2c_write_byte(address);                        // Write the address of the register.
    while(!i2c_is_tx_fifo_empty());                 // Wait in case Tx FIFO has data.
#if defined (__DA14531__)
    i2c_write_byte(0x0100 | I2C_STOP);              // Set R/W bit to 1 (read access).
#else
    i2c_write_byte(0x0100);                         // Set R/W bit to 1 (read access).
#endif
    while(!i2c_get_rx_fifo_level());                // Wait for the received byte.

    return (0xFF & i2c_read_byte());                // Get the received byte.
}

void sensor_i2c_write_byte(uint32_t address, uint8_t wr_data)
{
    while(!i2c_is_tx_fifo_empty());                 // Wait in case Tx FIFO has data.
    i2c_write_byte(address);                        // Write the address of the register.
#if defined (__DA14531__)
    i2c_write_byte((wr_data & 0xFF) | I2C_STOP);    // Write the data of the register.
#else
    i2c_write_byte(wr_data & 0xFF);                 // Write the data of the register.
#endif
    while(!i2c_is_tx_fifo_empty());                 // Wait in case Tx FIFO has data.

    while(i2c_is_master_busy());                    // Wait until no master activity
}

void sensor_spi_write_byte(uint8_t address, uint8_t wr_data, GPIO_PORT cs_port, GPIO_PIN cs_pin)
{
    // CS low
    spi_cs_low();
    // Address
    spi_access((uint16_t) address);
    // Data
    spi_access((uint16_t) wr_data);
    // CS high
    spi_cs_high();
}

uint8_t sensor_spi_read_byte(uint8_t address, GPIO_PORT cs_port, GPIO_PIN cs_pin)
{
    uint8_t result = 0;

    // CS low
    spi_cs_low();
    // Address
    spi_access((uint16_t) (address | 0x80));
    // Read data
    result = (uint8_t) spi_access(0x0000);
    // CS high
    spi_cs_high();

    return result;
}

uint32_t find_mode_register(uint8_t port_number)
{
    uint32_t port_reg = 0;

    // Get the register of the port
    switch (port_number)
    {
        case  0:    port_reg = P00_MODE_REG;  break;
        case  1:    port_reg = P01_MODE_REG;  break;
        case  2:    port_reg = P02_MODE_REG;  break;
        case  3:    port_reg = P03_MODE_REG;  break;
        case  4:    port_reg = P04_MODE_REG;  break;
        case  5:    port_reg = P05_MODE_REG;  break;
        case  6:    port_reg = P06_MODE_REG;  break;
        case  7:    port_reg = P07_MODE_REG;  break;
#if defined (__DA14531__)
        case  8:    port_reg = P08_MODE_REG;  break;
        case  9:    port_reg = P09_MODE_REG;  break;
        case 10:    port_reg = P010_MODE_REG; break;
        case 11:    port_reg = P011_MODE_REG; break;
#else
        case 10:    port_reg = P10_MODE_REG;  break;
        case 11:    port_reg = P11_MODE_REG;  break;
        case 12:    port_reg = P12_MODE_REG;  break;
        case 13:    port_reg = P13_MODE_REG;  break;
        case 14:    port_reg = P14_MODE_REG;  break;
        case 15:    port_reg = P15_MODE_REG;  break;
        case 20:    port_reg = P20_MODE_REG;  break;
        case 21:    port_reg = P21_MODE_REG;  break;
        case 22:    port_reg = P22_MODE_REG;  break;
        case 23:    port_reg = P23_MODE_REG;  break;
        case 24:    port_reg = P24_MODE_REG;  break;
        case 25:    port_reg = P25_MODE_REG;  break;
        case 26:    port_reg = P26_MODE_REG;  break;
        case 27:    port_reg = P27_MODE_REG;  break;
        case 28:    port_reg = P28_MODE_REG;  break;
        case 29:    port_reg = P29_MODE_REG;  break;
        case 30:    port_reg = P30_MODE_REG;  break;
        case 31:    port_reg = P31_MODE_REG;  break;
        case 32:    port_reg = P32_MODE_REG;  break;
        case 33:    port_reg = P33_MODE_REG;  break;
        case 34:    port_reg = P34_MODE_REG;  break;
        case 35:    port_reg = P35_MODE_REG;  break;
        case 36:    port_reg = P36_MODE_REG;  break;
        case 37:    port_reg = P37_MODE_REG;  break;
#endif
    }
    
    return(port_reg);
}

#if defined (__DA14531__)
void ble_tx_evt_handler()
{
    test_tx_packet_nr++;
}

void ble_rx_evt_handler()
{
    test_rx_irq_cnt++;
}
#endif

void gpio_wd_timer0_start(void)
{
    wdog_enabled = true;

    tim0_2_clk_div_config_t tim0_2_clk_div_config = {.clk_div = TIM0_2_CLK_DIV_8};

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

#if defined (__DA14531__)

static void timer_cb(void)
{
    // Turn off Rx and Tx. 
    SetWord32(RF_OVERRULE_REG, 5);
    SetByte(RF_RADIO_INIT_REG, (ADPLLDIG_PWR_SW1_EN ));  //RADIO_LDO_EN -> 0
    for(volatile uint8_t i = 0; i < 4 * 20; i++);
    
    rf_adplldig_activate();
    switch (test_state) {
    case STATE_UNMODULATED_ON:
        SetWord32(RF_OVERRULE_REG, 3);
        break;
    case STATE_START_CONTINUE_TX:
    case STATE_UNMODULATED_RX_ON:
        SetWord32(RF_OVERRULE_REG, 0);
        break;
    }
}

void start_refresh_timer(void)
{
    if (radio_cont_mode == false) {
        static tim0_2_clk_div_config_t clk_div_config =
        {
            .clk_div  = TIM0_2_CLK_DIV_8
        };
        
        // Enable the Timer0/Timer2 input clock
        timer0_2_clk_enable();

        // Set the Timer0/Timer2 input clock division factor to 8, so 16 MHz / 8 = 2 MHz input clock
        timer0_2_clk_div_set(&clk_div_config);

        timer0_init(TIM0_CLK_FAST, PWM_MODE_ONE, TIM0_CLK_NO_DIV);

        // set pwm Timer0 'On', Timer0 'high' and Timer0 'low' reload values
        timer0_set(20000, 5000, 2000); // 10msec

        timer0_register_callback(timer_cb);
        timer0_enable_irq();
        timer0_start();
    }
}

void stop_refresh_timer(void)
{
    timer0_disable_irq();
    timer0_stop();

    // Disable the Timer0/Timer2 input clock
    timer0_2_clk_disable();
}

void configure_radio_ldo(void)
{
    uint32_t val = GetWord32(RF_LDO_CTRL_REG);

    if (radio_cont_mode) {
        val |= LDO_RADIO_CONT_ENABLE | LDO_DTC_CONT_ENABLE | LDO_DCO_CONT_ENABLE;
        val &= ~(LDO_DCO_HOLD_OVR_VAL | LDO_DTC_HOLD_OVR_VAL | LDO_RADIO_HOLD_OVR_VAL);
        val |=   LDO_DCO_HOLD_OVR_EN  | LDO_DTC_HOLD_OVR_EN  | LDO_RADIO_HOLD_OVR_EN;
    } else {
        val &= ~(LDO_RADIO_CONT_ENABLE | LDO_DTC_CONT_ENABLE | LDO_DCO_CONT_ENABLE);
        val &= ~(LDO_DCO_HOLD_OVR_EN  | LDO_DTC_HOLD_OVR_EN  | LDO_RADIO_HOLD_OVR_EN);
    }
    SetWord32(RF_LDO_CTRL_REG, val);
}

void set_radio_ldo_configuration(bool enable)
{
    radio_cont_mode = enable;
    
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x0) {
        // Set DC/DC level to 2.5V or 1.1V
        SetBits16(POWER_LEVEL_REG, DCDC_LEVEL, enable ? 2 : 0);
    }
}

// definitions
/*
 * ARP - Automatic Radio Programming
 */
#define ARPT_PWR_ENTRIES    (1)
#define ARPT_PREF_ENTRIES   (9)
#define ARPT_TX_ENTRIES     (7)
#define ARPT_RX_ENTRIES     (8)


typedef struct {
    unsigned data      : 16;
    unsigned addr      : 14;
    unsigned data8or16n: 1;    // 1:8bit data, 0:16bit data
    unsigned valid     : 1;    // 1: valid, 0:invalid
} bf_t;

/*
 * ARP table entry definition
 */
typedef union {
    uint32_t value;
    bf_t bf;
} __attribute((aligned(4))) t_arp_reg;

/*
 * ARP Table structure
 */
struct s_arp_table {
  // ARPT_PWR
  t_arp_reg  arpt_pwr[ARPT_PWR_ENTRIES];
  uint32_t   null1;

  // ARPT_PREF
  uint16_t   tx_tbase;
  uint16_t   rx_tbase;
  t_arp_reg  arpt_pref[ARPT_PREF_ENTRIES];
  uint32_t   null2;

  // ARPT_TX
  t_arp_reg  tx_en_delay;
  t_arp_reg  arpt_tx[ARPT_TX_ENTRIES];
  uint32_t   null3;

  // ARPT_RX
  t_arp_reg  rx_en_delay;
  t_arp_reg  arpt_rx[ARPT_RX_ENTRIES];
  uint32_t   null4;
  } __attribute((aligned(4)));

extern volatile struct s_arp_table *arp_table;

static bool apply_arp_entry(volatile bf_t *reg)
{
    if (reg->valid == 0) {
        return false;
    }
    if (reg->data8or16n == 1) {
        SetByte(0x40000000 + reg->addr, reg->data);
    } else {
        SetWord16(0x40000000 + reg->addr, reg->data);
    }
    return true;
}

void apply_radio_arp_tx_configuration(void)
{
    apply_arp_entry(&arp_table->arpt_pwr[0].bf);
    
    for (int i = 0; i < ARPT_PREF_ENTRIES; i++) {
        if (apply_arp_entry(&arp_table->arpt_pref[i].bf) == false) {
            break;
        }
    }

    for (int i = 0; i < ARPT_TX_ENTRIES; i++) {
        if (apply_arp_entry(&arp_table->arpt_tx[i].bf) == false) {
            break;
        }
    }
}

static void usDelay(uint32_t nof_us)
{
    while ( nof_us-- )
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
}

uint32_t adc_531_get_vbat_sample(uint8_t vbat_type)
{
    uint32_t adc_sample;

    adc_config_t cfg = {
        .input_mode = ADC_INPUT_MODE_SINGLE_ENDED,
        .input = ADC_INPUT_SE_VBAT_HIGH,
        .smpl_time_mult = 10,
        .continuous = false,
        .interval_mult = 0,
        .input_attenuator = ADC_INPUT_ATTN_4X,
        .chopping = true,
        .oversampling = 1
    };

    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
    {
        // Boost mode.
        cfg.input = vbat_type ? ADC_INPUT_SE_VBAT_HIGH : ADC_INPUT_SE_VBAT_LOW;
    }
    else
    {
        cfg.input = vbat_type ? ADC_INPUT_SE_VBAT_LOW : ADC_INPUT_SE_VBAT_HIGH;
    }

    adc_init(&cfg);

    usDelay(40);

    adc_sample = adc_correct_sample(adc_get_sample());

    adc_disable();

    return adc_sample;
}

#endif // __DA14531__
