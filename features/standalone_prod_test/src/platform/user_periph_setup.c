/**
 ****************************************************************************************
 *
 * @file periph_setup.c
 *
 * @brief Peripherals setup and initialization.
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

#include "rwip_config.h"        // SW configuration
#include "user_periph_setup.h"  // peripheral configuration
#include "system_library.h"
#include "gpio.h"
#include "uart.h"               // UART initialization
#include "hci.h"
#include "syscntl.h"

/****************************************************************************************/
/* UART pin configuration                                                               */
/****************************************************************************************/
volatile uint8_t port_sel                   __SECTION_ZERO("prodtest_uninit");
bool device_wake_up                         __SECTION_ZERO("retention_mem_area0");
_uart_sel_pins_t uart_sel_pins              __SECTION_ZERO("retention_mem_area0");
uint8_t baud_rate_sel                       __SECTION_ZERO("retention_mem_area0");
uint8_t baud_rate_frac_sel                  __SECTION_ZERO("retention_mem_area0");

_uart_sel_pins_t uart_sel_pins;

#if defined (__DA14531__)
_reset_options      reset_mode              __SECTION_ZERO("retention_mem_area0");
#endif

#if DEVELOPMENT_DEBUG
/**
 ****************************************************************************************
 * @brief Reserve the GPIOs used by the application.
 ****************************************************************************************
*/
void GPIO_reservations(void)
{
#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif

#ifdef CONFIG_UART_GPIOS
    switch (port_sel)
    {
    case P0_0_AND_P0_1_INITIALIZED_FROM_EXT_TOOL:
        RESERVE_GPIO(UART1_TX, GPIO_PORT_0, GPIO_PIN_0, PID_UART1_TX);
        RESERVE_GPIO(UART1_RX, GPIO_PORT_0, GPIO_PIN_1, PID_UART1_RX);
        break;
    case P0_2_AND_P0_3_INITIALIZED_FROM_EXT_TOOL:
        RESERVE_GPIO(UART1_TX, GPIO_PORT_0, GPIO_PIN_2, PID_UART1_TX);
        RESERVE_GPIO(UART1_RX, GPIO_PORT_0, GPIO_PIN_3, PID_UART1_RX);
        break;
    case P0_4_AND_P0_5_INITIALIZED_FROM_EXT_TOOL:
        RESERVE_GPIO(UART1_TX, GPIO_PORT_0, GPIO_PIN_4, PID_UART1_TX);
        RESERVE_GPIO(UART1_RX, GPIO_PORT_0, GPIO_PIN_5, PID_UART1_RX);
        break;
#if defined(CFG_UART_ONE_WIRE_SUPPORT)
    case P0_3_SINGLE_W_INITIALIZED_FROM_EXT_TOOL:
        RESERVE_GPIO(UART1_RX, GPIO_PORT_0, GPIO_PIN_3, PID_UART1_RX);
        break;
    case P0_5_SINGLE_W_INITIALIZED_FROM_EXT_TOOL:
        RESERVE_GPIO(UART1_RX, GPIO_PORT_0, GPIO_PIN_5, PID_UART1_RX);
        break;
#endif
    case P0_6_AND_P0_7_INITIALIZED_FROM_EXT_TOOL:
        RESERVE_GPIO(UART1_TX, GPIO_PORT_0, GPIO_PIN_6, PID_UART1_TX);
        RESERVE_GPIO(UART1_RX, GPIO_PORT_0, GPIO_PIN_7, PID_UART1_RX);
        break;
    default:
        RESERVE_GPIO(UART1_TX, GPIO_PORT_0, GPIO_PIN_4, PID_UART1_TX);
        RESERVE_GPIO(UART1_RX, GPIO_PORT_0, GPIO_PIN_5, PID_UART1_RX);
        break;
    }
#elif (defined(UART1_TX_GPIO_PORT) && defined(UART1_TX_GPIO_PIN) && defined(UART1_RX_GPIO_PORT) && defined(UART1_RX_GPIO_PIN))
    if ((UART1_TX_GPIO_PORT != UART1_RX_GPIO_PORT) || (UART1_TX_GPIO_PIN != UART1_RX_GPIO_PIN)) {
        RESERVE_GPIO( UART1_TX, UART1_TX_GPIO_PORT,  UART1_TX_GPIO_PIN, PID_UART1_TX);
    }
    RESERVE_GPIO( UART1_RX, UART1_RX_GPIO_PORT,  UART1_RX_GPIO_PIN, PID_UART1_RX);
#else
    #error "=== No UART pin configuration selected in user_periph_setup.h ==="
#endif // CONFIG_UART_GPIOS

#ifdef STANDALONE_MODE
    RESERVE_GPIO( BUTTON, GPIO_BUTTON_PORT,  GPIO_BUTTON_PIN, PID_GPIO);
#endif
}

#endif //DEVELOPMENT_DEBUG

/**
 ****************************************************************************************
 * @brief Set function mode for GPIO pins.
 *        If DEVELOPMENT_DEBUG is defined then each pin must have been
 *        previously reserved in GPIO_reservations().
 ****************************************************************************************
 */
void set_pad_functions(void)
{
#ifdef __DA14586__
    // disallow spontaneous flash wake-up
    GPIO_ConfigurePin(SPI_EN_PORT, SPI_EN_PIN, OUTPUT, PID_GPIO, true);
#endif

#if defined (CFG_PRINTF_UART2)
    // Configure UART2 TX Pad
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif

    if (!device_wake_up)
    {
#if defined(CONFIG_UART_GPIOS) // Ports for UART are initialized from external tool
        switch (port_sel)
        {
            case P0_0_AND_P0_1_INITIALIZED_FROM_EXT_TOOL: // Ports for UART P0_0 & P0_1 are initialized from external tool
                update_uart_pads(GPIO_PORT_0, GPIO_PIN_0, GPIO_PORT_0, GPIO_PIN_1);
                break;
            case P0_2_AND_P0_3_INITIALIZED_FROM_EXT_TOOL: // Ports for UART P0_2 & P0_3 are initialized from external tool
                update_uart_pads(GPIO_PORT_0, GPIO_PIN_2, GPIO_PORT_0,GPIO_PIN_3);
                break;
            case P0_4_AND_P0_5_INITIALIZED_FROM_EXT_TOOL: // Ports for UART P0_4 & P0_5 are initialized from external tool
                update_uart_pads(GPIO_PORT_0, GPIO_PIN_4, GPIO_PORT_0, GPIO_PIN_5);
                break;
#if defined(CFG_UART_ONE_WIRE_SUPPORT)
            case P0_3_SINGLE_W_INITIALIZED_FROM_EXT_TOOL: // Ports for P0_3 is initialized from external tool
                update_uart_pads(GPIO_PORT_0, GPIO_PIN_3, GPIO_PORT_0, GPIO_PIN_3);
                break;
            case P0_5_SINGLE_W_INITIALIZED_FROM_EXT_TOOL: // Port for P0_5 is initialized from external tool
                update_uart_pads(GPIO_PORT_0, GPIO_PIN_5, GPIO_PORT_0, GPIO_PIN_5);
                break;
#endif
            case P0_6_AND_P0_7_INITIALIZED_FROM_EXT_TOOL: // Ports for UART P0_6 & P0_7 are initialized from external tool
                update_uart_pads(GPIO_PORT_0, GPIO_PIN_6, GPIO_PORT_0, GPIO_PIN_7);
                break;
            default:
                break;
        }
#elif (defined(UART1_TX_GPIO_PORT) && defined(UART1_TX_GPIO_PIN) && defined(UART1_RX_GPIO_PORT) && defined(UART1_RX_GPIO_PIN))
        update_uart_pads(UART1_TX_GPIO_PORT, UART1_TX_GPIO_PIN, UART1_RX_GPIO_PORT, UART1_RX_GPIO_PIN);
#else
    #error "=== No UART pin configuration selected in user_periph_setup.h ==="
#endif
        device_wake_up = true;
    }

    set_pad_uart();

#ifdef STANDALONE_MODE
    GPIO_SetPinFunction(GPIO_BUTTON_PORT, 
                        GPIO_BUTTON_PIN, 
                        (GPIO_BUTTON_POLARITY == WKUPCT_PIN_POLARITY_HIGH) ? INPUT_PULLDOWN : INPUT_PULLUP, 
                        PID_GPIO);
#endif
}

void set_pad_uart(void)
{
    GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_port_rx, (GPIO_PIN) uart_sel_pins.uart_rx_pin, INPUT, PID_UART1_RX, false);
    if ((uart_sel_pins.uart_port_tx != uart_sel_pins.uart_port_rx) ||
        (uart_sel_pins.uart_tx_pin != uart_sel_pins.uart_rx_pin)) {
        // Do not configure Tx pin for 1-wire UART
        GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_port_tx, (GPIO_PIN) uart_sel_pins.uart_tx_pin, OUTPUT, PID_UART1_TX, false);
    }
}

void unset_pad_uart(void)
{
    GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_port_rx, (GPIO_PIN) uart_sel_pins.uart_rx_pin, INPUT, PID_GPIO, false);
    if ((uart_sel_pins.uart_port_tx != uart_sel_pins.uart_port_rx) ||
        (uart_sel_pins.uart_tx_pin != uart_sel_pins.uart_rx_pin)) {
        // Do not configure Tx pin for 1-wire UART
        GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_port_tx, (GPIO_PIN) uart_sel_pins.uart_tx_pin, INPUT, PID_GPIO, false);
    }
}

#if defined (__DA14531__)
void set_hw_reset(void)
{
#if defined (CFG_HW_RESET_P00)
    switch(reset_mode)
    {
        case ENABLE_HW_RESET:
        {
            GPIO_DisablePorPin();
            if (GPIO_GetPinFunction(GPIO_PORT_0, GPIO_PIN_0) == FUNC_GPIO)
            {
                GPIO_SetPinFunction(GPIO_PORT_0, GPIO_PIN_0, INPUT_PULLDOWN, PID_GPIO);
                // Wait 10usec for the line to be discharged
                for (int i = 0; i < 4 * 10; i++);
                GPIO_Enable_HW_Reset();
            }
            else
            {
                // Precaution.
                GPIO_Disable_HW_Reset();
            }
            break;
        }
        case ENABLE_POR_RESET:
        {
            GPIO_Disable_HW_Reset();
            /*
                5: COLD: 511ms, 25C: 637ms, HOT: 818ms
                6: COLD: 614ms, 25C: 765ms, HOT: 982ms

                "6" was selected to work with external host that uses 1sec reset pulse.
            */
            GPIO_EnablePorPin(GPIO_PORT_0, GPIO_PIN_0, GPIO_POR_PIN_POLARITY_HIGH, 6);
            break;
        }
        default:
        case DISABLE_RESET:
        {
            GPIO_DisablePorPin();
            GPIO_Disable_HW_Reset();
            break;
        }
    }
#else
    {
        GPIO_Disable_HW_Reset();
    }
#endif
}
#endif

#if defined (__DA14531__)
void unset_rst_line(GPIO_PORT port, GPIO_PIN pin)
{
    // No action needed.
    if ((port != GPIO_PORT_0) ||
        (pin != GPIO_PIN_0))
    {
        // Do nothing.
        return;
    }

    // If 2w UART is enabled, the Tx line has to be released in order not to trigger a reset.
    if ((uart_sel_pins.uart_port_tx == GPIO_PORT_0) && (uart_sel_pins.uart_tx_pin == GPIO_PIN_0))
    {
        GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_port_tx, (GPIO_PIN) uart_sel_pins.uart_tx_pin, INPUT_PULLDOWN, PID_GPIO, false);
    }
}

void set_rst_line(GPIO_PORT port, GPIO_PIN pin)
{
    // No action needed.
    if ((port != GPIO_PORT_0) ||
        (pin != GPIO_PIN_0))
    {
        // Do nothing.
        return;
    }

    if ((uart_sel_pins.uart_port_tx != uart_sel_pins.uart_port_rx) ||
        (uart_sel_pins.uart_tx_pin != uart_sel_pins.uart_rx_pin))
    {
        // Restore Tx line for 2w UART.
        GPIO_ConfigurePin((GPIO_PORT) uart_sel_pins.uart_port_tx, (GPIO_PIN) uart_sel_pins.uart_tx_pin, OUTPUT, PID_UART1_TX, false);
    }
}

void handle_reset_state(GPIO_PORT port, GPIO_PIN pin)
{
    // Check if reset mode is enabled.
    if (reset_mode == DISABLE_RESET)
    {
        // Do nothing.
        return;
    }

    // No action needed.
    if ((port != GPIO_PORT_0) ||
        (pin != GPIO_PIN_0))
    {
        // Do nothing.
        return;
    }

    // Disable In case of 2w UART.
    if ((uart_sel_pins.uart_port_tx == GPIO_PORT_0) && (uart_sel_pins.uart_tx_pin == GPIO_PIN_0))
    {
        // Pre-caution.
        reset_mode = DISABLE_RESET;
    }

    // Set to Hw reset in case of 1w UART.
    if ((uart_sel_pins.uart_port_rx == uart_sel_pins.uart_port_tx) &&
        (uart_sel_pins.uart_rx_pin == uart_sel_pins.uart_tx_pin))
    {
        reset_mode = ENABLE_POR_RESET;
    }

    // Update reset state.
    set_hw_reset();
}
#endif

void handle_jtag(GPIO_PORT port, GPIO_PIN pin)
{
#if defined (__DA14531__)
    uint8_t debugger_enable = GetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE);

    if ( ( pin == GPIO_PIN_2 ) ||
         ( debugger_enable == 1 && (pin == GPIO_PIN_5 )  ) ||
         ( debugger_enable == 2 && (pin == GPIO_PIN_1 )  ) ||
         ( debugger_enable == 3 && (pin == GPIO_PIN_10 ) ) )
#else
    if ((port == GPIO_PORT_1) && ((pin == GPIO_PIN_4) || (pin == GPIO_PIN_5)))
#endif
    {
        // Wait until debugger is disconnected and then disable debuging
        while ((GetWord16(SYS_STAT_REG) & DBG_IS_UP) == DBG_IS_UP) {};
        SetBits16(SYS_CTRL_REG, DEBUGGER_ENABLE, 0);    // Close debugger
    }
}

void update_uart_pads(GPIO_PORT tx_port, GPIO_PIN tx_pin, GPIO_PORT rx_port, GPIO_PIN rx_pin)
{
    handle_jtag(tx_port, tx_pin);
    handle_jtag(rx_port, rx_pin);

    uart_sel_pins.uart_port_tx = tx_port;
    uart_sel_pins.uart_tx_pin = tx_pin;
    uart_sel_pins.uart_port_rx = rx_port;
    uart_sel_pins.uart_rx_pin = rx_pin;

#if defined (__DA14531__)
    if (reset_mode != DISABLE_RESET)
    {
        // Disable reset in case of 2w UART.
        if ((uart_sel_pins.uart_port_tx == GPIO_PORT_0) && (uart_sel_pins.uart_tx_pin == GPIO_PIN_0))
        {
            reset_mode = DISABLE_RESET;
        }
        // Set to HW reset in case of 1w UART.
        if ((uart_sel_pins.uart_port_rx == uart_sel_pins.uart_port_tx) &&
            (uart_sel_pins.uart_rx_pin == uart_sel_pins.uart_tx_pin))
        {
            reset_mode = ENABLE_HW_RESET;
        }
    }
    // Update reset state.
    set_hw_reset();
#endif

#if defined(CFG_UART_ONE_WIRE_SUPPORT)
    if ((tx_port == rx_port) && (tx_pin == rx_pin)) {
        uart_one_wire_enable(UART1, rx_port, rx_pin);
    }
    else {
        uart_one_wire_disable(UART1);
    }
#endif
}

#if !defined (__DA14531__)
void init_TXEN_RXEN_irqs(void)
{
    //init for TXEN
    SetBits16(RF_DIAGIRQ01_REG, DIAGIRQ_WSEL_0, 2); //SO SELECT RADIO_DIAG0
    SetBits16(RF_DIAGIRQ01_REG, DIAGIRQ_MASK_0, 1); //ENABLE IRQ
    SetBits16(RF_DIAGIRQ01_REG, DIAGIRQ_BSEL_0, 7); //BIT7 OF DIAG0 BUS, SO TXEN
    SetBits16(RF_DIAGIRQ01_REG, DIAGIRQ_EDGE_0, 0); //SELECT POS EDGE
    
    //init for RXEN
    SetBits16(RF_DIAGIRQ01_REG, DIAGIRQ_WSEL_1, 3); //SO SELECT RADIO_DIAG1
    SetBits16(RF_DIAGIRQ01_REG, DIAGIRQ_MASK_1, 1); //ENABLE IRQ
    SetBits16(RF_DIAGIRQ01_REG, DIAGIRQ_BSEL_1, 7); //BIT7 OF DIAG1 BUS, SO RXEN
    SetBits16(RF_DIAGIRQ01_REG, DIAGIRQ_EDGE_1, 0); //SELECT POS EDGE
    
    NVIC_EnableIRQ(BLE_RF_DIAG_IRQn); 
    NVIC_SetPriority(BLE_RF_DIAG_IRQn,4);     
    NVIC_ClearPendingIRQ(BLE_RF_DIAG_IRQn); //clear eventual pending bit, but not necessary becasuse this is already cleared automatically in HW
}
#endif

void periph_init(void)
{
#if !defined (__DA14531__)
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#else
#if defined (CFG_HW_RESET_P00)
    reset_mode = ENABLE_HW_RESET;
    GPIO_Enable_HW_Reset();
#else
    reset_mode = DISABLE_RESET;
    GPIO_Disable_HW_Reset();
#endif

    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);

    SetBits16(GP_ADC_CTRL_REG, GP_ADC_MUTE, 1);
#endif

    // ROM patch
    patch_func();

    // system init
    
    syscntl_use_highest_amba_clocks();            // set clocks (hclk and pclk) 16MHz
    
    SetWord16(SET_FREEZE_REG, FRZ_WDOG);          // stop watchdog

    SetBits16(CLK_PER_REG, UART1_ENABLE, 1);      // enable clock - always @16MHz

    syscntl_load_debugger_cfg();

    // Reset uart
    SetWord16(UART_SRR_REG, 7);

    // disable fifo
    SetWord16(UART_IIR_FCR_REG, 0);

    // Disable interrupts
    SetWord16(UART_IER_DLH_REG, 0);

    SetWord16(UART_LCR_REG, 0x83);  /// Set bit 8 om LCR in order to access DivisorLatch register

    // Initialize UART
    if (baud_rate_sel == 0)
    {
        baud_rate_sel = UART_BAUDRATE_115K2;
        baud_rate_frac_sel = UART_FRAC_BAUDRATE_115K2;
    }
    uart_init(baud_rate_sel, baud_rate_frac_sel, UART_CHARFORMAT_8);
    SetWord16(UART_LCR_REG, 3);  // no parity, 1 stop bit 8 data length and clear bit 8

#if defined (CFG_PRINTF_UART2)
    uart_cfg_t uart_cfg = {
        .baud_rate = UART2_BAUDRATE,
        .data_bits = UART2_DATABITS,
        .parity = UART2_PARITY,
        .stop_bits = UART2_STOPBITS,
        .auto_flow_control = UART2_AFCE,
        .use_fifo = UART2_FIFO,
        .tx_fifo_tr_lvl = UART2_TX_FIFO_LEVEL,
        .rx_fifo_tr_lvl = UART2_RX_FIFO_LEVEL,
        .intr_priority = 0,
    };
    uart_initialize(UART2, &uart_cfg);
#endif

    // Set pad functionality
    set_pad_functions();

    // Enable the pads
    GPIO_set_pad_latch_en(true);
}

