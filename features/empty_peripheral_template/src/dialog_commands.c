/**
 ****************************************************************************************
 *
 * @file dialog_commands.c
 *
 * @brief dialog vendor specific command handlers project source code.
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

#include "stdio.h"
#include "dialog_commands.h"
#include "hci_int.h"
#include "hci.h"
#include "co_hci.h"
#include "rwip.h"
#include "sdk_version.h"
#include "user_config_sw_ver.h"
#include "dialog_prod.h"
#ifndef __DA14531__
#include "rdtest_api.h"
#include "adc.h"
#endif
#include "user_periph_setup.h"
#include "otp_hdr.h"
#include "system_library.h"
#include "em_map.h"
#include "h4tl.h"
#include "uart.h"
#if defined (__DA14531__)
#include "Xtal_TRIM.h"
#endif
#include "llm.h"
#include "timer0.h"
#include "timer2.h"
#include "timer0_2.h"
#include "arch.h"
#include "arch_system.h"
#include "spi.h"
#include "i2c.h"

#if defined (__DA14531__)
#include "rf_531.h"
#endif

/*
 * DEFINES
 ****************************************************************************************
 */

// Value to generate 1s timeout when checking for a rising edge on the input pulse
#define PULSE_TIMEOUT    0x8CB6

// Maximum number of words that can be read or written by a command at once
#define MAX_READ_WRITE_OTP_WORDS 60

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

volatile uint8_t test_data_pattern;
volatile uint8_t test_freq;

/// Configuration struct for SPI bus.
static spi_cfg_t spi_cfg = {
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
        .port = GPIO_PORT_0,
        .pin = GPIO_PIN_1,
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

/// Configuration struct for I2C bus.
static i2c_cfg_t i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = I2C_SPEED_FAST,
    .mode = I2C_MODE_MASTER,
    .addr_mode = I2C_ADDRESSING_7B,
    .address = 0x00,
    .tx_fifo_level = 1,
    .rx_fifo_level = 1,
};

/*
 * LOCAL FUNCTIONS DECLARATIONS
 ****************************************************************************************
 */
extern uint32_t lp_clk_sel;
extern void uart_finish_transfers_func(void);
static uint8_t hci_otp_rd_data_cmd_cmp_evt_pk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
static uint8_t hci_wr_otp_cmd_upk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len);
static uint8_t hci_pack_bytes(uint8_t** pp_in, uint8_t** pp_out, uint8_t* p_in_end, uint8_t* p_out_end, uint8_t len);

// HCI dialog command descriptors (OGF Vendor Specific)
const struct hci_cmd_desc_tag hci_cmd_desc_tab_dialog_vs[] =
{
    CMD(CUSTOM_ACTION            , DBG, 0, PK_GEN_GEN, "B"                    , "L"                            ),
    CMD(SLEEP_TEST               , DBG, 0, PK_GEN_GEN, "BBB"                  , NULL                           ),
    CMD(XTAL_TRIM                , DBG, 0, PK_GEN_GEN, "3B"                   , "H"                            ),
    CMD(OTP_RW                   , DBG, 0, PK_GEN_GEN, "B6B"                  , "B6B"                          ),
    CMD(OTP_READ                 , DBG, 0, PK_GEN_SPE, "HB"                   , &hci_otp_rd_data_cmd_cmp_evt_pk),
    CMD(OTP_WRITE                , DBG, 0, PK_SPE_GEN, &hci_wr_otp_cmd_upk    , "BB"                           ),
    CMD(REGISTER_RW              , DBG, 0, PK_GEN_GEN, "BLL"                  , "BBL"                          ),
    CMD(AUDIO_TEST               , DBG, 0, PK_GEN_GEN, NULL                   , NULL                           ),
    CMD(FIRMWARE_VERSION_GET     , DBG, 0, PK_GEN_GEN, NULL                   , "66B"                          ),
    CMD(CHANGE_UART_PINS_ACTION  , DBG, 0, PK_GEN_GEN, "4B"                   , "H"                            ),
    CMD(RDTESTER                 , DBG, 0, PK_GEN_GEN, "BH"                   , NULL                           ),
    CMD(TX_TEST                  , DBG, 0, PK_GEN_GEN, "BBBBB"                , NULL                           ),
    CMD(START_PROD_RX_TEST       , DBG, 0, PK_GEN_GEN, "B"                    , NULL                           ),
    CMD(END_PROD_RX_TEST         , DBG, 0, PK_GEN_GEN, NULL                   , "HHHH"                         ),
    CMD(UNMODULATED_ON           , DBG, 0, PK_GEN_GEN, "BB"                   , NULL                           ),
    CMD(TX_START_CONTINUE_TEST   , DBG, 0, PK_GEN_GEN, "BB"                   , NULL                           ),
    CMD(TX_END_CONTINUE_TEST     , DBG, 0, PK_GEN_GEN, NULL                   , NULL                           ),
    CMD(SENSOR_TEST              , DBG, 0, PK_GEN_GEN, "17B"                  , "H"                            ),
    CMD(GPIO_SET                 , DBG, 0, PK_GEN_GEN, "6B"                   , "B"                            ),
    CMD(GPIO_READ                , DBG, 0, PK_GEN_GEN, "B"                    , "B"                            ),
    CMD(UART_LOOP                , DBG, 0, PK_GEN_GEN, "100B"                 , "100B"                         ),
    CMD(UART_BAUD                , DBG, 0, PK_GEN_GEN, "B"                    , "B"                            ),
    CMD(EXT32KHz_TEST            , DBG, 0, PK_GEN_GEN, NULL                   , "B"                            ),
    CMD(GPIO_WD                  , DBG, 0, PK_GEN_GEN, "2B"                   , "B"                            ),
    CMD(SLEEP_CLK_SEL            , DBG, 0, PK_GEN_GEN, "B"                    , "B"                            ),
    CMD(ADC_VBAT                 , DBG, 0, PK_GEN_GEN, "B"                    , "H"                            ),
#if defined (__DA14531__)
    CMD(SET_TX_POWER             , DBG, 0, PK_GEN_GEN, "B"                    , "B"                            ),
    CMD(CONFIGURE_TEST_MODE      , DBG, 0, PK_GEN_GEN, "B"                    , "B"                            ),
    CMD(RESET_MODE               , DBG, 0, PK_GEN_GEN, "B"                    , "B"                            ),
#endif
    CMD(PLATFORM_RESET           , DBG, 0, PK_GEN_GEN, "NULL"                 , "B"                            ),
};

const uint8_t dialog_commands_num = ARRAY_LEN (hci_cmd_desc_tab_dialog_vs);

/**
 ****************************************************************************************
 * @brief Handles the reception of the ADC read VBAT value command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_adc_vbat_handler(ke_msg_id_t const msgid,
                                                 struct hci_adc_vbat_dialog_cmd const *param,
                                                 ke_task_id_t const dest_id,
                                                 ke_task_id_t const src_id)
{
    // Structure type for the complete command event.
    struct hci_adc_vbat_dialog_cmd_cmp_evt *event =
        KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, src_id, HCI_ADC_VBAT_CMD_OPCODE, hci_adc_vbat_dialog_cmd_cmp_evt);

#if defined (__DA14531__)
    event->vbat_lvl = (uint16_t) adc_531_get_vbat_sample(param->vbat_type);
#else
    adc_offset_calibrate(ADC_INPUT_MODE_SINGLE_ENDED);
    if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
    {
        // Boost mode
        event->vbat_lvl = (uint16_t) adc_get_vbat_sample(true);
    }
    else
    {
        // Buck mode
        event->vbat_lvl = (uint16_t) adc_get_vbat_sample(false);
    }
#endif

    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the custom_action dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_custom_action_handler(ke_msg_id_t const msgid,
                                                 struct hci_custom_action_dialog_cmd const *param,
                                                 ke_task_id_t const dest_id,
                                                 ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_custom_action_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_CUSTOM_ACTION_CMD_OPCODE, hci_custom_action_dialog_cmd_cmp_evt);

    switch (param->inputData & 0x00FF)
    {
        /* Command 0x5F is used for UART resync test. */
        case 0x5F:
            event->returnData = param->inputData & 0x00FF;
            break;
        case 0x33:
            event->returnData = GetWord16(CLK_FREQ_TRIM_REG);
            break;
        /* Users can add their own commands and replies accordingly. */
        default:
            event->returnData = param->inputData & 0x00FF;
            break;
    }

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the audio_test dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_audio_test_handler(ke_msg_id_t const msgid,
                                              void const *param,
                                              ke_task_id_t const dest_id,
                                              ke_task_id_t const src_id)
{
    // structure type for the complete command event
    void *event = ke_msg_alloc(HCI_CMD_CMP_EVENT , src_id, HCI_AUDIO_TEST_CMD_OPCODE, 0);

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the firmware_version dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_firmware_version_get_handler(ke_msg_id_t const msgid,
                                                        void const *param,
                                                        ke_task_id_t const dest_id,
                                                        ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_firmware_version_get_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_FIRMWARE_VERSION_GET_CMD_OPCODE, hci_firmware_version_get_dialog_cmd_cmp_evt);

    //adding return values
    event->ble_ver_len = sizeof(SDK_VERSION);
    strcpy(event->ble_ver, SDK_VERSION);

    event->app_ver_len = sizeof(DA14585_REFDES_SW_VERSION);
    strcpy(event->app_ver, DA14585_REFDES_SW_VERSION);

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the change_uart_pins_action dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_change_uart_pins_action_handler(ke_msg_id_t const msgid,
                                                           struct hci_change_uart_pins_action_dialog_cmd const *param,
                                                           ke_task_id_t const dest_id,
                                                           ke_task_id_t const src_id)
{
    // Structure type for the complete command event
    struct hci_change_uart_pins_action_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_CHANGE_UART_PINS_ACTION_CMD_OPCODE, hci_change_uart_pins_action_dialog_cmd_cmp_evt);
    uint8_t data[4];
    data[0] = param->tx_port;
    data[1] = param->tx_pin;
    data[2] = param->rx_port;
    data[3] = param->rx_pin;
    // Check if new pins are valid
    event->status = check_uart_pins_cmd(data);
    // Send completion event
    hci_send_2_host(event);

    // Change to new pins if they are valid
    if (!event->status)
    {
        change_uart_pins_cmd(data);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the register_rw dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_register_rw_handler(ke_msg_id_t const msgid,
                                               struct hci_register_rw_dialog_cmd const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_register_rw_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_REGISTER_RW_CMD_OPCODE, hci_register_rw_dialog_cmd_cmp_evt);

    volatile void * reg_addr = 0;

    event->operation = param->operation;
    event->reserved = 0;

    reg_addr = (volatile void *)(param->addr);
    switch(param->operation)
    {
    case CMD__REGISTER_RW_OP_READ_REG32:
        event->data = GetWord32(reg_addr);
        break;
    case CMD__REGISTER_RW_OP_WRITE_REG32:
        SetWord32(reg_addr, param->data );
        break;
    case CMD__REGISTER_RW_OP_READ_REG16:
        event->data = GetWord16(reg_addr);
        break;
    case CMD__REGISTER_RW_OP_WRITE_REG16:
        SetWord16(reg_addr, param->data );
        break;
    default:
        break;
    }

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the tx_start_continue_test dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_tx_start_continue_test_handler(ke_msg_id_t const msgid,
                                                          struct hci_tx_start_continue_test_dialog_cmd const *param,
                                                          ke_task_id_t const dest_id,
                                                          ke_task_id_t const src_id)
{
    // structure type for the complete command event
    void *event = ke_msg_alloc(HCI_CMD_CMP_EVENT , src_id, HCI_TX_START_CONTINUE_TEST_CMD_OPCODE, 0);

    test_freq = param->frequency;
    test_data_pattern = param->payload_type;
    set_state_start_continue_tx();

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the otp_rw dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_otp_rw_handler(ke_msg_id_t const msgid,
                                          struct hci_otp_rw_dialog_cmd const *param,
                                          ke_task_id_t const dest_id,
                                          ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_otp_rw_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_OTP_RW_CMD_OPCODE, hci_otp_rw_dialog_cmd_cmp_evt);
    uint32_t word[2];

    memset(word, 0, sizeof(word));
    event->operation = param->operation;

    switch(param->operation)
    {
#if !defined (__DA14531__)
    case CMD__OTP_OP_RD_XTRIM:
        otp_read(OTP_HDR_TRIM_XTAL16M_ADDR, event->data , 2);
        break;

    case CMD__OTP_OP_WR_XTRIM:
        word[0] = param->data[0] | param->data[1] << 8 ;
        otp_write_words(OTP_HDR_TRIM_XTAL16M_OFFSET, word , 1);
        break;
#else
//DO NOTHING IN 531
#endif //!defined (__DA14531__)
        
    case CMD__OTP_OP_RD_BDADDR:
        otp_read(OTP_HDR_BDADDR_ADDR, event->data , 6);
        break;
    case CMD__OTP_OP_WR_BDADDR:
        word[0] = ((param->data[3] << 24) | (param->data[2] << 16) | (param->data[1] << 8) | (param->data[0]));
        word[1] = ( param->data[5] << 8 | param->data[4] );
#if defined (__DA14531__)
        otp_write_words(OTP_HDR_BDADDR_OFFSET, word , 2);
#else
        otp_write_words(OTP_HDR_BDADDR_OFFSET, word , 1);
#endif
        break;
    case CMD__OTP_OP_RE_XTRIM:
        //DO NOTHING IN 585
        break;

    case CMD__OTP_OP_WE_XTRIM:
        //DO NOTHING IN 585
        break;

    default:
        break;
    }

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the xtal_trim dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_xtal_trim_handler(ke_msg_id_t const msgid,
                                             struct hci_xtal_trim_dialog_cmd const *param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    // Structure type for the complete command event
    struct hci_xtal_trim_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_XTAL_TRIM_CMD_OPCODE, hci_xtal_trim_dialog_cmd_cmp_evt);
    uint16_t reg_value;
    uint16_t delta = param->trim_MSB << 8|param->trim_LSB;
    uint8_t xtal_cal_status = 0;
    int value;
    uint16_t gpio_prev_mode = 0x0000;
    uint32_t port_reg;
    uint8_t gpio_sel;
    GPIO_PORT gpio_port;
    GPIO_PIN gpio_pin;
#if !defined (__DA14531__)
    int auto_trim(uint8_t port_number);
#endif

    switch(param->operation)
    {
    case CMD__XTRIM_OP_RD:
        event->trim_value = GetWord16(CLK_FREQ_TRIM_REG);
        break;
    case CMD__XTRIM_OP_WR:
        SetWord16(CLK_FREQ_TRIM_REG, delta);
        event->trim_value = 0x00;
        break;
    case CMD__XTRIM_OP_EN:
#if !defined (__DA14531__)
        SetWord32(TEST_CTRL_REG, 1);
        SetWord16(P05_MODE_REG, OUTPUT | PID_GPIO);
#else
        if (GetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE) == 1) {
            // Debugger must be disabled before P05 is used as clock output.
            while (GetBits16(SYS_STAT_REG, DBG_IS_UP) != 0);
            SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, 0);
        }

        SetWord16(P05_MODE_REG, OUTPUT | PID_SYS_CLK);
#endif
        event->trim_value = 0x00;
        break;
    case CMD__XTRIM_OP_INC:
        reg_value = GetWord16(CLK_FREQ_TRIM_REG);
#if !defined (__DA14531__)
        if (reg_value <= 0xFFFF - delta)
#else
        if (reg_value <= 0xFF - delta)
#endif
        {
            SetWord16(CLK_FREQ_TRIM_REG, reg_value + delta);
        }
        event->trim_value = 0x00;
        break;
    case CMD__XTRIM_OP_DEC:
        reg_value = GetWord16(CLK_FREQ_TRIM_REG);
        if (reg_value >= delta)
        {
            SetWord16(CLK_FREQ_TRIM_REG, reg_value - delta);
        }
        event->trim_value = 0x00;
        break;
    case CMD__XTRIM_OP_DIS:
#if !defined (__DA14531__)
        SetWord32(TEST_CTRL_REG, 0);
#endif
        SetWord16(P05_MODE_REG, INPUT_PULLDOWN | PID_GPIO);
        event->trim_value = 0x00;
        break;
    case CMD__XTRIM_OP_CALTEST:
    case CMD__XTRIM_OP_CAL:
    {
        /* Check for pulse in Rx option.*/
        if (param->trim_LSB == 0xFE)
        {
            bool rx_pin_found = false;

            gpio_port = GPIO_PORT_0;
            gpio_pin = GPIO_PIN_0;

#if defined(__DA14531__)
    const GPIO_PORT MAX_GPIO_PORT = GPIO_PORT_0;
    const GPIO_PIN MAX_GPIO_PIN[] = {GPIO_PIN_11};
#else
    const GPIO_PORT MAX_GPIO_PORT = GPIO_PORT_3;
    const GPIO_PIN MAX_GPIO_PIN[] = {GPIO_PIN_7, GPIO_PIN_5, GPIO_PIN_9, GPIO_PIN_7};
#endif

            // initialize in case the GPIO is not found.
            gpio_sel = 0xFF;

            // Find the UART2 RX pin
            while (!rx_pin_found && ((gpio_port <= MAX_GPIO_PORT) && (gpio_pin <= MAX_GPIO_PIN[gpio_port])))
            {
                gpio_sel = gpio_port*10 + gpio_pin; // Applies for all IC...
                port_reg = find_mode_register(gpio_sel);
                rx_pin_found = ((GetWord16(port_reg) & 0x0F) == FUNC_UART_RX);

                if (rx_pin_found)
                {
                    // GPIO found.
                    break;
                }

#if !defined (__DA14531__)
                // Change port...
                if (gpio_pin == MAX_GPIO_PIN[gpio_port])
                {
                    gpio_port++;
                    gpio_pin = GPIO_PIN_0;
                    continue;
                }
#endif
                gpio_pin++;
            }

            if (!rx_pin_found)
            {
                gpio_sel = 0xFF;
            }
        }
        else
        {
            gpio_sel = param->trim_LSB;
#if defined (__DA14531__)
            gpio_port = GPIO_PORT_0;
            gpio_pin = (GPIO_PIN)gpio_sel;
#else
            gpio_port = (GPIO_PORT)(gpio_sel/10);
            gpio_pin = (GPIO_PIN)(gpio_sel%10);
#endif
        }

        // Get the register of the port
        port_reg = find_mode_register(gpio_sel);
        if (port_reg)
        {
            // Hold previous mode for the pin that will be used for the pulse.
            gpio_prev_mode = GetWord16(port_reg);

            handle_jtag(gpio_port, gpio_pin);
#if defined (__DA14531__)
            handle_reset_state(gpio_port, gpio_pin);
            unset_rst_line(gpio_port, gpio_pin);
#endif
            // Clear any previous role assigned to the pin and configured as GPIO with pull up.
            SetWord16(port_reg, INPUT_PULLUP | PID_GPIO);
#if defined (__DA14531__)
            value = auto_trim(param->trim_MSB, gpio_sel);
#else
            value = auto_trim(gpio_sel);
#endif
            // Pulse found in the pulse pin assigned GPIO was out of acceptable range
            if (value == -1)
            {
                xtal_cal_status = 0x01;
            }
            // No pulse found, or pulse > 740 ms
            else if (value == -2)
            {
                xtal_cal_status = 0x02;
            }
            else
            {
                if (param->operation == CMD__XTRIM_OP_CAL)
                {
                    uint32_t word[2];
                    memset(word, 0, sizeof(word));

                    // Write xtal trimming value in otp
                    word[0] = value & 0xFFFF;
#if !defined (__DA14531__)
                    int res;
                    res = otp_write_words(OTP_HDR_TRIM_XTAL16M_OFFSET, word, 1);
                    if (res != 0)
                    {
                        xtal_cal_status = 0x03;
                    }
#else
//DO NOTHING IN 531
#endif
                }
            }
        }
        else
        {
            xtal_cal_status = 0x04;
        }
        event->trim_value = xtal_cal_status;
        break;
    }
    default:
        break;
    }

    // Check if command is CMD__XTRIM_OP_CALTEST or CMD__XTRIM_OP_CAL to restore UART_RX pin
    if ((param->operation == CMD__XTRIM_OP_CALTEST) || (param->operation == CMD__XTRIM_OP_CAL))
    {
        // If pulse was generated on the UART Rx Pin, wait until the end of the pulse, to avoid UART collision.
        if (((gpio_prev_mode & PID) == PID_UART1_RX) || ((gpio_prev_mode & PID) == PID_UART2_RX))
        {
            // Set timeout.
            uint32_t timeout = 100*PULSE_TIMEOUT;
            // Wait until the pin gets high or timeout is reached.
            while(GPIO_GetPinStatus(gpio_port, gpio_pin) && (timeout-- != 0));

            // Reset timeout.
            timeout = 100*PULSE_TIMEOUT;
            // Wait until the pin gets high or timeout is reached.
            while(!GPIO_GetPinStatus(gpio_port, gpio_pin) && (timeout-- != 0));

            // Wait another 2ms before sending the response.
            for (volatile int cnt=0; cnt<(PULSE_TIMEOUT/10); cnt++){};
        }
    }
#if defined (__DA14531__)
    set_rst_line(gpio_port, gpio_pin);
#endif
    hci_send_2_host(event);

    // Check if command is CMD__XTRIM_OP_CALTEST or CMD__XTRIM_OP_CAL to restore UART_RX pin
    if ((param->operation == CMD__XTRIM_OP_CALTEST) || (param->operation == CMD__XTRIM_OP_CAL))
    {
        // If pulse was generated on the UART Rx Pin, wait until the end of the pulse, to avoid UART collision
        if (((gpio_prev_mode & PID) == PID_UART1_RX) || ((gpio_prev_mode & PID) == PID_UART2_RX))
        {
            // Timeout in 1sec
            uint16_t timeout = PULSE_TIMEOUT;
            // Wait until the pin gets high or timeout is reached
            while(!GPIO_GetPinStatus(gpio_port, gpio_pin) && (timeout-- != 0) );
            // Return pin to its previous mode
            if (port_reg != 0)
            {
                SetWord16(port_reg, gpio_prev_mode);
            }
        }
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the tx_start_continue_test dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_tx_end_continue_test_handler(ke_msg_id_t const msgid,
                                                        void const *param,
                                                        ke_task_id_t const dest_id,
                                                        ke_task_id_t const src_id)
{
    // structure type for the complete command event
    void *event = ke_msg_alloc(HCI_CMD_CMP_EVENT , src_id, HCI_TX_END_CONTINUE_TEST_CMD_OPCODE, 0);

    if (test_state == STATE_START_CONTINUE_TX)
    {
#if defined (__DA14531__)
        stop_refresh_timer();
#endif
        set_state_stop();
        SetWord32(BLE_RFTESTCNTL_REG,0);
        hci_send_2_host(event);
    }
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the start_prod_rx_test dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_start_prod_rx_test_handler(ke_msg_id_t const msgid,
                                                      struct hci_start_prod_rx_dialog_cmd const *param,
                                                      ke_task_id_t const dest_id,
                                                      ke_task_id_t const src_id)
{
    // structure type for the complete command event
    void *event = ke_msg_alloc(HCI_CMD_CMP_EVENT , src_id, HCI_START_PROD_RX_TEST_CMD_OPCODE, 0);

    test_freq = param->frequency;
    set_state_start_rx();

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the end_prod_rx_test dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_end_prod_rx_test_handler(ke_msg_id_t const msgid,
                                                    void const *param,
                                                    ke_task_id_t const dest_id,
                                                    ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_end_prod_rx_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_END_PROD_RX_TEST_CMD_OPCODE, hci_end_prod_rx_dialog_cmd_cmp_evt);

    if (test_state == STATE_START_RX)
    {
        set_state_stop();
#ifdef CFG_BLE_METRICS
        event->rp = metrics.rx_pkt;
        event->rp_SYNC_error = metrics.rx_err_sync;
        event->rp_CRC_error = metrics.rx_err_crc;
        event->RSSI = metrics.rx_rssi;
#endif

        hci_send_2_host(event);
    }
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the unmodulated_on dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_unmodulated_on_handler(ke_msg_id_t const msgid,
                                                  struct hci_unmodulated_on_dialog_cmd const *param,
                                                  ke_task_id_t const dest_id,
                                                  ke_task_id_t const src_id)
{
    // structure type for the complete command event
    void *event = ke_msg_alloc(HCI_CMD_CMP_EVENT , src_id, HCI_UNMODULATED_ON_CMD_OPCODE, 0);
    uint16_t cn;
    bool cmd_flag =false;

    switch(param->operation)
    {
    case UNMODULATED_CMD_MODE_OFF: //UNMODULATED OFF
#if !defined (__DA14531__)
        SetWord16(RF_BMCW_REG, 0);              //DISABLE OVERRULE CN_SEL IS 0
        SetWord16(RF_OVERRULE_REG, 0);          // SO DS IS NOT CORRECT, DISABLE DONE BY WRITING '0' TO ENABLE ISO '1' TO DISABLE
        SetWord16(RF_SYNTH_CTRL3_REG, 0x0003);  //ENABLE MODULATION
#else
        stop_refresh_timer();
        switch (test_state) {
        case STATE_UNMODULATED_RX_ON:
            set_state_stop();
            SetBits32(BLE_RFTESTCNTL_REG,INFINITERX, 0);
            break;
        case STATE_UNMODULATED_ON:
            SetWord32(RF_OVERRULE_REG, 0);
            rf_adplldig_deactivate();
            break;
        default:
            break;
        }
#endif
    
        NVIC_EnableIRQ(BLE_RF_DIAG_IRQn);
        NVIC_ClearPendingIRQ(BLE_RF_DIAG_IRQn); //clear eventual pending bit, but not necessary becasuse this is already cleared automatically in HW
        cmd_flag=true;
        test_state = STATE_IDLE;
        break;

    case UNMODULATED_CMD_MODE_TX: //UNMODULATED TX
        NVIC_DisableIRQ(BLE_RF_DIAG_IRQn);

        cn = param->frequency;
    
#if !defined (__DA14531__)

# if (LUT_PATCH_ENABLED)
        const volatile struct LUT_CFG_struct *pLUT_CFG; // = (const volatile struct LUT_CFG_struct *)(jump_table_struct[lut_cfg_pos]);
        pLUT_CFG = &LUT_CFG;
        if(!pLUT_CFG->HW_LUT_MODE) //so SW_LUT_MODE
        {
            set_rf_cal_cap(cn);
            SetBits16(RF_BMCW_REG, CN_SEL,1);       // ENABLE OVERRULE CN_SEL IS 1
            SetBits16(RF_BMCW_REG, CN_WR, cn);
        }
        else //so HW_LUT_MODE
        {
            //HW
            uint8_t * BLE_freq_tbl;
            BLE_freq_tbl = (uint8_t *)(_ble_base + EM_FT_OFFSET); // = EM base address + Frequency Table offset
            SetBits16(RF_BMCW_REG, CN_SEL,1);                                 // ENABLE OVERRULE CN_SEL IS 1
            SetBits16(RF_BMCW_REG, CN_WR, BLE_freq_tbl[cn]);
        }
# else //NO LUT
        SetBits16(RF_BMCW_REG, CN_SEL,1);                                     // ENABLE OVERRULE CN_SEL IS 1
        SetBits16(RF_BMCW_REG, CN_WR, cn);
# endif

        SetWord16(RF_SYNTH_CTRL3_REG, 0x4000);      //DISABLE MODULATION
        SetWord16(RF_OVERRULE_REG, RX_DIS_WR);      //DISABLE EVENTUAL RX
        SetWord16(RF_OVERRULE_REG, TX_EN_WR);       //ENABLE EVENTUAL TX
        test_state = STATE_UNMODULATED_ON;
#else
        configure_radio_ldo();
        apply_radio_arp_tx_configuration();

        // Enable ADPLL HCLK and reset ADPLL register file
        SetByte(RF_RADIO_INIT_REG + 0x1, (ADPLLDIG_HRESET_N | ADPLLDIG_HCLK_EN) >> 8);
        
        SetWord32(ADPLL_CN_CTRL_REG, (2402 << 16) | CS | cn);
        SetWord32(RF_OVERRULE_REG, 3);   // OVERRULE TX ENABLED
        test_state = STATE_UNMODULATED_ON;
        start_refresh_timer();
#endif // __DA14531__

        cmd_flag=true;
        break;
    case UNMODULATED_CMD_MODE_RX: //UNMODULATED RX
        NVIC_DisableIRQ(BLE_RF_DIAG_IRQn);

        cn = param->frequency;

#if !defined (__DA14531__)

# if (LUT_PATCH_ENABLED)
        pLUT_CFG= &LUT_CFG;;
        if(!pLUT_CFG->HW_LUT_MODE) //so SW_LUT_MODE
        {
            set_rf_cal_cap(cn);
            SetBits16(RF_BMCW_REG, CN_SEL,1);                                 // ENABLE OVERRULE CN_SEL IS 1
            SetBits16(RF_BMCW_REG, CN_WR, cn);
        }
        else //so HW_LUT_MODE
        {
        //HW
            uint8_t * BLE_freq_tbl;
            BLE_freq_tbl = (uint8_t *)(_ble_base + EM_FT_OFFSET); // = EM base address + Frequency Table offset
            SetBits16(RF_BMCW_REG, CN_SEL,1);                                 // ENABLE OVERRULE CN_SEL IS 1
            SetBits16(RF_BMCW_REG, CN_WR, BLE_freq_tbl[cn]);
        }
# else //NO LUT
        SetBits16(RF_BMCW_REG, CN_SEL,1);                                     // ENABLE OVERRULE CN_SEL IS 1
        SetBits16(RF_BMCW_REG, CN_WR, cn);
# endif

        SetWord16(RF_SYNTH_CTRL3_REG, 0x4000);      //DISABLE MODULATION
        SetWord16(RF_OVERRULE_REG, TX_DIS_WR);      //DISABLE EVENTUAL TX
        SetWord16(RF_OVERRULE_REG, RX_EN_WR);       //ENABLE EVENTUAL RX
        test_state = STATE_UNMODULATED_ON;
#else
        configure_radio_ldo();

        struct hci_le_rx_test_cmd  rx_con_test;
        rx_con_test.rx_freq = cn; 
        llm_test_mode_start_rx( (struct hci_le_rx_test_cmd const *)&rx_con_test);   
        SetBits32(BLE_RFTESTCNTL_REG,INFINITERX, 1);
        
        test_state = STATE_UNMODULATED_RX_ON;
        
        start_refresh_timer();
#endif // __DA14531__

        cmd_flag=true;
        break;
    default:
        break;
    }

    if(cmd_flag)
    {
        hci_send_2_host(event);
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the tx_test dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_tx_test_handler(ke_msg_id_t const msgid,
                                           struct hci_tx_test_dialog_cmd const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    test_freq = param->frequency;
    test_data_len = param->data_length;
    test_data_pattern = param->payload_type;
    text_tx_nr_of_packets = (param->NpacketsMSB<<8) | param->NpacketsLSB;
    set_state_start_tx();

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the sleep_test dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_sleep_test_handler(ke_msg_id_t const msgid,
                                              struct hci_sleep_test_dialog_cmd const *param,
                                              ke_task_id_t const dest_id,
                                              ke_task_id_t const src_id)
{
    // structure type for the status event
    struct hci_cmd_stat_event *event = KE_MSG_ALLOC(HCI_CMD_STAT_EVENT , src_id, HCI_SLEEP_TEST_CMD_OPCODE, hci_cmd_stat_event);

    event->status = 0x00;
    hci_send_2_host(event);

    // Finish current UART transfer
    uart_finish_transfers_func();

    switch(param->sleep_mode)
    {
    case CMD__SLEEP_MODE_ACTIVE:
        arch_disable_sleep();
        break;
    case CMD__SLEEP_MODE_EXTENDED:
        // Disable UART pins.
        unset_pad_uart();
        device_wake_up = false;
        if (param->mins_to_sleep == 0 && param->seconds_to_sleep == 0)
        {
            rwip_env.ext_wakeup_enable = 2;
        }
        else
        {
             rwip_env.ext_wakeup_enable = 1;
             rom_cfg_table[max_sleep_duration_periodic_wakeup_pos] = param->mins_to_sleep * 1600 * 60 + param->seconds_to_sleep * 1600;
        }
        arch_set_extended_sleep(false); // no OTP copy
        break;
    case CMD__SLEEP_MODE_DEEP:
        // Disable UART pins.
        unset_pad_uart();
        device_wake_up = false;
#if defined (__DA14531__)
        arch_set_deep_sleep(PD_SYS_DOWN_RAM_OFF, PD_SYS_DOWN_RAM_OFF, PD_SYS_DOWN_RAM_OFF, false);
#else
        arch_set_deep_sleep(false);
#endif
        break;
    default:
        break;
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the rdtest dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_rdtester_handler(ke_msg_id_t const msgid,
                                            struct hci_rdtest_dialog_cmd const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    // structure type for the complete command event
    void *event = ke_msg_alloc(HCI_CMD_CMP_EVENT , src_id, HCI_RDTESTER_CMD_OPCODE, 0);

    switch(param->operation)
    {
#if !defined (__DA14531__)
    case RDTESTER_INIT:
        rdtest_initialize(param->data & 0x00FF);
        break;
    case RDTESTER_UART_CONNECT:
        rdtest_uart_connect(param->data);
        break;
    case RDTESTER_UART_LOOPBACK:
        rdtest_make_loopback(param->data & 0x00FF);
        break;
    case RDTESTER_VBAT_CNTRL:
        rdtest_vbatcontrol(param->data);
        rdtest_uart_connect(param->data);
        break;
    case RDTESTER_VPP_CNTRL:
        rdtest_vppcontrol(param->data & 0x00FF);
        break;
    case RDTESTER_RST_PULSE:
        rdtest_resetpulse(param->data);
        break;
    case RDTESTER_UART_PULSE:
        rdtest_pulse_to_uart(param->data);
        if (param->data)
        {
            rdtest_generate_pulse();
        }
        else
        {
            rdtest_select_pulsewidth(0);
        }
        break;
    case RDTESTER_XTAL_PULSE:
        rdtest_generate_pulse();
        break;
    case RDTESTER_PULSE_WIDTH:
        rdtest_select_pulsewidth(param->data & 0x00FF);
        break;
#else
//DO NOTHING IN 531
#endif //!defined (__DA14531__)

    default:
        break;
    }

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the otp_read dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_otp_read_handler(ke_msg_id_t const msgid,
                                            struct hci_otp_read_dialog_cmd const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_otp_read_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_OTP_READ_CMD_OPCODE, hci_otp_read_dialog_cmd_cmp_evt);
    uint8_t length = 0;
    uint16_t init_addr = 0;
    uint8_t val[4];
    uint32_t i = 0;

    init_addr = param->address;
    length = param->num_of_words;

    /* Check that data length is null or bigger than 60*/
    if ((length == 0) || (length > MAX_READ_WRITE_OTP_WORDS))
    {
        event->status = CO_ERROR_INVALID_HCI_PARAM;
    }
    else
    {
        for (i = 0; i < length; i++)
        {
           otp_read(MEMORY_OTP_BASE + init_addr + (i*4) , val , 4);
           event->wordbyte[i*4]     = val[0];
           event->wordbyte[(i*4)+1] = val[1];
           event->wordbyte[(i*4)+2] = val[2];
           event->wordbyte[(i*4)+3] = val[3];
        }
         event->status = CO_ERROR_NO_ERROR;
    }

    event->num_of_words = length;

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the otp_write dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_otp_write_handler(ke_msg_id_t const msgid,
                                             struct hci_otp_write_dialog_cmd const *param,
                                             ke_task_id_t const dest_id,
                                             ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_otp_write_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_OTP_WRITE_CMD_OPCODE, hci_otp_write_dialog_cmd_cmp_evt);
    
    uint32_t length = 0;
    uint32_t init_addr = 0;
    uint8_t *data_buf;
    uint32_t i = 0;
    uint32_t word[2];
    memset(word, 0, sizeof(word));

    length = param->num_of_words;
    data_buf = (uint8_t*)&param->wordbyte[0];
    init_addr = param->start_addr;

    /* Check that data length is null or bigger than 60*/
    if ((length == 0) || (length > MAX_READ_WRITE_OTP_WORDS))
    {
        event->status = CO_ERROR_INVALID_HCI_PARAM;
    }
    else
    {
         for (i = 0; i < length ; i += 2)
         {
             word[0] =  data_buf[(i*4)+3] << 24 | data_buf[(i*4)+2] << 16 | data_buf[(i*4)+1] << 8 | data_buf[(i*4)];
             word[1] =  data_buf[(i*4)+7] << 24 | data_buf[(i*4)+6] << 16 | data_buf[(i*4)+5] << 8 | data_buf[(i*4)+4];
             otp_write_words( init_addr, word , 1);
             init_addr += 8;
         }
         event->status = CO_ERROR_NO_ERROR;
    }
    event->num_of_words = param->num_of_words;

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the sensor_test dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_sensor_test_handler(ke_msg_id_t const msgid,
                                               struct hci_sensor_test_dialog_cmd const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_sensor_test_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_SENSOR_TEST_CMD_OPCODE, hci_sensor_test_dialog_cmd_cmp_evt);

#if defined (__DA14531__)
    _reset_options prev_rst_op = reset_mode;
#endif

    switch(param->interface)
    {
    case 0: // SPI
    {
        uint16_t mode_reg_clk_gpio = 0;
        uint16_t mode_reg_di_gpio = 0;
        uint16_t mode_reg_do_gpio = 0;
        uint16_t mode_reg_cs_gpio = 0;
        uint16_t mode_reg_int_gpio = 0;

        // Verify GPIO selection.
        if ((!GPIO_is_valid((GPIO_PORT)param->spi_clk_or_i2c_port, (GPIO_PIN)param->spi_clk_or_i2c_pin)) ||         // CLK
            (!GPIO_is_valid((GPIO_PORT)param->spi_di_or_i2c_sda_port, (GPIO_PIN)param->spi_di_or_i2c_sda_pin)) ||   // MISO
            (!GPIO_is_valid((GPIO_PORT)param->spi_do_port, (GPIO_PIN)param->spi_do_pin)) ||                         // MOSI
            (!GPIO_is_valid((GPIO_PORT)param->spi_cs_port, (GPIO_PIN)param->spi_cs_pin)) ||                         // CS
            (param->int_gpio_check && (!GPIO_is_valid((GPIO_PORT)param->int_port, (GPIO_PIN)param->int_pin))))      // INT GPIO
        {
            event->data = 0xFF;
            break;
        }

        // JTAG
        {
            handle_jtag((GPIO_PORT)param->spi_clk_or_i2c_port, (GPIO_PIN)param->spi_clk_or_i2c_pin);
            handle_jtag((GPIO_PORT)param->spi_di_or_i2c_sda_port, (GPIO_PIN)param->spi_di_or_i2c_sda_pin);
            handle_jtag((GPIO_PORT)param->spi_do_port, (GPIO_PIN)param->spi_do_pin);
            handle_jtag((GPIO_PORT)param->spi_cs_port, (GPIO_PIN)param->spi_cs_pin);
            if (param->int_gpio_check)
            {
                handle_jtag((GPIO_PORT)param->int_port, (GPIO_PIN)param->int_pin);
            }
        }

#if defined (__DA14531__)
        // Handle hw_reset function
        {
            if ((param->spi_clk_or_i2c_port == GPIO_PORT_0      &&  param->spi_clk_or_i2c_pin == GPIO_PIN_0)    ||  // CLK
                (param->spi_di_or_i2c_sda_port == GPIO_PORT_0   &&  param->spi_di_or_i2c_sda_pin == GPIO_PIN_0) ||  // MISO
                (param->spi_do_port == GPIO_PORT_0              &&  param->spi_do_pin == GPIO_PIN_0)            ||  // MOSI
                (param->spi_cs_port == GPIO_PORT_0              &&  param->spi_cs_pin == GPIO_PIN_0)            ||  // CS
                (param->int_gpio_check && (param->int_port == GPIO_PORT_0 && param->int_pin == GPIO_PIN_0)))        // INT_GPIO
            {
                reset_mode = ENABLE_POR_RESET;
                // Update reset state.
                set_hw_reset();
            }
        }
#endif

        // Retain previous GPIO modes.
        {
            uint32_t reg_val = 0;

            if ((reg_val = find_mode_register(param->spi_clk_or_i2c_pin)) != 0)
            {
                mode_reg_clk_gpio = GetWord16(reg_val);
            }
            if ((reg_val = find_mode_register(param->spi_di_or_i2c_sda_pin)) != 0)
            {
                mode_reg_di_gpio = GetWord16(reg_val);
            }
            if ((reg_val = find_mode_register(param->spi_do_pin)) != 0)
            {
                mode_reg_do_gpio = GetWord16(reg_val);
            }
            if ((reg_val = find_mode_register(param->spi_cs_pin)) != 0)
            {
                mode_reg_cs_gpio = GetWord16(reg_val);
            }
            if ((reg_val = find_mode_register(param->int_pin)) != 0)
            {
                mode_reg_int_gpio = GetWord16(reg_val);
            }
        }

        // Set parameters in main structs.
        spi_cfg.cs_pad.port = (GPIO_PORT)param->spi_cs_port;
        spi_cfg.cs_pad.pin = (GPIO_PIN)param->spi_cs_pin;

        // Initialize SPI block.
        spi_initialize(&spi_cfg);

        // set_pad_spi.
        {
            // CLK
            GPIO_ConfigurePin((GPIO_PORT) param->spi_clk_or_i2c_port, (GPIO_PIN) param->spi_clk_or_i2c_pin, OUTPUT, PID_SPI_CLK, false);
            // MISO
            GPIO_ConfigurePin((GPIO_PORT) param->spi_di_or_i2c_sda_port, (GPIO_PIN) param->spi_di_or_i2c_sda_pin, INPUT, PID_SPI_DI, false);
            // MOSI
            GPIO_ConfigurePin((GPIO_PORT) param->spi_do_port, (GPIO_PIN) param->spi_do_pin, OUTPUT, PID_SPI_DO, false);
            // CS
            GPIO_ConfigurePin((GPIO_PORT) param->spi_cs_port, (GPIO_PIN) param->spi_cs_pin, OUTPUT, PID_SPI_EN, true);
        }

        // Test operation.
        {
            // INT/DRDY.
            if (param->int_gpio_check)
            {
                GPIO_ConfigurePin((GPIO_PORT) param->int_port, (GPIO_PIN) param->int_pin, INPUT_PULLDOWN, PID_GPIO, false);
            }
            // Write.
            if (param->rw == 1)
            {
                sensor_spi_write_byte((uint16_t) param->register_address, param->register_data_write, (GPIO_PORT) param->spi_cs_port, (GPIO_PIN) param->spi_cs_pin);
            }
            systick_wait(30000);
            event->data = sensor_spi_read_byte((uint16_t) param->register_address, (GPIO_PORT) param->spi_cs_port, (GPIO_PIN) param->spi_cs_pin);

            if (param->int_gpio_check)
            {
                for (volatile int i=0; i<500; i++){}
                event->data = GPIO_GetPinStatus((GPIO_PORT) param->int_port, (GPIO_PIN) param->int_pin);
            }
        }

        // Restore GPIO modes.
        {
            uint32_t reg_val = 0;

            if ((reg_val = find_mode_register(param->spi_clk_or_i2c_pin)) != 0)
            {
                SetWord16(reg_val, mode_reg_clk_gpio);
            }
            if ((reg_val = find_mode_register(param->spi_di_or_i2c_sda_pin)) != 0)
            {
                SetWord16(reg_val, mode_reg_di_gpio);
            }
            if ((reg_val = find_mode_register(param->spi_do_pin)) != 0)
            {
                SetWord16(reg_val, mode_reg_do_gpio);
            }
            if ((reg_val = find_mode_register(param->spi_cs_pin)) != 0)
            {
                SetWord16(reg_val, mode_reg_cs_gpio);
            }
            if ((reg_val = find_mode_register(param->int_port)) != 0)
            {
                SetWord16(reg_val, mode_reg_int_gpio);
            }
        }
        // SPI release.
        spi_release();
        break;
    }
    case 1: // I2C
    {
        uint16_t mode_reg_scl_gpio = 0;
        uint16_t mode_reg_sda_gpio = 0;
        uint16_t mode_reg_int_gpio = 0;

        // Verify GPIO selection.
        if ((!GPIO_is_valid((GPIO_PORT)param->spi_di_or_i2c_sda_port, (GPIO_PIN)param->spi_di_or_i2c_sda_pin)) ||   // SDA
            (!GPIO_is_valid((GPIO_PORT)param->spi_clk_or_i2c_port, (GPIO_PIN)param->spi_clk_or_i2c_pin)) ||         // SCL
            (param->int_gpio_check && (!GPIO_is_valid((GPIO_PORT)param->int_port, (GPIO_PIN)param->int_pin))))      // INT GPIO
        {
            event->data = 0xFF;
            break;
        }

        // JTAG
        {
            handle_jtag((GPIO_PORT)param->spi_di_or_i2c_sda_port, (GPIO_PIN)param->spi_di_or_i2c_sda_pin);
            handle_jtag((GPIO_PORT)param->spi_clk_or_i2c_port, (GPIO_PIN)param->spi_clk_or_i2c_pin);
            if (param->int_gpio_check)
            {
                handle_jtag((GPIO_PORT)param->int_port, (GPIO_PIN)param->int_pin);
            }
        }

#if defined (__DA14531__)
        // Handle hw_reset function
        {
            if ((param->spi_di_or_i2c_sda_port == GPIO_PORT_0   &&  param->spi_di_or_i2c_sda_pin == GPIO_PIN_0) ||  // SDA
                (param->spi_clk_or_i2c_port == GPIO_PORT_0      &&  param->spi_clk_or_i2c_pin == GPIO_PIN_0)    ||  // SCL
                (param->int_gpio_check && (param->int_port == GPIO_PORT_0 && param->int_pin == GPIO_PIN_0)))        // INT GPIO
            {
                reset_mode = ENABLE_POR_RESET;
                // Update reset state.
                set_hw_reset();
            }
        }
#endif
        // Retain previous GPIO modes.
        {
            uint32_t reg_val = 0;

            if ((reg_val = find_mode_register(param->spi_clk_or_i2c_pin)) != 0)
            {
                mode_reg_scl_gpio = GetWord16(reg_val);
            }
            if ((reg_val = find_mode_register(param->spi_di_or_i2c_sda_pin)) != 0)
            {
                mode_reg_sda_gpio = GetWord16(reg_val);
            }
            if ((reg_val = find_mode_register(param->int_port)) != 0)
            {
                mode_reg_int_gpio = GetWord16(reg_val);
            }
        }

        // Set parameters in main struct.
        i2c_cfg.address = param->i2c_slave_address;

        // Configure I2C environment.
        i2c_init(&i2c_cfg);

        // set_pad_eeprom.
        {
            // I2C_SCL GPIO initialize.
            GPIO_SetPinFunction((GPIO_PORT) param->spi_clk_or_i2c_port, (GPIO_PIN) param->spi_clk_or_i2c_pin, INPUT, PID_I2C_SCL);
            // I2C_SDA GPIO initialize.
            GPIO_SetPinFunction((GPIO_PORT) param->spi_di_or_i2c_sda_port, (GPIO_PIN) param->spi_di_or_i2c_sda_pin, INPUT, PID_I2C_SDA);
        }

        // Test operation.
        {
            // INT/DRDY.
            if (param->int_gpio_check)
            {
                GPIO_ConfigurePin((GPIO_PORT) param->int_port, (GPIO_PIN) param->int_pin, INPUT_PULLDOWN, PID_GPIO, false);
            }

            // Write.
            if (param->rw == 1)
            {
                sensor_i2c_write_byte((uint32_t) param->register_address, param->register_data_write);
            }
            systick_wait(30000);
            event->data  = sensor_i2c_read_byte((uint32_t) param->register_address);

            if (param->int_gpio_check)
            {
                for (volatile int i=0; i<500; i++){}
                event->data = GPIO_GetPinStatus((GPIO_PORT) param->int_port, (GPIO_PIN) param->int_pin);
            }
        }

        // Restore GPIOs.
        {
            uint32_t reg_val = 0;

            if ((reg_val = find_mode_register(param->spi_clk_or_i2c_pin)) != 0)
            {
                SetWord16(reg_val, mode_reg_scl_gpio);
            }
            if ((reg_val = find_mode_register(param->spi_di_or_i2c_sda_pin)) != 0)
            {
                SetWord16(reg_val, mode_reg_sda_gpio);
            }
            if ((reg_val = find_mode_register(param->int_port)) != 0)
            {
                SetWord16(reg_val, mode_reg_int_gpio);
            }
        }

        i2c_release();
        break;
    }
    default:
    {
        break;
    }
    }

#if defined (__DA14531__)
    // Restore reset state.
    reset_mode = prev_rst_op;
    set_hw_reset();
#endif

    // Restore option
    set_pad_functions();                                                                // Restore UART pins in case they are used in a sensor.
    // Send the answer.
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the gpio_set dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_gpio_set_handler(ke_msg_id_t const msgid,
                                            struct hci_gpio_set_dialog_cmd const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    // structure type for the status event
    struct hci_gpio_set_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT ,src_id, HCI_GPIO_SET_CMD_OPCODE, hci_gpio_set_dialog_cmd_cmp_evt);
    uint8_t err = 0x00;
    GPIO_PORT port;
    GPIO_PIN pin;
    GPIO_PUPD mode;
    uint8_t volt_rail;
    uint8_t state;
    uint8_t pwm;
    uint8_t duty_cycle;

    /* Transform the input. */
#if defined(__DA14531__)
    port = GPIO_PORT_0;
    pin = (GPIO_PIN) (param->gpio_pad);
#else
    port = (GPIO_PORT) (param->gpio_pad / 10);
    pin = (GPIO_PIN) (param->gpio_pad % 10);
#endif
    volt_rail = param->gpio_lvl;
    state = param->val;
    pwm = param->pwm;
    duty_cycle = param->duty_cycle;

    /* Validate the GPIO. */
    if (!GPIO_is_valid(port, pin))
    {
        err = 1;
    }

    /* Transform and validate the GPIO mode. */
    switch (param->mode)
    {
        case 0: mode = INPUT; break;
        case 1: mode = INPUT_PULLUP; break;
        case 2: mode = INPUT_PULLDOWN; break;
        case 3: mode = OUTPUT; break;
        default: err = 1; break;
    }

    /* Validate the GPIO voltage rail. 0 = 3.3V, 1 = 1.8V. */
    if (volt_rail > 1)
    {
        err = 1;
    }

    /* 0=Reset, 1=Set. Only valid in output mode. */
    if ((state > 1) && (param->mode >= OUTPUT))
    {
        err = 1;
    }

    /* Validate duty cycle. */
    if (duty_cycle > 100)
    {
        err = 1;
    }

    /* If error exists in the received message, reply with error. */
    if (err)
    {
        event->error = 0xFF;
        hci_send_2_host(event);
        return (KE_MSG_CONSUMED);
    }

    handle_jtag((GPIO_PORT)port, (GPIO_PIN)pin);
#if defined (__DA14531__)
    handle_reset_state((GPIO_PORT)port, (GPIO_PIN)pin);
#endif
    
    if (pwm > 0)
    {
        /* State=0 -> PWM stop. State=1 -> PWM start. */
        switch (state)
        {
            case 0: // Stop Timer2.
            {
                timer2_stop();
                GPIO_ConfigurePin(port, pin, INPUT_PULLDOWN, PID_GPIO, false);
                GPIO_ConfigurePinPower(port, pin, volt_rail ? GPIO_POWER_RAIL_1V : GPIO_POWER_RAIL_3V);
                break;
            }
            case 1: // Start PWM2.
            {
                GPIO_ConfigurePin(port, pin, OUTPUT, PID_PWM2, false);
                GPIO_ConfigurePinPower(port, pin, volt_rail ? GPIO_POWER_RAIL_1V : GPIO_POWER_RAIL_3V);

                tim0_2_clk_div_config_t tim0_2_clk_div_config = {.clk_div = TIM0_2_CLK_DIV_8};

                // Enable TIMER0 clock.
                timer0_2_clk_enable();

                // Sets TIMER0,TIMER2 clock division factor to 8, so TIM0 Fclk is F = 16MHz/8 = 2Mhz.
                timer0_2_clk_div_set(&tim0_2_clk_div_config);

                tim2_pwm_config_t tim2_pwm_config = {
                                                        .pwm_signal = TIM2_PWM_2,
                                                        .pwm_dc = duty_cycle,
#if defined (__DA14531__)
                                                        .pwm_offset = 0
#endif
                                                    };
                tim2_config_t tim2_config = {
#if defined (__DA14531__)
                                                .clk_source = TIM2_CLK_SYS,
#endif
                                                .hw_pause = TIM2_HW_PAUSE_OFF
                                            };

                timer2_config(&tim2_config);
                timer2_pwm_freq_set(pwm*1000, 2000000);
                timer2_pwm_signal_config(&tim2_pwm_config);

                timer2_start();
                break;
            }
            default:
            {
                err = 1;
                break;
            }
        }
    }
    else
    {
        GPIO_ConfigurePin( port, pin, mode, PID_GPIO, state==1 ? true : false);
        GPIO_ConfigurePinPower(port, pin, volt_rail ? GPIO_POWER_RAIL_1V : GPIO_POWER_RAIL_3V);
    }

    event->error = (err==1) ? 0xFF : 0x00;
    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the gpio_read dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_gpio_read_handler(ke_msg_id_t const msgid,
                                            struct hci_gpio_read_dialog_cmd const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    // structure type for the status event
    struct hci_gpio_read_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT ,src_id, HCI_GPIO_READ_CMD_OPCODE, hci_gpio_read_dialog_cmd_cmp_evt);
    uint8_t err = 0x00;
    GPIO_PORT port;
    GPIO_PIN pin;

    /* Transform the input. */
#if defined(__DA14531__)
    port = GPIO_PORT_0;
    pin = (GPIO_PIN) (param->gpio_pad);
#else
    port = (GPIO_PORT) (param->gpio_pad / 10);
    pin = (GPIO_PIN) (param->gpio_pad % 10);
#endif

    /* Validate the GPIO pin. */
    if(!GPIO_is_valid(port, pin))
    {
        err = 1;
    }

    /* If error exists in the received message, reply with error. */
    if (err)
    {
        event->data = 0xFF;
    }
    else
    {
        event->data = GPIO_GetPinStatus( port, pin);
    }

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the uart_loop dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_uart_loop_handler(ke_msg_id_t const msgid,
                                            struct hci_uart_loop_dialog_cmd const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    // structure type for the status event
    struct hci_uart_loop_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT ,src_id, HCI_UART_LOOP_CMD_OPCODE, hci_uart_loop_dialog_cmd_cmp_evt);

    memcpy(event->data, param->data, sizeof(event->data));

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the uart_baud dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_uart_baud_handler(ke_msg_id_t const msgid,
                                            struct hci_uart_baud_dialog_cmd const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    // Structure type for the status event.
    struct hci_uart_baud_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, src_id, HCI_UART_BAUD_CMD_OPCODE, hci_uart_baud_dialog_cmd_cmp_evt);

    if ((param->data == 5) ||
        (param->data == 4) ||
        (param->data == 3) ||
        (param->data == 2) ||
        (param->data == 1) ||
        (param->data == 0))
        event->error = 0;
    else event->error = 1;

    hci_send_2_host(event);
    uart_finish_transfers_func();

    for (volatile int i=0; i<200; i++) {}; // Wait some time for UART reset.

    switch (param->data)
    {
        case 5:
            baud_rate_sel = UART_BAUDRATE_38K4;
            baud_rate_frac_sel = UART_FRAC_BAUDRATE_38K4;
            break;
        case 4:
            baud_rate_sel = UART_BAUDRATE_1M;
            baud_rate_frac_sel = UART_FRAC_BAUDRATE_1M;
            break;
        case 3:
            baud_rate_sel = UART_BAUDRATE_115K2;
            baud_rate_frac_sel = UART_FRAC_BAUDRATE_115K2;
            break;
        case 2:
            baud_rate_sel = UART_BAUDRATE_57K6;
            baud_rate_frac_sel = UART_FRAC_BAUDRATE_57K6;
            break;
        case 1:
            baud_rate_sel = UART_BAUDRATE_19K2;
            baud_rate_frac_sel = UART_FRAC_BAUDRATE_19K2;
            break;
        case 0:
            baud_rate_sel = UART_BAUDRATE_9K6;
            baud_rate_frac_sel = UART_FRAC_BAUDRATE_9K6;
            break;
        default:
            baud_rate_sel = UART_BAUDRATE_115K2;
            baud_rate_frac_sel = UART_FRAC_BAUDRATE_115K2;
            break;
    }
    uart_init(baud_rate_sel, baud_rate_frac_sel, UART_CHARFORMAT_8);

    h4tl_init(rwip_eif_get(RWIP_EIF_HCIC));

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the watchdog GPIO dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_gpio_wd_handler(ke_msg_id_t const msgid,
                                           struct hci_gpio_wd_dialog_cmd const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    // Structure type for the complete command event.
    struct hci_gpio_wd_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, src_id, HCI_GPIO_WD_CMD_OPCODE, hci_gpio_wd_dialog_cmd_cmp_evt);
    uint8_t err = 0x00;
    GPIO_PORT port;
    GPIO_PIN pin;
    uint8_t volt_rail;

    /* Transform the input. */
#if defined(__DA14531__)
    port = GPIO_PORT_0;
    pin = (GPIO_PIN) (param->gpio_pad);
#else
    port = (GPIO_PORT) (param->gpio_pad / 10);
    pin = (GPIO_PIN) (param->gpio_pad % 10);
#endif
    volt_rail = param->gpio_lvl;

    /* Validate the GPIO voltage rail. 0 = 3.3V, 1 = 1.8V. */
    if (volt_rail > 1)
    {
        err = 1;
    }

    /* Validate the GPIO port/pin. */
    if (!GPIO_is_valid(port, pin))
    {
        err = 1;
    }

    handle_jtag((GPIO_PORT)port, (GPIO_PIN)pin);
#if defined (__DA14531__)
    // Disable hw_reset functionality.
    if ((port == GPIO_PORT_0) && (pin == GPIO_PIN_0))
    {
        GPIO_Disable_HW_Reset();
    }
#endif

    /* Check for error. Send error and return. */
    if (err)
    {
        event->error = 0xFF;
        hci_send_2_host(event);
        return (KE_MSG_CONSUMED);
    }

    WD_port = port;
    WD_pin = pin;
    WD_volt_rail = volt_rail;

    gpio_wd_timer0_start();

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the external 32KHz crystal dialog hci test command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_ext32khz_test_handler(ke_msg_id_t const msgid,
                                                 void const *param,
                                                 ke_task_id_t const dest_id,
                                                 ke_task_id_t const src_id)
{
    // Structure type for the complete command event.
    struct hci_ext32khz_test_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, src_id, HCI_EXT32KHz_TEST_CMD_OPCODE, hci_ext32khz_test_dialog_cmd_cmp_evt);
    int k = 0;

#ifdef __DA14531__
    bool xtal_enabled = (GetBits16(CLK_XTAL32K_REG, XTAL32K_ENABLE) == 1);

    // Enable XTAL32K if not enabled.
    if (!xtal_enabled)
    {
        SetBits16(CLK_XTAL32K_REG, XTAL32K_ENABLE, 0x1); // Enables the 32kHz XTAL oscillator.
        SetBits16(CLK_XTAL32K_REG, XTAL32K_CUR, 5);
        SetBits16(CLK_XTAL32K_REG, XTAL32K_RBIAS, 3);

        for (int i=0; i<16*200000/6; i++); // 200msec
    }

    if (GetBits16(CLK_XTAL32K_REG, XTAL32K_ENABLE) == 1)
#endif
    {
        // Calibration operation to understand if external 32KHz is mounted.
        SetBits16(CLK_REF_CNT_REG, REF_CNT_VAL, 1);
#ifdef __DA14531__
        SetBits16(CLK_REF_SEL_REG, EXT_CNT_EN_SEL, 0x00);
#endif
        SetBits16(CLK_REF_SEL_REG, REF_CLK_SEL, 0x2);   // XTAL32KHz select

        SetBits16(CLK_REF_SEL_REG, REF_CAL_START, 0x1); // Start calibration.

        k = 0;
        while ((GetBits16(CLK_REF_SEL_REG, REF_CAL_START) == 1) && (k<100)) {
            k++;
            for (int i=0; i<16*100/6; i++); // 100usec
        }

        if (GetBits16(CLK_REF_SEL_REG, REF_CAL_START) == 1)
            event->error = 0xFF;
        else event->error = 0;
    }
#ifdef __DA14531__
    else
    {
        event->error = 0xFF;
    }
#endif

#ifdef __DA14531__
    // Restore XTAL32K state
    if (!xtal_enabled)
    {
        SetBits16(CLK_XTAL32K_REG, XTAL32K_ENABLE, 0x0);
    }
#endif

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the sleep clock selection dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_sleep_clk_sel_handler(ke_msg_id_t const msgid,
                                                 struct hci_sleep_clk_sel_dialog_cmd const *param,
                                                 ke_task_id_t const dest_id,
                                                 ke_task_id_t const src_id)
{
    // Structure type for the complete command event.
    struct hci_sleep_clk_sel_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, src_id, HCI_SLEEP_CLK_SEL_CMD_OPCODE, hci_sleep_clk_sel_dialog_cmd_cmp_evt);
    event->error = 0x00;

#if defined (__DA14531__)
//DO NOTHING IN 531
#else
    // LP_CLK_XTAL32
    if (param->sleep_clk == 0x1)
    {
        lp_clk_sel = LP_CLK_XTAL32;
        SetBits16(CLK_32K_REG, XTAL32K_ENABLE, 1);  // Enable XTAL32KHz

        // Disable XTAL32 amplitude regulation in BOOST mode
        if (GetBits16(ANA_STATUS_REG, BOOST_SELECTED) == 0x1)
            SetBits16(CLK_32K_REG,  XTAL32K_DISABLE_AMPREG, 1);
        else
            SetBits16(CLK_32K_REG,  XTAL32K_DISABLE_AMPREG, 0);
        SetBits16(CLK_32K_REG,  XTAL32K_CUR, 5);
        SetBits16(CLK_32K_REG,  XTAL32K_RBIAS, 3);
        SetBits16(SYS_CTRL_REG, CLK32_SOURCE, 1);  // Select XTAL32K as LP clock
    // LP_CLK_RCX20
    }else if (param->sleep_clk == 0x0)
    {
        lp_clk_sel = LP_CLK_RCX20;
        SetBits16(CLK_RCX20K_REG, RCX20K_NTC, 0xB);
        SetBits16(CLK_RCX20K_REG, RCX20K_BIAS, 0);
        SetBits16(CLK_RCX20K_REG, RCX20K_TRIM, 0);
        SetBits16(CLK_RCX20K_REG, RCX20K_LOWF, 1);
        SetBits16(CLK_RCX20K_REG, RCX20K_ENABLE, 1);
        SetBits16(CLK_RCX20K_REG, RCX20K_SELECT, 1);
        SetBits16(SYS_CTRL_REG, CLK32_SOURCE, 0); // Select RCX as LP clock
        rcx20_calibrate();
    }
    else event->error = 0xFF;
#endif

    hci_send_2_host(event);
    return (KE_MSG_CONSUMED);
}

static int dialog_commands_platform_reset_handler(ke_msg_id_t const msgid,
                                                  void const *param,
                                                  ke_task_id_t const dest_id,
                                                  ke_task_id_t const src_id)
{
    // Structure type for the status event.
    struct hci_platform_reset_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, src_id, HCI_PLATFORM_RESET_CMD_OPCODE, hci_platform_reset_cmd_cmp_evt);

    event->error = 0;
    hci_send_2_host(event);

    SetWord16(RESET_FREEZE_REG, FRZ_WDOG);
    SetWord16(WATCHDOG_CTRL_REG, NMI_RST);
    SetWord16(WATCHDOG_REG, 5);
    // Wait for Watchdog to expire
    while (GetBits16(WATCHDOG_REG, WDOG_VAL));

    return (KE_MSG_CONSUMED);
}

#if defined (__DA14531__)

extern void rf_reinit(void);

static int dialog_commands_set_tx_power_handler(ke_msg_id_t const msgid,
                                                struct hci_set_tx_power_dialog_cmd const *param,
                                                ke_task_id_t const dest_id,
                                                ke_task_id_t const src_id)
{
    // Structure type for the status event.
    struct hci_set_tx_power_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, src_id, HCI_SET_TX_POWER_CMD_OPCODE, hci_set_tx_power_dialog_cmd_cmp_evt);

    event->error = 0;

    hci_send_2_host(event);

    rf_pa_pwr_set((rf_tx_pwr_lvl_t)param->data);

    rf_reinit();    // Update radio and ADPLL settings

    return (KE_MSG_CONSUMED);
}

static int dialog_commands_configure_test_mode_handler(ke_msg_id_t const msgid,
                                            struct hci_configure_test_mode_dialog_cmd const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    // Structure type for the status event.
    struct hci_configure_test_mode_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, src_id, HCI_CONFIGURE_TEST_MODE_CMD_OPCODE, hci_configure_test_mode_dialog_cmd_cmp_evt);

    event->error = 0;

    set_radio_ldo_configuration(param->data != 0);

    hci_send_2_host(event);

    switch (param->data)
    {

    }

    return (KE_MSG_CONSUMED);
}

static int dialog_commands_set_reset_mode_handler(ke_msg_id_t const msgid,
                                            struct hci_set_reset_mode_dialog_cmd const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    // Structure type for the status event.
    struct hci_set_reset_mode_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT, src_id, HCI_RESET_MODE_CMD_OPCODE, hci_set_reset_mode_dialog_cmd_cmp_evt);

    reset_mode = (_reset_options)param->reset_mode;
    set_hw_reset();

    event->error = 0;
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

#endif // __DA14531__

/// Special unpacking function for HCI Debug Write OTP Command
static uint8_t hci_wr_otp_cmd_upk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len)
{
    struct hci_otp_write_dialog_cmd* cmd = (struct hci_otp_write_dialog_cmd*) out;
    uint8_t* p_in = in;
    uint8_t* p_out = out;
    uint8_t* p_in_end = in + in_len;
    uint8_t* p_out_end = out + *out_len;
    uint8_t status = HCI_PACK_OK;

    // Check if there is input data to parse
    if(in != NULL)
    {
        do
        {
            uint8_t data_len;

            // Start address
            p_out = (uint8_t*) &cmd->start_addr;
            status = hci_pack_bytes(&p_in, &p_out, p_in_end, p_out_end, 2);
            if(status != HCI_PACK_OK)
                break;

            // Data length
            data_len = *p_in;
            p_out = &cmd->num_of_words;
            status = hci_pack_bytes(&p_in, &p_out, p_in_end, p_out_end, 1);
            if(status != HCI_PACK_OK)
                break;

            // Data
            data_len *= 4;
            p_out = &cmd->wordbyte[0];
            status = hci_pack_bytes(&p_in, &p_out, p_in_end, p_out_end, data_len);
            if(status != HCI_PACK_OK)
                break;

        } while(0);

        *out_len =  (uint16_t)(p_out - out);
    }
    else
    {
        // If no input data, size max is returned
        *out_len = sizeof(struct hci_dbg_wr_mem_cmd);
    }

    return status;
}

/// Special packing function for Command Complete Event of HCI  Read otp Command
static uint8_t hci_otp_rd_data_cmd_cmp_evt_pk(uint8_t *out, uint8_t *in, uint16_t* out_len, uint16_t in_len)
{
    struct hci_otp_read_dialog_cmd_cmp_evt* evt = (struct hci_otp_read_dialog_cmd_cmp_evt*)(in);
    uint8_t* p_in = in;
    uint8_t* p_out = out;
    uint8_t* p_in_end = in + in_len;
    uint8_t* p_out_end = out + *out_len;
    uint8_t status = HCI_PACK_OK;

    // Check if there is input data to parse
    if(in != NULL)
    {
        do
        {
            uint8_t data_len;

            // Status
            p_in = &evt->status;
            status = hci_pack_bytes(&p_in, &p_out, p_in_end, p_out_end, 1);
            if(status != HCI_PACK_OK)
                break;

            // Number of Words
            p_in = &evt->num_of_words;
            status = hci_pack_bytes(&p_in, &p_out, p_in_end, p_out_end, 1);
            if(status != HCI_PACK_OK)
                break;

            // Words
            data_len = evt->num_of_words * 4;
            p_in = &evt->wordbyte[0];
            status = hci_pack_bytes(&p_in, &p_out, p_in_end, p_out_end, data_len);
            if(status != HCI_PACK_OK)
                break;

        } while(0);

        *out_len =  (uint16_t)(p_out - out);
    }
    else
    {
        *out_len = 0;
    }

    return status;
}

/**
****************************************************************************************
* @brief Apply a basic pack operation
*
* @param[inout] pp_in      Current input buffer position
* @param[inout] pp_out     Current output buffer position
* @param[in]    p_in_end   Input buffer end
* @param[in]    p_out_end  Output buffer end
* @param[in]    len        Number of bytes to copy
*
* @return status
*****************************************************************************************
*/
static uint8_t hci_pack_bytes(uint8_t** pp_in, uint8_t** pp_out, uint8_t* p_in_end, uint8_t* p_out_end, uint8_t len)
{
    uint8_t status = HCI_PACK_OK;
    // Check if enough space in input buffer to read
    if((*pp_in + len) > p_in_end)
    {
        status = HCI_PACK_IN_BUF_OVFLW;
    }
    else
    {
        if(p_out_end != NULL)
        {
            // Check if enough space in out buffer to write
            if((*pp_out + len) > p_out_end)
            {
                status = HCI_PACK_OUT_BUF_OVFLW;
            }

            // Copy bytes
            memcpy(*pp_out, *pp_in, len);
        }
        *pp_in = (*pp_in + len);
        *pp_out = *pp_out + len;
    }

    return status;
}
// The message handlers for dialog HCI command complete events
const struct ke_msg_handler dialog_commands_handler_tab[] =
{
        {HCI_CUSTOM_ACTION_CMD_OPCODE           ,  (ke_msg_func_t)dialog_commands_custom_action_handler          },
        {HCI_SLEEP_TEST_CMD_OPCODE              ,  (ke_msg_func_t)dialog_commands_sleep_test_handler             },
        {HCI_XTAL_TRIM_CMD_OPCODE               ,  (ke_msg_func_t)dialog_commands_xtal_trim_handler              },
        {HCI_OTP_RW_CMD_OPCODE                  ,  (ke_msg_func_t)dialog_commands_otp_rw_handler                 },
        {HCI_OTP_READ_CMD_OPCODE                ,  (ke_msg_func_t)dialog_commands_otp_read_handler               },
        {HCI_OTP_WRITE_CMD_OPCODE               ,  (ke_msg_func_t)dialog_commands_otp_write_handler              },
        {HCI_REGISTER_RW_CMD_OPCODE             ,  (ke_msg_func_t)dialog_commands_register_rw_handler            },
        {HCI_AUDIO_TEST_CMD_OPCODE              ,  (ke_msg_func_t)dialog_commands_audio_test_handler             },
        {HCI_FIRMWARE_VERSION_GET_CMD_OPCODE    ,  (ke_msg_func_t)dialog_commands_firmware_version_get_handler   },
        {HCI_CHANGE_UART_PINS_ACTION_CMD_OPCODE ,  (ke_msg_func_t)dialog_commands_change_uart_pins_action_handler},
        {HCI_RDTESTER_CMD_OPCODE                ,  (ke_msg_func_t)dialog_commands_rdtester_handler               },
        {HCI_TX_TEST_CMD_OPCODE                 ,  (ke_msg_func_t)dialog_commands_tx_test_handler                },
        {HCI_START_PROD_RX_TEST_CMD_OPCODE      ,  (ke_msg_func_t)dialog_commands_start_prod_rx_test_handler     },
        {HCI_END_PROD_RX_TEST_CMD_OPCODE        ,  (ke_msg_func_t)dialog_commands_end_prod_rx_test_handler       },
        {HCI_UNMODULATED_ON_CMD_OPCODE          ,  (ke_msg_func_t)dialog_commands_unmodulated_on_handler         },
        {HCI_TX_START_CONTINUE_TEST_CMD_OPCODE  ,  (ke_msg_func_t)dialog_commands_tx_start_continue_test_handler },
        {HCI_TX_END_CONTINUE_TEST_CMD_OPCODE    ,  (ke_msg_func_t)dialog_commands_tx_end_continue_test_handler   },
        {HCI_SENSOR_TEST_CMD_OPCODE             ,  (ke_msg_func_t)dialog_commands_sensor_test_handler            },
        {HCI_GPIO_SET_CMD_OPCODE                ,  (ke_msg_func_t)dialog_commands_gpio_set_handler               },
        {HCI_GPIO_READ_CMD_OPCODE               ,  (ke_msg_func_t)dialog_commands_gpio_read_handler              },
        {HCI_UART_LOOP_CMD_OPCODE               ,  (ke_msg_func_t)dialog_commands_uart_loop_handler              },
        {HCI_UART_BAUD_CMD_OPCODE               ,  (ke_msg_func_t)dialog_commands_uart_baud_handler              },
        {HCI_EXT32KHz_TEST_CMD_OPCODE           ,  (ke_msg_func_t)dialog_commands_ext32khz_test_handler          },
        {HCI_GPIO_WD_CMD_OPCODE                 ,  (ke_msg_func_t)dialog_commands_gpio_wd_handler                },
        {HCI_SLEEP_CLK_SEL_CMD_OPCODE           ,  (ke_msg_func_t)dialog_commands_sleep_clk_sel_handler          },
        {HCI_ADC_VBAT_CMD_OPCODE                ,  (ke_msg_func_t)dialog_commands_adc_vbat_handler               },
#if defined (__DA14531__)
        {HCI_SET_TX_POWER_CMD_OPCODE            ,  (ke_msg_func_t)dialog_commands_set_tx_power_handler           },
        {HCI_CONFIGURE_TEST_MODE_CMD_OPCODE     ,  (ke_msg_func_t)dialog_commands_configure_test_mode_handler    },
        {HCI_RESET_MODE_CMD_OPCODE              ,  (ke_msg_func_t)dialog_commands_set_reset_mode_handler         },
#endif
        {HCI_PLATFORM_RESET_CMD_OPCODE          ,  (ke_msg_func_t)dialog_commands_platform_reset_handler         },
};

const uint8_t dialog_commands_handler_num = ARRAY_LEN (dialog_commands_handler_tab);
