# DA14585/DA14586/DA14531 Central Implementation

![Banner](https://s3.eu-central-1.amazonaws.com/lpccs-docs.renesas.com/metadata/BLE_SDK6_examples/connectivity/central/banner.svg?v=1)


## Example description

This project is intended to illustrate to the user:
- How to scan for peer devices
- How to parse advertisement data during the scan process
- How to connect to a peer device
- How to do a service discovery and parse the services for characteristic values
- How to perform GATT Reads, Writes, and handle notifications

This functionality can be verified by:
- Connecting to another peripheral device (Out of the Box supports Proximity Reporter)
- Reading serial output from the device.
	

## HW and SW configuration
This example runs on the BLE Smart SoC (System on Chip) devices:
- DA14585/DA14586 or DA14531 daughter board + DA145xxDEVKT-P PRO-Motherboard.
- DA14585/DA14586 Basic development Kit.
- DA14531 USB Kit

The user manuals for the development kits can be found:
- [DA14531 Development Kit Product page](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro) for the DA145xxDEVKT-P PRO-Motherboard.
- [DA14585 Development Kit Basic](https://www.dialog-semiconductor.com/products/da14585-development-kit-basic) for the Basic Development Kit.
- [DA14531 USB Development Kit Product page](https://www.dialog-semiconductor.com/products/da14531-development-kit-usb) for the DA14531 USB Development Kit.

* **Hardware configuration using the DA145xxDEVKT-P PRO-Motherboard**

	- When using DA14585/DA14586, with the proper project configuration, the UTX jumper needs to be placed.
	
	- When using DA14531, Jumper P2_6 from J2 to UTX (Pin 17 on J1) for serial UART communication.
	
	![da14531.jpg](assets/da14531.jpg).
	
    - When using DA14531 USB development kit no hardware configuration is needed. You need only to Redefine the UART2_TX_PIN to GPIO_PIN_5 (1 wire UART ) 
	  
	  **#define UART2_TX_PIN            GPIO_PIN_5**

	![da14531_usb.png](assets/da14531_usb.png).
	
* **Software configuration**

    - This example requires:
        * [SDK6.0.14](https://www.dialog-semiconductor.com/da14531_sdk_latest), Older versions of SDK6 require minimal changes.
        * SEGGER’s J-Link tools should be downloaded and installed.


## How to run the example

### Setup

For the initial setup of the project that involves linking the SDK to this SW example, please follow the Readme [here](../../Readme.md).

-Start Keil using the `central` Keil project file.

2. Expand the dialog shown in the red box in the image below.
	
	![Expand_Select_Device](assets/Expand_Select_Device.png)

3. Select your target device: DA14531, DA14585, DA14586

4. Open your favorite serial terminal application, e.g. TeraTerm, RealTerm, Putty - use 921600, 8, N, 1 as your settings. (A significant amount of traffic can occur during the scanning process)

5. Compile (F7) and launch (ctrl + F5) the example.\
If the warning (shown below) pops up press OK.

### Running the Example - Scan and Connect

1. Verify that your central started via the Terminal.  The default settings of the central will start scanning for devices that use a GAP_LOCAL_NAME (0x09) GAP flag type. If there are no devices in the area, there will be no output. 
	![terminal_local_name_output](assets/terminal_local_name_output.png)

2. The project is configured to filter for proximity reporters and to connect to the first advertisement seen. On a separate board, run proximity reporter.  Go within the SDK to target_apps/ble_examples/prox_reporter, open the project and compile and run as you did for this example.

3. If running from JTAG and the original example is deployed via JTAG, be sure to assign the proper JTAG serial number to each target.  In Keil, this can be done by right clicking on the project and going to Options for Target ....  Go to Debug then settings and select the serial number that matches the sticker on the board.

	![debug_jtag_settings](assets/debug_jtag_settings.png)
	
3. Once running, verify in the terminal that you see output for the connection and service discovery. 
	
	![terminal_on_connection](assets/terminal_on_connection.png)

4. After the service discovery, you can press the button the central device.  This will write to the alert service and start blinking the LED on the proximity reporter.  You should also see the write over the terminal if sent properly.

	![Gatt_Write](assets/Gatt_Write.png)
	
### Running the Example - Scan for all devices

1.  The project can be configured to filter on multiple GAP flags.  

2.  Go into user_central_config.h and change SCAN_FILTER to SCAN_FILTER_NONE

3.  Compile and Running

4.  All Local BLE devices will now be displayed over the terminal.

	![terminal_no_filter](assets/terminal_no_filter.png)


### General Project Notes
 - This example illustrates the basic building blocks for implementing a central device.  The 5xx devices are limited on memory and this should be taken into consideration
when implementing a central on the 5xx devices, is the limitation on memory.  The central devices should be geared more toward specific applications, opposed to a more generic central.  
This example was written generically enough that it can be exanded on for a central application.


- This example is written in a manner to have the capability of supporting more than one connection.  It must be noted that multiple connections requires
some slight modifications of the SDK, but is possible.  The 58x supports 6-8 connections where the 531 is limited to 3.

## Description of functions

The application functionality is broken up into three source code files: user_central_app.c, user_ble_gap.c and user_ble_gatt.c. The description of the most important functions is as follows:

1.  user_central_app.c
    -   app_button_press_cb: The function that will be called back when the button is pressed and the associated interrupt triggered. It will perform a write alert level to the Immediate Alert Service for every connected peripheral.
	-   configure_alert_button: Register the callback function and the interrupt which will fire on button press.
	-	handle_service_disc_finished: Will be called when a message is delivered to the application task which signals that the Service Discovery has finished. If the Battery Service is enabled on the peripheral, it will read the Battery Level and configure the button for alert.
	-	user_on_adv_report_ind: Application hook which formats and outputs the advertising reports and saves the identity of the peripheral for later connection.
	-	user_on_connection: Application hook for a connection event. It will trigger a Service Discovery. 
	-	handle_svc_ind: Outputs information of the Characteristics during the Service Discovery and checks for the Battery Service and Immediate Alert Service. Handles, UUIDs, properties etc. can be checked here for later processing and use by the application. 
2.	user_ble_gap.c
	-	user_ble_gap_start_scan: Configures scanning parameters and starts scanning for peripherals.
	-	user_ble_gap_connect: Initiates a connection request.
3.	user_ble_gatt.c
	-	user_gatt_discover_all_services: Starts a Service Discovery for all handle values (0 to maximum permitted, 0xFF).
	-	user_ble_gatt_write: Helper function to write values to Characteristics.
	-	user_gatt_read_simple: Helper function to read the value of a Characteristic.
	-	user_gatt_parse_service: Parses a service.


## Known Limitations

- There are no known limitations for this example. But you can check and refer to the following 
  application note for known hardware limitations.
For DA14531 devices:
  [DA14531 hardware limitations](https://www.dialog-semiconductor.com/sites/default/files/da14531_errata_1v0.pdf)
For DA14585 devices:
  [DA14585 hardware limitations](https://www.dialog-semiconductor.com/sites/default/files/da1458x-knownlimitations_2019_01_07.pdf)
- Dialog Software [Forum Link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software "Forum Link").
- You can also refer to the troubleshooting section in the [Getting Started with the DA14531 PRO Development Kit](http://lpccs-docs.dialog-semiconductor.com/UM-B-117-DA14531-Getting-Started-With-The-Pro-Development-Kit/index.html).

