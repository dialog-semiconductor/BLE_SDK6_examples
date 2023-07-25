/**
 ****************************************************************************************
 *
 * \file port_wkup_defs.h
 *
 * \brief Wake-up controller functions
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
 
#ifndef PORT_WKUP_DEFS_H_
#define PORT_WKUP_DEFS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "wkupct_quadec.h"
#include "stdint.h"
#include "gpio.h"

typedef void (*wkup_handler_function_t)(void);

typedef struct wkup_pin_config_s {
    GPIO_PORT port;
    GPIO_PIN  pin;
    uint8_t   polarity;
} wkup_pin_config_t;

typedef union {
    uint32_t pin_mask;
    wkup_pin_config_t pin_config;
} wkup_config_union_t;

typedef struct wkup_config_s {
    bool single_pin_input;
    wkup_config_union_t config;
    wkup_handler_function_t callback;
} wkup_config_t;

/**
 ****************************************************************************************
 * \brief
 *
 * \param[in]   port_0
 * \param[in]   port_1
 * \param[in]   port_2
 * \param[in]   port_3
 *
 * \return
 ****************************************************************************************
 */
#define WKUP_MASK(port_0, port_1, port_2, port_3) ( port_0                 | (port_1<<8)         | \
                                                  ((port_2&0x300)<<(14-8)) | ((port_2&0xFF)<<16) | \
                                                   (port_3<<24) )

#endif // PORT_WKUP_DEFS_H_

/**
 * \}
 * \}
 * \}
 */
