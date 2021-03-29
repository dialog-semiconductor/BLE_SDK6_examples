/**
 ****************************************************************************************
 *
 * \file port_timer_config.h
 *
 * \brief  Timer configuration file
 *
 * Copyright (C) 2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information  
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * <bluetooth.support@diasemi.com>
 *
 ****************************************************************************************
 */ 
 
#ifndef _PORT_TIMER_CONFIG_H
#define _PORT_TIMER_CONFIG_H

/**
 ****************************************************************************************
 * \addtogroup CONFIGURATION
 * \{
 * \addtogroup MODULE_CONFIG
 * \{
 * \addtogroup TIMER_CFG
 *
 * \brief Timer module configuration
 * \{
 ****************************************************************************************
 */

#include "port_timer_defs.h"
#include "user_rcu_example.h"

#if defined(AUDIO_TEST_MODE) || (defined(HAS_AUDIO) && defined(ENABLE_OTA_DEBUG) && defined(HAS_KBD))
    #include "user_rcu_audio.h"
#endif

#ifdef HAS_MOTION
    #include "app_motion.h"
#endif

#ifdef HAS_CONNECTION_FSM    
    #include "port_con_fsm_task.h"
    #include "port_adv_fsm.h"
#endif

#ifdef HAS_KBD 
    #include "port_kbd.h"
#endif

#ifdef HAS_PWR_MGR    
    #include "user_pwr_mgr.h"
#endif        

#ifdef HAS_LED_INDICATORS
    #include "port_leds.h"
#endif

#ifdef DEBUG_EMULATE_PACKET_LOSS
    #include "user_rcu_kbd.h"
#endif

/*************************************************************************************
 * Extended timers support (of more than 5 min)                                      *
 *************************************************************************************/
//#define EXTENDED_TIMERS_ON

enum port_timer_ids {   
#ifdef HAS_KBD
    APP_KBD_TIMER,
#endif    
#ifdef HAS_CONNECTION_FSM    
    APP_CON_FSM_TIMER,
    APP_CON_FSM_ENC_TIMER,
    APP_ADV_FSM_TIMER,
    APP_CON_FSM_ALT_PAIR_TIMER,
#endif    
#ifdef HAS_LONG_PRESS_POWERUP_BUTTON    
    APP_KEYPRESS_TIMER,
#endif
#if (BLE_SPOTA_RECEIVER) || (BLE_SUOTA_RECEIVER)
    APP_SUOTA_TIMEOUT_TIMER,
#endif    
#ifdef HAS_PWR_MGR    
    PWR_MGR_INACTIVITY_TIMER,
#endif        
#ifdef HAS_MOTION
    #if defined(MOTION_DEACTIVATION_TIMEOUT_IN_MS) && (MOTION_DEACTIVATION_TIMEOUT_IN_MS > 0)
        APP_MOT_DEACT_TIMER,
    #endif
    APP_MOT_TIMER,
#endif
#ifdef HAS_LED_INDICATORS    
    APP_LED_1_TIMER,
    APP_LED_2_TIMER,
#endif    
#ifdef DEBUG_EMULATE_PACKET_LOSS
    USER_RADIO_MUTE_TIMER,
#endif    
#ifdef AUDIO_TEST_MODE
    USER_AUDIO_TEST_MODE_TIMER,
#endif    
#if defined(HAS_AUDIO) && defined(ENABLE_OTA_DEBUG) && defined(HAS_KBD)
    USER_AUDIO_KBD_EMUL_TIMER,
#endif
    NUM_OF_TIMERS
};
//EVEENSCH
static const port_timer_handler_function_t port_timer_cbs[NUM_OF_TIMERS];
//#ifdef HAS_CONNECTION_FSM     
//    #ifdef HAS_KBD    
//    [APP_KBD_TIMER]                = port_kbd_scan_timer_handler,
//    #endif    
//    [APP_CON_FSM_TIMER]            = port_con_fsm_timer_handler,
//    [APP_CON_FSM_ENC_TIMER]        = port_con_fsm_enc_timer_handler,
//    [APP_ADV_FSM_TIMER]            = port_adv_fsm_adv_timer_handler,
//    [APP_CON_FSM_ALT_PAIR_TIMER]   = port_con_fsm_alt_pair_timer_handler,
//#endif
//#ifdef HAS_LONG_PRESS_POWERUP_BUTTON    
//    [APP_KEYPRESS_TIMER]           = user_long_keypress_timer_handler,
//#endif
//#if (BLE_SPOTA_RECEIVER) || (BLE_SUOTA_RECEIVER)
//    [APP_SUOTA_TIMEOUT_TIMER]      = user_suota_timeout_timer_handler,
//#endif    
//#ifdef HAS_PWR_MGR    
//    [PWR_MGR_INACTIVITY_TIMER]     = user_pwr_mgr_inactivity_timer_handler,
//#endif      
//#ifdef HAS_MOTION
//    #if defined(MOTION_DEACTIVATION_TIMEOUT_IN_MS) && (MOTION_DEACTIVATION_TIMEOUT_IN_MS > 0)
//    [APP_MOT_DEACT_TIMER]          = app_motion_deactivation_timer_handler,
//    #endif
//    [APP_MOT_TIMER]                = app_motion_timer_handler,
//#endif
//#ifdef HAS_LED_INDICATORS    
//    [APP_LED_1_TIMER]              = port_leds_timer1_handler,
//    [APP_LED_2_TIMER]              = port_leds_timer2_handler,
//#endif
//#ifdef DEBUG_EMULATE_PACKET_LOSS
//    [USER_RADIO_MUTE_TIMER]        = user_rcu_kbd_radio_mute_timer_handler,
//#endif    
//#ifdef AUDIO_TEST_MODE
//    [USER_AUDIO_TEST_MODE_TIMER]   = user_rcu_audio_test_mode_timer_handler,
//#endif  
//#if defined(HAS_AUDIO) && defined(ENABLE_OTA_DEBUG) && defined(HAS_KBD)
//    [USER_AUDIO_KBD_EMUL_TIMER]    = user_rcu_audio_kbd_emul_timer_handler,
//#endif
//};

/**
 * \}
 * \}
 * \}
 */

#endif	// _PORT_TIMER_CONFIG_H
