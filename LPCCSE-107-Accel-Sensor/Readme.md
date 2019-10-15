
# DA14585/DA14586 Reading out an I2C accelerometer and sending notification data

---


## Example description

This SDK6 DA14585 example shows how to communicate with an I2C accelerometer sensor.
The sensor data gets sent over BLE with a notification. 

An I2C sensor is not necessary to run this example.
Defining NO_SENSOR in ADXL345.h disables reading of the sensor. 
The application will send an incrementing number over BLE in this case. 

## HW and SW configuration


### Hardware configuration

- This example runs on The DA14585/DA14586 Bluetooth Smart SoC devices.
- The Basic or pro Development kit is needed for this example.
- Connect the SCL pin of the ADXL345 to pin 1-0 of the development board.
- Connect the SDA pin of the ADXL345 to pin 1-2 of the development board.
  - These pins are defined in the *ADXL345.h* file
  
![Hardware setup Fritzing](assets/ADXL_Fritzing.png)
  
- Make sure the CS pin of the ADXL345 is tied high, this enables I2C mode.
- The SDO pin is used to select the I2C address, the standard setting in *ADXL345.h* is for SDO high.
- Connect the USB Development kit to the host computer.


### Software configuration

- This example requires:
    - SDK6.0.10
	- A smartphone with a BLE scanning app (for example BLE scanner on Android or Lightblue on IOS)
	- **SEGGER’s J-Link** tools should be downloaded and installed.

## How to run the example

For initial setup of the example please refer to [this section of the dialog support portal](https://support.dialog-semiconductor.com/resource/da1458x-example-setup).

### Compile and run

- Open the project in Keil µVision 5
  - Optionally, change the parameters in ADXL345.h
- Compile and run the project
- Open the BLE scanner app and look for DLG-ACCL
- Connect to the device
- Subscribe to the notifications

You should now be able to see the live X, Y and Z acceleration(in milli g). This is shown on the screenshot below:

![Notification screenshot](assets/notification_screenshot.jpg)

## Evothings

This example also comes with an Evothings demo application. The application can connect to the DA14585, and read out the acceleration data.
There is also a 3d visualization of pitch and roll. The app is shown below.

![App screenshot](assets/app_screenshot.jpg)

### Installing the Evothings viewer

At the time of writing, the Evothings viewer app is temporarily unavailable on IOS, step 3 of the [Evothings download site](https://evothings.com/download/) will get updated once it becomes available again.  
The Android version of the app is available in the [Google Play store](https://play.google.com/store/apps/details?id=com.evothings.evothingsviewer).

### Running the example app

When the Evothings viewer app is started, you are able to connect to the demo project.  
The project is hosted at <span style="color:red;">INSERT LINK</span>, simply input this URL in the app and tap connect. The app will now load.  
When the app is loaded you can start scanning for the DA1458x device. If the device is found, the accelerometer data is shown without the device being connected.
This is possible because the accelerometer data is included in the manufacturer part of the advertisement data. This is handled in the *adv_data_update_timer_cb* function.  
After a connection is established, the smartphone subscribes to a notification on the DA1458x device. After this has happened, the device will start sending data to the smartphone.
This notification is handled by the following functions: *user_catch_rest_hndl*, *user_svc2_g_wr_ntf_handler*, *user_svc2_g_timer_cb_handler*. 

## Known Limitations


- There are No known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums).
- you can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.


## License


**************************************************************************************

 Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs included in this package ("Software") include confidential, proprietary information of Dialog Semiconductor. All Rights Reserved.
 
 THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’) AND MAY ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND.  THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS.  RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG’S PRIOR WRITTEN PERMISSION.
 
 UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED BY THE FCC OR OTHER LIKE AGENCIES.
 
 IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**************************************************************************************
