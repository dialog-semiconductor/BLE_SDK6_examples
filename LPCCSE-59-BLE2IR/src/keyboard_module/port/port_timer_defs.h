/**
 ****************************************************************************************
 *
 * \file port_timer_defs.h
 *
 * \brief 
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
 * \addtogroup TIMER
 * \{
 * \addtogroup PORT_TIMER
 * \{
 ****************************************************************************************	 
 */ 

#ifndef PORT_TIMER_DEFS_H_
#define PORT_TIMER_DEFS_H_

#include <stdbool.h>
#include <stdint.h>
#include "rwble_config.h"
#include "ke_msg.h"

typedef ke_msg_id_t timer_id_t;
typedef void (*port_timer_handler_function_t)(void);

struct set_timer_msg {
    ke_msg_id_t id;
    uint32_t time;
};

#endif // PORT_TIMER_DEFS_H_

/**
 * \}
 * \}
 * \}
 */
