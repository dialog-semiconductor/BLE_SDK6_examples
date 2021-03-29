/**
 ****************************************************************************************
 *
 * \file port_timer.h
 *
 * \brief timer functions
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
 
#ifndef PORT_TIMER_H_
#define PORT_TIMER_H_

#include <port_timer_config.h>
#include "port_platform.h"

/**
 ****************************************************************************************
 * \brief      Sets a timer to start counting
 *
 * \param[in]  timer_id     The timer's ID  
 * \param[in]  task_handler The timer's handling task
 * \param[in]  delay        The timer's delay in ms
 ****************************************************************************************
 */
void port_timer_set(timer_id_t const timer_id, task_id_t task_handler, uint32_t delay);

/**
 ****************************************************************************************
 * \brief      Clears a counting timer
 *
 * \param[in]  timer_id     The timer's ID  
 * \param[in]  task_handler The timer's handling task
 ****************************************************************************************
 */
void port_timer_clear(timer_id_t const timer_id, task_id_t task_handler);

 /**
  ****************************************************************************************
  * \brief      Clears an existing timer and returns its current counting value
  *
  * \param[in]  timer_id        The timer's ID  
  * \param[in]  task_handler    The timer's handling task
  *
  * \return     int             The timer's counting value
  ****************************************************************************************
  */
int port_timer_remove(timer_id_t const timer_id, task_id_t task_handler);

#endif // PORT_TIMER_H_

/**
 * \}
 * \}
 * \}
 */
