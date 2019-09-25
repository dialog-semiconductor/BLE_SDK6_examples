
# DMA example using SPI

---


## Example description

This example project demonstrates how to use DMA with the SPI peripheral.
**demo_spi**, executed upon startup, can be found in *user_spi.c*. The SPI functions are used from SDK6.0.11.x.

Configuration for SPI with DMA is done in *user_periph_setup.c*. 
To customize the SPI and DMA configuration change the defines in *user_periph_setup.h*, the SPI pin configuration can also be found here.
The function **spi_initialize**, that can be found in *user_periph_setup.c*, is also responsible for the initialization of DMA.

There are three important functions used in *user_spi.c*. **spi_send**, **spi_flash_read_data_dma** and **spi_transfer**.
All three functions set up the required DMA registers for their respective tasks. DMA is only used by **spi_send** and **spi_transfer** if \"SPI_OP_DMA\" is passed as
a parameter and \"CFG_SPI_DMA_SUPPORT\" is defined, otherwise the functions will be handled by the CPU. **spi_flash_read_data_dma** can only be used if \"CFG_SPI_DMA_SUPPORT\" is defined.  
   
It is very important to wait while the DMA is busy, if the rx or tx buffer is accessed while the DMA is busy a hardfault is very likely to occur.

This function, defined in user_spi.c, is used to wait while the channels are still used by DMA  
``void wait_for_DMA(void){
#if defined(CFG_SPI_DMA_SUPPORT)
	while(dma_get_channel_state(SPI_DMA_CHANNEL_RX) || dma_get_channel_state(SPI_DMA_CHANNEL_TX)){}
#endif
}``   
\"SPI_DMA_CHANNEL_RX\" and \"SPI_DMA_CHANNEL_TX\" can be found in *user_spi.h*  
	
To change the size of the tx and rx buffers change \"TEST_SIZE\" in *user_spi.h*. 

SPI without using DMA can also be tested, go to: Project -> Options for Target 'Your device, for example: DA14531' -> c/c++ and in the Preprocessor Symbols remove \"CFG_SPI_DMA_SUPPORT\".
   	
## HW and SW configuration


**Hardware configuration**

  - This example runs on the DA1458x and DA14531 Bluetooth Smart devices
  - The basic/pro development kit is needed for this example

 **Software configuration**

- This example requires:
  - Keil µVision 5
  - SDK6.0.11
  - **SEGGER’s J-Link** tools should be downloaded and installed


## How to run the example

For initial setup of the example please refer to [this section of the dialog support portal](https://support.dialog-semiconductor.com/resource/da1458x-example-setup).

### Initial Setup

- Configure the development kit as shown below 
- **The jumper wire connection between P29(P2_9) and P13(P1_3) is only needed for the DA14531**
	- This connection is used to place a cursor in SmartSnippets toolbox. P29 can be changed to another pin in *user_spi.h*, but make sure it is connected to P13
	- The DA1458x can directly access P13
- The jumper configuration for DA1458x and DA14531 is the same
  
![Jumper settings](assets/Motherboard_Configuration.png) 

- Select your device, the red box in the image below. 
![Select device](assets/Select_Device.png)

- (Optional) Change configurations
- (Optional) attach [SmartSnippets toolbox](https://s3.eu-west-2.amazonaws.com/lpccs-docs.dialog-semiconductor.com/SmartSnippetsToolbox5.0.8_UM/index.html) or logic analyzer     
- Compile the code and start debugging in Keil µVision 5

The resulting SPI transactions look like this:

![Analyzer capture](assets/Analyzer_Capture.png)

This demo application will generate a software cursor in the SmartSnippets toolbox power profiler. The cursor will be placed before the transfers,
as is shown below, this image is a capture of the DA14531

![Power Profiler screenshot](assets/DA14531_Smartsnippets_Capture.png)


## Known Limitations

- There are no known limitations for this example. But you can check and refer to the following application note for
[known hardware limitations](https://support.dialog-semiconductor.com/system/files/resources/DA1458x-KnownLimitations_2018_02_06.pdf "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums).
- you can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.


## License


**************************************************************************************

 Copyright (C) 2019 Dialog Semiconductor. This computer program or computer programs included in this package ("Software") include confidential, proprietary information of Dialog Semiconductor. All Rights Reserved.
 
 THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’) AND MAY ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND.  THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS.  RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG’S PRIOR WRITTEN PERMISSION.
 
 UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED BY THE FCC OR OTHER LIKE AGENCIES.
 
 IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**************************************************************************************
