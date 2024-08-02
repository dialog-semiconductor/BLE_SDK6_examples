# DA145xx I2C Master Slave example

## Example description

This example describes how to perform I2C data buffer transmission/reception between two boards in asynchronous mode (non-blocking communication).
The project is split in two parts: the Master Board and the Slave Board.

## HW and SW configuration


### Hardware configuration

- This example runs on the DA14585, DA14586 and DA14531 Bluetooth Smart SoC devices.
- Two DA145xx Pro Development Kit are needed for this example. One will act as Master, the other as slave.

 * **Hardware configuration for DA14585/DA14586 devices**
    - Connect the each DA145xx Pro Development Kit to the host computer.
    - Connect the SCL pins of the DA145xx Pro Development kits together. SCL is P0_2 by default.
    - Connect the SDA pins of the DA145xx Pro Development kits together. SDA is P0_1 by default.
    - This example uses SW3. 
    - UART TX on P0_4, place a jumper between J1:17 and J1:18, for printing functionality.

 * **Hardware configuration for DA14531 devices**
    - Connect the each DA145xx Pro Development Kit to the host computer.
    - Connect the SCL pins of the DA145xx Pro Development kits together. SCL is P0_6 by default.
    - Connect the SDA pins of the DA145xx Pro Development kits together. SDA is P0_7 by default.
    - This example uses SW2. 
    - UART TX on P0_3, place a jumper between J1:17 and J1:18, for printing functionality.(Note P0_3 is unavailable on the module daughterboard)
	
### Software configuration

- This example requires:
    - SDK v6.0.14 or later
	- **SEGGER’s J-Link** tools should be downloaded and installed.
     
## How to run the example

For the initial setup of the project that involves linking the SDK to this SW example, please follow the Readme [here](../../Readme.md).

### Compile and run

- Run Master I2C Keil project. Run the python script to import needed files from the SDK (You can refer to the previous section)
	- Under this Folder: `I2C_Master\project_environment` Open and Start Keil µVision.

- Run Slave I2C Keil project. Run the python script to import needed files from the SDK (You can refer to the previous section)
	- Under this Folder: `I2C_Slave\project_environment` Open and Start Keil µVision.
- Compile and run the project.

### I2C Master/Slave communication

The I2C peripheral configuration is ensured by the i2c_init API which Initialize I2C controller. Two applications functions are used for Transmission/Reception in slave and Master mode:
i2c_master_transmit_receive() and i2c_slave_receive_transmit()


The **i2c_master_receive_buffer_async()** and the **i2c_master_transmit_buffer_async()** functions allow respectively the reception and the transmission of a predefined data buffer in Master mode.
The **i2c_slave_receive_buffer_async()** and the **i2c_slave_transmit_buffer_async()** functions allow respectively the reception and the transmission of a predefined data buffer in Slave mode.


In a first step after the user **presses the Key on the Master Board**, I2C Master starts the communication by sending write_buffer through i2c_master_transmit_buffer_async() to I2C Slave which receives receive_buffer through i2c_slave_receive_buffer_async(). 
The I2C Slave sends transmit_buffer through i2c_slave_transmit_buffer_async() to the I2C Master which receives read_buffer through i2c_master_receive_buffer_async().

### Expected Results: UART Output

For each Board you need to Open a serial port with the following parameters.

		- baudrate: 115200
		- data: 8 bits
		- stop: 1 bit
		- parity: None
		- flow  control: none
		
 - As soon as the examples run on both boards (Slave and master) following message should appear on the terminals.

The following will be printed on the master side:
```
 ---------------
 I2C Master Demo
 ---------------
This is what the master sent to the slave
60 86 FC 07 51 01 FC 07 59 01 FC 07 71 01 FC 07 

This is what the master received from the slave
0F 0A 8C 3A 6E 6D D6 CB B3 16 3B 7F 2B 0C 47 36 
```
This will be printed on the slave side:
```
 ---------------
 I2C Slave Demo 
 ---------------
This is what the slave received from the master
60 86 FC 07 51 01 FC 07 59 01 FC 07 71 01 FC 07 

This is what the slave sent to the master
0F 0A 8C 3A 6E 6D D6 CB B3 16 3B 7F 2B 0C 47 36
```

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

