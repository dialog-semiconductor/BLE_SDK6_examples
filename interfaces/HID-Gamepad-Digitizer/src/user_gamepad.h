/**
 ****************************************************************************************
 *
 * @file user_gamepad.h
 *
 * @brief Gamepad controls implementation header code.
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
 * OR OTHER DEALINGS IN THE SOFTWARE.E.
 ****************************************************************************************
 */
#ifndef _USER_GAMEPAD_H_
#define _USER_GAMEPAD_H_
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 #include "rwip_config.h" 
 #include "rwble_config.h"
 #include "app.h"   
 #include "ke_msg.h"
 #include "user_periph_setup.h"
 
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */



 //button mapping

typedef enum
{
    BUTTON_A = 0,
    BUTTON_B,
		BUTTON_C,
		BUTTON_X,
		BUTTON_Y,
		BUTTON_Z,
		BUTTON_LB,
		BUTTON_RB,
		BUTTON_LT,
		BUTTON_RT,
		BUTTON_SEL,
		BUTTON_STA,
		BUTTON_MODE,
		BUTTON_TL,
		BUTTON_TR,
		BUTTON_COUNT
	//Do not change last item
}button_map_t;

typedef enum
{
    BUTTON_RELEASED = 0x00,
    BUTTON_PRESSED = 0x03
	//Do not change last item
}button_state_digitizer_t;


struct gamepad_digitizer_report_t{
	button_state_digitizer_t is_pressed;
	uint8_t mt_index;
	uint16_t x;
	uint16_t y;
};


struct gamepad_digitizer_map_button_t{
	uint16_t x;
	uint16_t y;
};

struct gamepad_digitizer_map_joystick_t{
	uint8_t														center_x;
	uint8_t                           center_y;
	uint8_t                           radius;  //out of 100
	uint8_t                           velocity;  //movement each time stamp
};
 
struct gamepad_button_t
{
	bool															valid;														
	GPIO_PORT 													port;
	GPIO_PIN           								pin;
	uint8_t 													polarity;
	bool															is_pressed;
	uint8_t 													mt_index;
	struct gamepad_digitizer_map_button_t		digitizer_map;
};
 
struct gamepad_joystick_t
{
	bool															activated;
	uint8_t														mt_index;
	uint16_t                          x;
	uint16_t													y;
	uint32_t													raw_x;
	uint32_t													raw_y;
};

struct gamepad_status_t{
	struct gamepad_button_t 		buttons[BUTTON_COUNT];
	struct gamepad_joystick_t		ls;
	struct gamepad_joystick_t		rs;
	uint8_t											mt_state;
	bool												gamepad_ready;
};

struct gamepad_digitizer_config_pack_t{
	struct gamepad_digitizer_map_button_t   button_map[4];
	struct gamepad_digitizer_map_joystick_t joystick_map[2];
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

extern struct gamepad_status_t gamepad_status;
extern struct gamepad_digitizer_map_joystick_t LS_config,RS_config;
extern bool    axis_polling_on;
#define DIGITIZER_MAX_RANGE 0x1FFF
#define MAX_MULTITOUCH      4
#define AXIS_UPDATE_PER		2   //*20ms
#define R_DEADZONE				8    //out of 100
#define LS_ADC_SAMPLE_MIN       0
#define ADC_SAMPLE_MAX				1860

#define DEFAULT_BUTTON_A       (struct gamepad_digitizer_map_button_t){75,65}
#define DEFAULT_BUTTON_B       (struct gamepad_digitizer_map_button_t){50,65}
#define DEFAULT_BUTTON_STA     (struct gamepad_digitizer_map_button_t){10,90}
#define DEFAULT_BUTTON_SEL     (struct gamepad_digitizer_map_button_t){25,65}
#define DEFAULT_LS_CONFIG			 (struct gamepad_digitizer_map_joystick_t){50,65,10,0}
#define DEFAULT_RS_CONFIG			 (struct gamepad_digitizer_map_joystick_t){50,50,0,30}



#define SCREEN_RATIO_H  19.5
#define SCREEN_RATIO_W  9

#define CFG_USE_DIGITIZER   (0)
#define CFG_USE_JOYSTICKS		(1)

/*
 * FUNCTION CLAIM
 ****************************************************************************************
 */
void user_gamepad_init(void);
void user_gamepad_enable_buttons(void);
void user_gamepad_config_digitizer(void);
void user_gamepad_toggle_axis_polling(bool on);
void app_hid_gamepad_event_handler(ke_msg_id_t const msgid,
                                         void const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id);
void user_gamepad_set_button_location(button_map_t button_index, struct gamepad_digitizer_map_button_t config);
void user_gamepad_config_joystick(struct gamepad_digitizer_map_joystick_t* config, struct gamepad_digitizer_map_joystick_t params);
#endif
