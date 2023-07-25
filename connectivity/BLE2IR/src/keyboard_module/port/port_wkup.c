/**
 ****************************************************************************************
 *
 * \file port_wkup.c
 *
 * \brief Wake-up controller functions
 *
 * Define symbol HAS_WKUP to include this module in the application.
 *
 * Copyright (C) 2017-2021 Dialog Semiconductor
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \addtogroup APP_UTILS
 * \{
 * \addtogroup WAKE_UP
 * \{
 * \addtogroup PORT_WKUP
 * \{
 ****************************************************************************************	 
 */

#ifdef HAS_WKUP

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "port_wkup.h"
#include "port_platform.h"
#include <user_periph_setup.h>

uint32_t wkup_pin_config __PORT_RETAINED;
uint32_t wkup_pol_config __PORT_RETAINED;

bool wkup_channel_enabled[NUM_OF_WKUP_CHANNELS] __PORT_RETAINED;
uint32_t wkup_mask[NUM_OF_WKUP_CHANNELS]        __PORT_RETAINED;

void port_wkup_handler(void)
{
    if (GetBits16(SYS_STAT_REG, PER_IS_DOWN)) {
        periph_init();
    }  
        
    bool continue_with_masks = true;
    
    for(int i=0; i<NUM_OF_WKUP_CHANNELS; i++) {
        if(wkup_channel_enabled[i] == true && wkup_config[i].single_pin_input == true) {
            bool status = GPIO_GetPinStatus(wkup_config[i].config.pin_config.port, wkup_config[i].config.pin_config.pin);
            if((wkup_config[i].config.pin_config.polarity == WKUPCT_PIN_POLARITY_LOW) == (status == 0)) {
                port_wkup_disable_irq((enum port_wkup_channel)i);
                wkup_config[i].callback();  
#ifndef WKUP_ALWAYS_CONTINUE_WITH_MASKS                
                continue_with_masks = false;
#endif                
            }
        }
    }
    
    if(continue_with_masks == true) {
        for(int i=0; i<NUM_OF_WKUP_CHANNELS; i++) {
            if(wkup_channel_enabled[i] == true && wkup_config[i].single_pin_input == false) {
                port_wkup_disable_irq((enum port_wkup_channel)i);
                wkup_config[i].callback();  
            }
        }
    }
    
    if(port_wkup_activity_callback != NULL) {
        port_wkup_activity_callback();
    }
}

void port_wkup_init(void)
{
    for(int i = 0; i < NUM_OF_WKUP_CHANNELS; i++) {
        if(wkup_config[i].single_pin_input == true) {
            wkup_mask[i] = WKUPCT_PIN_SELECT(wkup_config[i].config.pin_config.port, wkup_config[i].config.pin_config.pin);
        }
        else {
            wkup_mask[i] = wkup_config[i].config.pin_mask;
        }
    }
    
    wkup_pin_config = 0;
    wkup_pol_config = 0;
   	wkupct_disable_irq();
    wkupct_register_callback(port_wkup_handler);
}

void port_wkup_enable_irq(enum port_wkup_channel channel, uint32_t pol_pins)
{
		port_wkup_init();
    uint32_t mask = wkup_mask[channel];
    
    wkupct_disable_irq(); 
    ASSERT_ERROR(channel < NUM_OF_WKUP_CHANNELS);
    ASSERT_ERROR(mask != 0); // port_wkup_init() must be called before port_wkup_enable_irq()
    ASSERT_ERROR( (wkup_pin_config & mask) == 0); // Pins in mask must not be in use 
    
    wkup_channel_enabled[channel] = true;
    wkup_pin_config |= mask;
    wkup_pol_config = (wkup_pol_config & ~mask) | (pol_pins & mask);
    
    wkupct_enable_irq(wkup_pin_config, wkup_pol_config, 1, WKUP_DEBOUNCE_TIME);
}

void port_wkup_disable_irq(enum port_wkup_channel channel)
{
    wkup_channel_enabled[channel] = false;
    wkup_pin_config &= ~wkup_mask[channel];
    
    if(wkup_pin_config == 0) {
        wkupct_disable_irq();   
    }
    else {
        wkupct_enable_irq(wkup_pin_config, wkup_pol_config, 1, WKUP_DEBOUNCE_TIME);
    }
}

#endif

/**
 * \}
 * \}
 * \}
 */
