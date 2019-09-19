/**
 ****************************************************************************************
 *
 * @file app_user_config.h
 *
 * @brief Compile configuration file.
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

#ifndef _APP_USER_CONFIG_H_
#define _APP_USER_CONFIG_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup
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
#include "co_bt.h"
#include "gap.h"
#include "gapm.h"
#include "gapm_task.h"
#include "gapc_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define MS_TO_BLESLOTS(x)                   ((int) (((x) * 1000) / 625))
#define MS_TO_DOUBLESLOTS(x)                ((int) (((x) * 1000) / 1250))
#define MS_TO_TIMERUNITS(x)                 ((int) ((x) / 10))

#define US_TO_BLESLOTS(x)                   ((int) ((x) / 625))
#define US_TO_DOUBLESLOTS(x)                ((int) ((x) / 1250))
#define US_TO_TIMERUNITS(x)                 ((int) ((x) / 10000))

// Macro to calculate the addr_type value used when setting the device configuration
#define APP_CFG_ADDR_TYPE(x)                ((x & 0x70) >> 4)

// Macro to calculate the addr_src value used in air operation commands (advertising, scanning, etc.)
#define APP_CFG_ADDR_SRC(x)                 ((x & 0x03) >> 0)

/*
 ****************************************************************************************
 * Privacy / Addressing Supported Modes
 * Privacy Capabilities: Disabled, Host, Controller + Host.
 * Resolution Capabilities: Host, Controller + Host.
 * Address type: Public, Random Static, Resolvable Private Address (RPA), non-RPA (NRPA).
 * Note: Identity Address is always Public, when privacy feature is enabled.
 ****************************************************************************************
 */
/// Device does not claim privacy support.
/// Operates with Public BD Address.
/// It can resolve peer's RPA in Host.
#define APP_CFG_ADDR_PUB                    (0x00)

/// Device does not claim privacy support.
/// Operates with Random Static BD Address.
/// It can resolve peer's RPA in Host.
#define APP_CFG_ADDR_STATIC                 (0x10)

/// Device claims Host privacy support.
/// Operates with RPA.
/// Identity Address is Public.
/// It can resolve peer's RPA in Host.
#define APP_CFG_HOST_PRIV_RPA               (0x21)

/// Device claims Host privacy support.
/// Operates with NRPA.
/// Note: Can be used only in non-connectable modes/procedures.
#define APP_CFG_HOST_PRIV_NRPA              (0x22)

/// Device claims Controller + Host privacy support.
/// Operates with RPA.
/// If there is no matching entry in RAL, operates
/// with Public Address.
/// Identity Address is Public.
/// It can resolve peer's RPA in Controller + Host.
#define APP_CFG_CNTL_PRIV_RPA_PUB           (0x41)

/// Device claims Controller + Host privacy support.
/// Operates with RPA.
/// If there is no matching entry in RAL, operates with RPA.
/// Identity Address is Public.
/// It can resolve peer's RPA in Controller + Host.
/// Note1: This configuration, adds a dummy entry to RAL and uses
/// its address in the air operation command to be executed, to find a
/// matching entry, and therefore, generate an RPA in the contoller.
/// Note2: Air operation commands shall use as peer address the address
/// of the dummy entry, in order to find the matching entry and generate the RPA.
#define APP_CFG_CNTL_PRIV_RPA_RAND          (0xC1)

/*
 ****************************************************************************************
 * Controller Privacy Modes
 * By default, network privacy mode is used when private addresses are resolved
 * and generated by the Controller.
 * This is a global flag for all devices in RAL. However, privacy mode can be
 * selected for each device, explicitly. The Controller is instructed for each
 * peer in the resolving list.
 ****************************************************************************************
 */
/// Local device will only accept advertising packets from bonded peer
/// devices that contain private addresses.
#define APP_CFG_CNTL_PRIV_MODE_NETWORK      0

/// Local device will accept advertising packets from bonded peer devices
/// that contain either a private or a non-private address.
#define APP_CFG_CNTL_PRIV_MODE_DEVICE       1


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Device name write permissions requirements
enum device_name_write_perm
{
    NAME_WRITE_DISABLE  = (0x00 << GAPM_POS_ATT_NAME_PERM),
    NAME_WRITE_ENABLE   = (0x01 << GAPM_POS_ATT_NAME_PERM),
    NAME_WRITE_UNAUTH   = (0x02 << GAPM_POS_ATT_NAME_PERM),
    NAME_WRITE_AUTH     = (0x03 << GAPM_POS_ATT_NAME_PERM),
    /// NAME_WRITE_SECURE is not supported by SDK
};

/// Device appearance write permissions requirements
enum device_appearance_write_perm
{
    APPEARANCE_WRITE_DISABLE  = (0x00 << GAPM_POS_ATT_APPEARENCE_PERM),
    APPEARANCE_WRITE_ENABLE   = (0x01 << GAPM_POS_ATT_APPEARENCE_PERM),
    APPEARANCE_WRITE_UNAUTH   = (0x02 << GAPM_POS_ATT_APPEARENCE_PERM),
    APPEARANCE_WRITE_AUTH     = (0x03 << GAPM_POS_ATT_APPEARENCE_PERM),
    /// APPEARANCE_WRITE_SECURE is not supported by SDK
};

/*
 ****************************************************************************************
 *
 * Advertising related configuration
 *
 ****************************************************************************************
 */
struct advertise_configuration {
    /**
     * Own BD address source of the device:
     * - GAPM_STATIC_ADDR: Public or Random Static Address according to device address configuration
     * - GAPM_GEN_RSLV_ADDR: Generated Resolvable Private Address
     * - GAPM_GEN_NON_RSLV_ADDR: Generated non-Resolvable Private Address
     */
    uint8_t addr_src;

    /// Minimum interval for advertising
    uint16_t intv_min;

    /// Maximum interval for advertising
    uint16_t intv_max;

    /// Advertising channel map (@see adv_channel_map)
    uint8_t channel_map;

    /*************************
     * Advertising information
     *************************
     */

    /// Host information advertising data (GAPM_ADV_NON_CONN and GAPM_ADV_UNDIRECT)
    /// Advertising mode :
    /// - GAP_NON_DISCOVERABLE: Non discoverable mode
    /// - GAP_GEN_DISCOVERABLE: General discoverable mode
    /// - GAP_LIM_DISCOVERABLE: Limited discoverable mode
    /// - GAP_BROADCASTER_MODE: Broadcaster mode
    uint8_t mode;

    /// Host information advertising data (GAPM_ADV_NON_CONN and GAPM_ADV_UNDIRECT)
    /// - ADV_ALLOW_SCAN_ANY_CON_ANY: Allow both scan and connection requests from anyone
    /// - ADV_ALLOW_SCAN_WLST_CON_ANY: Allow both scan req from White List devices only and
    ///   connection req from anyone. Supported only in broadcaster role.
    /// - ADV_ALLOW_SCAN_ANY_CON_WLST: Allow both scan req from anyone and connection req
    ///   from White List devices only. Supported only in broadcaster role.
    /// - ADV_ALLOW_SCAN_WLST_CON_WLST: Allow scan and connection requests from White List
    ///   devices only. Supported only in broadcaster role.
    uint8_t adv_filt_policy;

    /// Address of peer device
    uint8_t peer_addr[BD_ADDR_LEN];

    /// Address type of peer device (0=public/1=random)
    uint8_t peer_addr_type;
};

/*
 ****************************************************************************************
 *
 * Parameter update configuration
 *
 ****************************************************************************************
 */
struct connection_param_configuration {
    /// Connection interval minimum
    uint16_t intv_min;

    /// Connection interval maximum
    uint16_t intv_max;

    /// Latency
    uint16_t latency;

    /// Supervision timeout
    uint16_t time_out;

    /// Minimum Connection Event Duration
    uint16_t ce_len_min;

    /// Maximum Connection Event Duration
    uint16_t ce_len_max;
};

/*
 ****************************************************************************************
 *
 * GAPM configuration
 *
 ****************************************************************************************
 */
struct gapm_configuration {
    /// Device Role: Central, Peripheral, Observer, Broadcaster or All roles. (@see enum gap_role)
    enum gap_role role;

    /// Maximal MTU
    uint16_t max_mtu;

    /// Device Address Type
    /// - GAPM_CFG_ADDR_PUBLIC: Device Address is a Public address
    /// - GAPM_CFG_ADDR_PRIVATE: Device Address is a Random Static address
    /// - GAPM_CFG_ADDR_PRIVACY: Device Address generated using Privacy feature
    uint8_t addr_type;

    /***********************
     * Privacy configuration
     ***********************
     */

    /// Random Static address defined by user
    uint8_t addr[BD_ADDR_LEN];

    /// Duration before regenerate device address when privacy is enabled.
    uint16_t renew_dur;

    /// Device IRK used for Resolvable Private Address generation (LSB first)
    uint8_t irk[KEY_LEN];

    /****************************
     * ATT database configuration
     ****************************
     */

    /// Attribute database configuration (@see enum gapm_att_cfg_flag)
    ///    7     6    5     4     3    2    1    0
    /// +-----+-----+----+-----+-----+----+----+----+
    /// | DBG | RFU | SC | PCP | APP_PERM |NAME_PERM|
    /// +-----+-----+----+-----+-----+----+----+----+
    /// - Bit [0-1]: Device Name write permission requirements for peer device (@see gapm_write_att_perm)
    /// - Bit [2-3]: Device Appearance write permission requirements for peer device (@see gapm_write_att_perm)
    /// - Bit [4]  : Slave Preferred Connection Parameters present
    /// - Bit [5]  : Service change feature present in GATT attribute database.
    /// - Bit [6]  : Reserved
    /// - Bit [7]  : Enable Debug Mode
    uint8_t  att_cfg;

    /// GAP service start handle
    uint16_t gap_start_hdl;

    /// GATT service start handle
    uint16_t gatt_start_hdl;

    /**************************************************
     * Data packet length extension configuration (4.2)
     **************************************************
     */

    /// Maximal MPS
    uint16_t max_mps;

    /// Maximal Tx octets (connInitialMaxTxOctets value, as defined in 4.2 Specification)
    uint16_t max_txoctets;

    /// Maximal Tx time (connInitialMaxTxTime value, as defined in 4.2 Specification)
    uint16_t max_txtime;
};

/*
 ****************************************************************************************
 *
 * Central configuration
 *
 ****************************************************************************************
 */
struct central_configuration {
    /// GAPM requested operation:
    /// - GAPM_CONNECTION_DIRECT: Direct connection operation
    /// - GAPM_CONNECTION_AUTO: Automatic connection operation
    /// - GAPM_CONNECTION_SELECTIVE: Selective connection operation
    /// - GAPM_CONNECTION_NAME_REQUEST: Name Request operation (requires to start a direct
    ///   connection)
    uint8_t code;

    /// Own BD address source of the device:
    ///  - GAPM_STATIC_ADDR: Public or Random Static Address according to device address configuration
    ///  - GAPM_GEN_RSLV_ADDR: Generated Resolvable Private Address
    ///  - GAPM_GEN_NON_RSLV_ADDR: Generated non-Resolvable Private Address
    uint8_t addr_src;

    /// Scan interval
    uint16_t scan_interval;

    /// Scan window size
    uint16_t scan_window;

    /// Minimum of connection interval
    uint16_t con_intv_min;

    /// Maximum of connection interval
    uint16_t con_intv_max;

    /// Connection latency
    uint16_t con_latency;

    /// Link supervision timeout
    uint16_t superv_to;

     /// Minimum CE length
    uint16_t ce_len_min;

    /// Maximum CE length
    uint16_t ce_len_max;

    /**************************************************************************************
     * Peer device information (maximum number of peers = 8)
     **************************************************************************************
     */

    /// BD Address of device
    uint8_t peer_addr_0[BD_ADDR_LEN];

    /// Address type of the device 0=public/1=random
    uint8_t peer_addr_0_type;

    /// BD Address of device
    uint8_t peer_addr_1[BD_ADDR_LEN];

    /// Address type of the device 0=public/1=random
    uint8_t peer_addr_1_type;

    /// BD Address of device
    uint8_t peer_addr_2[BD_ADDR_LEN];

    /// Address type of the device 0=public/1=random
    uint8_t peer_addr_2_type;

    /// BD Address of device
    uint8_t peer_addr_3[BD_ADDR_LEN];

    /// Address type of the device 0=public/1=random
    uint8_t peer_addr_3_type;

    /// BD Address of device
    uint8_t peer_addr_4[BD_ADDR_LEN];

    /// Address type of the device 0=public/1=random
    uint8_t peer_addr_4_type;

    /// BD Address of device
    uint8_t peer_addr_5[BD_ADDR_LEN];

    /// Address type of the device 0=public/1=random
    uint8_t peer_addr_5_type;

    /// BD Address of device
    uint8_t peer_addr_6[BD_ADDR_LEN];

    /// Address type of the device 0=public/1=random
    uint8_t peer_addr_6_type;

    /// BD Address of device
    uint8_t peer_addr_7[BD_ADDR_LEN];

    /// Address type of the device 0=public/1=random
    uint8_t peer_addr_7_type;
};

/*
 ****************************************************************************************
 *
 * Security related configuration
 *
 ****************************************************************************************
 */
struct security_configuration
{
    /**************************************************************************************
     * IO capabilities (@see gap_io_cap)
     *
     * - GAP_IO_CAP_DISPLAY_ONLY          Display Only
     * - GAP_IO_CAP_DISPLAY_YES_NO        Display Yes No
     * - GAP_IO_CAP_KB_ONLY               Keyboard Only
     * - GAP_IO_CAP_NO_INPUT_NO_OUTPUT    No Input No Output
     * - GAP_IO_CAP_KB_DISPLAY            Keyboard Display
     *
     **************************************************************************************
     */
    enum gap_io_cap iocap : 8;

    /**************************************************************************************
     * OOB information (@see gap_oob)
     *
     * - GAP_OOB_AUTH_DATA_NOT_PRESENT    OOB Data not present
     * - GAP_OOB_AUTH_DATA_PRESENT        OOB data present
     *
     **************************************************************************************
     */
    enum gap_oob oob : 8;

    /**************************************************************************************
     * Authentication (@see gap_auth / gap_auth_mask)
     *
     * - GAP_AUTH_NONE      None
     * - GAP_AUTH_BOND      Bond
     * - GAP_AUTH_MITM      MITM
     * - GAP_AUTH_SEC       Secure Connection
     * - GAP_AUTH_KEY       Keypress Notification
     *
     **************************************************************************************
     */
    uint8_t auth;

    /**************************************************************************************
     * Encryption key size (7 to 16) - LTK Key Size
     *
     **************************************************************************************
     */
    uint8_t key_size;

    /**************************************************************************************
     * Initiator key distribution (@see gap_kdist)
     *
     * - GAP_KDIST_NONE                   No Keys to distribute
     * - GAP_KDIST_ENCKEY                 LTK (Encryption key) in distribution
     * - GAP_KDIST_IDKEY                  IRK (ID key)in distribution
     * - GAP_KDIST_SIGNKEY                CSRK (Signature key) in distribution
     * - Any combination of the above
     *
     **************************************************************************************
     */
    uint8_t ikey_dist;

    /**************************************************************************************
     * Responder key distribution (@see gap_kdist)
     *
     * - GAP_KDIST_NONE                   No Keys to distribute
     * - GAP_KDIST_ENCKEY                 LTK (Encryption key) in distribution
     * - GAP_KDIST_IDKEY                  IRK (ID key)in distribution
     * - GAP_KDIST_SIGNKEY                CSRK (Signature key) in distribution
     * - Any combination of the above
     *
     **************************************************************************************
     */
    uint8_t rkey_dist;

    /**************************************************************************************
     * Device security requirements (minimum security level). (@see gap_sec_req)
     *
     * - GAP_NO_SEC                 No security (no authentication and encryption)
     * - GAP_SEC1_NOAUTH_PAIR_ENC   Unauthenticated pairing with encryption
     * - GAP_SEC1_AUTH_PAIR_ENC     Authenticated pairing with encryption
     * - GAP_SEC1_SEC_PAIR_ENC      Authenticated LE Secure Connections pairing with encryption
     * - GAP_SEC2_NOAUTH_DATA_SGN   Unauthenticated pairing with data signing
     * - GAP_SEC2_AUTH_DATA_SGN     Authentication pairing with data signing
     *
     **************************************************************************************
     */
    enum gap_sec_req sec_req : 8;
};

/// @} APP

#endif // _APP_USER_CONFIG_H_
