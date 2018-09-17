# BLE Erratum 10734 - security vulnerability
## This SDK6 DA14585 example shows 
1. how to send a command from the applicaiton layer to ensure to update the security key and proof it by analyzing SMP in BLE


## Features
1.Search the code with [sec test] string to identify all the changes being made
2.CFG_APP_SECURITY_FIX is used to write down the code
3.After 3 pairing success or pairing failure the security key will be updated
4.Keys will be stored in the SPI flash
5.Printing over UART for debugging info and optional data logging is done


## HW setup
DA14585 Basic/Pro kit, UART TX/RX jumper on P04/P05, SPI jumper on P00/P03/P05/P06/BATT_SEL
You need 3 pin jumper on P05

## SW setup 
Note: 
SDK6.0.10 is used for development
UART terminal: 115200, no flow control
SPI is used (see HW setup)

Run the dlg_make_keil5_env_v1.002.py 
Example:: >python dlg_make_keil5_env_v1.002.py -sdkpath <DA14585 SDK6 path in your file directory>
Run the example after compilation
1. Central will try to make a connection using pairing and bonding mechanism
2. Peripheral will response pairing is successful, check serial debugging information over UART
3. Central will issue a disconnection
4. Forget the device bonding information from the central
5. Repeat from step 1 to 4, 3 more times and verify the information exchange over UART for debugging
6. Use a ble sniffer and take a look at the SMP protocol, for the 1st 3 times of successful pairing the peripheral will show the same public key
7. When the forth time the pairing will be successful the SMP will show in the sniffer that the public key is changed.
8. Same applies for unsuccessful pairing process




