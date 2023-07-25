/**
 ****************************************************************************************
 *
 * \file user_hogpd_config.h
 *
 * \brief HOGPDD configuration file.
 *
 * Copyright (C) 2017-2021 Dialog Semiconductor
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

#ifndef _USER_HOGPD_CONFIG_H_
#define _USER_HOGPD_CONFIG_H_

/**
 ****************************************************************************************
 * \addtogroup CONFIGURATION
 * \{
 * \addtogroup PROFILE_CONFIG
 * \{
 * \addtogroup HOGPD_CFG
 *
 * \brief BLE HOGPD profile configuration
 * \{
 ****************************************************************************************
 */
 
#include "app_hogpd_defs.h"
#include "hogpd.h"
#include "hogpd_task.h"
#include "user_gamepad.h"

#define HID_MOUSE_MOTION_REPORT_ID       	0x01 // Mouse motion report
#define HID_MOUSE_MOTION_REPORT_SIZE     	8    // The size of the mouse motion report



#if CFG_USE_DIGITIZER                                         
	#define HID_DIGITIZER_REPORT_ID        		0x02 // Mouse buttons report
	#define HID_DIGITIZER_REPORT_SIZE        			6    // The size of the Mouse buttons report
                                         
	#define HID_DIGITIZER2_REPORT_ID     		0x03 // Advanced buttons report
	#define HID_DIGITIZER2_REPORT_SIZE   		6    // The size of the advanced buttons report
#else
	#define HID_GAMEPAD_AXIS_REPORT_ID        		0x02 // Mouse buttons report
	#define HID_GAMEPAD_AXIS_REPORT_SIZE	        			4    // The size of the Mouse buttons report
                                         
	#define HID_GAMEPAD_BUTTONS_REPORT_ID     		0x03 // Advanced buttons report
	#define HID_GAMEPAD_BUTTONS_REPORT_SIZE   		2    // The size of the advanced buttons report
#endif

enum hogpd_indexes {

		HID_MOUSE_MOTION_REPORT_IDX = 0,
#if CFG_USE_DIGITIZER
    HID_DIGITIZER_REPORT_IDX,
		HID_DIGITIZER2_REPORT_IDX,
#else
	  HID_GAMEPAD_AXIS_REPORT_IDX,
		HID_GAMEPAD_BUTTONS_REPORT_IDX,
#endif
    HID_NUM_OF_REPORTS // Don't remove this.
};

static const hogpd_reports_t hogpd_reports[HID_NUM_OF_REPORTS] = 
{

	[HID_MOUSE_MOTION_REPORT_IDX]  = {.id             = HID_MOUSE_MOTION_REPORT_ID,           
                                    .size           = HID_MOUSE_MOTION_REPORT_SIZE,     
                                    .cfg            = HOGPD_CFG_REPORT_IN | HOGPD_REPORT_NTF_CFG_MASK | HOGPD_CFG_REPORT_WR, 
                                    .read_callback  = NULL,
                                    .write_callback = NULL},
#if CFG_USE_DIGITIZER
	[HID_DIGITIZER_REPORT_IDX]  	 = {.id             = HID_DIGITIZER_REPORT_ID,           
                                    .size           = HID_DIGITIZER_REPORT_SIZE,     
                                    .cfg            = HOGPD_CFG_REPORT_IN | HOGPD_REPORT_NTF_CFG_MASK | HOGPD_CFG_REPORT_WR, 
                                    .read_callback  = NULL,
                                    .write_callback = NULL},																	
																		
	[HID_DIGITIZER2_REPORT_IDX]    = {.id           	= HID_DIGITIZER2_REPORT_ID,           
                                    .size           = HID_DIGITIZER_REPORT_SIZE,     
                                    .cfg            = HOGPD_CFG_REPORT_IN | HOGPD_REPORT_NTF_CFG_MASK | HOGPD_CFG_REPORT_WR,
                                    .read_callback  = NULL,
                                    .write_callback = NULL},
#else
	[HID_GAMEPAD_AXIS_REPORT_IDX]  	 = {.id           = HID_GAMEPAD_AXIS_REPORT_ID,           
                                    .size           = HID_GAMEPAD_AXIS_REPORT_SIZE,     
                                    .cfg            = HOGPD_CFG_REPORT_IN | HOGPD_REPORT_NTF_CFG_MASK | HOGPD_CFG_REPORT_WR, 
                                    .read_callback  = NULL,
                                    .write_callback = NULL},																	
																		
	[HID_GAMEPAD_BUTTONS_REPORT_IDX]    = {.id        = HID_GAMEPAD_BUTTONS_REPORT_ID,           
                                    .size           = HID_GAMEPAD_BUTTONS_REPORT_SIZE,     
                                    .cfg            = HOGPD_CFG_REPORT_IN | HOGPD_REPORT_NTF_CFG_MASK | HOGPD_CFG_REPORT_WR,
                                    .read_callback  = NULL,
                                    .write_callback = NULL},

#endif																		
};

static const hogpd_params_t hogpd_params = {
/**
 ****************************************************************************************
 * Use BOOT MODE                                                                        
 ****************************************************************************************
 */
    .boot_protocol_mode   = true,//false,
        
/**
 ****************************************************************************************
 * Include BATT in HID                                                                  
 ****************************************************************************************
 */
    .batt_external_report = false,
        
/**
 ****************************************************************************************
 * Set RemoteWakeup mode. Remote Host may not handle properly remote wakeup when the 
 * inactivity timeout is on. Some Hosts do not expect to receive LL_TERMINATE_IND from 
 * Wakeup capable devices while they are sleeping.
 ****************************************************************************************
 */
    .remote_wakeup        = false,
        
/**
 ****************************************************************************************
 * Set normally connectable mode                                                                
 ****************************************************************************************
 */
#ifdef NORMALLY_CONNECTABLE    
    .normally_connectable = true,
#else
    .normally_connectable = false,
#endif  
  
/**
 ****************************************************************************************
 * \brief Callback for storing CCC and attributes
 ****************************************************************************************
 */
#ifdef HAS_CONNECTION_FSM
    .store_attribute_callback = user_store_ccc,
#else
    .store_attribute_callback = NULL,
#endif
};


// Report Descriptor == Report Map (HID1_11.pdf section E.6)
static const uint8_t report_map[]  =
{
#if CFG_USE_DIGITIZER
	HID_USAGE_PAGE          (HID_USAGE_PAGE_DIGITIZER),                   // USAGE_PAGE (Digitizer)
    HID_USAGE               (HID_GEN_DIGITIZER_USAGE_PEN),                    // USAGE (Pen)
    HID_COLLECTION          (HID_APPLICATION),                                  // COLLECTION (Application)
        HID_REPORT_ID           (HID_DIGITIZER_REPORT_ID),                                             // REPORT_ID (1)
        HID_USAGE_PAGE          (HID_USAGE_PAGE_DIGITIZER),
        HID_USAGE               (HID_GEN_DIGITIZER_USAGE_STYLUS),
        HID_COLLECTION          (HID_LOGICAL),                                      // COLLECTION (Logical)
            HID_USAGE_PAGE          (HID_USAGE_PAGE_DIGITIZER),
            HID_USAGE               (HID_GEN_DIGITIZER_USAGE_TIP_SWITCH),
            HID_LOGICAL_MIN_8       (0x00),
            HID_LOGICAL_MAX_8       (0x01),
            HID_REPORT_SIZE         (0x01),
            HID_REPORT_COUNT        (0x01),
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),         // INPUT (Data,Var,Abs)            
            HID_USAGE               (HID_GEN_DIGITIZER_USAGE_IN_RANGE),                 // USAGE (X)
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),         // INPUT (Data,Var,Abs)  
            HID_REPORT_SIZE         (0x06),
            HID_REPORT_COUNT        (0x01),
            HID_INPUT               (HID_CONST_BIT | HID_ARY_BIT | HID_ABS_BIT),
            HID_USAGE               (0x51),                                             //Multitouch index
            HID_REPORT_SIZE         (0x08),
            HID_REPORT_COUNT        (0x01),
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),                                    
            HID_USAGE_PAGE          (HID_USAGE_PAGE_GENERIC_DESKTOP),
            HID_LOGICAL_MIN_16      (0x00,0x00),
            HID_LOGICAL_MAX_16      (0xff,0x1f),
            HID_REPORT_SIZE         (0x10),
            HID_UNIT_EXPONENT       (0x00),
            HID_UNIT                (0x00),
            HID_USAGE               (HID_GEN_DESKTOP_USAGE_X),
            HID_PHYSICAL_MIN_16     (0x00,0x00),
            HID_PHYSICAL_MAX_16     (0x62,0x00),
            HID_REPORT_COUNT        (0x01),
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),
            HID_USAGE               (HID_GEN_DESKTOP_USAGE_Y),
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),
        HID_END_COLLECTION,                                                         //   END_COLLECTION

    HID_END_COLLECTION,                           // END_COLLECTION
		
		HID_USAGE_PAGE          (HID_USAGE_PAGE_DIGITIZER),                   // USAGE_PAGE (Digitizer)
    HID_USAGE               (HID_GEN_DIGITIZER_USAGE_PEN),                    // USAGE (Pen)
    HID_COLLECTION          (HID_APPLICATION),                                  // COLLECTION (Application)
        HID_REPORT_ID           (HID_DIGITIZER2_REPORT_ID),                                             // REPORT_ID (1)
        HID_USAGE_PAGE          (HID_USAGE_PAGE_DIGITIZER),
        HID_USAGE               (HID_GEN_DIGITIZER_USAGE_STYLUS),
        HID_COLLECTION          (HID_LOGICAL),                                      // COLLECTION (Logical)
            HID_USAGE_PAGE          (HID_USAGE_PAGE_DIGITIZER),
            HID_USAGE               (HID_GEN_DIGITIZER_USAGE_TIP_SWITCH),
            HID_LOGICAL_MIN_8       (0x00),
            HID_LOGICAL_MAX_8       (0x01),
            HID_REPORT_SIZE         (0x01),
            HID_REPORT_COUNT        (0x01),
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),         // INPUT (Data,Var,Abs)            
            HID_USAGE               (HID_GEN_DIGITIZER_USAGE_IN_RANGE),                 // USAGE (X)
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),         // INPUT (Data,Var,Abs)  
            HID_REPORT_SIZE         (0x06),
            HID_REPORT_COUNT        (0x01),
            HID_INPUT               (HID_CONST_BIT | HID_ARY_BIT | HID_ABS_BIT),
            HID_USAGE               (0x51),                                             //Multitouch index
            HID_REPORT_SIZE         (0x08),
            HID_REPORT_COUNT        (0x01),
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),                                    
            HID_USAGE_PAGE          (HID_USAGE_PAGE_GENERIC_DESKTOP),
            HID_LOGICAL_MIN_16      (0x00,0x00),
            HID_LOGICAL_MAX_16      (0xff,0x1f),
            HID_REPORT_SIZE         (0x10),
            HID_UNIT_EXPONENT       (0x00),
            HID_UNIT                (0x00),
            HID_USAGE               (HID_GEN_DESKTOP_USAGE_X),
            HID_PHYSICAL_MIN_16     (0x00,0x00),
            HID_PHYSICAL_MAX_16     (0x62,0x00),
            HID_REPORT_COUNT        (0x01),
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),
            HID_USAGE               (HID_GEN_DESKTOP_USAGE_Y),
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),
        HID_END_COLLECTION,                                                         //   END_COLLECTION

    HID_END_COLLECTION,                           // END_COLLECTION
#else
    HID_USAGE_PAGE          (HID_USAGE_PAGE_GENERIC_DESKTOP),                   // USAGE_PAGE (Generic Desktop)
    HID_USAGE               (HID_GEN_DESKTOP_USAGE_GAMEPAD),                    // USAGE (Gamepad)
    HID_COLLECTION          (HID_APPLICATION),                                  // COLLECTION (Application)

        HID_REPORT_ID           (HID_GAMEPAD_AXIS_REPORT_ID),                                             // REPORT_ID (1)
        HID_COLLECTION          (HID_LOGICAL),                                      // COLLECTION (Logical)
            HID_USAGE               (HID_GEN_DESKTOP_USAGE_X),                          // USAGE (X)
            HID_USAGE               (HID_GEN_DESKTOP_USAGE_Y),                          // USAGE (Y)
            HID_USAGE               (HID_GEN_DESKTOP_USAGE_Z),                          // USAGE (Z)
            HID_USAGE               (HID_GEN_DESKTOP_USAGE_RZ),                         // USAGE (RZ)
            //HID_USAGE               (HID_GEN_DESKTOP_USAGE_RY),                         // USAGE (RY)
            //HID_USAGE               (HID_GEN_DESKTOP_USAGE_RZ),                         // USAGE (RZ)
            HID_LOGICAL_MIN_8       (0x00),                                             // LOGICAL_MINIMUM (0)
            HID_LOGICAL_MAX_8       (0xff),                                       // LOGICAL_MAXIMUM (255)
            HID_PHYSICAL_MIN_8      (0x00),                                             // PHYSICAL_MINIMUM (0)
            HID_PHYSICAL_MAX_8      (0xff),                                       // PHYSICAL_MAXIMUM (255)
            HID_REPORT_SIZE         (0x08),                                             // REPORT_SIZE (8)
            HID_REPORT_COUNT        (0x04),                                             // REPORT_COUNT (3)
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),         // INPUT (Data,Var,Abs)
        HID_END_COLLECTION,                                                         //   END_COLLECTION
    
        HID_REPORT_ID           (HID_GAMEPAD_BUTTONS_REPORT_ID),                                             // REPORT_ID (2)    
        HID_COLLECTION          (HID_LOGICAL),                                      // COLLECTION (Logical)
            HID_USAGE_PAGE          (HID_USAGE_PAGE_BUTTONS),                           // USAGE_PAGE (Buttons)
            HID_USAGE_MIN_8         (0x01),                                             // USAGE_MINIMUM (Button A)
            HID_USAGE_MAX_8         (0x08),                                             // USAGE_MAXIMUM (Button R1)
            HID_REPORT_COUNT        (0x08),                                             // REPORT_COUNT (16)
            HID_REPORT_SIZE         (0x01),                                             // REPORT_SIZE (1)
            HID_LOGICAL_MAX_8       (0x01),                                             // LOGICAL_MAXIMUM (1)
            HID_LOGICAL_MIN_8       (0x00),                                             // LOGICAL_MINIMUM (0)
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),         // Input (Data, Variable, Absolute)
						
			HID_USAGE_PAGE          (HID_USAGE_PAGE_BUTTONS),                           // USAGE_PAGE (Buttons)
            HID_USAGE_MIN_8         (0x09),                                             // USAGE_MINIMUM (Button R2)
            HID_USAGE_MAX_8         (0x0F),                                             // USAGE_MAXIMUM (Button TR)
            HID_REPORT_COUNT        (0x07),                                             // REPORT_COUNT (16)
            HID_REPORT_SIZE         (0x01),                                             // REPORT_SIZE (1)
            HID_LOGICAL_MAX_8       (0x01),                                             // LOGICAL_MAXIMUM (1)
            HID_LOGICAL_MIN_8       (0x00),                                             // LOGICAL_MINIMUM (0)
            HID_INPUT               (HID_DATA_BIT | HID_VAR_BIT | HID_ABS_BIT),         // Input (Data, Variable, Absolute)
            HID_REPORT_COUNT        (0x01),                                             //1 bit padding
            HID_INPUT               (HID_CONST_BIT | HID_ARY_BIT | HID_ABS_BIT),        //  Input (Cnst,Ary,Abs) 
            //HID_REPORT_COUNT        (0x01),                                             // Report Count (1)
            //HID_REPORT_SIZE         (0x06),                                             // Report Size (6)
            //HID_INPUT               (HID_CONST_BIT),                                    // Input (Constant) for padding    
        HID_END_COLLECTION,                          //   END_COLLECTION

    HID_END_COLLECTION                           // END_COLLECTION
#endif
};

#endif // _USER_HOGPD_CONFIG_H_
