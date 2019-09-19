/**
 ****************************************************************************************
 *
 * @file app_adv_data.h
 *
 * @brief Advertising Data definitions.
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
 
#ifndef _APP_ADV_DATA_H_
#define _APP_ADV_DATA_H_

#include <stdio.h>

/****************************************************************************************
 * Advertising Data types.
 * See https://www.bluetooth.org/en-us/specification/assigned-numbers/generic-access-profile
 ****************************************************************************************/
#define ADV_TYPE_FLAGS                                      "\x01" /* Flags */
#define ADV_TYPE_INCOMPLETE_LIST_16BIT_SERVICE_IDS          "\x02" /* Incomplete list of 16-bit Service IDs */
#define ADV_TYPE_COMPLETE_LIST_16BIT_SERVICE_IDS            "\x03" /* Complete list of 16-bit Service IDs */
#define ADV_TYPE_INCOMPLETE_LIST_32BIT_SERVICE_IDS          "\x04" /* Incomplete list of 32-bit Service IDs (not relevant for Bluetooth 4.0) */
#define ADV_TYPE_COMPLETE_LIST_32BIT_SERVICE_IDS            "\x05" /* Complete list of 32-bit Service IDs (not relevant for Bluetooth 4.0) */
#define ADV_TYPE_INCOMPLETE_LIST_128BIT_SERVICE_IDS         "\x06" /* Incomplete list of 128-bit Service IDs */
#define ADV_TYPE_COMPLETE_LIST_128BIT_SERVICE_IDS           "\x07" /* Complete list of 128-bit Service IDs */
#define ADV_TYPE_SHORTENED_LOCAL_NAME                       "\x08" /* Shortened Local Name */
#define ADV_TYPE_COMPLETE_LOCAL_NAME                        "\x09" /* Complete Local Name */
#define ADV_TYPE_TX_POWER_LEVEL                             "\x0A" /* TX Power Level (in dBm) */
#define ADV_TYPE_CLASS_OF_DEVICE                            "\x0D" /* Class of Device */
#define ADV_TYPE_SIMPLE_PAIRING_HASH_C                      "\x0E" /* Simple Pairing Hash C */
#define ADV_TYPE_SIMPLE_PAIRING_RANDOM_R                    "\x0F" /* Simple Pairing Randomizer R */
#define ADV_TYPE_DEVICE_ID                                  "\x10" /* Device ID */
#define ADV_TYPE_SECURITY_MAN_OUT_OF_BAND_FLAGS             "\x11" /* Security Manager Out of Band Flags */
#define ADV_TYPE_SLAVE_CONNECTION_INTERVAL_RANGE            "\x12" /* Slave Connection Interval Range */
#define ADV_TYPE_LIST_16BIT_SOLICITATION_UUIDS              "\x14" /* List of 16-bit Service Solicitation UUIDs */
#define ADV_TYPE_LIST_32BIT_SOLICITATION_UUIDS              "\x1F" /* List of 32-bit Service Solicitation UUIDs */
#define ADV_TYPE_LIST_128BIT_SOLICITATION_UUIDS             "\x15" /* List of 128-bit Service Solicitation UUIDs */
#define ADV_TYPE_SERVICE_DATA_16BIT_UUID                    "\x16" /* Service Data - 16-bit UUID */
#define ADV_TYPE_SERVICE_DATA_32BIT_UUID                    "\x20" /* Service Data - 32-bit UUID */
#define ADV_TYPE_SERVICE_DATA_128BIT_UUID                   "\x21" /* Service Data - 128-bit UUID */
#define ADV_TYPE_LE_SECURE_CON_CONFIRM_VAL                  "\x22" /* LE Secure Connections Confirmation Value */
#define ADV_TYPE_LE_SECURE_CON_RANDOM_VAL                   "\x23" /* LE Secure Connections Random Value */
#define ADV_TYPE_PUBLIC_TARGET_ADDRESS                      "\x17" /* Public Target Address */
#define ADV_TYPE_RANDOM_TARGET_ADDRESS                      "\x18" /* Random Target Address */
#define ADV_TYPE_APPEARANCE                                 "\x19" /* Appearance */
#define ADV_TYPE_ADVERTISING_INTERVAL                       "\x1A" /* Advertising Interval */
#define ADV_TYPE_LE_BLUETOOTH_DEVICE_ADDRESS                "\x1B" /* LE Bluetooth Device Address */
#define ADV_TYPE_LE_ROLE                                    "\x1C" /* LE Role */
#define ADV_TYPE_SIMPLE_PAIRING_HASH_C256                   "\x1D" /* Simple Pairing Hash C-256 */
#define ADV_TYPE_SIMPLE_PAIRING_RANDOM_C256                 "\x1E" /* Simple Pairing Randomizer R-256 */
#define ADV_TYPE_URI                                        "\x24" /* Uniform Resource Identifier */
#define ADV_TYPE_INFO_DATA_3D                               "\x3D" /* 3D Information Data */
#define ADV_TYPE_MANUFACTURER_SPECIFIC_DATA                 "\xFF" /* Manufacturer Specific Data */

/****************************************************************************************
 * AD Type Flag - Bit set.
 ****************************************************************************************/
#define ADV_FLAG_LE_LIMITED_DISCOVERABLE        "\x01" /* LE Limited Discoverable Mode */
#define ADV_FLAG_LE_GENERAL_DISCOVERABLE        "\x02" /* LE General Discoverable Mode */
#define ADV_FLAG_BREDR_NOT_SUPPORTED            "\x04" /* BR/EDR Not Supported */
#define ADV_FLAG_SIMULTANEOUS_LE_BREDR_C        "\x08" /* Simultaneous LE and BR/EDR to Same Device Capable (Controller) */
#define ADV_FLAG_SIMULTANEOUS_LE_BREDR_H        "\x10" /* Simultaneous LE and BR/EDR to Same Device Capable (Host) */

/****************************************************************************************
 * 16bit Service UUIDs in air format.
 * See https://developer.bluetooth.org/gatt/services/Pages/ServicesHome.aspx
 ****************************************************************************************/
#define ADV_UUID_ALERT_NOTIFICATION_SERVICE         "\x11\x18" /* Alert Notification Service */
#define ADV_UUID_BATTERY_SERVICE                    "\x0F\x18" /* Battery Service */
#define ADV_UUID_BLOOD_PRESSURE_SERVICE             "\x10\x18" /* Blood Pressure */
#define ADV_UUID_CURRENT_TIME_SERVICE               "\x05\x18" /* Current Time Service */
#define ADV_UUID_CYCLING_SPEED_AND_CADENCE          "\x16\x18" /* Cycling Speed and Cadence */
#define ADV_UUID_DEVICE_INFORMATION_SERVICE         "\x0A\x18" /* Device Information Service */
#define ADV_UUID_GLUCOSE_SERVICE                    "\x08\x18" /* Glucose */
#define ADV_UUID_HEALTH_THERMOMETER_SERVICE         "\x09\x18" /* Health Thermometer */
#define ADV_UUID_HEART_RATE_SERVICE                 "\x0D\x18" /* Heart Rate */
#define ADV_UUID_HUMAN_INTERFACE_DEVICE_SERVICE     "\x12\x18" /* Human Interface Device */
#define ADV_UUID_IMMEDIATE_ALERT_SERVICE            "\x02\x18" /* Immediate Alert */
#define ADV_UUID_LINK_LOSS_SERVICE                  "\x03\x18" /* Link Loss */
#define ADV_UUID_NEXT_DST_CHANGE_SERVICE            "\x07\x18" /* Next DST Change Service */
#define ADV_UUID_PHONE_ALERT_STATUS_SERVICE         "\x0E\x18" /* Phone Alert Status Service */
#define ADV_UUID_REFERENCE_TIME_UPDATE_SERVICE      "\x06\x18" /* Reference Time Update Service */
#define ADV_UUID_RUNNING_SPEED_AND_CADENCE          "\x14\x18" /* Running Speed and Cadence */
#define ADV_UUID_SCAN_PARAMETERS_SERVICE            "\x13\x18" /* Scan Parameters */
#define ADV_UUID_TX_POWER_SERVICE                   "\x04\x18" /* Tx Power */
#define ADV_UUID_GENERIC_ACCESS_SERVICE             "\x00\x18" /* Generic Access */
#define ADV_UUID_GENERIC_ATTRIBUTE_SERVICE          "\x01\x18" /* Generic Attribute */
#define ADV_UUID_SCAN_PARAMETERS_SERVICE            "\x13\x18" /* Scan Parameters */
#define ADV_UUID_AUTOMATION_IO_SERVICE              "\x15\x18" /* Automation IO */
#define ADV_UUID_CYCLING_POWER_SERVICE              "\x18\x18" /* Cycling Power */
#define ADV_UUID_LOCATION_NAVIGATION_SERVICE        "\x19\x18" /* Location and Navigation */
#define ADV_UUID_ENVIROMENT_SENSE_SERVICE           "\x1A\x18" /* Environmental Sensing */
#define ADV_UUID_BODY_COMPOSITION_SERVICE           "\x1B\x18" /* Body Composition */
#define ADV_UUID_USER_DATA_SERVICE                  "\x1C\x18" /* User Data */
#define ADV_UUID_WEIGHT_SCALE_SERVICE               "\x1D\x18" /* Weight Scale */
#define ADV_UUID_BOND_MANAGMENT_SERVICE             "\x1E\x18" /* Bond Management */
#define ADV_UUID_GLUCOSE_CONTINUOUS_SERVICE         "\x1F\x18" /* Continuous Glucose Monitoring */
#define ADV_UUID_INTERNET_PROTOCOL_SERVICE          "\x20\x18" /* Internet Protocol Support */
#define ADV_UUID_INDOOR_POSITION_SERVICE            "\x21\x18" /* Indoor Positioning */
#define ADV_UUID_PULSE_OXIMETER_SERVICE             "\x22\x18" /* Pulse Oximeter */

/// Proprietary
#define ADV_UUID_SUOTAR_SERVICE                     "\xF5\xFE" /* SUOTAR Service */

/****************************************************************************************
 * Values for the Appearance AD type in air format.
 * See https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml
 ****************************************************************************************/
#define ADV_APPEARANCE_UNKNOWN                                        "\x00\x00"    /* Unknown of unspecified appearance type */
#define ADV_APPEARANCE_GENERIC_PHONE                                  "\x40\x00"    /* Generic Phone */
#define ADV_APPEARANCE_GENERIC_COMPUTER                               "\x80\x00"    /* Generic Computer */
#define ADV_APPEARANCE_GENERIC_WATCH                                  "\xC0\x00"    /* Generic Watch */
#define ADV_APPEARANCE_WATCH_SPORTS_WATCH                             "\xC1\x00"    /* Sports Watch */
#define ADV_APPEARANCE_GENERIC_CLOCK                                  "\x00\x01"    /* Generic Clock */
#define ADV_APPEARANCE_GENERIC_DISPLAY                                "\x40\x01"    /* Generic Display */
#define ADV_APPEARANCE_GENERIC_REMOTE_CONTROL                         "\x80\x01"    /* Generic Remote Control */
#define ADV_APPEARANCE_GENERIC_EYE_GLASSES                            "\x0C\x01"    /* Generic Eye Glasses */
#define ADV_APPEARANCE_GENERIC_TAG                                    "\x00\x02"    /* Generic Tag */
#define ADV_APPEARANCE_GENERIC_KEYRING                                "\x40\x02"    /* Generic Keyring */
#define ADV_APPEARANCE_GENERIC_MEDIA_PLAYER                           "\x80\x02"    /* Generic Media Player */
#define ADV_APPEARANCE_GENERIC_BARCODE_SCANNER                        "\xC0\x02"    /* Generic Barcode Scanner */
#define ADV_APPEARANCE_GENERIC_THERMOMETER                            "\x00\x03"    /* Generic Thermometer */
#define ADV_APPEARANCE_THERMOMETER_EAR                                "\x01\x03"    /* Ear Thermometer */
#define ADV_APPEARANCE_GENERIC_HEART_RATE_SENSOR                      "\x40\x03"    /* Generic Heart Rate Sensor */
#define ADV_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT              "\x41\x03"    /* Belt Heart Rate Sensor */
#define ADV_APPEARANCE_GENERIC_BLOOD_PRESSURE                         "\x80\x03"    /* Generic Blood Pressure */
#define ADV_APPEARANCE_BLOOD_PRESSURE_ARM                             "\x81\x03"    /* Arm Blood Pressure */
#define ADV_APPEARANCE_BLOOD_PRESSURE_WRIST                           "\x82\x03"    /* Wrist Blood Pressure */
#define ADV_APPEARANCE_HUMAN_INTERFACE_DEVICE_HID                     "\xC0\x03"    /* Human Interface Device (HID) */
#define ADV_APPEARANCE_KEYBOARD                                       "\xC1\x03"    /* Keyboard */
#define ADV_APPEARANCE_MOUSE                                          "\xC2\x03"    /* Mouse */
#define ADV_APPEARANCE_JOYSTICK                                       "\xC3\x03"    /* Joystick */
#define ADV_APPEARANCE_GAMEPAD                                        "\xC4\x03"    /* Gamepad */
#define ADV_APPEARANCE_DIGITIZER_TABLET                               "\xC5\x03"    /* Digitizer Tablet */
#define ADV_APPEARANCE_CARD_READER                                    "\xC6\x03"    /* Card Read */
#define ADV_APPEARANCE_DIGITAL_PEN                                    "\xC7\x03"    /* Digital Pen */
#define ADV_APPEARANCE_BARCODE_SCANNER                                "\xC8\x03"    /* Barcode Scanner */
#define ADV_APPEARANCE_GENERIC_GLUCOSE_METER                          "\x00\x04"    /* Generic Glucose Meter */
#define ADV_APPEARANCE_GENERIC_RUNNING_WALKING_SENSOR                 "\x40\x04"    /* Generic Running/Walking Sensor */
#define ADV_APPEARANCE_RUNNING_WALKING_SENSOR_IN_SHOE                 "\x41\x04"    /* In Shoe Running/Walking Sensor */
#define ADV_APPEARANCE_RUNNING_WALKING_SENSOR_ON_SHOE                 "\x42\x04"    /* On Shoe Running/Walking Sensor */
#define ADV_APPEARANCE_RUNNING_WALKING_SENSOR_ON_HIP                  "\x43\x04"    /* On Hip Running/Walking Sensor */
#define ADV_APPEARANCE_GENERIC_CYCLING                                "\x80\x04"    /* Generic Cycling */
#define ADV_APPEARANCE_CYCLING_CYCLING_COMPUTER                       "\x81\x04"    /* Cycling Computer */
#define ADV_APPEARANCE_CYCLING_SPEED_SENSOR                           "\x82\x04"    /* Cycling Speed Senspr */
#define ADV_APPEARANCE_CYCLING_CADENCE_SENSOR                         "\x83\x04"    /* Cycling Cadence Sensor */
#define ADV_APPEARANCE_CYCLING_POWER_SENSOR                           "\x84\x04"    /* Cycling Power Sensor */
#define ADV_APPEARANCE_CYCLING_SPEED_AND_CADENCE_SENSOR               "\x85\x04"    /* Cycling Speed and Cadence Sensor */
#define ADV_APPEARANCE_PULSE_OXIMETER_GENERIC                         "\x40\x0C"    /* Generic Pulse Oximeter */
#define ADV_APPEARANCE_PULSE_OXIMETER_FINGERTIP                       "\x41\x0C"    /* Fingertip Pulse Oximeter */
#define ADV_APPEARANCE_PULSE_OXIMETER_WRIST_WORN                      "\x42\x0C"    /* Wrist Worn Pulse Oximeter */
#define ADV_APPEARANCE_OUTDOOR_GENERIC                                "\x40\x14"    /* Generic Outdoor */
#define ADV_APPEARANCE_OUTDOOR_LOCATION_DISPLAY_DEVICE                "\x41\x14"    /* Outdoor Location Display Device */
#define ADV_APPEARANCE_OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE "\x42\x14"    /* Outdoor Location and Navigation Display Device */
#define ADV_APPEARANCE_OUTDOOR_LOCATION_POD                           "\x43\x14"    /* Outdoor Location Pod */
#define ADV_APPEARANCE_OUTDOOR_LOCATION_AND_NAVIGATION_POD            "\x44\x14"    /* Outdoor Location and Navigation Pod */

#define ADV_DIALOG_MANUFACTURER_CODE                                  "\x00\xD2"

#endif  // _APP_ADV_DATA_H_
