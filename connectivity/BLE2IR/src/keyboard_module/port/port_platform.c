/**
 ****************************************************************************************
 *
 * \file port_platform.c
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
 * \addtogroup PLATFORM
 * \{
 * \addtogroup PORT_PLATFORM
 * \{
 ****************************************************************************************	 
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <port_platform.h>
#include "l2cm.h"
#include "l2cc_task.h"

uint32_t port_get_time(void)
{
#if (RWBLE_SW_VERSION_MAJOR >= 8)  
    uint32_t ea_time_get_halfslot_rounded(void);   
    return ((ea_time_get_halfslot_rounded() >> 4) & BLE_GROSSTARGET_MASK);
#else    
    extern uint32_t ke_time(void);
    return ke_time();
#endif    
}

__asm void port_delay_usec(uint32_t delay)
{
        push {r1}
        movs r1, #4
        muls r1, r0, r1
loop
        subs  r1, #1
        bne loop
        pop {r1}
        bx lr
}

#if (RWBLE_SW_VERSION_MAJOR >= 8)
uint8_t port_attmdb_find_by_uuid(uint8_t conidx, uint16_t *start_hdl, uint16_t end_hdl, uint8_t uuid_len, uint8_t *uuid, bool service)
{
    uint8_t attmdb_get_next_att(uint16_t * handle, struct attm_elmt*elmt);
    uint8_t attmdb_get_uuid(struct attm_elmt *elmt, uint8_t* uuid_len, uint8_t* uuid, bool srv_uuid, bool air);

    uint8_t att_uuid_len;
    uint8_t att_uuid[ATT_UUID_128_LEN];
    bool found = false;
    uint8_t status = ATT_ERR_NO_ERROR;

    struct attm_elmt elmt;
    
    do {
        status = attmdb_get_next_att(start_hdl, &elmt);

        /* end of database - attribute not found */
        if(status != ATT_ERR_NO_ERROR) {
            *start_hdl = end_hdl;
            break;
        } else if(*start_hdl <= end_hdl) {
            // retrieve attribute UUID
            status = attmdb_get_uuid(&elmt, &(att_uuid_len), att_uuid, service, false);
            ASSERT_INFO(status == ATT_ERR_NO_ERROR, *start_hdl, status);

            // compare UUIDs
            if(attm_uuid_comp(uuid, uuid_len, att_uuid, att_uuid_len)) {
                found = true;
            } else { /* check next handle if no uuid found */
                (*start_hdl)++;
            }
        }
    } while (!found &&
             (*start_hdl <= end_hdl) &&
             (*start_hdl != ATT_INVALID_HANDLE));

    if(!found) {
        status = ATT_ERR_ATTRIBUTE_NOT_FOUND;
    }

    return status;    
}
#endif

uint8_t port_atts_find_value_by_uuid(uint8_t conidx, uint16_t *start_hdl, uint16_t end_hdl, uint8_t uuid_len, uint8_t *uuid)
{
#if (RWBLE_SW_VERSION_MAJOR >= 8)    
    return port_attmdb_find_by_uuid(conidx, start_hdl, end_hdl, uuid_len, uuid, false);
#else
    extern uint8_t atts_find_uuid(uint16_t *start_hdl, uint16_t end_hdl, uint8_t uuid_len, uint8_t *uuid);

    return atts_find_uuid(start_hdl, 0xFFFF, ATT_UUID_16_LEN, uuid);
#endif    
}

#ifdef DEBUG_EMULATE_PACKET_LOSS
typedef enum app_radio_status { 
    APP_OTHER_RADIO_ON=0,
    APP_OTHER_RADIO_OFF=1,
    APP_OTHER_WAITING_TO_SWITCH_RADIO_ON,
    APP_OTHER_WAITING_TO_SWITCH_RADIO_OFF
} app_radio_status_t;

static  app_radio_status_t  internal_radio_state  __PORT_RETAINED; 

app_radio_status_t port_probe_radio(void)
{       
     return(internal_radio_state);
}

void port_enable_radio(void)
{
     if (internal_radio_state!=APP_OTHER_RADIO_ON) {
        internal_radio_state=APP_OTHER_WAITING_TO_SWITCH_RADIO_ON;
     }
}

void port_disable_radio(void)
{   
     if (internal_radio_state!=APP_OTHER_RADIO_OFF) {
        internal_radio_state=APP_OTHER_WAITING_TO_SWITCH_RADIO_OFF;
     }
}

/**
 ****************************************************************************************
 * \brief       
 ****************************************************************************************
 */
static void port_force_disable_radio(void)
{
     internal_radio_state=APP_OTHER_RADIO_OFF;
     SetBits16(RF_OVERRULE_REG, 4, APP_OTHER_RADIO_OFF);
     SetBits16(RF_OVERRULE_REG, 1, APP_OTHER_RADIO_OFF);
}

/**
 ****************************************************************************************
 * \brief       
 ****************************************************************************************
 */
static void port_force_enable_radio(void)
{
     internal_radio_state=APP_OTHER_RADIO_ON;
     SetBits16(RF_OVERRULE_REG, 4, APP_OTHER_RADIO_ON);
     SetBits16(RF_OVERRULE_REG, 1, APP_OTHER_RADIO_ON);
}
  
void port_handle_radio_ctrl(void)
{
    switch (internal_radio_state) {
    case APP_OTHER_RADIO_ON: 
    case APP_OTHER_WAITING_TO_SWITCH_RADIO_ON:
        port_force_enable_radio();
        break;
    case APP_OTHER_RADIO_OFF: 
    case APP_OTHER_WAITING_TO_SWITCH_RADIO_OFF:
        port_force_disable_radio();
        break;
    }
}
#endif // DEBUG_EMULATE_PACKET_LOSS

struct l2cc_pdu_send_req *port_create_l2cc_pdu(uint16_t length, uint16_t handle)
{
    struct l2cc_pdu_send_req *pkt = KE_MSG_ALLOC_DYN(L2CC_PDU_SEND_REQ,
                                                     KE_BUILD_ID(TASK_L2CC, app_env[0].conidx),
                                                     TASK_APP, l2cc_pdu_send_req,
                                                     length);
    if (!pkt) {
        return NULL;
    }
    pkt->pdu.chan_id   = L2C_CID_ATTRIBUTE;
    // Set packet opcode.
    pkt->pdu.data.code = L2C_CODE_ATT_HDL_VAL_NTF;
    pkt->pdu.data.hdl_val_ntf.handle = handle;
    pkt->pdu.data.hdl_val_ntf.value_len = length;
    
    return pkt;
}

bool port_send_notification(uint16_t handle, uint8_t *data, uint16_t length, bool low_priority)
{    
    uint16_t available, already_in;
    uint16_t buffer_size;
#if (RWBLE_SW_VERSION_MAJOR >= 8) 
        buffer_size = l2cm_get_buffer_size(0); // 27 if packet length extension is not used
#else
        buffer_size = 27;    
#endif   
    
    uint8_t number_of_packets = 1;
    int16_t remaining_len = length - (buffer_size-7);
    
    if(remaining_len > 0) {                   
        number_of_packets += (uint8_t)(remaining_len/buffer_size+((remaining_len%buffer_size)>0 ? 1 : 0));  
    }   
    
    ASSERT_ERROR(number_of_packets < MAX_TX_BUFS);
    
    available = l2cm_get_nb_buffer_available();
    
    if(low_priority == true) {
        already_in = MAX_TX_BUFS-available;
        if (already_in >= MAX_TX_BUFS/2) {
            return false;
        }
    }
    
    if(available < number_of_packets) {
        return false;
    }

    
    struct l2cc_pdu_send_req *pkt = port_create_l2cc_pdu(length, handle);

    if (!pkt) {
        return false;
    }
    // copy the content to value 
    memcpy(pkt->pdu.data.hdl_val_ntf.value, data, length);
    
    if(GetBits16(CLK_RADIO_REG, BLE_ENABLE) == 0) {
        arch_ble_force_wakeup();
    }
    
    ke_msg_send(pkt);
    return true;
}

/**
 * \}
 * \}
 * \}
 */
