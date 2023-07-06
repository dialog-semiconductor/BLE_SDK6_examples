#ifndef _DCI_SVC_H_
#define _DCI_SVC_H_

#include <stdint.h>
#include "ke_msg.h"
#include "custs1_task.h"
#include "dci_config.h"

#ifdef ENABLE_DCI_SERVICE

/****************************************************************************************
 * @brief Free response buffer pending transmission after a disconnect
 * @return none
 ****************************************************************************************
*/
void dci_svc_cleanup_on_disconnect(void);


/****************************************************************************************
 * @brief Initialize dci_svc_env
 * @return none
 ****************************************************************************************
*/
void dci_svc_init(void);

/****************************************************************************************
 * @brief Notification confirmation handler. A notification has been sent to the client,
					continue processing the current command
 * @param[in] param   Pointer to the parameters of the message.
 * @return none
 ****************************************************************************************
*/
void dci_svc_notification_cfm_handler(struct custs1_val_ntf_cfm const *param);

/****************************************************************************************
 * @brief Write request validation handler for Debug Crash Info Service. Write requests 
					to the Debug Crash Info Service RX characteristic will result in a call to this 
					function. This function is an opportunuty to reject the request before it is 
					written to the BLE database. If the write is accepted, the request will result 
					in a call to dci_svc_write_handler().
 * @param[in] att_idx   handle of the attribute written
 * @param[in] offset    offset at which data has to be written
 * @param[in] length 		length of data to write
 * @param[in] value  	pointer to data to write
 * @return error code indicating write is accepted or reason for rejection 
					(see enum hl_err in rwble_hl_error.h)
 ****************************************************************************************
*/
uint8_t dci_svc_validate_write_handler(uint16_t att_idx, uint16_t offset, uint16_t length, uint8_t* value);

/****************************************************************************************
 * @brief Write request handler for the Debug Crash Info Service
 * @param[in] msgid   Id of the message received.
 * @param[in] param   Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance.
 * @param[in] src_id  ID of the sending task instance.
 * @return none
 ****************************************************************************************
*/
void dci_svc_write_handler(ke_msg_id_t const msgid,
                           struct custs1_val_write_ind const *param,
                           ke_task_id_t const dest_id,
                           ke_task_id_t const src_id);

#endif // ENABLE_DCI_SERVICE
#endif //_DCI_SVC_H_
