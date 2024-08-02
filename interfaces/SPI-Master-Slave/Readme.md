# DA14531 SPI Master Slave example

## Example description

This example describes how to perform SPI data buffer transmission/reception between two DA14531 devices via DMA. 
The project is split in two parts: The master board and the slave board.

## HW and SW configuration


### Hardware configuration

- This example runs on the DA1453x Bluetooth Smart SoC devices.
- Two DA14531 or DA1453x Pro Development Kits are needed for this example. One will act as master,   the other as slave.
- You need to make the following interconnections between the two DA14531 or DA1453x Pro Development Kits.
  The interconnections between the two DA145xx Pro Development Kits are given in the `user_periph_setup.h` file, on your setup, the SPI signals (CLK, MOSI, MISO, SC) need to have a 1kOhm resistor each in series and the SPI CLK must be in a twisted-pair configuration with the GND.
- Also, on each board you must connect P05 (P25 on the J2 DA14531 Signals) to UTX pin 17 on J1 (J5.12) in order to have UART console logging.

![SPI-Master-Slave-Connections](assets/SPI-Master-Slave-Connection.jpg)


### Software configuration

- This example requires:
    - SDK v6.0.16 or later
	- **SEGGER’s J-Link** tools should be downloaded and installed.
     
## How to run the example

For the initial setup of the project that involves linking the SDK to this SW example, please follow the readme [here](../../Readme.md).

### Compile and run

- Run master SPI Keil project. Run the python script to import needed files from the SDK (You can refer to the previous section)
- Under this folder: `SPI_Master\project_environment` open and start Keil µVision.

- Run slave SPI Keil project. Run the python script to import needed files from the SDK (You can refer to the previous section)
- Under this Folder: `SPI_Slave\project_environment` open and start Keil µVision.
- You build/download/run **first the DA14531_SLAVE** to one of the boards and then you build/download/run the DA14531_MASTER.
- You open two UART terminals for logging messages from both the master and the slave.	

### SPI Master/Slave communication

At the beginning of the main program the `system_init()` is called for system initialization. 
The SPI communication is initialized during the peripheral initialization through the function `periph_init()` function. The `spi_transfer` function 
is called to start sending/receiving data to/from SPI in full duplex mode.
The `spi_master_demo_out_buffer` and `spi_slave_demo_out_buffer` are a predefined buffers (12 bytes) with data to send/receive to/from SPI. 
The SPI master starts the communication by sending `spi_master_demo_out_buffer`  and receiving `spi_slave_demo_out_buffer`.
The callback function `spi_transfer_user_callback` is used to check the transfer status and to return in interrupt/DMA-driven transferring operations.
Finally,  

### Expected Results: UART Output

For each Board you need to Open a serial port with the following parameters.

		- baudrate: 115200
		- data: 8 bits
		- stop: 1 bit
		- parity: None
		- flow  control: none
		
 - As soon as the examples run on both boards (Slave and master) the following messages should appear on the terminals.

The following will be printed on the master and slave sides:

![SPI-SLAVE-MASTER_DA14531](assets/output_results.png)

### Expected Results: Signal Outputs

![SPI-SLAVE-MASTER_DA14531](assets/logic_trace.png)


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

