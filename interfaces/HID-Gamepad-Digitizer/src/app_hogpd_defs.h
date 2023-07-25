/**
 ****************************************************************************************
 *
 * @file app_hogpd_defs.h
 *
 * @brief HID report map definitions.
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

#ifndef _APP_HOGPD_DEFS_H_
#define _APP_HOGPD_DEFS_H_

#include "rwip_config.h"
#include "hogp_common.h"
#include "hogpd_task.h"

typedef void (*store_attribute_callback_t)(uint16_t uuid, int attr_num, int value);

typedef struct {
    bool boot_protocol_mode;
    bool batt_external_report;
    bool remote_wakeup;
    bool normally_connectable;
    store_attribute_callback_t store_attribute_callback;
} hogpd_params_t;

typedef struct {
    uint8_t id;
    uint8_t cfg;
    uint16_t size;
    void (*read_callback)(uint8_t conidx, uint8_t *report_data, uint16_t *report_length);
    void (*write_callback)(uint8_t conidx, struct hogpd_report_info *report_info);
} hogpd_reports_t;

#define HID_USAGE_PAGE_SENSOR									 0x05,0x20
                                                                 
//sensor category usages                                         
#define HID_USAGE_SENSOR_TYPE_COLLECTION						 0x09,0x01
                                                                 
//sensor category biometric                                      
#define HID_USAGE_SENSOR_CATEGORY_BIOMETRIC						 0x09,0x10
#define HID_USAGE_SENSOR_TYPE_BIOMETRIC_PRESENCE				 0x09,0x11
#define HID_USAGE_SENSOR_TYPE_BIOMETRIC_PROXIMITY				 0x09,0x12
#define HID_USAGE_SENSOR_TYPE_BIOMETRIC_TOUCH					 0x09,0x13
                                                                 
//sensor category electrical                                     
#define HID_USAGE_SENSOR_CATEGORY_ELECTRICAL					 0x09,0x20
#define HID_USAGE_SENSOR_TYPE_ELECTRICAL_CAPACITANCE			 0x09,0x21
#define HID_USAGE_SENSOR_TYPE_ELECTRICAL_CURRENT				 0x09,0x22
#define HID_USAGE_SENSOR_TYPE_ELECTRICAL_POWER					 0x09,0x23
#define HID_USAGE_SENSOR_TYPE_ELECTRICAL_INDUCTANCE				 0x09,0x24
#define HID_USAGE_SENSOR_TYPE_ELECTRICAL_RESISTANCE				 0x09,0x25
#define HID_USAGE_SENSOR_TYPE_ELECTRICAL_VOLTAGE				 0x09,0x26
#define HID_USAGE_SENSOR_TYPE_ELECTRICAL_POTENTIOMETER			 0x09,0x27
#define HID_USAGE_SENSOR_TYPE_ELECTRICAL_FREQUENCY				 0x09,0x28
#define HID_USAGE_SENSOR_TYPE_ELECTRICAL_PERIOD					 0x09,0x29
                                                                 
//sensor category environmental                                  
#define HID_USAGE_SENSOR_CATEGORY_ENVIRONMENTAL					 0x09,0x30
#define HID_USAGE_SENSOR_TYPE_ENVIRONMENTAL_ATMOSPHERIC_PRESSURE 0x09,0x31
#define HID_USAGE_SENSOR_TYPE_ENVIRONMENTAL_HUMIDITY			 0x09,0x32
#define HID_USAGE_SENSOR_TYPE_ENVIRONMENTAL_TEMPERATURE			 0x09,0x33
#define HID_USAGE_SENSOR_TYPE_ENVIRONMENTAL_WIND_DIRECTION		 0x09,0x34
#define HID_USAGE_SENSOR_TYPE_ENVIRONMENTAL_WIND_SPEED			 0x09,0x35
                                                                 
//sensor category light                                          
#define HID_USAGE_SENSOR_CATEGORY_LIGHT							 0x09,0x40
#define HID_USAGE_SENSOR_TYPE_LIGHT_AMBIENTLIGHT				 0x09,0x41
#define HID_USAGE_SENSOR_TYPE_LIGHT_CONSUMER_INFRARED			 0x09,0x42
                                                                 
//sensor category location                                       
#define HID_USAGE_SENSOR_CATEGORY_LOCATION						 0x09,0x50
#define HID_USAGE_SENSOR_TYPE_LOCATION_BROADCAST				 0x09,0x51
#define HID_USAGE_SENSOR_TYPE_LOCATION_DEAD_RECKONING			 0x09,0x52
#define HID_USAGE_SENSOR_TYPE_LOCATION_GPS						 0x09,0x53
#define HID_USAGE_SENSOR_TYPE_LOCATION_LOOKUP					 0x09,0x54
#define HID_USAGE_SENSOR_TYPE_LOCATION_OTHER					 0x09,0x55
#define HID_USAGE_SENSOR_TYPE_LOCATION_STATIC					 0x09,0x56
#define HID_USAGE_SENSOR_TYPE_LOCATION_TRIANGULATION			 0x09,0x57
                                                                 
//sensor category mechanical                                     
#define HID_USAGE_SENSOR_CATEGORY_MECHANICAL					 0x09,0x60
#define HID_USAGE_SENSOR_TYPE_MECHANICAL_BOOLEAN_SWITCH			 0x09,0x61
#define HID_USAGE_SENSOR_TYPE_MECHANICAL_BOOLEAN_SWITCH_ARRAY	 0x09,0x62
#define HID_USAGE_SENSOR_TYPE_MECHANICAL_MULTIVALUE_SWITCH		 0x09,0x63
#define HID_USAGE_SENSOR_TYPE_MECHANICAL_FORCE					 0x09,0x64
#define HID_USAGE_SENSOR_TYPE_MECHANICAL_PRESSURE				 0x09,0x65
#define HID_USAGE_SENSOR_TYPE_MECHANICAL_STRAIN					 0x09,0x66
#define HID_USAGE_SENSOR_TYPE_MECHANICAL_SCALE_WEIGHT			 0x09,0x67
#define HID_USAGE_SENSOR_TYPE_MECHANICAL_VIBRATOR				 0x09,0x68
#define HID_USAGE_SENSOR_TYPE_MECHANICAL_HALL_EFFECT_SWITCH		 0x09,0x69
                                                                 
//sensor category motion                                         
#define HID_USAGE_SENSOR_CATEGORY_MOTION						 0x09,0x70
#define HID_USAGE_SENSOR_TYPE_MOTION_ACCELEROMETER_1D			 0x09,0x71
#define HID_USAGE_SENSOR_TYPE_MOTION_ACCELEROMETER_2D			 0x09,0x72
#define HID_USAGE_SENSOR_TYPE_MOTION_ACCELEROMETER_3D			 0x09,0x73
#define HID_USAGE_SENSOR_TYPE_MOTION_GYROMETER_1D				 0x09,0x74
#define HID_USAGE_SENSOR_TYPE_MOTION_GYROMETER_2D				 0x09,0x75
#define HID_USAGE_SENSOR_TYPE_MOTION_GYROMETER_3D				 0x09,0x76
#define HID_USAGE_SENSOR_TYPE_MOTION_MOTION_DETECTOR			 0x09,0x77
#define HID_USAGE_SENSOR_TYPE_MOTION_SPEEDOMETER				 0x09,0x78
#define HID_USAGE_SENSOR_TYPE_MOTION_ACCELEROMETER				 0x09,0x79
#define HID_USAGE_SENSOR_TYPE_MOTION_GYROMETER					 0x09,0x7A
                                                                 
//sensor category orientation                                    
#define HID_USAGE_SENSOR_CATEGORY_ORIENTATION					 0x09,0x80
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_COMPASS_1D			 0x09,0x81
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_COMPASS_2D			 0x09,0x82
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_COMPASS_3D			 0x09,0x83
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_INCLINOMETER_1D		 0x09,0x84
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_INCLINOMETER_2D		 0x09,0x85
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_INCLINOMETER_3D		 0x09,0x86
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_DISTANCE_1D			 0x09,0x87
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_DISTANCE_2D			 0x09,0x88
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_DISTANCE_3D			 0x09,0x89
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_DEVICE_ORIENTATION	 0x09,0x8A
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_COMPASS				 0x09,0x8B
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_INCLINOMETER			 0x09,0x8C
#define HID_USAGE_SENSOR_TYPE_ORIENTATION_DISTANCE				 0x09,0x8D
                                                                 
//sensor category scanner                                        
#define HID_USAGE_SENSOR_CATEGORY_SCANNER						 0x09,0x90
#define HID_USAGE_SENSOR_TYPE_SCANNER_BARCODE					 0x09,0x91
#define HID_USAGE_SENSOR_TYPE_SCANNER_RFID						 0x09,0x92
#define HID_USAGE_SENSOR_TYPE_SCANNER_NFC						 0x09,0x93
                                                                 
//sensor category time                                           
#define HID_USAGE_SENSOR_CATEGORY_TIME							 0x09,0xA0
#define HID_USAGE_SENSOR_TYPE_TIME_ALARM						 0x09,0xA1
#define HID_USAGE_SENSOR_TYPE_TIME_RTC							 0x09,0xA2
                                                                 
//sensor category other                                          
#define HID_USAGE_SENSOR_CATEGORY_OTHER							 0x09,0xE0
#define HID_USAGE_SENSOR_TYPE_OTHER_CUSTOM						 0x09,0xE1
#define HID_USAGE_SENSOR_TYPE_OTHER_GENERIC						 0x09,0xE2
#define HID_USAGE_SENSOR_TYPE_OTHER_GENERIC_ENUMERATOR			 0x09,0xE3
                                                                 
//unit usages                                                    
#define HID_USAGE_SENSOR_UNITS_NOT_SPECIFIED					 0x65,0x00
#define HID_USAGE_SENSOR_UNITS_LUX								 0x67,0xE1,0x00,0x00,0x01
#define HID_USAGE_SENSOR_UNITS_KELVIN							 0x67,0x01,0x00,0x01,0x00
#define HID_USAGE_SENSOR_UNITS_FAHRENHEIT						 0x67,0x03,0x00,0x01,0x00
#define HID_USAGE_SENSOR_UNITS_PASCAL							 0x66,0xF1,0xE1
#define HID_USAGE_SENSOR_UNITS_NEWTON							 0x66,0x11,0xE1
#define HID_USAGE_SENSOR_UNITS_METERS_PER_SECOND				 0x66,0x11,0xF0
#define HID_USAGE_SENSOR_UNITS_METERS_PER_SEC_SQRD				 0x66,0x11,0xE0
#define HID_USAGE_SENSOR_UNITS_FARAD							 0x67,0xE1,0x4F,0x20,0x00
#define HID_USAGE_SENSOR_UNITS_AMPERE							 0x67,0x01,0x00,0x10,0x00
#define HID_USAGE_SENSOR_UNITS_WATT								 0x66,0x21,0xD1
#define HID_USAGE_SENSOR_UNITS_HENRY							 0x67,0x21,0xE1,0xE0,0x00
#define HID_USAGE_SENSOR_UNITS_OHM								 0x67,0x21,0xD1,0xE0,0x00
#define HID_USAGE_SENSOR_UNITS_VOLT								 0x67,0x21,0xD1,0xF0,0x00
#define HID_USAGE_SENSOR_UNITS_HERTZ							 0x66,0x01,0xF0
#define HID_USAGE_SENSOR_UNITS_DEGREES							 0x65,0x14
#define HID_USAGE_SENSOR_UNITS_DEGREES_PER_SECOND				 0x66,0x14,0xF0
#define HID_USAGE_SENSOR_UNITS_DEGREES_PER_SEC_SQRD				 0x66,0x14,0xE0
#define HID_USAGE_SENSOR_UNITS_RADIANS							 0x65,0x12
#define HID_USAGE_SENSOR_UNITS_RADIANS_PER_SECOND				 0x66,0x12,0xF0
#define HID_USAGE_SENSOR_UNITS_RADIANS_PER_SEC_SQRD				 0x66,0x12,0xE0
#define HID_USAGE_SENSOR_UNITS_SECOND							 0x66,0x01,0x10
#define HID_USAGE_SENSOR_UNITS_GAUSS							 0x67,0x01,0xE1,0xF0,0x00
#define HID_USAGE_SENSOR_UNITS_GRAM								 0x66,0x01,0x01
#define HID_USAGE_SENSOR_UNITS_CENTIMETER						 0x65,0x11
                                                                 
#ifdef DEFINE_NON_HID_UNITS                                      
#define HID_USAGE_SENSOR_UNITS_CELSIUS							 �Use Unit(Kelvin) and subtract 273.15�
#define HID_USAGE_SENSOR_UNITS_KILOGRAM							 �Use Unit(gram) and UnitExponent(0x03)�
#define HID_USAGE_SENSOR_UNITS_METER							 �Use Unit(centimeter) and UnitExponent(0x02)�
#define HID_USAGE_SENSOR_UNITS_BAR								 �Use Unit(Pascal) and UnitExponent(0x05)�
#define HID_USAGE_SENSOR_UNITS_KNOT								 �Use Unit(m/s) and multiply by 1852/3600�
#define HID_USAGE_SENSOR_UNITS_PERCENT							 �Use Unit(Not_Specified)�
#define HID_USAGE_SENSOR_UNITS_G								 �Use Unit(m/s2) and divide by 9.8�
#define HID_USAGE_SENSOR_UNITS_MILLISECOND						 �Use Unit(second) and UnitExponent(0x0D)�
#define HID_USAGE_SENSOR_UNITS_MILLIGAUSS						 �Use Unit(Gauss) and UnitExponent(0x0D)�
#endif

//data type usage switches -- we use them as modifiers for sensor properties & data fields
//to create thresholds, for example.
//NOTE: the usage tables actually define these as two bytes, but in order
//to get the define macros to work so these are �or-ed� these are defined
//here as only one byte.
#define HID_USAGE_SENSOR_DATA_MOD_NONE							 0x00 // US
#define HID_USAGE_SENSOR_DATA_MOD_CHANGE_SENSITIVITY_ABS		 0x10 // US
#define HID_USAGE_SENSOR_DATA_MOD_MAX							 0x20 // US
#define HID_USAGE_SENSOR_DATA_MOD_MIN							 0x30 // US
#define HID_USAGE_SENSOR_DATA_MOD_ACCURACY						 0x40 // US
#define HID_USAGE_SENSOR_DATA_MOD_RESOLUTION					 0x50 // US
#define HID_USAGE_SENSOR_DATA_MOD_THRESHOLD_HIGH				 0x60 // US
#define HID_USAGE_SENSOR_DATA_MOD_THRESHOLD_LOW					 0x70 // US
#define HID_USAGE_SENSOR_DATA_MOD_CALIBRATION_OFFSET			 0x80 // US
#define HID_USAGE_SENSOR_DATA_MOD_CALIBRATION_MULTIPLIER		 0x90 // US
#define HID_USAGE_SENSOR_DATA_MOD_REPORT_INTERVAL				 0xA0 // US
#define HID_USAGE_SENSOR_DATA_MOD_FREQUENCY_MAX					 0xB0 // US
#define HID_USAGE_SENSOR_DATA_MOD_PERIOD_MAX					 0xC0 // US
#define HID_USAGE_SENSOR_DATA_MOD_CHANGE_SENSITIVITY_RANGE_PCT	 0xD0 // US
#define HID_USAGE_SENSOR_DATA_MOD_CHANGE_SENSITIVITY_REL_PCT	 0xE0 // US
#define HID_USAGE_SENSOR_DATA_MOD_VENDOR_RESERVED				 0xF0 // US
                                                                 
//state usages                                                   
#define HID_USAGE_SENSOR_STATE									 0x0A,0x01,0x02 // NAry
	//state selectors                                            
	#define HID_USAGE_SENSOR_STATE_UNKNOWN						 0x0A,0x00,0x08 // Sel
	#define HID_USAGE_SENSOR_STATE_READY						 0x0A,0x01,0x08 // Sel
	#define HID_USAGE_SENSOR_STATE_NOT_AVAILABLE				 0x0A,0x02,0x08 // Sel
	#define HID_USAGE_SENSOR_STATE_NO_DATA						 0x0A,0x03,0x08 // Sel
	#define HID_USAGE_SENSOR_STATE_INITIALIZING					 0x0A,0x04,0x08 // Sel
	#define HID_USAGE_SENSOR_STATE_ACCESS_DENIED				 0x0A,0x05,0x08 // Sel
	#define HID_USAGE_SENSOR_STATE_ERROR						 0x0A,0x06,0x08 // Sel
                                                                 
//event usages                                                   
#define HID_USAGE_SENSOR_EVENT									 0x0A,0x02,0x02 // NAry
	//event selectors                                            
	#define HID_USAGE_SENSOR_EVENT_UNKNOWN						 0x0A,0x10,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_STATE_CHANGED				 0x0A,0x11,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_PROPERTY_CHANGED				 0x0A,0x12,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_DATA_UPDATED					 0x0A,0x13,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_POLL_RESPONSE				 0x0A,0x14,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_CHANGE_SENSITIVITY			 0x0A,0x15,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_MAX_REACHED					 0x0A,0x16,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_MIN_REACHED					 0x0A,0x17,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_UPWARD	 0x0A,0x18,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_HIGH_THESHOLD_CROSS_ABOVE	 0x0A,0x18,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_DOWNWARD 0x0A,0x19,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_BELOW	 0x0A,0x19,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_LOW_THRESHOLD_CROSS_UPWARD	 0x0A,0x1A,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_LOW_THRESHOLD_CROSS_ABOVE	 0x0A,0x1A,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_LOW_THRESHOLD_CROSS_DOWNWARD	 0x0A,0x1B,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_LOW_THRESHOLD_CROSS_BELOW	 0x0A,0x1B,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_UPWARD	 0x0A,0x1C,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_ABOVE	 0x0A,0x1C,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_DOWNWARD 0x0A,0x1D,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_BELOW	 0x0A,0x1D,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_PERIOD_EXCEEDED				 0x0A,0x1E,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_FREQUENCY_EXCEEDED			 0x0A,0x1F,0x08 // Sel
	#define HID_USAGE_SENSOR_EVENT_COMPLEX_TRIGGER				 0x0A,0x20,0x08 // Sel
                                                                 
//property usages (get/set feature report)                       
#define HID_USAGE_SENSOR_PROPERTY								 0x0A,0x00,0x03
#define HID_USAGE_SENSOR_PROPERTY_FRIENDLY_NAME					 0x0A,0x01,0x03
#define HID_USAGE_SENSOR_PROPERTY_PERSISTENT_UNIQUE_ID			 0x0A,0x02,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_STATUS					 0x0A,0x03,0x03
#define HID_USAGE_SENSOR_PROPERTY_MINIMUM_REPORT_INTERVAL		 0x0A,0x04,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_MANUFACTURER			 0x0A,0x05,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_MODEL					 0x0A,0x06,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_SERIAL_NUMBER			 0x0A,0x07,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_DESCRIPTION			 0x0A,0x08,0x03
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_CONNECTION_TYPE		 0x0A,0x09,0x03 // NAry
	//begin connection type selectors
	#define HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_INTEGRATED 0x0A,0x30,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_ATTACHED	0x0A,0x31,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_EXTERNAL	0x0A,0x32,0x08 // Sel
	//end connection type selectors                                 
#define HID_USAGE_SENSOR_PROPERTY_SENSOR_DEVICE_PATH				0x0A,0x0A,0x03
#define HID_USAGE_SENSOR_PROPERTY_HARDWARE_REVISION					0x0A,0x0B,0x03
#define HID_USAGE_SENSOR_PROPERTY_FIRMWARE_VERSION					0x0A,0x0C,0x03
#define HID_USAGE_SENSOR_PROPERTY_RELEASE_DATE						0x0A,0x0D,0x03
#define HID_USAGE_SENSOR_PROPERTY_REPORT_INTERVAL					0x0A,0x0E,0x03
#define HID_USAGE_SENSOR_PROPERTY_CHANGE_SENSITIVITY_ABS			0x0A,0x0F,0x03
#define HID_USAGE_SENSOR_PROPERTY_CHANGE_SENSITIVITY_RANGE_PCT		0x0A,0x10,0x03
#define HID_USAGE_SENSOR_PROPERTY_CHANGE_SENSITIVITY_REL_PCT		0x0A,0x11,0x03
#define HID_USAGE_SENSOR_PROPERTY_ACCURACY							0x0A,0x12,0x03
#define HID_USAGE_SENSOR_PROPERTY_RESOLUTION						0x0A,0x13,0x03
#define HID_USAGE_SENSOR_PROPERTY_RANGE_MAXIMUM						0x0A,0x14,0x03
#define HID_USAGE_SENSOR_PROPERTY_RANGE_MINIMUM						0x0A,0x15,0x03
#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE					0x0A,0x16,0x03 // NAry
	//begin reporting state selectors
	#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS				0x0A,0x40,0x08 // Sel
	#define HID_USAGE_REPORTING_STATE_ON_NONE								0x0A,0x40,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS			0x0A,0x41,0x08 // Sel
	#define HID_USAGE_REPORTING_STATE_ON_ALL								0x0A,0x41,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS		0x0A,0x42,0x08 // Sel
	#define HID_USAGE_REPORTING_STATE_ON_THRESHOLD							0x0A,0x42,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_WAKE		0x0A,0x43,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_WAKE		0x0A,0x44,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_WAKE	0x0A,0x45,0x08 // Sel
	//end reporting state selectors
#define HID_USAGE_SENSOR_PROPERTY_SAMPLING_RATE							0x0A,0x17,0x03
#define HID_USAGE_SENSOR_PROPERTY_RESPONSE_CURVE						0x0A,0x18,0x03
#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE							0x0A,0x19,0x03 // NAry
	//begin power state selectors
	#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_UNDEFINED				0x0A,0x50,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D0_FULL_POWER			0x0A,0x51,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D1_LOW_POWER			0x0A,0x52,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D2_STANDBY_WITH_WAKE	0x0A,0x53,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D3_SLEEP_WITH_WAKE	0x0A,0x54,0x08 // Sel
	#define HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D4_POWER_OFF			0x0A,0x55,0x08 // Sel
	//end power state selectors

//data type location
//data field usages (input report
#define HID_USAGE_SENSOR_DATA_LOCATION									 0x0A,0x00,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_DESIRED_ACCURACY					 0x0A,0x01,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_ALTITUDE_ANTENNA_SEALEVEL		 0x0A,0x02,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_DIFFERENTIAL_REFERENCE_STATION_ID 0x0A,0x03,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_ALTITIDE_ELIPSOID_ERROR			 0x0A,0x04,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_ALTITIDE_ELIPSOID				 0x0A,0x05,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_ALTITUDE_SEALEVEL_ERROR			 0x0A,0x06,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_ALTITUDE_SEALEVEL				 0x0A,0x07,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_DGPS_DATA_AGE					 0x0A,0x08,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_ERROR_RADIUS						 0x0A,0x09,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_FIX_QUALITY						 0x0A,0x0A,0x04 // NAry
	//begin fix quality selectors                                        
	#define HID_USAGE_SENSOR_DATA_FIX_QUALITY_NO_FIX					 0x0A,0x70,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_FIX_QUALITY_GPS						 0x0A,0x71,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_FIX_QUALITY_DGPS						 0x0A,0x72,0x08 // Sel
	//end fix quality selectors					                         
#define HID_USAGE_SENSOR_DATA_LOCATION_FIX_TYPE							 0x0A,0x0B,0x04 // NAry
	//begin fix type selectors                                           
	#define HID_USAGE_SENSOR_DATA_FIX_TYPE_NO_FIX						 0x0A,0x80,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_FIX_TYPE_GPS_SPS_MODE_FIX_VALID		 0x0A,0x81,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_FIX_TYPE_DGPS_SPS_MODE_FIX_VALID		 0x0A,0x82,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_FIX_TYPE_GPS_PPS_MODE_FIX_VALID		 0x0A,0x83,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_FIX_TYPE_REAL_TIME_KINEMATIC			 0x0A,0x84,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_FIX_TYPE_FLOAT_RTK					 0x0A,0x85,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_FIX_TYPE_ESTIMATED_DEAD_RECKONING		 0x0A,0x86,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_FIX_TYPE_MANUAL_INPUT_MODE			 0x0A,0x87,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_FIX_TYPE_SIMULATOR_MODE				 0x0A,0x88,0x08 // Sel
	//end fix type selectors                                             
#define HID_USAGE_SENSOR_DATA_LOCATION_GEOIDAL_SEPARATION				 0x0A,0x0C,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_GPS_OPERATION_MODE				 0x0A,0x0D,0x04 // NAry
	//begin gps operation mode selectors                                 
	#define HID_USAGE_SENSOR_DATA_GPS_OP_MODE_MANUAL					 0x0A,0x90,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_GPS_OP_MODE_AUTOMATIC					 0x0A,0x91,0x08 // Sel
	//end gps operation mode selectors
#define HID_USAGE_SENSOR_DATA_LOCATION_GPS_SELECTION_MODE				 0x0A,0x0E,0x04 // NAry
	//begin gps selection mode selectors
	#define HID_USAGE_SENSOR_DATA_GPS_SEL_MODE_AUTONOMOUS				 0x0A,0xA0,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_GPS_SEL_MODE_DGPS						 0x0A,0xA1,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_GPS_SEL_MODE_ESTIMATED_DEAD_RECKONING	 0x0A,0xA2,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_GPS_SEL_MODE_MANUAL_INPUT				 0x0A,0xA3,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_GPS_SEL_MODE_SIMULATOR				 0x0A,0xA4,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_GPS_SEL_MODE_DATA_NOT_VALID			 0x0A,0xA5,0x08 // Sel
	//end gps selection mode selectors
#define HID_USAGE_SENSOR_DATA_LOCATION_GPS_STATUS						 0x0A,0x0F,0x04 // NAry
	//begin gps status selectors                                         
	#define HID_USAGE_SENSOR_DATA_GPS_STATUS_DATA_VALID					 0x0A,0xB0,0x08 // Sel
	#define HID_USAGE_SENSOR_DATA_GPS_STATUS_DATA_NOT_VALID				 0x0A,0xB1,0x08 // Sel
	//end gps status selectors                                           
#define HID_USAGE_SENSOR_DATA_LOCATION_POSITION_DILUTION_OF_PRECISION	 0x0A,0x10,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_HORIZONTAL_DILUTION_OF_PRECISION	 0x0A,0x11,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_VERTICAL_DILUTION_OF_PRECISION	 0x0A,0x12,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_LATITUDE							 0x0A,0x13,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_LONGITUDE						 0x0A,0x14,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_TRUE_HEADING						 0x0A,0x15,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_MAGNETIC_HEADING					 0x0A,0x16,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_MAGNETIC_VARIATION				 0x0A,0x17,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_SPEED							 0x0A,0x18,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_SATELLITES_IN_VIEW				 0x0A,0x19,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_SATELLITES_IN_VIEW_AZIMUTH		 0x0A,0x1A,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_SATELLITES_IN_VIEW_ELEVATION		 0x0A,0x1B,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_SATELLITES_IN_VIEW_ID			 0x0A,0x1C,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_SATELLITES_IN_VIEW_PRNs			 0x0A,0x1D,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_SATELLITES_IN_VIEW_STN_RATIO		 0x0A,0x1E,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_SATELLITES_USED_COUNT			 0x0A,0x1F,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_SATELLITES_USED_PRNs				 0x0A,0x20,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_NMEA_SENTENCE					 0x0A,0x21,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_ADDRESS_LINE_1					 0x0A,0x22,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_ADDRESS_LINE_2					 0x0A,0x23,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_CITY								 0x0A,0x24,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_STATE_OR_PROVINCE				 0x0A,0x25,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_COUNTRY_OR_REGION				 0x0A,0x26,0x04
#define HID_USAGE_SENSOR_DATA_LOCATION_POSTAL_CODE						 0x0A,0x27,0x04
                                                                         
//property usages (get/set feature report)                               
#define HID_USAGE_SENSOR_PROPERTY_LOCATION								 0x0A,0x2A,0x04
#define HID_USAGE_SENSOR_PROPERTY_LOCATION_DESIRED_ACCURACY				 0x0A,0x2B,0x04 // NAry
	//begin location desired accuracy selectors                          
	#define HID_USAGE_SENSOR_DESIRED_ACCURACY_DEFAULT					 0x0A,0x60,0x08 // Sel
	#define HID_USAGE_SENSOR_DESIRED_ACCURACY_HIGH						 0x0A,0x61,0x08 // Sel
	#define HID_USAGE_SENSOR_DESIRED_ACCURACY_MEDIUM					 0x0A,0x62,0x08 // Sel
	#define HID_USAGE_SENSOR_DESIRED_ACCURACY_LOW						 0x0A,0x63,0x08 // Sel
//end location desired accuracy selectors                                
                                                                         
//data type environmental                                                
//data field usages (input report)                                       
#define HID_USAGE_SENSOR_DATA_ENVIRONMENTAL								 0x0A,0x30,0x04
#define HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_ATMOSPHERIC_PRESSURE		 0x0A,0x31,0x04
#define HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_REFERENCE_PRESSURE			 0x0A,0x32,0x04
#define HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_RELATIVE_HUMIDITY			 0x0A,0x33,0x04
#define HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE 				 0x0A,0x34,0x04
#define HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_WIND_DIRECTION 				 0x0A,0x35,0x04
#define HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_WIND_SPEED 					 0x0A,0x36,0x04
                                                                         
//property usages (get/set feature report)                               
#define HID_USAGE_SENSOR_PROPERTY_ENVIRONMENTAL 						 0x0A,0x40,0x04
#define HID_USAGE_SENSOR_PROPERTY_ENVIRONMENTAL_REFERENCE_PRESSURE 		 0x0A,0x41,0x04
                                                                         
//data type motion                                                       
//data field usages (input report)                                       
#define HID_USAGE_SENSOR_DATA_MOTION 									 0x0A,0x50,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_STATE 								 0x0A,0x51,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION 						 0x0A,0x52,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION_X_AXIS 				 0x0A,0x53,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION_Y_AXIS 				 0x0A,0x54,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ACCELERATION_Z_AXIS 				 0x0A,0x55,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_VELOCITY 					 0x0A,0x56,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_VELOCITY_X_AXIS 			 0x0A,0x57,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_VELOCITY_Y_AXIS 			 0x0A,0x58,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_VELOCITY_Z_AXIS 			 0x0A,0x59,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_POSITION 					 0x0A,0x5A,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_POSITION_X_AXIS 			 0x0A,0x5B,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_POSITION_Y_AXIS 			 0x0A,0x5C,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_ANGULAR_POSITION_Z_AXIS 			 0x0A,0x5D,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_SPEED 								 0x0A,0x5E,0x04
#define HID_USAGE_SENSOR_DATA_MOTION_INTENSITY 							 0x0A,0x5F,0x04
                                                                         
//data type orientation                                                  
//data field usages (input report)                                       
#define HID_USAGE_SENSOR_DATA_ORIENTATION 								 0x0A,0x70,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_MAGNETIC_HEADING 				 0x0A,0x71,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_MAGNETIC_HEADING_X 			 0x0A,0x72,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_MAGNETIC_HEADING_Y 			 0x0A,0x73,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_MAGNETIC_HEADING_Z 			 0x0A,0x74,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_COMPENSATED_MAGNETIC_NORTH 	 0x0A,0x75,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_COMPENSATED_TRUE_NORTH 		 0x0A,0x76,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_MAGNETIC_NORTH 				 0x0A,0x77,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_TRUE_NORTH 					 0x0A,0x78,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_DISTANCE 						 0x0A,0x79,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_DISTANCE_X 					 0x0A,0x7A,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_DISTANCE_Y 					 0x0A,0x7B,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_DISTANCE_Z 					 0x0A,0x7C,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_DISTANCE_OUT_OF_RANGE 		 0x0A,0x7D,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_TILT 							 0x0A,0x7E,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_TILT_X 						 0x0A,0x7F,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_TILT_Y 						 0x0A,0x80,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_TILT_Z 						 0x0A,0x81,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_ROTATION_MATRIX 				 0x0A,0x82,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_QUATERNION 					 0x0A,0x83,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_MAGNETIC_FLUX 				 0x0A,0x84,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_MAGNETIC_FLUX_X_AXIS 			 0x0A,0x85,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_MAGNETIC_FLUX_Y_AXIS 			 0x0A,0x86,0x04
#define HID_USAGE_SENSOR_DATA_ORIENTATION_MAGNETIC_FLUX_Z_AXIS 			 0x0A,0x87,0x04
                                                                         
//data type mechanical                                                   
//data field usages (input report)                                       
#define HID_USAGE_SENSOR_DATA_MECHANICAL								 0x0A,0x90,0x04
#define HID_USAGE_SENSOR_DATA_MECHANICAL_BOOLEAN_SWITCH_STATE			 0x0A,0x91,0x04
#define HID_USAGE_SENSOR_DATA_MECHANICAL_BOOLEAN_SWITCH_ARRAY_STATES	 0x0A,0x92,0x04
#define HID_USAGE_SENSOR_DATA_MECHANICAL_MULTIVALUE_SWITCH_VALUE		 0x0A,0x93,0x04
#define HID_USAGE_SENSOR_DATA_MECHANICAL_FORCE							 0x0A,0x94,0x04
#define HID_USAGE_SENSOR_DATA_MECHANICAL_ABSOLUTE_PRESSURE				 0x0A,0x95,0x04
#define HID_USAGE_SENSOR_DATA_MECHANICAL_GAUGE_PRESSURE					 0x0A,0x96,0x04
#define HID_USAGE_SENSOR_DATA_MECHANICAL_STRAIN							 0x0A,0x97,0x04
#define HID_USAGE_SENSOR_DATA_MECHANICAL_WEIGHT							 0x0A,0x98,0x04
                                                                         
//property usages (get/set feature report)                               
#define HID_USAGE_SENSOR_PROPERTY_MECHANICAL							 0x0A,0xA0,0x04
#define HID_USAGE_SENSOR_PROPERTY_MECHANICAL_VIBRATION_STATE			 0x0A,0xA1,0x04
#define HID_USAGE_SENSOR_DATA_MECHANICAL_VIBRATION_SPEED_FORWARD		 0x0A,0xA2,0x04
#define HID_USAGE_SENSOR_DATA_MECHANICAL_VIBRATION_SPEED_BACKWARD		 0x0A,0xA3,0x04
	                                                                     
//data type biometric                                                    
//data field usages (input report)                                       
#define HID_USAGE_SENSOR_DATA_BIOMETRIC									 0x0A,0xB0,0x04
#define HID_USAGE_SENSOR_DATA_BIOMETRIC_HUMAN_PRESENCE					 0x0A,0xB1,0x04
#define HID_USAGE_SENSOR_DATA_BIOMETRIC_HUMAN_PROXIMITY_RANGE			 0x0A,0xB2,0x04
#define HID_USAGE_SENSOR_DATA_BIOMETRIC_HUMAN_PROXIMITY_OUT_OF_RANGE	 0x0A,0xB3,0x04
#define HID_USAGE_SENSOR_DATA_BIOMETRIC_HUMAN_TOUCH_STATE				 0x0A,0xB4,0x04
	                                                                     
//data type light sensor                                                 
//data field usages (input report)                                       
#define HID_USAGE_SENSOR_DATA_LIGHT 									 0x0A,0xD0,0x04
#define HID_USAGE_SENSOR_DATA_LIGHT_ILLUMINANCE 						 0x0A,0xD1,0x04
#define HID_USAGE_SENSOR_DATA_LIGHT_COLOR_TEMPERATURE 					 0x0A,0xD2,0x04
#define HID_USAGE_SENSOR_DATA_LIGHT_CHROMATICITY 						 0x0A,0xD3,0x04
#define HID_USAGE_SENSOR_DATA_LIGHT_CHROMATICITY_X 						 0x0A,0xD4,0x04
#define HID_USAGE_SENSOR_DATA_LIGHT_CHROMATICITY_Y 						 0x0A,0xD5,0x04
#define HID_USAGE_SENSOR_DATA_LIGHT_CONSUMER_IR_SENTENCE_RECEIVE 		 0x0A,0xD6,0x04
                                                                         
//property usages (get/set feature report)                               
#define HID_USAGE_SENSOR_PROPERTY_LIGHT 								 0x0A,0xE0,0x04
#define HID_USAGE_SENSOR_PROPERTY_LIGHT_CONSUMER_IR_SENTENCE_SEND 		 0x0A,0xE1,0x04
                                                                         
//data type scanner                                                      
//data field usages (input report)                                       
#define HID_USAGE_SENSOR_DATA_SCANNER 									 0x0A,0xF0,0x04
#define HID_USAGE_SENSOR_DATA_SCANNER_RFID_TAG 							 0x0A,0xF2,0x04
#define HID_USAGE_SENSOR_DATA_SCANNER_NFC_SENTENCE_RECEIVE 				 0x0A,0xF3,0x04
                                                                         
//property usages (get/set feature report)                               
#define HID_USAGE_SENSOR_PROPERTY_SCANNER 								 0x0A,0xF8,0x04
#define HID_USAGE_SENSOR_PROPERTY_SCANNER_NFC_SENTENCE_SEND 			 0x0A,0xF8,0x04
	                                                                     
//data type electrical                                                   
//data field usages (input report)                                       
#define HID_USAGE_SENSOR_DATA_ELECTRICAL 								 0x0A,0x00,0x05
#define HID_USAGE_SENSOR_DATA_ELECTRICAL_CAPACITANCE 					 0x0A,0x01,0x05
#define HID_USAGE_SENSOR_DATA_ELECTRICAL_CURRENT 						 0x0A,0x02,0x05
#define HID_USAGE_SENSOR_DATA_ELECTRICAL_POWER 							 0x0A,0x03,0x05
#define HID_USAGE_SENSOR_DATA_ELECTRICAL_INDUCTANCE 					 0x0A,0x04,0x05
#define HID_USAGE_SENSOR_DATA_ELECTRICAL_RESISTANCE 					 0x0A,0x05,0x05
#define HID_USAGE_SENSOR_DATA_ELECTRICAL_VOLTAGE 						 0x0A,0x06,0x05
#define HID_USAGE_SENSOR_DATA_ELECTRICAL_FREQUENCY 						 0x0A,0x07,0x05
#define HID_USAGE_SENSOR_DATA_ELECTRICAL_PERIOD 						 0x0A,0x08,0x05
#define HID_USAGE_SENSOR_DATA_ELECTRICAL_PERCENT_OF_RANGE 				 0x0A,0x09,0x05
	                                                                     
//data type time                                                         
//data field usages (input report)                                       
#define HID_USAGE_SENSOR_DATA_TIME 										 0x0A,0x20,0x05
#define HID_USAGE_SENSOR_DATA_TIME_YEAR 								 0x0A,0x21,0x05
#define HID_USAGE_SENSOR_DATA_TIME_MONTH 								 0x0A,0x22,0x05
#define HID_USAGE_SENSOR_DATA_TIME_DAY 									 0x0A,0x23,0x05
#define HID_USAGE_SENSOR_DATA_TIME_DAY_OF_WEEK 							 0x0A,0x24,0x05
#define HID_USAGE_SENSOR_DATA_TIME_HOUR 								 0x0A,0x25,0x05
#define HID_USAGE_SENSOR_DATA_TIME_MINUTE 								 0x0A,0x26,0x05
#define HID_USAGE_SENSOR_DATA_TIME_SECOND 								 0x0A,0x27,0x05
#define HID_USAGE_SENSOR_DATA_TIME_MILLISECOND 							 0x0A,0x28,0x05
#define HID_USAGE_SENSOR_DATA_TIME_TIMESTAMP 							 0x0A,0x29,0x05
#define HID_USAGE_SENSOR_DATA_TIME_JULIAN_DAY_OF_YEAR 					 0x0A,0x2A,0x05
	                                                                     
//property usages (get/set feature report)                               
#define HID_USAGE_SENSOR_PROPERTY_TIME 									 0x0A,0x30,0x05
#define HID_USAGE_SENSOR_PROPERTY_TIME_TIME_ZONE_OFFSET_FROM_UTC 		 0x0A,0x31,0x05
#define HID_USAGE_SENSOR_PROPERTY_TIME_TIME_ZONE_NAME 					 0x0A,0x32,0x05
#define HID_USAGE_SENSOR_PROPERTY_TIME_DAYLIGHT_SAVINGS_TIME_OBSERVED 	 0x0A,0x33,0x05
#define HID_USAGE_SENSOR_PROPERTY_TIME_TIME_TRIM_ADJUSTMENT 			 0x0A,0x34,0x05
#define HID_USAGE_SENSOR_PROPERTY_TIME_ARM_ALARM 						 0x0A,0x35,0x05

//data type custom 
//data field usages (input report) 
#define HID_USAGE_SENSOR_DATA_CUSTOM 							 0x0A,0x40,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_USAGE 						 0x0A,0x41,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_BOOLEAN_ARRAY 				 0x0A,0x42,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE 						 0x0A,0x43,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_1 					 0x0A,0x44,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_2 					 0x0A,0x45,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_3 					 0x0A,0x46,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_4  					 0x0A,0x47,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_5  					 0x0A,0x48,0x05
#define HID_USAGE_SENSOR_DATA_CUSTOM_VALUE_6  					 0x0A,0x49,0x05
 							
//data type generic
//data field usages (input report)
#define HID_USAGE_SENSOR_DATA_GENERIC							 0x0A,0x60,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_GUID_OR_PROPERTYKEY		 0x0A,0x61,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_CATEGORY_GUID				 0x0A,0x62,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_TYPE_GUID					 0x0A,0x63,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_EVENT_PROPERTYKEY			 0x0A,0x64,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_PROPERTY_PROPERTYKEY		 0x0A,0x65,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_DATAFIELD_PROPERTYKEY		 0x0A,0x66,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_EVENT						 0x0A,0x67,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_PROPERTY					 0x0A,0x68,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_DATAFIELD					 0x0A,0x69,0x05
#define HID_USAGE_SENSOR_DATA_ENUMERATOR_TABLE_ROW_INDEX		 0x0A,0x6A,0x05
#define HID_USAGE_SENSOR_DATA_ENUMERATOR_TABLE_ROW_COUNT		 0x0A,0x6B,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_GUID_OR_PROPERTYKEY_KIND	 0x0A,0x6C,0x05 // NAry
	//begin GorPK kind selectors
	#define HID_USAGE_SENSOR_GORPK_KIND_CATEGORY				 0x0A,0xD0,0x08 // Sel
	#define HID_USAGE_SENSOR_GORPK_KIND_TYPE					 0x0A,0xD1,0x08 // Sel
	#define HID_USAGE_SENSOR_GORPK_KIND_EVENT					 0x0A,0xD2,0x08 // Sel
	#define HID_USAGE_SENSOR_GORPK_KIND_PROPERTY				 0x0A,0xD3,0x08 // Sel
	#define HID_USAGE_SENSOR_GORPK_KIND_DATAFIELD				 0x0A,0xD4,0x08 // Sel
	//end GorPK kind selectors
#define HID_USAGE_SENSOR_DATA_GENERIC_GUID						 0x0A,0x6D,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_PROPERTYKEY				 0x0A,0x6E,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_TOP_LEVEL_COLLECTION_ID	 0x0A,0x6F,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_REPORT_ID					 0x0A,0x70,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_REPORT_ITEM_POSITION_INDEX 0x0A,0x71,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_FIRMWARE_VARTYPE			 0x0A,0x72,0x05 // NAry
	//begin firmware vartype selectors
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_NULL			 0x0A,0x00,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_BOOL			 0x0A,0x01,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_UI1			 0x0A,0x02,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_I1				 0x0A,0x03,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_UI2			 0x0A,0x04,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_I2				 0x0A,0x05,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_UI4			 0x0A,0x06,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_I4				 0x0A,0x07,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_UI8			 0x0A,0x08,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_I8				 0x0A,0x09,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_R4				 0x0A,0x0A,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_R8				 0x0A,0x0B,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_WSTR			 0x0A,0x0C,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_STR			 0x0A,0x0D,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_CLSID			 0x0A,0x0E,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_VECTOR_VT_UI1	 0x0A,0x0F,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16E0			 0x0A,0x10,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16E1			 0x0A,0x11,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16E2			 0x0A,0x12,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16E3			 0x0A,0x13,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16E4			 0x0A,0x14,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16E5			 0x0A,0x15,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16E6			 0x0A,0x16,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16E7			 0x0A,0x17,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16E8			 0x0A,0x18,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16E9			 0x0A,0x19,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16EA			 0x0A,0x1A,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16EB			 0x0A,0x1B,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16EC			 0x0A,0x1C,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16ED			 0x0A,0x1D,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16EE			 0x0A,0x1E,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F16EF			 0x0A,0x1F,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32E0			 0x0A,0x20,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32E1			 0x0A,0x21,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32E2			 0x0A,0x22,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32E3			 0x0A,0x23,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32E4			 0x0A,0x24,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32E5			 0x0A,0x25,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32E6			 0x0A,0x26,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32E7			 0x0A,0x27,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32E8			 0x0A,0x28,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32E9			 0x0A,0x29,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32EA			 0x0A,0x2A,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32EB			 0x0A,0x2B,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32EC			 0x0A,0x2C,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32ED			 0x0A,0x2D,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32EE			 0x0A,0x2E,0x09 // Sel
	#define HID_USAGE_SENSOR_FIRMWARE_VARTYPE_VT_F32EF			 0x0A,0x2F,0x09 // Sel	
	//end firmware vartype selectors
#define HID_USAGE_SENSOR_DATA_GENERIC_UNIT_OF_MEASURE			 0x0A,0x73,0x05 // NAry
	//begin unit of measure selectors
	#define HID_USAGE_SENSOR_GENERIC_UNIT_NOT_SPECIFIED			 0x0A,0x40,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_LUX					 0x0A,0x41,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_DEGREES_KELVIN		 0x0A,0x42,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_DEGREES_CELSIUS		 0x0A,0x43,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_PASCAL				 0x0A,0x44,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_NEWTON				 0x0A,0x45,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_METERS_PER_SECOND		 0x0A,0x46,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_KILOGRAM				 0x0A,0x47,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_METER					 0x0A,0x48,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_METERS_PER_SEC_SQRD	 0x0A,0x49,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_FARAD					 0x0A,0x4A,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_AMPERE				 0x0A,0x4B,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_WATT					 0x0A,0x4C,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_HENRY					 0x0A,0x4D,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_OHM					 0x0A,0x4E,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_VOLT					 0x0A,0x4F,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_HERTZ					 0x0A,0x50,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_BAR					 0x0A,0x51,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_DEGREES_ANTI_CLOCKWISE 0x0A,0x52,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_DEGREES_CLOCKWISE		 0x0A,0x53,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_DEGREES				 0x0A,0x54,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_DEGREES_PER_SECOND	 0x0A,0x55,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_DEGREES_PER_SEC_SQRD	 0x0A,0x56,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_KNOT					 0x0A,0x57,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_PERCENT				 0x0A,0x58,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_SECOND				 0x0A,0x59,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_MILLISECOND			 0x0A,0x5A,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_G						 0x0A,0x5B,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_BYTES					 0x0A,0x5C,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_MILLIGAUSS			 0x0A,0x5D,0x09 // Sel
	#define HID_USAGE_SENSOR_GENERIC_UNIT_BITS					 0x0A,0x5E,0x09 // Sel
	//end unit of measure selectors
#define HID_USAGE_SENSOR_DATA_GENERIC_UNIT_EXPONENT				 0x0A,0x74,0x05 // NAry
	//begin unit exponent selectors
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_0					 0x0A,0x70,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_1					 0x0A,0x71,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_2					 0x0A,0x72,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_3					 0x0A,0x73,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_4					 0x0A,0x74,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_5					 0x0A,0x75,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_6					 0x0A,0x76,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_7					 0x0A,0x77,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_8					 0x0A,0x78,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_9					 0x0A,0x79,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_A					 0x0A,0x7A,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_B					 0x0A,0x7B,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_C					 0x0A,0x7C,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_D					 0x0A,0x7D,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_E					 0x0A,0x7E,0x09 //Sel
	#define HID_USAGE_SENSOR_GENERIC_EXPONENT_F					 0x0A,0x7F,0x09 //Sel
	//end unit exponent selectors
#define HID_USAGE_SENSOR_DATA_GENERIC_REPORT_SIZE				 0x0A,0x75,0x05
#define HID_USAGE_SENSOR_DATA_GENERIC_REPORT_COUNT				 0x0A,0x76,0x05

//property usages (get/set feature report)
#define HID_USAGE_SENSOR_PROPERTY_GENERIC						 0x0A,0x80,0x05
#define HID_USAGE_SENSOR_PROPERTY_ENUMERATOR_TABLE_ROW_INDEX	 0x0A,0x81,0x05
#define HID_USAGE_SENSOR_PROPERTY_ENUMERATOR_TABLE_ROW_COUNT	 0x0A,0x82,0x05

////////////////////////////////////////////////////////////////////////////////////
//
// Other HID definitions
//
////////////////////////////////////////////////////////////////////////////////////

//NOTE: These definitions are designed to permit compiling the HID report descriptors
// with somewhat self-explanatory information to help readability and reduce errors

//input,output,feature flags
//#define HID_DATA_ARR_ABS	 0x00
//#define HID_CONST_ARR_ABS	 0x01
//#define HID_DATA_VAR_ABS	 0x02
//#define HID_CONST_VAR_ABS	 0x03
//#define HID_DATA_VAR_REL	 0x06

#define HID_DATA_BIT      0x00
#define HID_CONST_BIT     0x01
                        
#define HID_ARY_BIT       0x00
#define HID_VAR_BIT       0x02
                        
#define HID_ABS_BIT       0x00
#define HID_REL_BIT       0x04
                        
#define HID_NWRP_BIT      0x00
#define HID_WRP_BIT       0x08
                        
#define HID_LIN_BIT       0x00
#define HID_NLIN_BIT      0x10
                        
#define HID_PREF_BIT      0x00
#define HID_NPREF_BIT     0x20
                        
#define HID_NNUL_BIT      0x00
#define HID_NUL_BIT       0x40
                        
#define HID_NVOL_BIT      0x00
#define HID_VOL_BIT       0x08

//collection flags
#define HID_PHYSICAL	  0x00
#define HID_APPLICATION	  0x01
#define HID_LOGICAL		  0x02
#define HID_NAMED_ARRAY	  0x04
#define HID_USAGE_SWITCH  0x05

// Usage Pages (HID Usage Tables 1.12 Section 3, Table 1)
#define HID_USAGE_PAGE_GENERIC_DESKTOP 0x01
#define HID_USAGE_PAGE_GAME_CONTROLS	 0x05
#define HID_USAGE_PAGE_KEY_CODES       0x07
#define HID_USAGE_PAGE_LEDS            0x08
#define HID_USAGE_PAGE_BUTTONS         0x09
#define HID_USAGE_PAGE_CONSUMER        0x0C
#define HID_USAGE_PAGE_DIGITIZER			 0x0D

// Generic Desktop Page Usages (HID Usage Tables 1.12 Section 4, Table 6)
#define HID_GEN_DESKTOP_USAGE_POINTER  0x01
#define HID_GEN_DESKTOP_USAGE_MOUSE    0x02
#define HID_GEN_DESKTOP_USAGE_JOYSTICK 0x04
#define HID_GEN_DESKTOP_USAGE_GAMEPAD  0x05
#define HID_GEN_DESKTOP_USAGE_KEYBOARD 0x06
#define HID_GEN_DESKTOP_USAGE_X        0x30
#define HID_GEN_DESKTOP_USAGE_Y        0x31
#define HID_GEN_DESKTOP_USAGE_Z        0x32
#define HID_GEN_DESKTOP_USAGE_RX       0x33
#define HID_GEN_DESKTOP_USAGE_RY       0x34
#define HID_GEN_DESKTOP_USAGE_RZ       0x35
#define HID_GEN_DESKTOP_USAGE_WHEEL    0x38

// DIGITIZER Page Usages (HID Usage Tables 1.12 Section 4, Table 6)
#define HID_GEN_DIGITIZER_USAGE_DIGITIZER  			0x01
#define HID_GEN_DIGITIZER_USAGE_PEN  				0x02
#define HID_GEN_DIGITIZER_USAGE_LIGHT_PEN  			0x03
#define HID_GEN_DIGITIZER_USAGE_STYLUS  			0x20
#define HID_GEN_DIGITIZER_USAGE_IN_RANGE			0x32
#define HID_GEN_DIGITIZER_USAGE_TIP_SWITCH			0x42

// Consumer Usages (HID Usage Tables 1.12 Section 4, Table 6)
#define HID_CONSUMER_USAGE_CONSUMER_CONTROL         0x01
#define HID_CONSUMER_USAGE_NUMERIC_KEY_PAD          0x02
#define HID_CONSUMER_USAGE_PROGRAMMABLE_BUTTONS     0x03

//other
#define HID_UNDEFINED					0x00
#define HID_USAGE_PAGE(a)				0x05,a
#define HID_USAGE_PAGE_VENDOR_DEFINED	0x06,0x00,0xFF
#define HID_USAGE(a)					0x09,a
#define HID_USAGE16(a,b)				0x0A,a,b
#define HID_USAGE_SENSOR_DATA(a,b)		a|b			//This or-s the mod into usage
#define HID_COLLECTION(a)				0xA1,a
#define HID_REPORT_ID(a)				0x85,a
#define HID_REPORT_SIZE(a)				0x75,a
#define HID_REPORT_COUNT(a)				0x95,a
#define HID_USAGE_MIN_8(a)				0x19,a
#define HID_USAGE_MIN_16(a,b)			0x1A,a,b
#define HID_USAGE_MAX_8(a)				0x29,a
#define HID_USAGE_MAX_16(a,b)			0x2A,a,b
#define HID_LOGICAL_MIN_8(a)			0x15,a
#define HID_LOGICAL_MIN_16(a,b)			0x16,a,b
#define HID_LOGICAL_MIN_32(a,b,c,d)		0x17,a,b,cd
#define HID_LOGICAL_MAX_8(a)			0x25,a
#define HID_LOGICAL_MAX_16(a,b)			0x26,a,b
#define HID_LOGICAL_MAX_32(a,b,c,d)		0x27,a,b,c,d
#define HID_PHYSICAL_MIN_8(a)			0x35,a
#define HID_PHYSICAL_MIN_16(a,b)		0x36,a,b
#define HID_PHYSICAL_MIN_32(a,b,c,d)	0x37,a,b,c,d
#define HID_PHYSICAL_MAX_8(a)			0x45,a
#define HID_PHYSICAL_MAX_16(a,b)		0x46,a,b
#define HID_PHYSICAL_MAX_32(a,b,c,d)	0x47,a,b,c,d
#define HID_UNIT_EXPONENT(a)			0x55,a
#define HID_UNIT(a)						0x65,a
#define HID_INPUT(a)					0x81,a
#define HID_OUTPUT(a)					0x91,a
#define HID_FEATURE(a)					0xB1,a
#define HID_END_COLLECTION				0xC0

#endif // _APP_HOGPD_DEFS_H_
