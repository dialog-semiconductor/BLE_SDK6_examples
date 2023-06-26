/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "user_segger_rtt_periph.h"
#include "SEGGER_SWD_printf.h"
#include "app_easy_timer.h"

timer_hnd timer_test;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/
static void test_timer_cb(void)
{
		SWD_printf("Test Timer CB\r\n\r\n", NULL);
		timer_test = app_easy_timer(100, test_timer_cb);
}
void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
		SWD_printf("%s\r\n", __func__);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
		SWD_printf("%s\r\n", __func__);
}

void user_app_on_set_dev_config_complete(void)
{
		default_app_on_set_dev_config_complete();
		SWD_printf("%s\r\n\r\n", __func__);
	
		timer_test = app_easy_timer(100, test_timer_cb);
}
 

/// @} APP