/**
 ****************************************************************************************
 *
 * \file port_wkup_config.h
 *
 * \brief  WAKEUP controller configuration file
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
 */
 
#ifndef _PORT_WKUP_CONFIG_H_
#define _PORT_WKUP_CONFIG_H_

/**
 ****************************************************************************************
 * \addtogroup CONFIGURATION
 * \{
 * \addtogroup MODULE_CONFIG
 * \{
 * \addtogroup WKUP_CFG
 *
 * \brief Wake-up controller module configuration
 * \{
 ****************************************************************************************
 */

#include "port_wkup_defs.h"

#ifdef HAS_KBD
    #include "port_kbd.h"
#endif

#ifdef HAS_GPIO_KEYS 
#include "port_gpio_keys.h"
#include "app_gpio_keys.h"
#endif
#include "user_rcu_example.h"

#if defined(HAS_TOUCHPAD_TRACKPAD) || defined(HAS_TOUCHPAD_SLIDER)
    #include "port_touchpad.h"
    #include <app_touchpad_config.h>
#endif

/**
 ****************************************************************************************
 * \brief Define WKUP_ALWAYS_CONTINUE_WITH_MASKS to always call the callbacks of channels
 *        having single_pin_input = false, even when a single-pin channel wakeup source
 *        has been found.
 ****************************************************************************************
 */
#undef WKUP_ALWAYS_CONTINUE_WITH_MASKS

enum port_wkup_channel {
#ifdef HAS_KBD           
    WKUP_KBD_CHANNEL,
    #ifdef HAS_POWERUP_BUTTON	    
    WKUP_KBD_POWER_BUTTON_CHANNEL,
    #endif    
#endif
    
#if defined(HAS_TOUCHPAD_TRACKPAD) || defined(HAS_TOUCHPAD_SLIDER)
    WKUP_TOUCHPAD_CHANNEL,
#endif    

#ifdef HAS_GPIO_KEYS    
    WKUP_GPIO_KEYS_CHANNEL,
#endif    

    NUM_OF_WKUP_CHANNELS  // Do not remove. Must always be the last entry.
};

static const wkup_config_t wkup_config[] = {
#ifdef HAS_KBD
    [WKUP_KBD_CHANNEL] = {
        .single_pin_input = false,
        .callback         = port_kbd_scan_wakeup_handler,
        .config.pin_mask  = KBD_WKUP_MASK,
    },
		
    #ifdef HAS_POWERUP_BUTTON	    
    [WKUP_KBD_POWER_BUTTON_CHANNEL] = {
        .single_pin_input           = true,
        .callback                   = port_kbd_scan_wakeup_handler,
        .config.pin_config.port     = KBD_POWER_BUTTON_PORT,
        .config.pin_config.pin      = KBD_POWER_BUTTON_PIN,
        .config.pin_config.polarity = WKUPCT_PIN_POLARITY_LOW,
    },
    #endif    
#endif    
    
#ifdef HAS_GPIO_KEYS  
    [WKUP_GPIO_KEYS_CHANNEL] = {
        .single_pin_input = false,
        .callback         = port_gpio_keys_wakeup_handler,
        .config.pin_mask  = GPIO_WKUP_MASK,
    },        
#endif
    
#if defined(HAS_TOUCHPAD_TRACKPAD) || defined(HAS_TOUCHPAD_SLIDER)
    [WKUP_TOUCHPAD_CHANNEL] = {
        .single_pin_input           = true,
        .callback                   = port_touchpad_handler,
        .config.pin_config.port     = TOUCHPAD_INT_PORT,
        .config.pin_config.pin      = TOUCHPAD_INT_PIN,
        .config.pin_config.polarity = TOUCHPAD_INT_POLARITY,
    },
#endif
    {0},
};

#ifdef HAS_ACTION_INACTIVITY_TIMEOUT
    static const wakeup_handler_function_t port_wkup_activity_callback = user_wakeup_activity_callback;
#else
    static const wakeup_handler_function_t port_wkup_activity_callback = NULL;
#endif

#define WKUP_DEBOUNCE_TIME (0)         // in msec

/**
 * \}
 * \}
 * \}
 */

#endif	// _PORT_WKUP_CONFIG_H_