# DA14531/585/586 Multirole Example

![Banner](https://s3.eu-central-1.amazonaws.com/lpccs-docs.renesas.com/metadata/BLE_SDK6_examples/connectivity/multirole/banner.svg?v=1)

## Example Description

The example demonstrates the capabilities of the DA14531/585/586 as a Central and a peripheral i.e. scan and advertise role. 

**Note**: This example applies for DA14585/586 as well. The DA14531 is capable of handling up to 3 connections and the DA14585/586 can handle up to 8 connections.

The project works as a Central connecting to 2 peripherals, and once connected, it advertises so another central can connect to it.

**Note**: This project assumes that the 2 peripherals will stay connected to the DA14531 central. 

## HW setup
 - 3 DA14531 Pro-DK kit with Module/Daugherboard connected to host computer via USB and an android or iPhone.
 - For Central, extra jumpers are required for the UART to see the communication logs. Check the setup shown below for DA14531 and DA14585/586.
 
 <ins>DA14531 daughterboard</ins>
 
 ![da14531.jpg](assets/da14531.svg)
 

  <ins>DA14535 daughterboard</ins>
 
 ![da14535.jpg](assets/da14535_535dau.svg)

  <ins>DA14531 module</ins>
  
 ![module.jpg](assets/module.svg)
 
 <ins>DA14585 daughterboard</ins>
 
 ![da14585.jpg](assets/da14585.svg)
 
 
 The user manuals for the development kits can be found [here](https://www.renesas.com/eu/en/document/mat/um-b-117-da14531-getting-started-pro-development-kit-html-chinese?r=1564826) for the DA145xxDEVKT-P PRO-Motherboard.

## SW setup 

 - SDK 6 latest version
 - SmartSnippets Toolbox 5.0.14.3038 
 - a terminal for UART logs (teraterm, termite, etc..)
 
## How to run the example

1. For the initial setup of the project that involves linking the SDK to this SW example, please follow the Readme [here](https://github.com/dialog-semiconductor/BLE_SDK6_examples).

2. Download Prox_reporter binaries with **different BD addresses and default device name "DLG-PROXR"** to 2 DA14531 devices (as explained [here](http://lpccs-docs.dialog-semiconductor.com/UM-B-083/getting_started/getting_started.html)). 
You can refer to this [document](http://lpccs-docs.dialog-semiconductor.com/um-b-138/introduction.html) to program the flash with the prox_reporter binaries. Press reset (SW1) on the motherboard and verify with an Android device that they have all started advertising

Note: To configure your device with different BD addresses, follow [here](http://lpccs-docs.dialog-semiconductor.com/DA145xx_Advertising_Tutorial/setting_the_BD_address_and_device_name.html#setting-the-bd-address)

3. Using Keil IDE, open the multirole project and compile the project to build the indexes. You will see errors but ignore it for this step, error will go away after the compilation in step 7.

4. To be able to override the default functions of the SDK the __ EXCLUDE_ROM_APP_TASK __ should be defined in the C/C++ tab in the "options for target" in keil, like so,

	![exclude_rom_tasks.jpg](assets/exclude_rom_tasks.jpg)

5. This example needs changes in the SDK to work. To apply the patch file, execute the following command inside your sdk directory:
```
git apply path/to/patch/SDK6patch.diff
```
	
6. Build and compile the project.		

7. Download the binary to another DA14531 device (Use flash programmer tool as mentioned in step 1). 

**Note**: Use any android/iPhone BLE scanner to connect/disconnect to/from the DA14531.

**Note: For the DA14585, using more than 4 active connections, the default value of the #define CFG_RET_DATA_SIZE 2048, needs to changed to 4096, in the user_config.h file.**


## Supported use-case for the example

Below mentioned is the use-case that is supported with this example. In future releases, more use-cases will be added especially with regards to the disconnections.

For the ease of understanding, following notations are used. 

* C0/P0 -> DA14531 central/peripheral
* P1,P2 -> DA14531 peripheral 
* C1 -> Phone central

**Use case:** The flow of this use-case is mentioned below. Basically the peripherals starts to advertise first and then the Central starts scan process, and it finds and connects to 2 peripherals. 

<ins>Connection<ins>
- P1 is advertising
- P2 is advertising
- C0 starts to scan, finds P1 and P2 and connects
- P0 advertises
- C1 connects to P0

<ins>Disconnection<ins>
- C1 disconnects
- P0 advertises
- C1 connects to P0 again

This can be verified in the UART logs.

**Note**: While using iPhone, if you wish to disconnect C1 from DA14531 device, please check the iPhone bluetooth settings as the phone might have to be disconnected from there. 
Once it disconnects, you will see P0 advertising again while being connected to P1 and P2. 
For Android, you can use any BLE scanner app to connect and disconnect.

**Note**: UART terminal: 921600, 8N1, no flow control

![logs.jpg](assets/logs.jpg)

## Limitations

- The current example doesn't handle the disconnection of P1 or P2 from their central C0. This will be handled in future release.

## Troubleshooting

- Try a different USB1 cable.

- Try different jumper wires, if used.

- Verify using any BLE scanner that the 2 peripherals are advertising before attempting connection. 

- If none of the above helps, please check with the customer support team who would be glad to provide you the solution.

