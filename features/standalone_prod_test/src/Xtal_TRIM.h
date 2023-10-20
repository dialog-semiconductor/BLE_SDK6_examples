/**
 ****************************************************************************************
 *
 * @file Xtal_TRIM.h
 *
 * @brief Automated XTAL trim calculation core API.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef XTAL_TRIM_H_
#define XTAL_TRIM_H_

#include "rwip.h"

// Status codes
#define XTAL_PLTFRM_NO_ERROR            (0)                         // XTAL trim internal proc successfull.
#define PULSE_OUT_OF_RANGE_ERROR        (-1)                        // Pulse found in the pulse pin assigned GPIO was out of acceptable range.
#define NO_PULSE_ERROR                  (-2)                        // No pulse found, or pulse > 740 ms (measure_pulse aborts).
#define WRITING_VAL_TO_OTP_ERROR        (-3)                        // Failed to write value in OTP.
#define INVALID_GPIO_ERROR              (-4)                        // Wrong GPIO configuration.
#define WRONG_XTAL_SOURCE_ERROR         (-5)                        // Incorrect pulse detected.

#define AUTO_XTAL_TEST_DBG_EN           (0)                         // Enable/Disable debug parameters.

int auto_trim(uint8_t XTAL_select, uint8_t port_number);
#if AUTO_XTAL_TEST_DBG_EN
void TRIM_test (int S1, int S2);                                    // testing
#endif

#endif /* XTAL_TRIM_H_ */
