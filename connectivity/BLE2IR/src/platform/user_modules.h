/**
 ****************************************************************************************
 *
 * \file user_modules.h
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

#ifndef _USER_MODULES_H_
#define _USER_MODULES_H_

/**
 ****************************************************************************************
 * \addtogroup USER
 * \{
 * \addtogroup USER_APP
 * \{
 * \addtogroup USER_MODULES
 *
 * \brief User modules implementation
 * \{
 ****************************************************************************************
 */

#include <user_modules_config.h>

/**
 ****************************************************************************************
 * \brief Reserves all the gpios registered by every user module
 ****************************************************************************************
 */
void user_modules_reserve_gpios(void);

/**
 ****************************************************************************************
 * \brief Initializes all the gpios registered by every user module
 ****************************************************************************************
 */
void user_modules_init_gpios(void);

/**
 ****************************************************************************************
 * \brief Initializes all the user modules
 ****************************************************************************************
 */
void user_modules_init(void);

/**
 ****************************************************************************************
 * \brief Calls all the "on_disconnect" user module registered functions
 ****************************************************************************************
 */
void user_modules_on_disconnect(void);

/**
 ****************************************************************************************
 * \brief Calls all the "on_ble_powered" user module registered functions
 ****************************************************************************************
 */
bool user_modules_on_ble_powered(void);

/**
 ****************************************************************************************
 * \brief Calls all the "on_system_powered" user module registered functions
 ****************************************************************************************
 */
uint8_t user_modules_on_system_powered(void);

/**
 * \}
 * \}
 * \}
 */

#endif // _USER_MODULES_H_
