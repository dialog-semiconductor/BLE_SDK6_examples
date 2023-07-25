/**
 ****************************************************************************************
 *
 * \file app_kbd_config.h
 *
 * \brief Keyboard module configuration header file.
 *
 * Copyright (C) 2017-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */

#ifndef _APP_KBD_CONFIG_H_
#define _APP_KBD_CONFIG_H_

/**
 ****************************************************************************************
 * \addtogroup CONFIGURATION
 * \{
 * \addtogroup MODULE_CONFIG
 * \{
 * \addtogroup KEYBOARD_CFG
 *
 * \brief Keyboard module configuration
 * \{
 ****************************************************************************************
 */

#include "app_kbd_defs.h"
#include "port_kbd.h"
#include "user_rcu_example.h"

/**
 ****************************************************************************************
 * \brief Keyboard application parameters
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * Set the size of the keycode buffer. If set to more than 255 then the read and write 
 * pointers are changed from 8-bit to 16-bit wide.
 ****************************************************************************************
 */
#define KBD_KEYCODE_BUFFER_SIZE   32

/**
 ****************************************************************************************
 * Set the number of keys that can be debounced at the same time. 
 ****************************************************************************************
 */
#define KBD_DEBOUNCE_BUFFER_SIZE  5

/*
 * Set the arrays used by the application to be placed in .constdata section.
 * If not defined, the arrays will be put in the ".data" section which can optionally be
 * compressed and placed elsewhere.
 * WARNING: If not defined then the following options are available:
 *     - uncompress data in RetRAM once at startup
 *     - uncompress data in SysRAM once at startup and use Extended Sleep (done automatically if EXT_SLEEP_SETUP is used)
 *     - uncompress data in SysRAM at startup and after every Deep Sleep wakeup
 */
#undef ARRAYS_AS_CONST_DATA

/**
 ****************************************************************************************
 * Use different Full and Partial scan periods                                          
 ****************************************************************************************
 */
#undef ALTERNATIVE_SCAN_TIMES_ON

static const kbd_params_t kbd_params = {
/**
 ****************************************************************************************
 * Continuous key scanning. The chip won't go to sleep in this case.                    
 ****************************************************************************************
 */
    .scan_always_active  = false,
/**
 ****************************************************************************************
 * Delayed wakeup (requires pressing a key for X ms to start)                           
 ****************************************************************************************
 */
    .delayed_wakeup      = false,
/**
 ****************************************************************************************
 * Time in usec to hold down a key for the system to wake-up when 
 * kbd_params.delayed_wakeup is true
 ****************************************************************************************
 */
    .delayed_wakeup_time_in_us = 2000,
        
/**
 ****************************************************************************************
 * Function lock key Fn is locked when pressed once                                     
 ****************************************************************************************
 */
    .has_fn_lock         = false,

/**
 ****************************************************************************************
 * Custom key used to enter the passcode in addition to keyboard and keypad "enter" keys
 ****************************************************************************************
 */
    .passcode_enter_key  = NULL,

/**
 ****************************************************************************************
 *  This function sends notifications to the application
 ****************************************************************************************
 */    
    .notify_callback     = user_kbd_notification_cb,

/**
 ****************************************************************************************
 * This function reportskey events to the application
 ****************************************************************************************
 */    
#ifdef HAS_IR    
    .key_detect_callback = user_kbd_key_detected_cb,
#else
    .key_detect_callback = NULL,
#endif    

/**
 ****************************************************************************************
 * Time in usec from row activation until the column inputs are sampled.
 ****************************************************************************************
 */
    .row_scan_time_in_us      = 150,

/**
 ****************************************************************************************
 * Time in usec for a full scan cycle. All rows in are scanned in a full scan cycle. 
 ****************************************************************************************
 */
#ifdef ALTERNATIVE_SCAN_TIMES_ON
    .full_scan_cycle_in_us    = 2000,
#else
    .full_scan_cycle_in_us    = 3000,
#endif    
    
/**
 ****************************************************************************************
 * Time in usec for a partial scan cycle. Only rows having key activity are scanned. The
 * remaining idle rows are monitored for key activity. When a key of an idle row is 
 * pressed a full scan cycle is initiated.
 ****************************************************************************************
 */
#ifdef ALTERNATIVE_SCAN_TIMES_ON
    .partial_scan_cycle_in_us = 1000,
#else
    .partial_scan_cycle_in_us = 3000,
#endif

/**
 ****************************************************************************************
 * In general, debounce counters cannot be applied accurately. The reason is that 
 * debouncing is done in SW, based on SysTick interrupt events that are used to 
 * trigger the execution of the FSM.
 * Thus, in the final application it is suggested to modify the definitions of the two
 * counters (below) so that the real debouncing time is applied accurately in all
 * cases. First, key presses need to be examined. It should be checked whether the
 * debouncing period is correct both after wake-up (when the RC16 is used which is 
 * approx. 14.5MHz) and when a 2nd key is pressed (assuming that in this case the BLE 
 * core woke up to send the HID report of the 1st key press, which means that the 
 * system runs with XTAL16).
 * Release debouncing period must also be enforced. In all cases, though, the XTAL16 clk
 * is used and there are no special cases.
 * Must be multiple of partial_scan_cycle_in_us.
 ****************************************************************************************
 */    
    .press_debounce_counter_in_us   = 12000,
    .release_debounce_counter_in_us = 24000,    
};

static const kbd_scan_util_funcs_t app_kbd_scan_funcs = {
    .init                    = port_kbd_scan_init,                   
    .stop                    = port_kbd_scan_stop,                   
    .is_idle                 = port_kbd_scan_is_idle,                
    .is_active               = port_kbd_scan_is_active,              
    .init_scan_gpios         = port_kbd_init_scan_gpios,          
    .get_new_scan_data       = port_kbd_scan_get_new_scan_data,      
    .get_scan_data           = port_kbd_scan_get_scan_data,          
    .fsm_update              = port_kbd_scan_fsm_update,             
    .enable_delayed_scanning = port_kbd_scan_enable_delayed_scanning,
    .delayed_start_proc      = port_kbd_scan_delayed_start_proc,     
};  

/**
 * \}
 * \}
 * \}
 */

#endif // _APP_KBD_CONFIG_H_
