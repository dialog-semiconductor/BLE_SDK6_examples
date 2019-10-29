/**
 ****************************************************************************************
 *
 * @file user_hibernation.c
 *
 * @brief Proximity reporter project source code.
 *
 * Copyright (c) 2015-2019 Dialog Semiconductor. All rights reserved.
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
#include "arch_api.h"
#include "app_task.h"
#include "app_proxr.h"

#include "gpio.h"

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
 * @return void
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
 * @return void
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
 * @return void
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

#if defined (__DA14531__)

#if defined (CFG_EXT_SLEEP_WAKEUP_RTC) || defined (CFG_DEEP_SLEEP_WAKEUP_RTC)
/**
 ****************************************************************************************
 * @brief RTC interrupt handler routine for wakeup.
 * @return void
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
 * @return void
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
 * @return void
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
 * @return void
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

#if defined (CFG_APP_GOTO_DEEP_SLEEP)
/**
 ****************************************************************************************
 * @brief Put the system into deep sleep mode. It demonstrates the deep sleep mode usage
 *        and how the system can wake up from it. The exit from the deep sleep state causes 
 *        a system reboot.
 * @note  The system can wake up from deep sleep by:
 *          - external wake up interrupt, caused e.g. by button press (properly configured GPIO pin)
 *          - power on reset, caused e.g. by button press (properly configured GPIO pin)
 *          - interrupt generated from RTC
 *          - interrupt generated from Timer1
 *        When the system exits deep sleep state, the boot process is triggered.
 *        The application code has to be programmed in an external memory resource or
 *        in the OTP memory, in order for the system to reboot properly.
 * @return void
 ****************************************************************************************
*/
static void put_system_into_deep_sleep(void)
{
#if defined (CFG_DEEP_SLEEP_WAKEUP_POR)
    // Configure button for POR
    GPIO_EnablePorPin(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, GPIO_POR_PIN_POLARITY_LOW, GPIO_GetPorTime());
#endif

#if defined (CFG_DEEP_SLEEP_WAKEUP_GPIO)
    wkupct_enable_irq(WKUPCT_PIN_SELECT(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN), // Select pin
                      WKUPCT_PIN_POLARITY(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, WKUPCT_PIN_POLARITY_LOW), // Polarity low
                      1, // 1 event
                      0); // debouncing time = 0
#endif

#if defined (CFG_DEEP_SLEEP_WAKEUP_RTC)
    configure_rtc_wakeup();
#endif

#if defined (CFG_DEEP_SLEEP_WAKEUP_TIMER1)
    configure_timer1_wakeup();
#endif

    // Go to deep sleep
    arch_set_deep_sleep(CFG_DEEP_SLEEP_RAM1,
                        CFG_DEEP_SLEEP_RAM2,
                        CFG_DEEP_SLEEP_RAM3,
                        CFG_DEEP_SLEEP_PAD_LATCH_EN);
}
#endif // (CFG_APP_GOTO_DEEP_SLEEP)

#else

/**
 ****************************************************************************************
 * @brief Put the system into deep sleep mode. It demonstrates the deep sleep mode usage
 *        and how the system can wake up from it. Once the system enters deep sleep state
 *        it retains NO RAM blocks. The exit from the deep sleep state causes a system
 *        reboot.
 * @note  The system can wake up from deep sleep by:
 *          - external wake up interrupt, caused e.g. by button press (properly configured GPIO pin)
 *          - power on reset, caused e.g. by button press (properly configured GPIO pin)
 *          - H/W reset button press or power cycle (at any time)
 *        When the system exits deep sleep state, the boot process is triggered.
 *        The application code has to be programmed in an external memory resource or
 *        in the OTP memory, in order for the system to reboot properly.
 * @return void
 ****************************************************************************************
*/
void put_system_into_deep_sleep(void)
{
#if defined (CFG_DEEP_SLEEP_WAKEUP_GPIO)
    // Configure button for wake-up interrupt
    app_button_enable();

    // Set deep sleep - external interrupt wake up
    arch_set_deep_sleep(true);

#elif defined (CFG_DEEP_SLEEP_WAKEUP_POR)
    // Configure button for POR
    GPIO_EnablePorPin(GPIO_BUTTON_PORT, GPIO_BUTTON_PIN, GPIO_POR_PIN_POLARITY_LOW, GPIO_GetPorTime());

    // Set deep sleep - POR wake up
    arch_set_deep_sleep(false);

#else
    // Do nothing.
    // The system will eventually enter the selected Extended sleep state.
    // A button press will wake up the system if the respective GPIO is configured as a wake up interrupt.
#endif
}

#endif

void app_advertise_complete(const uint8_t status)
{
#if (BLE_PROX_REPORTER)
    app_proxr_alert_stop();
#endif

    // Disable wakeup for BLE and timer events. Only external (GPIO) wakeup events can wakeup processor.
    if (status == GAP_ERR_CANCELED)
    {
        arch_ble_ext_wakeup_on();
    }

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

#if defined (CFG_APP_GOTO_DEEP_SLEEP)
    // Put system into deep sleep
    put_system_into_deep_sleep();
#elif defined (__DA14531__) && defined (CFG_APP_GOTO_HIBERNATION)
    // Put system into hibernation
    arch_set_hibernation(HIB_WAKE_UP_PIN_MASK,
                         CFG_HIBERNATION_RAM1,
                         CFG_HIBERNATION_RAM2,
                         CFG_HIBERNATION_RAM3,
                         CFG_HIBERNATION_REMAP,
                         CFG_HIBERNATION_PAD_LATCH_EN
                        );
												
		//power down the flash after entering hibernation mode										
		spi_flash_power_down();

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

/// @} APP
