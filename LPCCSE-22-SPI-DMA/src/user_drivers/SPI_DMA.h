/**
 ****************************************************************************************
 *
 * @file SPI_DMA.h
 *
 * @brief SPI DMA handling fuctions.
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs included in this package ("Software") include confidential, proprietary information of Dialog Semiconductor. All Rights Reserved.
 * 
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’) AND MAY ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND.  THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS.  RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG’S PRIOR WRITTEN PERMISSION.
 * 
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED BY THE FCC OR OTHER LIKE AGENCIES.
 * 
 * IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 *
 ****************************************************************************************
 */
 
 #include <stdint.h>
 
 /**
 ****************************************************************************************
 * @brief SPI dma transmit only, does not control cs line.
 * @param[in] data_to_send Address of the first byte to send
 * @param[in] length       Amount of bytes to send
 * @return void
 ****************************************************************************************
 */ 
 void spi_dma_transmit(uint8_t* data_to_send, uint16_t length);
 
 /**
 ****************************************************************************************
 * @brief SPI dma receive only, does not control cs line.
 * @param[in] data_buffer  Starting address of the data buffer
 * @param[in] length       Amount of bytes to receive
 * @return void
 ****************************************************************************************
 */ 
 void spi_dma_receive(uint8_t* data_buffer, uint16_t length);
 
 /**
 ****************************************************************************************
 * @brief SPI dma transmit/receive, does not control cs line.
 * @param[in] data_to_send Address of the first byte to send
 * @param[in] data_buffer  Starting address of the data buffer
 * @param[in] length       Amount of bytes to send/receive
 * @return void
 ****************************************************************************************
 */ 
 void spi_dma_tranceive(uint8_t* data_to_send, uint8_t* data_buffer, uint16_t length);
 
 /**
 ****************************************************************************************
 * @brief Wait for the DMA transfers to finish.
 * @return void
 ****************************************************************************************
 */ 
 void wait_for_dma(void);
 
 /**
 ****************************************************************************************
 * @brief Small demo of the above functions, this function is called on startup.
 * @return void
 ****************************************************************************************
 */
 void demo_spi(void);
 
