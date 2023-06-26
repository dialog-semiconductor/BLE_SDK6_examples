/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * The MIT License (MIT)
 * Copyright (c) 2021 Renesas Electronics Corporation. All rights reserved.
 * 
 * The MIT License (MIT)
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************************
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
#include "arch_console.h"
#include "syscntl.h"

void write_buffer_initialization(void *buffer, uint16_t len, uint8_t cell_size);
void i2c_slave_receive_transmit(void);

static volatile unsigned int counter = 0;
volatile uint16_t length = 0;

uint8_t transmit_buffer[16];
uint8_t receive_buffer[16];

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
	  printf_string(UART,"\r\n I2C Slave Demo ");
    printf_string(UART,"\r\n ---------------");
}

void printbufferdata(void) {

		printf_string(UART,"\r\nThis is what the slave received from the master\r\n");
	
	for (size_t i = 0; i < sizeof(receive_buffer); i++) {
			printf_byte(UART, *(receive_buffer + i));
			printf_string(UART, " ");
	}
	
  printf_string(UART,"\r\n");
	printf_string(UART,"\r\n");
	
	printf_string(UART,"\r\nThis is what the slave sent to the master\r\n");

	for (size_t i = 0; i < sizeof(transmit_buffer); i++) {
			printf_byte(UART, *(transmit_buffer + i));
			printf_string(UART, " ");
	}
  

  printf_string(UART,"\r\n");
	
}

//Transmit/Receive multiple bytes to I2C master asynchronously. 
void i2c_slave_receive_transmit(void) {
   
    printbanner();

	while (1) {
				
    i2c_cfg_t i2c_configuration;

    memset(&transmit_buffer, 0, sizeof(transmit_buffer));
    memset(&receive_buffer, 0, sizeof(transmit_buffer));
    memset(&i2c_configuration, 0, sizeof(i2c_configuration));
    i2c_configuration.speed = I2C_SPEED_MODE;
    i2c_configuration.mode = I2C_MODE_SLAVE;
    i2c_configuration.addr_mode = I2C_ADDRESS_MODE;
    i2c_configuration.address = I2C_TARGET_ADDRESS;
    i2c_configuration.clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET;
    i2c_configuration.clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET;
    i2c_configuration.clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET;
    i2c_configuration.clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET;

    write_buffer_initialization(transmit_buffer, sizeof(transmit_buffer), sizeof(transmit_buffer[0]));

    i2c_init(&i2c_configuration);
    
    counter = 0;
		
		//Receive multiple bytes from master 
    i2c_slave_receive_buffer_async(receive_buffer, sizeof(receive_buffer), read_cb, NULL);

    /* wait for write to complete */
    while (counter < 1)
    {
        __WFI();
    }

    //Transmit an array of data to a master device on the I2C bus
    i2c_slave_transmit_buffer_async(transmit_buffer, sizeof(transmit_buffer), write_cb, NULL);

    /* wait for read to complete */
    while (counter < 2)
    {
        __WFI();
    }

	printbufferdata();

	}		

}

void write_buffer_initialization(void *buffer, uint16_t len, uint8_t cell_size)
{
    uint8_t * ram1 = (uint8_t *) 0x07FC4000;
     /* Initialize table with values from memory */
    memcpy(transmit_buffer, ram1, len);

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
	  i2c_slave_receive_transmit();

    // should never reach here.
    GPIO_SetActive(LED_PORT, LED_PIN);
    while(1);
}