/**
 ****************************************************************************************
 *
 * @file user_custs1_impl.c
 *
 * @brief Capacitative_sensor project Custom1 Server implementation source code.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gpio.h"
#include "app_api.h"
#include "app.h"
#include "prf_utils.h"
#include "custs1.h"
#include "custs1_task.h"
#include "user_custs1_def.h"
#include "user_custs1_impl.h"
#include "Capacitative_sensor.h"
#include "user_periph_setup.h"
#include "fdc2x1x.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
timer_hnd cap_timer = EASY_TIMER_INVALID_TIMER;
uint8_t cap_string[20];

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

uint8_t user_int_to_string(int16_t input, uint8_t *s){
    uint8_t length = 1;
    if(input < 0){
        s[0] = '-';
    } else {
        s[0] = ' ';
    }
    input = abs(input);
    if(input  >= 10000){
        s[length++] = '0' + ((input / 10000) % 10);
    }
    if(input  >= 1000){
        s[length++] = '0' + ((input / 1000) % 10);
    }
    if(input  >= 100){
        s[length++] = '0' + ((input / 100) % 10);
    }
    if(input  >= 10){
        s[length++] = '0' + ((input / 10) % 10);
    }

    s[length++] = '0' + (input% 10);
    return length;
}

void user_svc1_cap_send_ntf(void)
{

    uint8_t string_length = user_int_to_string(read_fdc2x1x(0), cap_string);

    struct custs1_val_ntf_ind_req* req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                             prf_get_task_from_id(TASK_ID_CUSTS1),
                             TASK_APP,
                             custs1_val_ntf_ind_req,
                             string_length);


    req->conidx = 0;
    req->notification = true;
    req->handle = SVC1_IDX_CAPACITANCE_VALUE_VAL;
    req->length = string_length;
    for(uint8_t i = 0; i < string_length; i++){
        req->value[i] = cap_string[i];
    }

    ke_msg_send(req);

    cap_timer = app_easy_timer(NOTIFICATION_DELAY / 10, user_svc1_cap_send_ntf); //Do this again in 100 ms (10*10)
}

void user_svc1_cap_wr_ntf_handler(struct custs1_val_write_ind const *param){
    if(param->value[0]){
        if(cap_timer == EASY_TIMER_INVALID_TIMER){
            //Start the Z timer if it is not running
            cap_timer = app_easy_timer(10, user_svc1_cap_send_ntf);
        }
    }
    else{
        if(cap_timer != EASY_TIMER_INVALID_TIMER){
            //Stop the Z timer if it is running
            app_easy_timer_cancel(cap_timer);
            cap_timer = EASY_TIMER_INVALID_TIMER;
        }
    }
}
