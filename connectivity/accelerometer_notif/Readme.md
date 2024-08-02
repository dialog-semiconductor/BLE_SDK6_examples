# Reading an I2C accelerometer and sending BLE notifications

## Example description

This example shows how to acquire data from an I2C accelerometer and send the measurements with BLE notifications using a DA14531 or DA14585/586 device. You could use a BLE Scanner app to read out the measurements.

An I2C sensor is not necessary to run this example.
Defining NO_SENSOR in ``ADXL345.h`` disables reading of the sensor. 
The application will send an incrementing number over BLE in this case. 

## HW and SW configuration


### Hardware configuration

- This example runs on the DA14531 or DA14585/DA14586 Bluetooth Smart SoC devices.
- The DA145xx Pro Development Kit is needed for this example.

Follow the hardware configuration according to your daughterboard, DA14531 or DA14585/DA14586.

* **Hardware configuration for DA14531 devices**
    - Connect the USB1 connector of the DA145xx Pro Development Kit to the host computer.
    - Connect the positive rail of your breadboard to pin V3 of the development board and the negative rail to any ground pin of the development board (the ones marked with a dash). Power up the sensor by connecting the 3.3V pin of the ADXL345 to your positive rail and the GND pin of the sensor to your negative rail.
    - Tie high the SDO and CS pins of the sensor to your positive rail. This enables the sensor to operate in I2C mode and adjusts the I2C sensor address to match the one defined in the project.
    - Connect the SCL pin of the ADXL345 to pin P23 of the development board.
    - Connect the SDA pin of the ADXL345 to pin P21 of the development board.

 * **Hardware configuration for DA14585/DA14586 devices**
    - Connect the USB1 connector of the DA145xx Pro Development Kit to the host computer.
    - Connect the positive rail of your breadboard to pin V3 of the development board and the negative rail to any ground pin of the development board (the ones marked with a dash). Power up the sensor by connecting the 3.3V pin of the ADXL345 to your positive rail and the GND pin of the sensor to your negative rail.
    - Tie high the SDO and CS pins of the sensor to your positive rail. This enables the sensor to operate in I2C mode and adjusts the I2C sensor address to match the one defined in the project.
    - Connect the SCL pin of the ADXL345 to pin P13 of the development board.
    - Connect the SDA pin of the ADXL345 to pin P11 of the development board.
  
### Software configuration

This example requires:
- SDK v6.0.14 or later
- **SEGGER’s J-Link** tools should be downloaded and installed.
- An application like LightBlue Explorer should be used to act as a BLE Scanner and view the received measurement values. It can be found on [Google Play](https://play.google.com/store/apps/details?id=com.punchthrough.lightblueexplorer) or on the [App Store](https://apps.apple.com/gb/app/lightblue-explorer/id557428110).

## How to run the example

For the initial setup of the project that involves linking the SDK to this SW example, please follow the Readme [here](https://github.com/dialog-semiconductor/BLE_SDK6_examples).

### Compile and run

- Start Keil µVision.
  - Optionally, change the parameters in ADXL345.h
- Select the DA1453x, DA14585 or DA14586 device in the box shown below

    ![select_5xx_device](assets/select_5xx_device.png)

- Compile and run the project.

### Connecting to the device
- Open the BLE scanner app and look for DLG-ACCL
    
    ![device_list](assets/capture_device_list.jpg)

- Connect to the device. Lightblue will list all the available services.

    ![connected](assets/connected.jpg)

- Select the characteristic you wish to explore, like the X acceleration data. 

    ![pick_characteristic](assets/pick_characteristic.jpg)

- Subscribe to notifications and select the UTF-8 String data representation. You will be now ready to receive live the X acceleration (in milli g).
    
    ![subscribe](assets/subscribe.jpg)

## Further reading

- [Wireless Connectivity Forum](https://lpccs-docs.renesas.com/lpc_docs_index/DA145xx.html)



## Known Limitations

- There are no known limitations for this example. But you can check and refer to the following application note for
[SDK6 known limitations](https://lpccs-docs.renesas.com/sdk6_kll/index.html)

## Feedback and support ?

If you have any comments or suggestions about this document, you can contact us through:

- [Wireless Connectivity Forum](https://community.renesas.com/wireles-connectivity)

- [Contact Technical Support](https://www.renesas.com/eu/en/support?nid=1564826&issue_type=technical)

- [Contact a Sales Representative](https://www.renesas.com/eu/en/buy-sample/locations)

