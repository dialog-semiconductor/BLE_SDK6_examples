/**
 ****************************************************************************************
 *
 * @file DA14531_Quuppa_DF_BckChn_FlexBeacon.h
 *
 * @brief DA14531_Quuppa_DF_BckChn_FlexBeacon project header file.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _DA14531_QUUPPA_DF_BCKCHN_FLEXBEACON_H_
#define _DA14531_QUUPPA_DF_BCKCHN_FLEXBEACON_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwble_config.h"
#include "gapm_task.h"                 // gap functions and messages
#include "arch_wdg.h"
#include "app_default_handlers.h"
#include "co_error.h"                  // error code definitions

#include "gapc_task.h"                 // gap functions and messages
#include "app_task.h"                  // application task
#include "app.h"                       // application definitions
#include "app_callback.h"
#include "arch_api.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define SHOW_ALL_MESSAGES		(0) // 0 = print only Timers messages, 1 = print all debug messages

#define MAN_QUUPPA_COMP_F1_ID             {0xff, 0xc7, 0x00, 0xf1}  // 0xFF = Manufacturing data, 0x00C7 = Quuppa, 0xF1 = REQ packet 

// REQ/RSP Payload Type
#define UNSUPPORTED_REQ    0x00
#define DEVICE_INFO        0x01
#define BACK_CHANNEL_INFO  0x02
#define DEVELOPER_SPECIFIC 0xFF

// Configurable TX Power (only for DA14531)
#define MOTION_ADV_POWER	       RF_TX_PWR_LVL_0d0 //(0dBm) //RF_TX_PWR_LVL_PLUS_2d5		 //(+2.5dBm)
#define STATIC_ADV_POWER         RF_TX_PWR_LVL_0d0 //(0dBm) //RF_TX_PWR_LVL_PLUS_2d5    //RF_TX_PWR_LVL_MINUS_7d0	 //(-7.0dBm)
#define RX_ON_ADV_POWER	         RF_TX_PWR_LVL_0d0 //(0dBm) //RF_TX_PWR_LVL_PLUS_2d5		 //(+2.5dBm)
#define RSP_ADV_POWER	           RF_TX_PWR_LVL_0d0 //(0dBm) //RF_TX_PWR_LVL_PLUS_2d5		 //(+2.5dBm)
#define DEV_INFO_ADV_POWER       RF_TX_PWR_LVL_0d0 //(0dBm)

#define QUUPPA_DEVICE_TYPE 0x20 // Profile 2.ID BADGE
//#define QUUPPA_DEVICE_TYPE 0x80 // Profile 5.FORKLIFT/VEHICLE FAST RX RATE

#if(QUUPPA_DEVICE_TYPE == 0x20)
// TX Rate Triggered state advertising interval
#define MOTION_ADV_INTERVAL	             333		//(333 * 1msec	=   333msec  =>   3Hz)
/* Periodic RX ON Triggered State timer */
#define PERIODIC_RX_ON_INTERVAL_MOTION  (200)  //  200 * 10ms =    2000msec  => 0.5Hz

#define MOTION_ADV_HEADER                0x69 // Quuppa Header 0x69 = [7] RX ON [0], [6] Bck Chn [1], [5-4] TagID=BDAdd [10],[3-2] 0dBm Tx power [10], [1-0] DF TX Rate(1-6Hz)[01]

#define RX_ON_MOTION_ADV_HEADER          0xE9 //0xE8 // Quuppa Header 0xE8 = [7] RX ON [1], [6] Bck Chn [1], [5-4] TagID=BDAdd [10],[3-2] 0dBm Tx power [10], [1-0] DF TX Rate(<1Hz)[00]

// TX Rate Default state advertising interval
#define STATIC_ADV_INTERVAL	             10000	 //( 10000 * 1msec =  10000msec => 0.1Hz)
/* Periodic RX ON Default timer */
#define PERIODIC_RX_ON_INTERVAL_STATIC  (999)   //  999 * 10ms =   9990msec => 0.1Hz

#define STATIC_ADV_HEADER                0x68 // Quuppa Header 0x68 = [7] RX ON [0], [6] Bck Chn [1], [5-4] TagID=BDAdd [10],[3-2] 0dBm Tx power [10], [1-0] DF TX Rate(<1Hz)[00]

#define RX_ON_STATIC_ADV_HEADER          0xE8 // Quuppa Header 0xE8 = [7] RX ON [1], [6] Bck Chn [1], [5-4] TagID=BDAdd [10],[3-2] 0dBm Tx power [10], [1-0] DF TX Rate(<1Hz)[00]

#define RX_ON_ADV_INTERVAL			 300	   // 300 *  1ms = 300msec
/* RX ON cancel timer: must be maximum 2/3 of RX_ON_ADV_INTERVAL to have only one ADV event */
// This is the delay after RX_ON=1 to start scan operation
#define RX_ON_CANCEL_TIMER       (25)    //  25*10ms = 250msec

#define RSP_ADV_INTERVAL			    100	   // 100 *  1ms = 100msec
/* RSP duration timer: must be 2.5 times the RSP_ADV_INTERVAL to have three ADV events */
#define RSP_CANCEL_TIMER         (25)    //  25 * 10ms = 250msec

#define SCAN_INTERVAL_MS  	      50
#define SCAN_WINDOW_MS    	      40
#define SCAN_CANCEL_TIMER        (25)    //  25 * 10ms = 250msec

#else // QUUPPA_DEVICE_TYPE 0x80 // Profile 5.FORKLIFT/VEHICLE FAST RX RATE (simplified)
// TX Rate Triggered state advertising interval
#define MOTION_ADV_INTERVAL	               111	//(  111 * 1msec =    111msec =>   9Hz)
/* Periodic RX ON Triggered State timer */
#define PERIODIC_RX_ON_INTERVAL_MOTION    (20)  //    20 * 10ms =     200msec =>   5Hz

#define MOTION_ADV_HEADER                0x6A // Quuppa Header 0x6A = [7] RX ON [0], [6] Bck Chn [1], [5-4] TagID=BDAdd [10],[3-2] 0dBm Tx power [10], [1-0] DF TX Rate(7-14Hz)[10]

#define RX_ON_MOTION_ADV_HEADER          0xE9 // Quuppa Header 0xE9 = [7] RX ON [1], [6] Bck Chn [1], [5-4] TagID=BDAdd [10],[3-2] 0dBm Tx power [10], [1-0] DF TX Rate(1-6Hz)[01]

// TX Rate Default state advertising interval
#define STATIC_ADV_INTERVAL	             10000	 //( 10000 * 1msec =  10000msec => 0.1Hz)
/* Periodic RX ON Default timer */
#define PERIODIC_RX_ON_INTERVAL_STATIC   (999)   //  999 * 10ms =   9990msec => 0.1Hz

#define STATIC_ADV_HEADER                0x68 // Quuppa Header 0x68 = [7] RX ON [0], [6] Bck Chn [1], [5-4] TagID=BDAdd [10],[3-2] 0dBm Tx power [10], [1-0] DF TX Rate(<1Hz)[00]

#define RX_ON_STATIC_ADV_HEADER          0xE8 // Quuppa Header 0xE8 = [7] RX ON [1], [6] Bck Chn [1], [5-4] TagID=BDAdd [10],[3-2] 0dBm Tx power [10], [1-0] DF TX Rate(<1Hz)[00]

#define RX_ON_ADV_INTERVAL			  60	   // 60 *  1ms = 60msec
/* RX ON cancel timer: must be maximum 2/3 of RX_ON_ADV_INTERVAL to have only one ADV event */
// This is the delay after RX_ON=1 to start scan operation
#define RX_ON_CANCEL_TIMER        (4)    //  4*10ms = 40msec

#define RSP_ADV_INTERVAL			    20	   //  20 *  1ms = 20msec
/* RSP duration timer: must be 2.5 times the RSP_ADV_INTERVAL to have three ADV events */
#define RSP_CANCEL_TIMER         (7)    //   7 * 10ms = 70msec

#define SCAN_INTERVAL_MS  	      50
#define SCAN_WINDOW_MS    	      40
#define SCAN_CANCEL_TIMER         (5)    //  5 * 10ms = 50msec

#endif

#define DEV_INFO_ADV_INTERVAL			RX_ON_ADV_INTERVAL
/* DEV_INFO duration timer: must be 2/3 times the DEV_INFO_ADV_INTERVAL to have one ADV events */
#define DEV_INFO_CANCEL_TIMER     RX_ON_CANCEL_TIMER

// Configurable timeout lenght
#define TIMEOUT_FOR_SLEEP			      720000	 //( 720000  	* 10msec 	= 	  7200sec= 2h), The maximum allowed value is 41943sec (4194300 * 10ms)

/* Duration of timer for connection parameter update request */
#define APP_PARAM_UPDATE_REQUEST_TO         (1000)   // 1000*10ms = 10sec, The maximum allowed value is 41943sec (4194300 * 10ms)

/* Manufacturer specific data constants */
#define APP_AD_MSD_COMPANY_ID       (0x00C7)
#define APP_AD_MSD_COMPANY_ID_LEN   (2)
#define APP_AD_MSD_DATA_LEN         (24)
#define DEVELOPER_ID                        (0x0008)  // Dialog Developer ID assigned by Quuppa

/* Advertising data update timer */
#define APP_ADV_DATA_UPDATE_TO              (3000)   // 3000*10ms = 30sec, The maximum allowed value is 41943sec (4194300 * 10ms)

#define APP_BUTTON_LONG_PRESS_TO            (400)    // 400*10ms = 4sec, The maximum allowed value is 41943sec (4194300 * 10ms)

#define APP_PERIPHERAL_CTRL_TIMER_DELAY 100

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef enum 
{
    MOTION_ADVERTISING,
    STATIC_ADVERTISING,
	  RX_ON_ADVERTISING,
	  SCAN,
	  RSP_ADVERTISING,
	  DEV_INFO,
		SLEEP 						
}advertising_state;
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Application initialization function.
 ****************************************************************************************
*/
void user_app_init(void);

/**
 ****************************************************************************************
 * @brief Advertising function.
 ****************************************************************************************
*/
void user_app_adv_start(void);

/**
 ****************************************************************************************
 * @brief Connection function.
 * @param[in] connection_idx Connection Id index
 * @param[in] param Pointer to GAPC_CONNECTION_REQ_IND message
 ****************************************************************************************
*/
void user_app_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Undirect advertising completion function.
 * @param[in] status Command complete event message status
 ****************************************************************************************
*/
void user_app_adv_undirect_complete(uint8_t status);

/**
 ****************************************************************************************
 * @brief Callback checking BLE events to toggle LED
 * @return void
 ****************************************************************************************
*/
arch_main_loop_callback_ret_t user_ble_powered_on(void);

/**
 ****************************************************************************************
 * @brief Disconnection function.
 * @param[in] param Pointer to GAPC_DISCONNECT_IND message
 ****************************************************************************************
*/
void user_app_disconnect(struct gapc_disconnect_ind const *param);

/**
 ****************************************************************************************
 * @brief Handles the messages that are not handled by the SDK internal mechanisms.
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 ****************************************************************************************
*/
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief DB initialization callback
 * @return void
 ****************************************************************************************
*/
void user_app_on_db_init_complete(void);

/**
 ****************************************************************************************
 * @brief Function to start static undirect advertising
 * @return void
 ****************************************************************************************
*/

void user_static_undirect_adv_start(void);

/**
 ****************************************************************************************
 * @brief Function to start motion undirected advertising
 * @return void
 ****************************************************************************************
*/

void user_motion_undirect_adv_start(void);
													
/**
 ****************************************************************************************
 * @brief Function to start rx on undirected advertising
 * @return void
 ****************************************************************************************
*/

void user_rx_on_undirect_adv_start(void);

/**
 ****************************************************************************************
 * @brief Function to start rsp undirected advertising
 * @return void
 ****************************************************************************************
*/

void user_rsp_undirect_adv_start(void);

/**
 ****************************************************************************************
 * @brief Function to start rsp undirected advertising
 * @return void
 ****************************************************************************************
*/

void user_dev_info_undirect_adv_start(void);

/**
 ****************************************************************************************
 * @brief Send the device to sleep .
 * @param[in] none
 * @return void
 ****************************************************************************************
*/                          
void user_enable_extended_sleep(void);
                          

 
/**
 ****************************************************************************************
 * @brief Function controlling the advertising state
 * @return void
 ****************************************************************************************
*/

void user_change_adv_state(advertising_state state);

void user_on_scanning_completed(uint8_t reason);
void user_on_adv_report_ind(struct gapm_adv_report_ind const * adv_ind);
void user_app_configuration_func(uint8_t role);

void user_app_adv_nonconn_complete(uint8_t status);

void flash_itf_de_init(void);

void flash_itf_init(void);


/// @} APP

#endif // _DA14531_QUUPPA_DF_BCKCHN_FLEXBEACON_H_
