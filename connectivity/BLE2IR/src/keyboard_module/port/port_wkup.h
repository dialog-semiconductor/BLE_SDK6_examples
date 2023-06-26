/**
 ****************************************************************************************
 *
 * \file port_wkup.h
 *
 * \brief Wake-up controller functions
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
 * \addtogroup APP_UTILS
 * \{
 * \addtogroup WAKE_UP
 * \{
 * \addtogroup PORT_WKUP
 * \{
 ****************************************************************************************	 
 */

#ifndef PORT_WKUP_H_
#define PORT_WKUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <port_wkup_config.h>

/**
 ****************************************************************************************
 * \brief Initializes the WKUP platform       
 ****************************************************************************************
 */
void port_wkup_init(void);

/**
 ****************************************************************************************
 * \brief Enables the WKUP interrupt for a specific channel
 *
 * \param[in]  channel   The channel for which the wakeup interrupt should be enabled
 * \param[in]  pol_pins  The pin polarity of this wake up channel
 ****************************************************************************************
 */
void port_wkup_enable_irq(enum port_wkup_channel channel, uint32_t pol_pins);

/**
 ****************************************************************************************
 * \brief Disables the WKUP interrupt for a specific channel
 *
 * \param[in]  channel   The channel for which the wakeup interrupt should be disabled
 ****************************************************************************************
 */
void port_wkup_disable_irq(enum port_wkup_channel channel);

#endif // PORT_WKUP_H_

/**
 * \}
 * \}
 * \}
 */