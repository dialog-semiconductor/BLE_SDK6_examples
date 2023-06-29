/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation. All rights reserved.
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_periph_setup.h"
#include "datasheet.h"
#include "system_library.h"
#include "rwip_config.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "syscntl.h"
#include "spi.h"
#include "spi_531.h"

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
 
 /****************************************************************************************/
/* I2C LIS2DH configuration                                                             */
/****************************************************************************************/


#define I2C_SLAVE_ADDRESS 0x19                // Set slave device address
#define I2C_SPEED_MODE    I2C_SPEED_STANDARD  // Speed mode: I2C_SPEED_STANDARD (100 kbits/s), I2C_SPEED_FAST (400 kbits/s)
#define I2C_ADDRESS_MODE  I2C_ADDRESSING_7B   // Addressing mode: {I2C_ADDRESSING_7B, I2C_ADDRESSING_10B}
#define I2C_ADDRESS_SIZE  I2C_1BYTE_ADDR      // Address width: {I2C_1BYTE_ADDR, I2C_2BYTES_ADDR, I2C_3BYTES_ADDR}


#ifdef __ACCEL_IF_I2C__
/// Configuration struct for I2C
i2c_cfg_t i2c_cfg = {
    .clock_cfg.ss_hcnt = I2C_SS_SCL_HCNT_REG_RESET,
    .clock_cfg.ss_lcnt = I2C_SS_SCL_LCNT_REG_RESET,
    .clock_cfg.fs_hcnt = I2C_FS_SCL_HCNT_REG_RESET,
    .clock_cfg.fs_lcnt = I2C_FS_SCL_LCNT_REG_RESET,
    .restart_en = I2C_RESTART_ENABLE,
    .speed = I2C_SPEED_MODE,
    .mode = I2C_MODE_MASTER,
    .addr_mode = I2C_ADDRESS_MODE,
    .address = I2C_SLAVE_ADDRESS,
    .tx_fifo_level = 16,
    .rx_fifo_level = 16,
};

#elif defined(__ACCEL_IF_SPI__)

spi_cfg_t spi_cfg = 
{
    /// SPI master/slave mode
    .spi_ms = SPI_MS_MODE_MASTER,
    .spi_cp = SPI_CP_MODE_3,
    .spi_speed = SPI_SPEED_MODE_8MHz,
    .spi_wsz = SPI_MODE_8BIT,
    .spi_cs = SPI_CS_0,
    .cs_pad.port = SPI_PORT,
		.cs_pad.pin = SPI_CS_PIN,

#if defined (CFG_SPI_DMA_SUPPORT)

    /// SPI DMA Channel Pair Configuration
    .spi_dma_channel = SPI_DMA_CHANNEL_01,

    /// SPI DMA Priority Configuration
    .spi_dma_priority = DMA_PRIO_1,
#endif

    /// SPI Capture at current or next edge.
    .spi_capture = SPI_MASTER_EDGE_CAPTURE
};


#else
	#error "Please define i2c/spi interface
#endif


#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/

#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif
	
		RESERVE_GPIO(SPI_INT1, LIS2DH_INT1_PORT, LIS2DH_INT1_PIN, PID_GPIO);

#if defined(__ACCEL_IF_I2C__)
    RESERVE_GPIO(I2C_CLK, I2C_SCL_PORT, I2C_SCL_PIN, PID_I2C_SCL);
    RESERVE_GPIO(I2C_SDA, I2C_SDA_PORT, I2C_SDA_PIN, PID_SDA_SCL);
    RESERVE_GPIO(I2C_MODE, I2C_MODE_PORT, I2C_MODE_PIN, PID_GPIO);
    RESERVE_GPIO(I2C_ADDR, I2C_ADDR_PORT, I2C_ADDR_PIN, PID_GPIO);
#endif 
	
#if defined(__ACCEL_IF_SPI__)
		RESERVE_GPIO(SPI_CS, SPI_PORT, SPI_CS_PIN, PID_SPI_EN);
    RESERVE_GPIO(SPI_DI, SPI_PORT, SPI_DI_PIN, PID_SPI_DI);
		RESERVE_GPIO(SPI_DO, SPI_PORT, SPI_DO_PIN, PID_SPI_DO);
    RESERVE_GPIO(SPI_CLK, SPI_PORT, SPI_CLK_PIN, PID_SPI_CLK);
#endif 
}

#endif

void set_pad_functions(void)
{
/*
    i.e. to set P0_1 as Generic purpose Output:
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
*/

#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#endif

#if defined (CFG_PRINTF_UART2)
    // Configure UART2 TX Pad
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif

#if defined(__ACCEL_IF_I2C__)
		GPIO_ConfigurePin(I2C_SCL_PORT, I2C_SCL_PIN, INPUT, PID_I2C_SCL, false);
    GPIO_ConfigurePin(I2C_SDA_PORT, I2C_SDA_PIN, INPUT, PID_I2C_SDA, false);
		GPIO_ConfigurePin(I2C_MODE_PORT, I2C_MODE_PIN, OUTPUT, PID_GPIO, true);
    GPIO_ConfigurePin(I2C_ADDR_PORT, I2C_ADDR_PIN, OUTPUT, PID_GPIO, true);
#endif 
	
#if defined(__ACCEL_IF_SPI__)
		GPIO_ConfigurePin(SPI_PORT, SPI_CS_PIN, OUTPUT, PID_SPI_EN, false);
    GPIO_ConfigurePin(SPI_PORT, SPI_DI_PIN, INPUT, PID_SPI_DI, false);
		GPIO_ConfigurePin(SPI_PORT, SPI_DO_PIN, OUTPUT, PID_SPI_DO, false);
    GPIO_ConfigurePin(SPI_PORT, SPI_CLK_PIN, OUTPUT, PID_SPI_CLK, false);
#endif 
    
    GPIO_ConfigurePin(LIS2DH_INT1_PORT, LIS2DH_INT1_PIN, INPUT, PID_GPIO, true);

}

#if defined (CFG_PRINTF_UART2)
// Configuration struct for UART2
static const uart_cfg_t uart_cfg = {
    .baud_rate = UART2_BAUDRATE,
    .data_bits = UART2_DATABITS,
    .parity = UART2_PARITY,
    .stop_bits = UART2_STOPBITS,
    .auto_flow_control = UART2_AFCE,
    .use_fifo = UART2_FIFO,
    .tx_fifo_tr_lvl = UART2_TX_FIFO_LEVEL,
    .rx_fifo_tr_lvl = UART2_RX_FIFO_LEVEL,
    .intr_priority = 2,
};
#endif

void periph_init(void)
{
#if defined (__DA14531__)
    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#else
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    // ROM patch
    patch_func();

    // Initialize peripherals
#if defined (CFG_PRINTF_UART2)
    // Initialize UART2
    uart_initialize(UART2, &uart_cfg);
#endif

    // Set pad functionality
    set_pad_functions();

#if defined(__ACCEL_IF_I2C__)
		i2c_init(&i2c_cfg);
#endif 

#if defined(__ACCEL_IF_SPI__)
		spi_initialize(&spi_cfg);
#endif 
	

    // Enable the pads
    GPIO_set_pad_latch_en(true);
}