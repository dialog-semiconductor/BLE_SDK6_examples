## renesas_ble_abstraction_gtl_project

## Example description

This example updates the basic proximity monitor service demonstration from the standard SDK `6.0.22.1401\projects\target_apps\ble_examples\prox_reporter_ext`. this example includes the device information client service and uses the External Processor configuration with GTL over UART. This setup allows the DA14531 and DA14535 PMOD devices to boot from an external MCU.

- Devices naming:
    - DA1453x is refering to DA14531-00, DA14531-01, DA14530 and DA14535.
    - DA1458x is refering to DA14585 and DA14586.
    - The DA14531-00 is the main DA14531 device. The -00 is just a new naming to introduce the variant DA14531-01. The DA14531-01 is a ROM variant of the main DA14531-00.
    - The DA14535 is a DA14531 upgrade.
## HW and SW configuration

- **Hardware configuration**
    - This example runs on the DA1453x (DA14531-00, DA14531-01 and DA14535) Bluetooth Smart SoC devices.
    - For the DA14531 PMOD product page, you can refer to [US159-DA14531EVZ BLE Pmod](https://www.renesas.com/us/en/products/wireless-connectivity/bluetooth-low-energy/us159-da14531evz-low-power-bluetooth-pmod-board-renesas-quickconnect-iot)

    - For the DA14531 getting started guide you can refer to [UM-B-117](https://lpccs-docs.renesas.com/UM-B-117-DA14531-Getting-Started-With-The-Pro-Development-Kit/index.html)

    - For the DA14535 getting started guide you can refer to this [UM-B-165](https://lpccs-docs.renesas.com/DA14535/UM-B-165-DA14531-Getting-Started-With-The-Pro-Development-Kit/index.html#device-family-getting-started-with-the-pro-development-kits)

    - For the DA14531 Module getting started guide you can refer to this [UM-B-139](https://lpccs-docs.renesas.com/UM-B-139-Getting-Started-with-DA14531-TINY-Module/index.html)

    - For detailed instructions to build a fully working solution that allows booting from host Renesas microcontroller please refer to the [UM-B-172: Getting started with DA1453x and FSP BLE Framework on Renesas Microcontrollers](https://lpccs-docs.renesas.com/DA1453x-FSP_BLE_Framework/UM-B-172/index.html).

- **Software configuration**
    - [SDK6 latest version](https://www.renesas.com/sdk6_latest)

## How to run the example

The generated GTL binaries are located in the `renesas_ble_abstraction_gtl_project\binaries` folder. These binaries can be used when compiling with the FSP. In an FSP project, the GTL binary is found at `ra/renesas/wireless/da14xxx/r_ble_gtl/r_ble_gtl_image.c`. For more details about the SDK updates that enable booting the DA1453x from an external Renesas microcontroller via UART, refer to [UM-B-119](https://lpccs-docs.renesas.com/UM-B-119_DA14585-DA14531_SW_Platform_Reference/User_guides/User_guides.html#fsp-attach).

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

