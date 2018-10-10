# BLE Central Security Demo Project

## Example description
A DA14585/6 Central side project with basic scan/connect and security feature. The main purpose is to demonstrate the basic pairing, encryption and bonding process on central side.
It will actively pair up with peripheral devices and store bonding informations on connection or passively response to security request from peripheral. None SDK related functions on both central and peripheral sides can be printed to track the process of pairing/bonding process.

# Features
1. Interactive scanner through the UART serial port terminal
2. Pairing process actively started by central side on connection or passively triggered by security request from periphral side
3. Attempt to directly encrypt the connection instead of pairing if the bonding info is already existed
4. Attempt to relaunch pairing process if peripheral side missing previousely bonded key **(SDK code modification required for this feature)**.


## HW and SW configuration
* **Hardware configuration**
	- DA14585 Basic/Pro dev kit * 2.

	* Peripheral: 
		- Jumpers placed as standard SPI flash setup, plus an extra jumper for UTX prints on P04.

	* Central:
		- Jumpers placed as standard SPI flash setup, plus extra jumpers for UTX prints on P04,and jumping wiring for **URX on P02**.

* **Software configuration**
	* Peripheral:
		- Use original SDK included ble_app_security for peripheral side app.
		- Change **.security_request_scenario** under **user_default_hnd_conf** in *user_config.h* to alter the security request behavior. DEF_SEC_REQ_NEVER by default.

	* Central:
		- Serial Terminal (e.g. TeraTerm) is required for the demo to work. A key input for number is required to choose the device to connect.
		- Define/Undefine **CFG_SHOW_FUNC_LOG** in *da1458x_config_basic.h* to enable/disable security related function prints. Disabled by default.
		- Define/undefine **CFG_SECURITY_ACTIVE** in *da1458x_config_basic.h* to change security behavior. When defined, central device will send bonding request on connection, ignoring the security request. When undefined, central device will wait for security_request from peripheral to start the pairing process.

	* Compatiblity:
The project was originally created on SDK6.10.511, some codes in SDK files should be modified for full feature.
The source file should work with other SDK file without any modification.

Following codes should be modified in SDK file to handle the missing key error during the encrypt process, otherwise connecting to a previously paired peripheral with missing key will result in ASSERT_ERROR:
- Added handling for SMP_ERROR_ENC_KEY_MISSING error in *gapc_cmp_evt_handler* in *app_task.c*
```
static int gapc_cmp_evt_handler(ke_msg_id_t const msgid,
                                struct gapc_cmp_evt const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id)
{
    switch(param->operation)
    {
        // reset completed
        case GAPC_UPDATE_PARAMS:
        {
            if ((param->status != CO_ERROR_NO_ERROR))
            {
                // it's application specific what to do when the Param Upd request is rejected
                CALLBACK_ARGS_1(user_app_callbacks.app_on_update_params_rejected, param->status)
            }
            else
            {
                CALLBACK_ARGS_0(user_app_callbacks.app_on_update_params_complete)
            }
        }
        break;

        default:
        {
            if(param->status != GAP_ERR_NO_ERROR)
            {
                if(param->status==SMP_ERROR_ENC_KEY_MISSING){
                    CALLBACK_ARGS_0(user_extra_app_callbacks.app_on_encrypt_key_missing)
                }
                else{
                    ASSERT_ERROR(0); // unexpected error
                }
            }
            if (app_process_catch_rest_cb != NULL)
            {
                app_process_catch_rest_cb(msgid, param, dest_id, src_id);
            }
        }
        break;
    }

    return (KE_MSG_CONSUMED);
}
```

## Known Limitations


- The central app should work with any BLE peripheral theoratically, but only garanteed to work with ble_app_security example in the SDK
- For ease of the key input, only 10 peripherals will be scanned and printed at one scanning activity
- The demo will take no action after successfully pairing/encryption, manually reset on either side is required to run the demo again

## License


**************************************************************************************

 Copyright (c) 2018 Dialog Semiconductor. All rights reserved.

 This software ("Software") is owned by Dialog Semiconductor. By using this Software
 you agree that Dialog Semiconductor retains all intellectual property and proprietary
 rights in and to this Software and any use, reproduction, disclosure or distribution
 of the Software without express written permission or a license agreement from Dialog
 Semiconductor is strictly prohibited. This Software is solely for use on or in
 conjunction with Dialog Semiconductor products.

 EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.

**************************************************************************************