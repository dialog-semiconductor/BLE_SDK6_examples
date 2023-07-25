/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 *
 * @brief All in one project Custom1 Server implementation source code.
 *
 * Copyright (c) 2015-2021 Renesas Electronics Corporation and/or its affiliates
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
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "adc.h"
#include "app.h"
#include "app_api.h"
#include "gpio.h"
#include "timer0.h"
#include "user_midi.h"
#include "user_custs1_def.h"
#include "user_custs1_impl.h"
#include "user_periph_setup.h"
#include "wkupct_quadec.h"

struct app_proj_env_tag user_app_env __attribute__((section(".bss."))); //@RETENTION MEMORY


/*
 * DEFINES
 ****************************************************************************************
 */
/* notes to midi code */
#define		C__4 60
#define		Ci_4 61
#define		D__4 62
#define		Di_4 63
#define		E__4 64
#define		F__4 65
#define		Fi_4 66
#define		G__4 67
#define		Gi_4 68
#define		A__4 69
#define		Ai_4 70
#define		B__4 71
#define		C__5 72
#define		Ci_5 73
#define		D__5 74
#define		Di_5 75
#define		E__5 76
#define		F__5 77
#define		Fi_5 78
#define		G__5 79
#define		Gi_5 80
#define		A__5 81
#define		Ai_5 82
#define		B__5 83

typedef struct
{
	uint8_t 	header;
	uint8_t	timestamp;
	uint8_t	status;
	uint8_t	note;
	uint8_t	velocity;
}midi_event_packet_t;

typedef union
{
	midi_event_packet_t	midi_event;
	uint8_t payload[5];
}midi_data_t;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

ke_msg_id_t timer_used      __attribute__((section(".bss."))); // @RETENTION MEMORY

/*
 * LOCAL FUNCTIONS DECLARATION
 ****************************************************************************************
*/
static void user_app_set_button_event(uint8_t);
static void user_app_disable_button(void);
static void user_app_button_press_cb(void);


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static uint32_t get_timestamp(void)
{
    uint32_t timestamp = 0;
    return timestamp;
}

void send_note(uint8_t *data, uint8_t len)
{
            // Update button characteristic
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          len);

    req->conidx = app_env->conidx;
    req->notification = true;
    req->handle = SVC1_IDX_LONG_VALUE_VAL;
    memcpy(req->value, data, len);
    req->length = len;

    ke_msg_send(req);
}

void midi_note_on(uint8_t note, uint8_t velocity)
{
	static midi_data_t note_on;

	uint32_t temp = get_timestamp();                              /* Get the timestamp from the CRYOTIMER */
	temp = temp & 0x00001fff;                                   /* Mask it - only the lower 13 bit needed */

	note_on.midi_event.header 		= 0x80 | (temp >> 7);       /* Header byte = 0b10xxxxxx where xxxxxxx is top 6 bits of timestamp */
	note_on.midi_event.timestamp 	= 0x80 | (temp & 0x003f);   /* Timestamp byte = 0b1xxxxxxx where xxxxxxx is lower 7 bits of timestamp */
	note_on.midi_event.status 		= 0x90;                     /* Status byte = 0b1sssnnnn where sss is message type and nnnn is channel */
	note_on.midi_event.note			= note;                     /* setting the note parameter */
	note_on.midi_event.velocity		= velocity;                 /* setting the velocity parameter */

	/* sending the assembled midi message */
	send_note((uint8_t *)&note_on, 5);
}

void midi_note_off(uint8_t note, uint8_t velocity)
{
	static midi_data_t note_off;

	uint32_t temp = get_timestamp();						        /* 1ms timestamp */
	temp = temp & 0x00001fff;									/* only the lower 13 bit needed */

	note_off.midi_event.header 		= 0x80 | (temp >> 7); 		/* Header byte = 0b10xxxxxx where xxxxxxx is top 6 bits of timestamp */
	note_off.midi_event.timestamp 	= 0x80 | (temp & 0x003f);   /* Timestamp byte = 0b1xxxxxxx where xxxxxxx is lower 7 bits of timestamp */
	note_off.midi_event.status 		= 0x80;						/* Status byte = 0b1sssnnnn where sss is message type and nnnn is channel */
	note_off.midi_event.note		= note;						/* setting the note parameter */
	note_off.midi_event.velocity	= velocity;					/* setting the velocity parameter */

	send_note((uint8_t *)&note_off, 5);
}

/**
 ****************************************************************************************
 * @brief Set button event configuration
 * @return void
 ****************************************************************************************
*/
static void user_app_set_button_event(uint8_t next_event)
{

    wkupct_register_callback(user_app_button_press_cb);

	  wkupct_enable_irq(WKUPCT_PIN_SELECT(GPIO_SW2_PORT, GPIO_SW2_PIN),
                      WKUPCT_PIN_POLARITY(GPIO_SW2_PORT, GPIO_SW2_PIN, next_event), // polarity
                      1, // 1 event
                      10); // debouncing time = 10 ms

}

static void user_app_enable_button(void)
{
    uint8_t next_btn_event = 0;

		    // Read button state
    if(GPIO_GetPinStatus( GPIO_SW2_PORT, GPIO_SW2_PIN ))
    {
        user_app_env.custs1_btn_state = CUSTS1_BTN_STATE_RELEASED;
        next_btn_event = WKUPCT_PIN_POLARITY_LOW;
    }
    else
    {
        user_app_env.custs1_btn_state = CUSTS1_BTN_STATE_PRESSED;
        next_btn_event = WKUPCT_PIN_POLARITY_HIGH;
    }

    // Configure next button event
    user_app_set_button_event(next_btn_event);
}    

/**
 ****************************************************************************************
 * @brief Disable button action.
 * @return void
 ****************************************************************************************
*/
static void user_app_disable_button(void)
{
    wkupct_disable_irq();
}

/**
 ****************************************************************************************
 * @brief Callback function for button action. Update button state characteristic.
 * @return void
 ****************************************************************************************
*/
static void user_app_button_press_cb(void)
{
    uint8_t next_btn_event = 0;
    static uint8_t pressed_cnt = 60;
		
    // Read button state
    if(GPIO_GetPinStatus( GPIO_SW2_PORT, GPIO_SW2_PIN ))
    {
        user_app_env.custs1_btn_state = CUSTS1_BTN_STATE_RELEASED;
        next_btn_event = WKUPCT_PIN_POLARITY_LOW;
    }
    else
    {
        user_app_env.custs1_btn_state = CUSTS1_BTN_STATE_PRESSED;
        next_btn_event = WKUPCT_PIN_POLARITY_HIGH;
    }
		
		
    if (user_app_env.custs1_btn_state == CUSTS1_BTN_STATE_PRESSED)
    {
        midi_note_on(pressed_cnt, 100);
    }
    else
    {
        midi_note_off(pressed_cnt, 100);
        pressed_cnt++;
        if (pressed_cnt > 80) pressed_cnt = 60;
    }
    // Configure next button event
    user_app_set_button_event(next_btn_event);
}

/**
 ****************************************************************************************
 * @brief Enable peripherals used by application.
 * @return void
 ****************************************************************************************
*/
void user_app_enable_periphs(void)
{
    // Update button state characteristic
    user_app_enable_button();
}

/**
 ****************************************************************************************
 * @brief Disable peripherals used by application.
 * @return void
 ****************************************************************************************
*/
void user_app_disable_periphs(void)
{
    user_app_disable_button();
}
