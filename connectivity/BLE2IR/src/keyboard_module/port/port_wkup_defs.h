/**
 ****************************************************************************************
 *
 * \file port_wkup_defs.h
 *
 * \brief Wake-up controller functions
 *
 * Copyright (C) 2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information  
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com>
 *
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
