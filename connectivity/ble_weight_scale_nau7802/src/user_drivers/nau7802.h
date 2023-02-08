/**
 ****************************************************************************************
 *
 * @file nau7805.h
 *
 * @brief Driver implementation for NAU7802 bridge sensor ADC.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _BMP388_H_
#define _BMP388_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
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
#include <stdint.h>
#include <stdbool.h>

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
 
 int nau7802_init(void);
 int nau7802_enable_adc(void);
 int nau7802_sw_reset(void);
 int nau7802_get_chip_rev(uint8_t * const chip_id);
 int nau7802_get_ofset_load(int *offsload);
 int nau7802_get_ug_load(uint32_t *load);
 int nau7802_get_adc(int *adcval);
 int nau7802_zero_scale(void);
 int nau7802_calib_scale(uint32_t weight);


/// @} APP

#endif // _BMP388_H_
