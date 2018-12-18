/**
 ****************************************************************************************
 *
 * @file SPI_DMA.c
 *
 * @brief SPI DMA source code.
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
 
 
 #include "SPI_DMA.h"
 #include "spi.h"
 #include "user_periph_setup.h"
 #include "arch_api.h"
 
/// USE DMA
 #ifdef CFG_USE_DMA
 #define USE_DMA 1
 #else
 #define USE_DMA 0
 #endif
 #define NUMBYTES 100
 
 /**
 ****************************************************************************************
 * @brief SPI dma transmit only, does not control cs line.
 * @param[in] data_to_send Address of the first byte to send
 * @param[in] length       Amount of bytes to send
 * @return void
 ****************************************************************************************
 */ 
 void spi_dma_transmit(uint8_t* data_to_send, uint16_t length){
	 #if USE_DMA
	 length = length-1; //We will be handeling the first byte ourself
	 SetWord16(DMA0_A_STARTL_REG, ((uint32_t) data_to_send + 1) & 0xFFFF);          //Set the lower 16 bits of the source
	 SetWord16(DMA0_A_STARTH_REG, (((uint32_t) data_to_send + 1) >> 16) & 0xFFFF);  //Set the upper 16 bits of the source
	 SetWord16(DMA0_B_STARTL_REG, (uint32_t) SPI_RX_TX_REG0 & 0xFFFF);              //Set the lower 16 bits of the destination
	 SetWord16(DMA0_B_STARTH_REG, ((uint32_t) SPI_RX_TX_REG0 >> 16) & 0xFFFF);      //Set the upper 16 bits of the destination
	 SetWord16(DMA0_LEN_REG, length);                                               //Set the length of the transfer 
	 SetWord16(DMA0_INT_REG, length);                                               //Generate an interrupt after the transfer 
	 
	 SetBits16(DMA_REQ_MUX_REG, DMA01_SEL, 0x0); //Map the spi data req signals to dma0/1
	 
	 SetBits16(DMA0_CTRL_REG, AINC, 1);      //Enable source address incrementation
	 SetBits16(DMA0_CTRL_REG, BINC, 0);      //Disable destination address incrementation
	 SetBits16(DMA0_CTRL_REG, DREQ_MODE, 1); //Dma is triggered by data req
	 SetBits16(DMA0_CTRL_REG, BW, 0);        //Dma bus width is 8 bits
	 SetBits16(DMA0_CTRL_REG, IRQ_ENABLE, 1);//Enable interrupt on channel 0
	 SetBits16(DMA0_CTRL_REG, DMA_ON, 1);    //Turn on dma channel
	 
	 NVIC_ClearPendingIRQ(DMA_IRQn); //Clear the interrupt for safety
	 NVIC_EnableIRQ(DMA_IRQn);       //Enable the interrupt
	 
	 spi_access(data_to_send[0]); //Start transfer by sending the first byte manually
	 #else
	 SetBits16(SPI_CTRL_REG1, SPI_FIFO_MODE, 0x2); //Use the TX fifo for increased speed 
	 for(int i = 0; i < length; i++){
		 while(GetBits16(SPI_CTRL_REG, SPI_TXH)){ //Wait while TX fifo is full		   
	   }
	   SetWord16(SPI_RX_TX_REG0, data_to_send[i]); //Write a byte to the fifo
	 }
	 while(GetBits16(SPI_CTRL_REG1, SPI_BUSY)){ //Wait for the transfers to finish		   
	 }
	 SetBits16(SPI_CTRL_REG1, SPI_FIFO_MODE, 0x3); //Reset the fifos 
	 #endif //USE_DMA
 }
 
 /**
 ****************************************************************************************
 * @brief SPI dma receive only, does not control cs line.
 * @param[in] data_buffer  Starting address of the data buffer
 * @param[in] length       Amount of bytes to receive
 * @return void
 ****************************************************************************************
 */ 
 
 uint8_t volatile filler; //Make sure this address does not get reused
 
 void spi_dma_receive(uint8_t* data_buffer, uint16_t length){
	 #if USE_DMA
	 length = length-1; //We will be handeling the first byte ourself
	 SetWord16(DMA0_A_STARTL_REG, (uint32_t) SPI_RX_TX_REG0 & 0xFFFF);              //Set the lower 16 bits of the source
	 SetWord16(DMA0_A_STARTH_REG, ((uint32_t) SPI_RX_TX_REG0 >> 16) & 0xFFFF);      //Set the upper 16 bits of the source
	 SetWord16(DMA0_B_STARTL_REG, ((uint32_t) data_buffer + 1) & 0xFFFF);           //Set the lower 16 bits of the destination
	 SetWord16(DMA0_B_STARTH_REG, (((uint32_t) data_buffer + 1) >> 16) & 0xFFFF);   //Set the upper 16 bits of the destination
	 SetWord16(DMA0_LEN_REG, length);                                               //Set the length of the transfer 
	 SetWord16(DMA0_INT_REG, length);                                               //Generate an interrupt after the transfer 
	 
	 filler = 0x00; //What to transmit as filler, usually 0x00 or 0xff
	 
	 SetWord16(DMA1_A_STARTL_REG, ((uint32_t) &filler) & 0xFFFF);                   //Set the lower 16 bits of the source
	 SetWord16(DMA1_A_STARTH_REG, (((uint32_t) &filler) >> 16) & 0xFFFF);           //Set the upper 16 bits of the source
	 SetWord16(DMA1_B_STARTL_REG, (uint32_t) SPI_RX_TX_REG0 & 0xFFFF);              //Set the lower 16 bits of the destination
	 SetWord16(DMA1_B_STARTH_REG, ((uint32_t) SPI_RX_TX_REG0 >> 16) & 0xFFFF);      //Set the upper 16 bits of the destination
	 SetWord16(DMA1_LEN_REG, length);                                               //Set the length of the transfer 
	 SetWord16(DMA1_INT_REG, length);                                               //Generate an interrupt after the transfer 
	 
	 SetBits16(DMA_REQ_MUX_REG, DMA01_SEL, 0x0); //Map the spi data req signals to dma0/1
	 
	 SetBits16(DMA0_CTRL_REG, AINC, 0);      //Disable source address incrementation
	 SetBits16(DMA0_CTRL_REG, BINC, 1);      //Enable destination address incrementation
	 SetBits16(DMA0_CTRL_REG, DREQ_MODE, 1); //Dma is triggered by data req
	 SetBits16(DMA0_CTRL_REG, BW, 0);        //Dma bus width is 8 bits
	 SetBits16(DMA0_CTRL_REG, DMA_ON, 1);    //Turn on dma channel
	 
	 SetBits16(DMA1_CTRL_REG, AINC, 0);      //Disable source address incrementation
	 SetBits16(DMA1_CTRL_REG, BINC, 0);      //Disable destination address incrementation
	 SetBits16(DMA1_CTRL_REG, DREQ_MODE, 1); //Dma is triggered by data req
	 SetBits16(DMA1_CTRL_REG, IRQ_ENABLE, 1);//Enable interrupt on channel 1
	 SetBits16(DMA1_CTRL_REG, BW, 0);        //Dma bus width is 8 bits
	 SetBits16(DMA1_CTRL_REG, DMA_ON, 1);    //Turn on dma channel
	 
	 NVIC_ClearPendingIRQ(DMA_IRQn); //Clear the interrupt for safety
	 NVIC_EnableIRQ(DMA_IRQn);       //Enable the interrupt
	 
	 data_buffer[0] = spi_access(filler); //Start transfer by receiving the first byte manually
	 #else
	 SetBits16(SPI_CTRL_REG1, SPI_FIFO_MODE, 0x0); //Use the fifos for increased speed
	 while(GetBits16(SPI_CTRL_REG, SPI_INT_BIT)){  //Clear the rx fifo, just in case
		 uint8_t temp = GetByte(SPI_RX_TX_REG0);
		 SetByte(SPI_CLEAR_INT_REG, 1); //Clear the spi int bit
	 }
	 uint16_t sentbytes = 0;
	 uint16_t receivedbytes = 0;
	 SetByte(SPI_CLEAR_INT_REG, 1); //Clear the spi int bit
	 
	 while(receivedbytes < length){
		 if((!GetBits16(SPI_CTRL_REG, SPI_TXH)) && sentbytes < length){ //If tx buffer is not full and we still have bytes to send
			 sentbytes++;
			 SetByte(SPI_RX_TX_REG0, 0); //Write a byte to the fifo
		 }
		 if(GetBits16(SPI_CTRL_REG, SPI_INT_BIT)){ //If data in rx buffer
			 data_buffer[receivedbytes++] = GetByte(SPI_RX_TX_REG0); //Receive a byte
			 SetByte(SPI_CLEAR_INT_REG, 1); //Clear the spi int bit
		 }
	 }
	 SetBits16(SPI_CTRL_REG1, SPI_FIFO_MODE, 0x3); //Reset the fifos
	 
	 #endif //USE_DMA
 }
 
 /**
 ****************************************************************************************
 * @brief SPI dma transmit/receive, does not control cs line.
 * @param[in] data_to_send Address of the first byte to send
 * @param[in] data_buffer  Starting address of the data buffer
 * @param[in] length       Amount of bytes to send/receive
 * @return void
 ****************************************************************************************
 */ 
 void spi_dma_tranceive(uint8_t* data_to_send, uint8_t* data_buffer, uint16_t length){
	 #if USE_DMA
	 length = length-1; //We will be handeling the first byte ourself
	 SetWord16(DMA0_A_STARTL_REG, (uint32_t) SPI_RX_TX_REG0 & 0xFFFF);               //Set the lower 16 bits of the source
	 SetWord16(DMA0_A_STARTH_REG, ((uint32_t) SPI_RX_TX_REG0 >> 16) & 0xFFFF);       //Set the upper 16 bits of the source
	 SetWord16(DMA0_B_STARTL_REG, ((uint32_t) data_buffer + 1) & 0xFFFF);            //Set the lower 16 bits of the destination
	 SetWord16(DMA0_B_STARTH_REG, (((uint32_t) data_buffer + 1) >> 16) & 0xFFFF);    //Set the upper 16 bits of the destination
	 SetWord16(DMA0_LEN_REG, length);                                                //Set the length of the transfer 
	 SetWord16(DMA0_INT_REG, length);                                                //Generate an interrupt after the transfer 
	 
	 SetWord16(DMA1_A_STARTL_REG, ((uint32_t) data_to_send)+ 1 & 0xFFFF);            //Set the lower 16 bits of the source
	 SetWord16(DMA1_A_STARTH_REG, ((((uint32_t) data_to_send) + 1) >> 16) & 0xFFFF); //Set the upper 16 bits of the source
	 SetWord16(DMA1_B_STARTL_REG, (uint32_t) SPI_RX_TX_REG0 & 0xFFFF);               //Set the lower 16 bits of the destination
	 SetWord16(DMA1_B_STARTH_REG, ((uint32_t) SPI_RX_TX_REG0 >> 16) & 0xFFFF);       //Set the upper 16 bits of the destination
	 SetWord16(DMA1_LEN_REG, length);                                                //Set the length of the transfer 
	 SetWord16(DMA1_INT_REG, length);                                                //Generate an interrupt after the transfer 
	 
	 SetBits16(DMA_REQ_MUX_REG, DMA01_SEL, 0x0); //Map the spi data req signals to dma0/1
	 
	 SetBits16(DMA0_CTRL_REG, AINC, 0);      //Disable source address incrementation
	 SetBits16(DMA0_CTRL_REG, BINC, 1);      //Enable destination address incrementation
	 SetBits16(DMA0_CTRL_REG, DREQ_MODE, 1); //Dma is triggered by data req
	 SetBits16(DMA0_CTRL_REG, BW, 0);        //Dma bus width is 8 bits
	 SetBits16(DMA0_CTRL_REG, DMA_ON, 1);    //Turn on dma channel
	 
	 SetBits16(DMA1_CTRL_REG, AINC, 1);      //Enable source address incrementation
	 SetBits16(DMA1_CTRL_REG, BINC, 0);      //Disable destination address incrementation
	 SetBits16(DMA1_CTRL_REG, DREQ_MODE, 1); //Dma is triggered by data req
	 SetBits16(DMA1_CTRL_REG, BW, 0);        //Dma bus width is 8 bits
	 SetBits16(DMA1_CTRL_REG, IRQ_ENABLE, 1);//Enable interrupt on channel 1
	 SetBits16(DMA1_CTRL_REG, DMA_ON, 1);    //Turn on dma channel
	 
	 NVIC_ClearPendingIRQ(DMA_IRQn); //Clear the interrupt for safety
	 NVIC_EnableIRQ(DMA_IRQn);       //Enable the interrupt
	 
	 data_buffer[0] = spi_access(data_to_send[0]); //Start transfer by tranceiving the first byte manually
	 #else
	 SetBits16(SPI_CTRL_REG1, SPI_FIFO_MODE, 0x0); //Use the fifos for increased speed
	 while(GetBits16(SPI_CTRL_REG, SPI_INT_BIT)){  //Clear the rx fifo, just in case
		 uint8_t temp = GetByte(SPI_RX_TX_REG0);
		 SetByte(SPI_CLEAR_INT_REG, 1); //Clear the spi int bit
	 }
	 uint16_t sentbytes = 0;
	 uint16_t receivedbytes = 0;
	 SetByte(SPI_CLEAR_INT_REG, 1); //Clear the spi int bit
	 
	 while(receivedbytes < length){
		 if((!GetBits16(SPI_CTRL_REG, SPI_TXH)) && sentbytes < length){ //If tx buffer is not full and we still have bytes to send
			 SetByte(SPI_RX_TX_REG0, data_to_send[sentbytes++]); //Write a byte to the fifo
		 }
		 if(GetBits16(SPI_CTRL_REG, SPI_INT_BIT)){ //If data in rx buffer
			 data_buffer[receivedbytes++] = GetByte(SPI_RX_TX_REG0); //Receive a byte
			 SetByte(SPI_CLEAR_INT_REG, 1); //Clear the spi int bit
		 }
	 }
	 SetBits16(SPI_CTRL_REG1, SPI_FIFO_MODE, 0x3); //Reset the fifos
	 
	 #endif //USE_DMA
 }
 /**
 ****************************************************************************************
 * @brief Wait for the DMA transfers to finish.
 * @return void
 ****************************************************************************************
 */ 
 void wait_for_dma(){
	 #if USE_DMA
	 while(GPIO_GetPinStatus(SPI_EN_GPIO_PORT, SPI_EN_GPIO_PIN) == false){//Idle while the cs pin is low 
																																				//This check is to make sure we do not resume prematurely
	   __wfi(); //Put the cpu in idle mode and wait for the dma interrupt
	 }
	 #else
	 spi_cs_high();
	 #endif //USE_DMA
 }
 
 /**
 ****************************************************************************************
 * @brief DMA interrupt handler.
 * @return void
 ****************************************************************************************
 */
 void DMA_Handler(void) //DMA interrupt handler
 {
	 NVIC_ClearPendingIRQ(DMA_IRQn);
	 NVIC_DisableIRQ(DMA_IRQn);
	 spi_cs_high();
 }
 
 /**
 ****************************************************************************************
 * @brief Small demo of the above functions, this function is called on startup.
 * @return void
 ****************************************************************************************
 */
 void demo_spi(){
	 uint16_t amount_of_bytes = NUMBYTES; // Amount of bytes to send as test
	 uint8_t data_to_send[NUMBYTES];
	 uint8_t receive_buffer[NUMBYTES];
	 for(int i = 0; i <= NUMBYTES; i++){// Fill the data array with incrementing numbers
		 data_to_send[i] = i;
	 }
	 //Send demo data array
	 arch_set_pxact_gpio(); //Place a software cursor in the Smartsnippets toolbox power analyser
	 spi_cs_low();
	 spi_dma_transmit(data_to_send, amount_of_bytes);
	 wait_for_dma(); //Wait for the transfer to finish
	 //
	 
	 //Read out some of the SPI flash
	 spi_cs_low();
	 spi_access(0x03); //Send read command
	 spi_dma_receive(receive_buffer, amount_of_bytes);
	 wait_for_dma(); //Wait for the transfer to finish
	 //
	 
	 for(int i = 0; i <= amount_of_bytes; i++){
		 data_to_send[i] = 0; //Clear data to send
	 }
	 
	 //Read out SPI flash with the tranceive command
	 data_to_send[0] = 0x03;   //Read command for spi flash
	 spi_cs_low();             //Make the slave select low
	 spi_dma_tranceive(data_to_send, receive_buffer, amount_of_bytes);
	 wait_for_dma(); //Wait for the transfer to finish
	 //
 }
