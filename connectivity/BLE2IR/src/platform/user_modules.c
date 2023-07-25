/**
 ****************************************************************************************
 *
 * \file user_modules.c
 *
 * \brief
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
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * \addtogroup USER
 * \{
 * \addtogroup USER_APP
 * \{
 * \addtogroup USER_MODULES
 *
 * \{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "user_modules.h"
#include <user_modules_config.h>


#define CALL_CALLBACK(cb) {if (cb != NULL) cb();}

extern volatile uint64_t GPIO_status;
extern int GPIO[NO_OF_PORTS][NO_OF_MAX_PINS_PER_PORT];

void user_modules_reserve_gpios(void)
{
    for(int i = 0; i < sizeof(user_module_config)/sizeof(module_config_t); i++) {
        pin_type_t *pin = user_module_config[i].pins_config.pins;
        if (pin != NULL){      
            for(int j = 0; j < user_module_config[i].pins_config.num_of_pins; j++) {
                PORT_RESERVE_GPIO(pin[j]);
            }
        }
    }
}

void user_modules_init_gpios(void)
{
    for(int i = 0; i < sizeof(user_module_config)/sizeof(module_config_t); i++) {
        if(user_module_config[i].init_gpios != NULL) {
            user_module_config[i].init_gpios();
        }
        else {
            pin_type_t *pin = user_module_config[i].pins_config.pins;
            if(pin != NULL) {
                for(int j = 0; j < user_module_config[i].pins_config.num_of_pins; j++) {
                    PORT_CONFIGURE_GPIO_DEFAULT(pin[j]);
                }
            }
        }
    }
}

void user_modules_init(void)
{
    for(int i = 0; i < sizeof(user_module_config)/sizeof(module_config_t); i++) {
        CALL_CALLBACK(user_module_config[i].init);
    }
}

void user_modules_on_disconnect(void)
{
    for(int i = 0; i < sizeof(user_module_config)/sizeof(module_config_t); i++) {
        CALL_CALLBACK(user_module_config[i].on_disconnect);
    }
}

bool user_modules_on_ble_powered(void)
{
    bool keep_powered = false;
    for(int i = 0; i < sizeof(user_module_config)/sizeof(module_config_t); i++) {
        if(user_module_config[i].on_ble_powered != NULL) {
            keep_powered |= user_module_config[i].on_ble_powered();
        }
    }
    
    return keep_powered;
}

uint8_t user_modules_on_system_powered(void)
{
    uint8_t power_status = false;
    for(int i = 0; i < sizeof(user_module_config)/sizeof(module_config_t); i++) {
        if(user_module_config[i].on_system_powered != NULL) {
            power_status |= user_module_config[i].on_system_powered();
        }
    }
    
    return power_status;
}

/**
 * \}
 * \}
 * \}
 */
