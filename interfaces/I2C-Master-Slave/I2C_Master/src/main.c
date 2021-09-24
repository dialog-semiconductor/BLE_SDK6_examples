/**
 *******************************************************************************
 *
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (‘DIALOG’)
 * AND MAY ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY
 * KIND. THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND
 * PLATFORMS. RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD
 * PARTY WITHOUT DIALOG’S PRIOR WRITTEN PERMISSION.
 *
 * TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED “AS IS”,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A
 * PARTICULAR PURPOSE, MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL
 * WARRANTIES THAT MAY ARISE FROM COURSE OF DEALING, CUSTOM OR USAGE IN TRADE.
 * FOR THE SAKE OF CLARITY, DIALOG AND ITS AFFILIATES AND ITS AND THEIR
 * SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY REPRESENTATIONS (A) REGARDING
 * THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED SOFTWARE IN TERMS OF
 * CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, AND (B) THAT
 * THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY OR
 * INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS
 * PROMULGATED BY THE FCC OR OTHER LIKE AGENCIES.
 *
 * IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 *******************************************************************************
 *
 * @file main.c
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "arch_system.h"
#include "user_periph_setup.h"
#include "i2c.h"
#include "gpio.h"
#include "uart.h"
#include "uart_utils.h"
#include "syscntl.h"

void write_buffer_initialization(void *buffer, uint16_t len, uint8_t cell_size);
void i2c_master_transmit_receive(void);


static volatile unsigned int counter = 0;

uint8_t write_buffer[16];
uint8_t read_buffer[16];

static void write_cb(void *cb_data, uint16_t len, bool success)
{
	counter++;
}

static void read_cb(void *cb_data, uint16_t len, bool success)
{
	counter++;
}

void printbanner(void) {

	printf_string(UART,"\r\n ---------------");
	printf_string(UART,"\r\n I2C Master Demo");
  printf_string(UART,"\r\n ---------------");

}

void printbufferdata(void) {

    printf_string(UART,"\r\nThis is what the master sent to the slave\r\n");
	
	for (size_t i = 0; i < sizeof(write_buffer); i++) {
			printf_byte(UART, *(write_buffer + i));
			printf_string(UART, " ");
	}

	printf_string(UART,"\r\n");
	printf_string(UART,"\r\n");
	
	printf_string(UART,"\r\nThis is what the master received from the slave\r\n");
	
	for (size_t i = 0; i < sizeof(read_buffer); i++) {
			printf_byte(UART, *(read_buffer + i));
			printf_string(UART, " ");
	}
	
    printf_string(UART,"\r\n");
}

void busy_wait(uint32_t delay) {
    for (volatile uint32_t i = 0; i < delay; i++);
}

//Write/Read multiple bytes to/From I2C slave asynchronously. 
void i2c_master_transmit_receive(void) {
   
    printbanner();

	while (1) {
				
    while(GPIO_GetPinStatus(KEY_PORT, KEY_PIN)) {
            __NOP();
        }
		
	// this is for the key debouncing,
    // not a clever way but this is just a demo.
    busy_wait(BUSY_WAIT_AMOUNT);
		
	i2c_cfg_t i2c_configuration;
			
    memset(&write_buffer, 0, sizeof(write_buffer));
    memset(&read_buffer, 0, sizeof(read_buffer));
    memset(&i2c_configuration, 0, sizeof(i2c_configuration));
    i2c_configuration.speed = I2C_SPEED_MODE;
    i2c_configuration.mode = I2C_MODE_MASTER;
    i2c_configuration.addr_mode = I2C_ADDRESS_MODE;
    i2c_configuration.address = I2C_TARGET_ADDRESS;
    i2c_configuration.clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET;
    i2c_configuration.clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET;
    i2c_configuration.clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET;
    i2c_configuration.clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET;
		i2c_configuration.tx_fifo_level     = 15;
		i2c_configuration.rx_fifo_level     = 15;
			
    //Initialisation	
	write_buffer_initialization(write_buffer, sizeof(write_buffer), sizeof(write_buffer[0]));
			
	i2c_init(&i2c_configuration);

    counter = 0;

     // Write data to slave
    i2c_master_transmit_buffer_async(write_buffer, sizeof(write_buffer), write_cb, NULL, I2C_FLAGS );

				
	/* wait for write to complete */
    while (counter < 1) {
        __WFI();
    }
				
		
    //wait some time from test to test for slave to become idle
    for (volatile int k = 0; k < 25000; k++);
    //wait some time from test to test for slave to become idle
    for (volatile int k = 0; k < 25000; k++);
		
    // Receive data from the slave
    i2c_master_receive_buffer_async(read_buffer, sizeof(read_buffer), read_cb, NULL, I2C_FLAGS);


    /* wait for read to complete */
    while (counter < 2) {
        __WFI();
    }
		
    //wait some time from test to test for slave to become idle
    for (volatile int k = 0; k < 25000; k++);
    //wait some time from test to test for slave to become idle
    for (volatile int k = 0; k < 25000; k++);
		
	//Print Data	
	printbufferdata();
				
	}		

}

void write_buffer_initialization(void *buffer, uint16_t len, uint8_t cell_size)
{
	uint8_t  *temp_buffer8 = (uint8_t *)buffer;
	uint16_t *temp_buffer16 = (uint16_t *)buffer;;

	uint8_t * ram1 = (uint8_t *) 0x07FC0000;
	/* Initialize table with values from memory */
	if (cell_size == 1)
	{
		memcpy(temp_buffer8, ram1, len);
	}

	if (cell_size == 2)
		{
			for (uint16_t i = 0 ; i < len; i++)
			{
				temp_buffer16[i] =  0x00FF & (*ram1);
				(ram1)++;
			}
		}


}

/**
 *******************************************************************************
 * @brief  main function
 * 
 *******************************************************************************
 */
int main (void) {
    system_init();
    periph_init();
	i2c_master_transmit_receive();

    // should never reach here.
    GPIO_SetActive(LED_PORT, LED_PIN);
    while(1);
}

