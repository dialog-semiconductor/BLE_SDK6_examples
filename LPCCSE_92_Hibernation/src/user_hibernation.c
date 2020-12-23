/**
 ****************************************************************************************
 *
 * @file user_proxr.c
 *
 * @brief Proximity reporter project source code.
 *
 * Copyright (C) 2015-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "gapc_task.h"
#include "user_periph_setup.h"
#include "wkupct_quadec.h"
#include "app_easy_msg_utils.h"
#include "gpio.h"
#include "app_security.h"
#include "user_hibernation.h"
#include "arch.h"
#include "arch_api.h"
#if defined (__DA14531__) && (defined (CFG_APP_GOTO_HIBERNATION) || defined (CFG_APP_GOTO_STATEFUL_HIBERNATION))
#include "arch_hibernation.h"
#endif
#include "app_task.h"
#include "app_proxr.h"

#if defined (__DA14531__)
#include "rtc.h"
#include "timer1.h"
#endif

#if (BLE_SUOTA_RECEIVER)
#include "app_suotar.h"
#endif

#if defined (CFG_SPI_FLASH_ENABLE)
#include "spi_flash.h"
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles APP_WAKEUP_MSG sent when device exits deep sleep. Triggered by button press.
 ****************************************************************************************
*/
static void app_wakeup_cb(void)
{
    // If state is not idle, ignore the message
    if (ke_state_get(TASK_APP) == APP_CONNECTABLE)
    {
        default_advertise_operation();
    }
}

/**
 ****************************************************************************************
 * @brief Routine to resume system from sleep state.
 ****************************************************************************************
 */
static void app_resume_system_from_sleep(void)
{
#if !defined (__DA14531__)
    if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))
    {
        periph_init();
    }
#endif

    if (arch_ble_ext_wakeup_get())
    {
        arch_set_sleep_mode(app_default_sleep_mode);
        arch_ble_force_wakeup();
        arch_ble_ext_wakeup_off();
        app_easy_wakeup();
    }
}

/**
 ****************************************************************************************
 * @brief Button press callback function. Registered in WKUPCT driver.
 ****************************************************************************************
 */
static void app_button_press_cb(void)
{
#if (BLE_PROX_REPORTER)
    if (alert_state.lvl != PROXR_ALERT_NONE)
    {
        app_proxr_alert_stop();
    }
#endif
    app_resume_system_from_sleep();

    app_button_enable();
}

void app_button_enable(void)
{
    app_easy_wakeup_set(app_wakeup_cb);
    wkupct_register_callback(app_button_press_cb);

    if (GPIO_GetPinStatus(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN))
    {
        wkupct_enable_irq(WKUPCT_PIN_SELECT(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN), // select pin (GPIO_BUTTON_PORT, GPIO_BUTTON_PIN)
                          WKUPCT_PIN_POLARITY(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, WKUPCT_PIN_POLARITY_LOW), // polarity low
                          1, // 1 event
                          0); // debouncing time = 0
    }
}

#if (BLE_SUOTA_RECEIVER)
void on_suotar_status_change(const uint8_t suotar_event)
{
#if (!SUOTAR_SPI_DISABLE)
    uint8_t dev_id;

    // Release Flash from power down
    spi_flash_release_from_power_down();

    // Try to auto-detect the device
    spi_flash_auto_detect(&dev_id);

    if (suotar_event == SUOTAR_END)
    {
        // Power down SPI Flash
        spi_flash_power_down();
    }
#endif
}
#endif

void user_app_on_disconnect(struct gapc_disconnect_ind const *param)
{
    default_app_on_disconnect(NULL);

#if (BLE_BATT_SERVER)
    app_batt_poll_stop();
#endif

#if (BLE_SUOTA_RECEIVER)
    // Issue a platform reset when it is requested by the suotar procedure
    if (suota_state.reboot_requested)
    {
        // Reboot request will be served
        suota_state.reboot_requested = 0;

        // Platform reset
        platform_reset(RESET_AFTER_SUOTA_UPDATE);
    }
#endif

#if BLE_PROX_REPORTER
    app_proxr_alert_stop();
#endif
}

//#if defined (__DA14531__)

#if defined (CFG_EXT_SLEEP_WAKEUP_RTC) || defined (CFG_DEEP_SLEEP_WAKEUP_RTC)
/**
 ****************************************************************************************
 * @brief RTC interrupt handler routine for wakeup.
 ****************************************************************************************
*/

static void rtc_interrupt_hdlr(uint8_t event)
{
#if defined (CFG_EXT_SLEEP_WAKEUP_RTC)
    app_resume_system_from_sleep();
#endif
}

/**
 ****************************************************************************************
 * @brief Configure RTC to generate an interrupt after 10 seconds.
 ****************************************************************************************
*/
static void configure_rtc_wakeup(void)
{
    rtc_time_t alarm_time;

    // Init RTC
    rtc_reset();

    // Configure the RTC clock; RCX is the RTC clock source (14420 Hz)
    rtc_clk_config(RTC_DIV_DENOM_1000, 14420);
    rtc_clock_enable();

    rtc_config_t cfg = {.hour_clk_mode = RTC_HOUR_MODE_24H, .keep_rtc = 0};

    rtc_time_t time = {.hour_mode = RTC_HOUR_MODE_24H, .pm_flag = 0, .hour = 11,
                       .minute = 55, .sec = 30, .hsec = 00};

    // Alarm interrupt in ten seconds
    alarm_time = time;
    alarm_time.sec += 10;

    // Initialize RTC, set time and data, register interrupt handler callback function and enable seconds interrupt
    rtc_init(&cfg);

    // Start RTC
    rtc_set_time_clndr(&time, NULL);
    rtc_set_alarm(&alarm_time, NULL, RTC_ALARM_EN_SEC);

    // Clear pending interrupts
    rtc_get_event_flags();
    rtc_register_intr(rtc_interrupt_hdlr, RTC_INTR_ALRM);
#if defined (CFG_EXT_SLEEP_WAKEUP_RTC)
    app_easy_wakeup_set(app_wakeup_cb);
#endif
}
#endif

#if defined (CFG_EXT_SLEEP_WAKEUP_TIMER1) || defined (CFG_DEEP_SLEEP_WAKEUP_TIMER1)
/**
 ****************************************************************************************
 * @brief Timer1 interrupt handler routine for wakeup.
 ****************************************************************************************
*/

static void timer1_interrupt_hdlr(void)
{
#if defined (CFG_EXT_SLEEP_WAKEUP_TIMER1)
    app_resume_system_from_sleep();
#endif
}

/**
 ****************************************************************************************
 * @brief Configure Timer1 to generate an interrupt when it reaches its max value.
 ****************************************************************************************
*/
static void configure_timer1_wakeup(void)
{
    timer1_count_options_t count_options = {.input_clk = TIM1_CLK_SRC_LP,
                                            .free_run = TIM1_FREE_RUN_ON,
                                            .irq_mask = TIM1_IRQ_MASK_OFF,
                                            .count_dir = TIM1_CNT_DIR_UP,
                                            .reload_val = TIM1_RELOAD_MAX,
    };
    // Enable Timer1 interrupt
    timer1_enable_irq();
#if defined (CFG_EXT_SLEEP_WAKEUP_TIMER1)
    app_easy_wakeup_set(app_wakeup_cb);
#endif
    timer1_count_config(&count_options, timer1_interrupt_hdlr);

    // Start the Timer
    timer1_start();
}
#endif

void user_app_on_init(void)
{
	spi_flash_power_down();
	
	default_app_on_init();
}


void app_advertise_complete(const uint8_t status)
{
    if ((status == GAP_ERR_NO_ERROR) || (status == GAP_ERR_CANCELED))
    {

#if (BLE_PROX_REPORTER)
        app_proxr_alert_stop();
#endif
    }

    if (status == GAP_ERR_CANCELED)
    {
        arch_ble_ext_wakeup_on();

#if defined (__DA14531__)
    // Configure PD_TIM
#if defined (CFG_EXT_SLEEP_WAKEUP_RTC) || defined (CFG_EXT_SLEEP_WAKEUP_TIMER1) || \
    defined (CFG_DEEP_SLEEP_WAKEUP_RTC) || defined (CFG_DEEP_SLEEP_WAKEUP_TIMER1)
        // Ensure PD_TIM is open
        SetBits16(PMU_CTRL_REG, TIM_SLEEP, 0);
        // Wait until PD_TIM is opened
        while ((GetWord16(SYS_STAT_REG) & TIM_IS_UP) != TIM_IS_UP);
#else
        // Close PD_TIM
        SetBits16(PMU_CTRL_REG, TIM_SLEEP, 1);
        // Wait until PD_TIM is closed
        while ((GetWord16(SYS_STAT_REG) & TIM_IS_DOWN) != TIM_IS_DOWN);
#endif
#endif


#if defined (__DA14531__) && defined (CFG_APP_GOTO_HIBERNATION)
			
				//powering down flash before entering hibernation 
				spi_flash_power_down(); 			
				
        // Put system into hibernation
        arch_set_hibernation(HIB_WAKE_UP_PIN_MASK,
                             CFG_HIBERNATION_RAM1,
                             CFG_HIBERNATION_RAM2,
                             CFG_HIBERNATION_RAM3,
                             CFG_HIBERNATION_REMAP,
                             CFG_HIBERNATION_PAD_LATCH_EN);
#elif defined (__DA14531__) && defined (CFG_APP_GOTO_STATEFUL_HIBERNATION)
       
				
				//powering down flash before entering state-aware hibernation
				spi_flash_power_down(); 	
				
				 // Put system into stateful hibernation
        arch_set_stateful_hibernation(HIB_WAKE_UP_PIN_MASK,
                                      CFG_STATEFUL_HIBERNATION_RAM1,
                                      CFG_STATEFUL_HIBERNATION_RAM2,
                                      CFG_STATEFUL_HIBERNATION_RAM3,
                                      CFG_STATEFUL_HIBERNATION_REMAP,
                                      CFG_STATEFUL_HIBERNATION_PAD_LATCH_EN);
        #if (DEVELOPMENT_DEBUG)
            // Turn on the orange LED (D5 on the 376-18-B Motherboard)
            SetWord16(P09_MODE_REG, ((uint32_t) OUTPUT) | ((uint32_t) PID_GPIO));
            SetWord16(P0_SET_DATA_REG, 1 << GPIO_ALERT_LED_PIN);
            // Keep it on for a couple of seconds
            for (uint32_t i = 4*2000000; i != 0; i--)
            {
                __NOP();
            }
            // Turn it off
            SetWord16(P0_RESET_DATA_REG, 1 << GPIO_ALERT_LED_PIN);
        #endif // DEVELOPMENT_DEBUG

        // Configure button to trigger wake-up interrupt from extended sleep
        app_button_enable();
#elif defined (__DA14531__) && defined (CFG_EXT_SLEEP_WAKEUP_RTC)
        // Configure RTC to trigger wake-up interrupt from extended sleep
        configure_rtc_wakeup();
#elif defined (__DA14531__) && defined (CFG_EXT_SLEEP_WAKEUP_TIMER1)
        // Configure TIMER1 to trigger wake-up interrupt from extended sleep
        configure_timer1_wakeup();
#else
        // Configure button to trigger wake-up interrupt from extended sleep
        app_button_enable();
#endif
    }
}

/// @} APP
