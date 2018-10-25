/**
 ****************************************************************************************
 *
 * @file user_adv_example.h
 *
 * @brief Barebone application header file.
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs 
 * included in this package ("Software") include confidential, proprietary information 
 * of Dialog Semiconductor. All Rights Reserved.
 * 
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’) AND MAY
 * ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND. THIS 
 * SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS. RECIPIENT 
 * SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG’S PRIOR 
 * WRITTEN PERMISSION.
 * 
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT 
 * TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT 
 * WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT 
 * LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, 
 * MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM 
 * COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS
 * AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY 
 * REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED 
 * SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, 
 * AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY 
 * OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED 
 * BY THE FCC OR OTHER LIKE AGENCIES.
 * 
 * IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 ****************************************************************************************
 */

#ifndef _USER_ADV_EXAMPLE_H_
#define _USER_ADV_EXAMPLE_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup
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
#include "gapc_task.h"                 // gap functions and messages
#include "app_task.h"                  // application task
#include "app.h"                       // application definitions
#include "app_callback.h"

/*
 * DEFINES
 ****************************************************************************************
 */	

// Configurable advertising intervals
#define UNDIRECT_ADV_INTERVAL	150		//(150 	* 1msec 	=  150msec)
#define NONCONN_ADV_INTERVAL	1050	//(1050 * 1msec		= 1050msec)

// Configurable state lenght (for timer based example)
#define	UNDIRECT_ADV_DURATION	200		//(200 	* 10msec 	= 		2sec)
#define	NONCONN_ADV_DURATION	300		//(300 	* 10msec 	= 		3sec)
#define SLEEP_DURATION				250		//(250 	* 10msec 	= 	2.5sec) 

/* Duration of timer for connection parameter update request */
#define APP_PARAM_UPDATE_REQUEST_TO         (1000)   // 1000*10ms = 10sec, The maximum allowed value is 41943sec (4194300 * 10ms)

/* Advertising data update timer */
#define APP_ADV_DATA_UPDATE_TO              (500)   // 500*10ms = 5sec, The maximum allowed value is 41943sec (4194300 * 10ms)

/* Manufacturer specific data constants */
#define APP_AD_MSD_COMPANY_ID               (0xABCD)
#define APP_AD_MSD_COMPANY_ID_LEN           (2)
#define APP_AD_MSD_DATA_LEN                 (sizeof(uint16_t))

// declaring three enumerated advertising_state types
typedef enum 
{
		UNDIRECT_ADVERTISING,
		NONCON_ADVERTISING,
		SLEEP 						
}advertising_state;


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Application initialization function.
 * @return void
 ****************************************************************************************
*/
void user_app_init(void);

/**
 ****************************************************************************************
 * @brief Advertising function.
 * @return void
 ****************************************************************************************
*/
void user_app_adv_start(void);

/**
 ****************************************************************************************
 * @brief Connection function.
 * @param[in] conidx        Connection Id index
 * @param[in] param         Pointer to GAPC_CONNECTION_REQ_IND message
 * @return void
 ****************************************************************************************
*/
void user_app_connection(const uint8_t conidx, struct gapc_connection_req_ind const *param);

/**
 ****************************************************************************************
 * @brief Undirect advertising completion function.
 * @param[in] status Command complete event message status
 * @return void
 ****************************************************************************************
*/
void user_app_adv_undirect_complete(uint8_t status);

/**
 ****************************************************************************************
 * @brief Non connectable advertising completion function.
 * @param[in] status Command complete event message status
 * @return void
 ****************************************************************************************
*/
void user_app_adv_nonconn_complete(uint8_t status);

/**
 ****************************************************************************************
 * @brief Disconnection function.
 * @param[in] param         Pointer to GAPC_DISCONNECT_IND message
 * @return void
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
 * @return void
 ****************************************************************************************
*/
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id);				
													
													
#ifdef ADV_EXAMPLE
/**
 ****************************************************************************************
 * @brief Function to start non-connectable advertising
 * @return void
 ****************************************************************************************
*/

void user_noncon_adv_start(void);

/**
 ****************************************************************************************
 * @brief Function to start undirected advertising
 * @return void
 ****************************************************************************************
*/

void user_undirect_adv_start(void);

/**
 ****************************************************************************************
 * @brief Callback checking BLE events to toggle LED
 * @return void
 ****************************************************************************************
*/
arch_main_loop_callback_ret_t user_ble_powered_on(void);

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

#endif // ADV_EXAMPLE
#endif //_USER_ADV_EXAMPLE_H_

/// @} APP

