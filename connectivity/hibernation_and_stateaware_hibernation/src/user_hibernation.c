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
#include "user_hibernation.h"
#include "arch.h"
#include "arch_api.h"
#include "arch_hibernation.h"
#include "app_task.h"
#include "app_proxr.h"


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

void user_app_on_disconnect(struct gapc_disconnect_ind const *param)
{
    default_app_on_disconnect(NULL);


#if BLE_PROX_REPORTER
    app_proxr_alert_stop();
#endif
}

//#if defined (__DA14531__)


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
		// Close PD_TIM
		SetBits16(PMU_CTRL_REG, TIM_SLEEP, 1);
		// Wait until PD_TIM is closed
		while ((GetWord16(SYS_STAT_REG) & TIM_IS_DOWN) != TIM_IS_DOWN);
#endif

#if defined (__DA14531__) && defined (CFG_APP_GOTO_HIBERNATION)
			
				// Put system into hibernation
        arch_set_hibernation(HIB_WAKE_UP_PIN_MASK,
                             CFG_HIBERNATION_RAM1,
                             CFG_HIBERNATION_RAM2,
                             CFG_HIBERNATION_RAM3,
                             CFG_HIBERNATION_REMAP,
                             CFG_HIBERNATION_PAD_LATCH_EN);
#elif defined (__DA14531__) && defined (CFG_APP_GOTO_STATEFUL_HIBERNATION)
       
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

        // Configure button to trigger wake-up interrupt from extended sleep
        app_button_enable();
#endif
    }
}

/// @} APP
