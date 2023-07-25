/**
 ****************************************************************************************
 *
 * @file user_app.c
 *
 * @brief Pressure sensor user application.
 *
 * Copyright (c) 2012-2021 Dialog Semiconductor
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_api.h"
#include "bmp388.h"
#include "user_app.h"
#include "user_periph_setup.h"
#include "wkupct_quadec.h"
#include "custs1_task.h"
#include "user_custs1_def.h"

#ifdef CFG_PRINTF
#include "arch_console.h"
#endif

#define MEASURE_PERIOD          500         /* Units of 10ms */

static void set_pressure_char_value(uint32_t value);
static void set_temperature_char_value(uint16_t value);
static void send_pressure_char_notify(uint32_t value);
static void send_temperature_char_notify(uint16_t value);
static void wakeup_callback(void);
static void measure_timer_cb(void);

static uint8_t pressure_notify      __SECTION_ZERO("retention_mem_area0");
static uint8_t temperature_notify   __SECTION_ZERO("retention_mem_area0");
static timer_hnd measure_timer      __SECTION_ZERO("retention_mem_area0");

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief   Called at system start up.
 *
 * @param   None
 *
 * @return  None
 ****************************************************************************************
*/
void user_app_on_init(void)
{
  /* Initial BMP388 configuration */
  struct bmp388_cfg cfg = {
    .odr   = BMP388_ODR_100,
    .osr_t = BMP388_OVERSAMPLING_x1,
    .osr_p = BMP388_OVERSAMPLING_x1,
  };

  #ifdef CFG_PRINTF
    arch_printf("\n\r%s", __FUNCTION__);
    arch_printf("\n\rInitializing BMP388...");
  #endif

  /* Configure handler for BMP388 interrupts */
  wkupct_register_callback(wakeup_callback);
  wkupct_enable_irq(WKUPCT_PIN_SELECT(BMP388_INT_PORT, BMP388_INT_PIN),
                    WKUPCT_PIN_POLARITY(BMP388_INT_PORT, BMP388_INT_PIN, WKUPCT_PIN_POLARITY_LOW),
                    1,
                    0);

  /* Initialize BMP388 interface and sensor */
  (void)bmp388_init();
  (void)bmp388_config(&cfg);
  (void)bmp388_set_int_active_level(BMP388_INT_LVL_ACT_LO);
  (void)bmp388_enable_drdy_int();

  #ifdef CFG_PRINTF
  {
    arch_printf("\n\rBMP388 Register Dump");
    (void)bmp388_dump_regs(arch_printf);
  }
  #endif

  default_app_on_init();
}

/**
 ****************************************************************************************
 * @brief   Called on database initialization completion event.
 *
 * @param   None
 *
 * @return  None
 ****************************************************************************************
*/
void user_app_on_db_init_complete(void)
{
  #ifdef CFG_PRINTF
    arch_printf("\n\r%s", __FUNCTION__);
  #endif

  /* Set initial values of pressure and temperature characteristics */
  set_pressure_char_value(0);
  set_temperature_char_value(0);

  /* Start a single first measurement */
  measure_timer = app_easy_timer(MEASURE_PERIOD, measure_timer_cb);

  default_app_on_db_init_complete();
}

/**
 ****************************************************************************************
 * @brief   Called when central connects to peripheral.
 *
 * @param   conidx - Connection index.
 *
 * @return  None
 ****************************************************************************************
*/
void user_app_on_connection(uint8_t conidx, struct gapc_connection_req_ind const *param)
{
  #ifdef CFG_PRINTF
    arch_printf("\n\r%s", __FUNCTION__);
  #endif

  default_app_on_connection(conidx, param);
}

/**
 ****************************************************************************************
 * @brief   Called when central disconnects from peripheral.
 *
 * @param   param - Disconnect parameters
 *
 * @return  None
 ****************************************************************************************
*/
void user_app_on_disconnect(struct gapc_disconnect_ind const *param)
{
  #ifdef CFG_PRINTF
    arch_printf("\n\r%s", __FUNCTION__);
  #endif

  default_app_on_disconnect(param);
}

/**
 ****************************************************************************************
 * @brief   Callback for handling messages from custom profile.
 *
 * @param   msgid - Message ID
 *
 * @return  None
 ****************************************************************************************
*/
void user_app_catch_rest_hndl(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{
  #ifdef CFG_PRINTF
    arch_printf("\n\r%s", __FUNCTION__);
  #endif

  if (msgid == CUSTS1_VAL_WRITE_IND) {
    struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

    switch (msg_param->handle)
    {
      case SVC1_IDX_PRESSURE_NTF_CFG:
      {
        pressure_notify = msg_param->value[0];
      }
      break;

      case SVC1_IDX_TEMPERATURE_NTF_CFG:
      {
        temperature_notify = msg_param->value[0];
      }
      break;

      default:
      {
      }
      break;
    }
  }
}

/**
 ****************************************************************************************
 * @brief   Wakeup callback.
 *
 * @param   None.
 *
 * @return  None.
 ****************************************************************************************
*/
static void wakeup_callback(void)
{
  periph_init();

  #ifdef CFG_PRINTF
    arch_printf("\n\r%s", __FUNCTION__);
  #endif

  /* Determine source of interrupt */
  if (GPIO_GetPinStatus(BMP388_INT_PORT, BMP388_INT_PIN) == 0) {
    uint8_t status;

    if (bmp388_get_int_status(&status) == 0) {
      if (status & BMP388_INT_STATUS_DRDY) {
        int16_t temp;
        uint32_t press;

        (void)bmp388_get_temperature(&temp);
        (void)bmp388_get_pressure(&press);

        set_pressure_char_value(press);
        set_temperature_char_value((uint16_t)temp);

        /* Send notifications if central has subscribed */
        if (ke_state_get(TASK_APP) == APP_CONNECTED) {
          if (pressure_notify) {
            send_pressure_char_notify(temp);
          }
          if (temperature_notify) {
            send_temperature_char_notify(press);
          }
        }

        arch_printf("\n\rtemp: %d.%dC", temp/100, temp%100);
        arch_printf("\n\rpress: %dPa", press);
        arch_printf("\n\rpress: %d.%dinHg", press/3386, press%3386);
      }
    }
    /* Restart timer that will trigger periodic measurement */
    measure_timer = app_easy_timer(MEASURE_PERIOD, measure_timer_cb);

  }

	wkupct_enable_irq(WKUPCT_PIN_SELECT(BMP388_INT_PORT, BMP388_INT_PIN),
									WKUPCT_PIN_POLARITY(BMP388_INT_PORT, BMP388_INT_PIN, WKUPCT_PIN_POLARITY_LOW),
									1,
									0);
	
}

/**
 ****************************************************************************************
 * @brief   Measure timer callback.
 *
 * @param   None.
 *
 * @return  None.
 ****************************************************************************************
*/
static void measure_timer_cb(void)
{
  #ifdef CFG_PRINTF
    arch_printf("\n\r%s", __FUNCTION__);
  #endif

	 /* Start a single first measurement */
	bmp388_start_meas(BMP388_MODE_FORCED, 
                        BMP388_PRESS_MEAS_ENABLE, 
                        BMP388_TEMP_MEAS_ENABLE);

}

/**
 ****************************************************************************************
 * @brief   Set temperature characteristic value.
 *
 * @param   value - Temperature value
 *
 * @return  None
 ****************************************************************************************
*/
static void set_temperature_char_value(uint16_t value)
{
  struct custs1_val_set_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_SET_REQ,
                                                    prf_get_task_from_id(TASK_ID_CUSTS1),
                                                    TASK_APP,
                                                    custs1_val_set_req,
                                                    DEF_SVC1_TEMPERATURE_CHAR_LEN);

  req->handle = SVC1_IDX_TEMPERATURE_VAL;
  req->length = DEF_SVC1_TEMPERATURE_CHAR_LEN;
  memcpy(req->value, &value, DEF_SVC1_TEMPERATURE_CHAR_LEN);

  ke_msg_send(req);
}

/**
 ****************************************************************************************
 * @brief   Set pressure characteristic value.
 *
 * @param   value - Pressure value
 *
 * @return  None
 ****************************************************************************************
*/
static void set_pressure_char_value(uint32_t value)
{
  struct custs1_val_set_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_SET_REQ,
                                                    prf_get_task_from_id(TASK_ID_CUSTS1),
                                                    TASK_APP,
                                                    custs1_val_set_req,
                                                    DEF_SVC1_PRESSURE_CHAR_LEN);

  req->handle = SVC1_IDX_PRESSURE_VAL;
  req->length = DEF_SVC1_PRESSURE_CHAR_LEN;
  memcpy(req->value, &value, DEF_SVC1_PRESSURE_CHAR_LEN);

  ke_msg_send(req);
}

/**
 ****************************************************************************************
 * @brief   Send temperature notification.
 *
 * @param   value - Temperature value
 *
 * @return  None
 ****************************************************************************************
*/
static void send_temperature_char_notify(uint16_t value)
{
  struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                        prf_get_task_from_id(TASK_ID_CUSTS1),
                                                        TASK_APP,
                                                        custs1_val_ntf_ind_req,
                                                        DEF_SVC1_TEMPERATURE_CHAR_LEN);

  req->handle = SVC1_IDX_TEMPERATURE_VAL;
  req->length = DEF_SVC1_TEMPERATURE_CHAR_LEN;
  req->notification = true;
  memcpy(req->value, &value, DEF_SVC1_TEMPERATURE_CHAR_LEN);

  ke_msg_send(req);
}

/**
 ****************************************************************************************
 * @brief   Send pressure notification.
 *
 * @param   value - Pressure value
 *
 * @return  None
 ****************************************************************************************
*/
static void send_pressure_char_notify(uint32_t value)
{
  struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                        prf_get_task_from_id(TASK_ID_CUSTS1),
                                                        TASK_APP,
                                                        custs1_val_ntf_ind_req,
                                                        DEF_SVC1_PRESSURE_CHAR_LEN);

  req->handle = SVC1_IDX_PRESSURE_VAL;
  req->length = DEF_SVC1_PRESSURE_CHAR_LEN;
  req->notification = true;
  memcpy(req->value, &value, DEF_SVC1_PRESSURE_CHAR_LEN);

  ke_msg_send(req);
}

/// @} APP
