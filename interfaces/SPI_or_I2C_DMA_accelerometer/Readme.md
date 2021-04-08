# SPI or I2C reading with DMA

---


## Example description

This example demonstrates how to use SPI or I2C to interface with the LIS2DH accelerometer. The accelerometer will store it's measurements in an internal 32 measurement fifo. When this fifo is full the interrupt pin is asserted, triggering an interrupt on the DA1453x. In the triggered interrupt handler, a DMA transfer is setup to read the fifo contents with minimal CPU involvement. At the end of the DMA transfer, another interrupt is fired, in which the retrieved data is handled. In this example the data is printed out over UART. 
   	
## HW and SW configuration


**Hardware configuration**

This example runs on the Dialog DA14531 and DA14530 BLE Smart SoC (System on Chip) devices and uses the [steval-mki135v1 LIS2DH accelerometer board](https://www.st.com/en/evaluation-tools/steval-mki135v1.html).


The user manuals for the DA145xxDEVKT-P PRO-Motherboard can be found [here](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro).


* __Hardware configuration DA1453x using DA145xxDEVKT-P PRO-Motherboard__
- Connect P21 to SDA.
- Connect P23 to CS.
- Connect P24 to SCL.
- Connect P25 to SDO.
- Connect P29 to INT1.

- Connect P26 to pin 17 of J1 (The middle pin next to UTX)

- VDDIO and VDD should be connected to v3 on the motherboard

 **Software configuration**

This example requires:
- [SDK6.0.14](https://www.dialog-semiconductor.com/da14531_sdk_latest).
- Keil5.
- __SEGGER’s J-Link__ tools should be downloaded and installed.

The UART connection uses a baudrate of 1000000(1M). 8 bits data, 1 stop bit and no parity.

## How to run the example
### Setup
Before launching the Keil project, make sure to link the SDK and project environment using the Python linker script `dlg_make_keil_vx.xxx`. More information [here](http://lpccs-docs.dialog-semiconductor.com/Software_Example_Setup/index.html).
1. Start Keil using the `lis2dh.uvprojx` Keil project file.

2. Choose wether to use I2C or SPI via the dropdown menu. (Connections are the same for both)

3. Compile (F7) and launch (ctrl + F5) the example.

## Expected Results

When the app starts up it will send the following message:

    user_app_on_set_dev_config_complete

Afterwards, the following will be printed everytime the FIFO is read out.

    Samples Read from FIFO: 31
     x:78, y:160, z:-50
     .... repeated for all samples from the FIFO

## Troubleshooting
- Please check that the steps according to your daughter board (DA14531) and mother board (basic development kit or DA145xxDEVKT-P PRO-Motherboard) are followed correctly.
- If there are problems with SmartSnippets Toolbox the user manual can be found [here](http://lpccs-docs.dialog-semiconductor.com/SmartSnippetsToolbox5.0.8_UM/index.html)
- If none of the above points help, please check the user manual according to your daughter board and mother board. User manual can be found [here](https://www.dialog-semiconductor.com/products/da14531-development-kit-pro) for the DA145xxDEVKT-P PRO-Motherboard and [here](https://www.dialog-semiconductor.com/sites/default/files/um-b-048_da14585da14586_getting_started_guide_v2.0_0.pdf) for the Basic Development Kit.

