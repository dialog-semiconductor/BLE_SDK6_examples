/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs included in this package ("Software") include confidential, proprietary information of Dialog Semiconductor. All Rights Reserved.
 *
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR (�DIALOG�) AND MAY ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND.  THIS SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS.  RECIPIENT SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG�S PRIOR WRITTEN PERMISSION.
 *
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED �AS IS�, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED BY THE FCC OR OTHER LIKE AGENCIES.
 *
 * IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration
#include "user_periph_setup.h"       // peripheral configuration
#include "gpio.h"
#include "uart.h"                    // UART initialization
#include "system_library.h"
#include "m_drv_interface.h"
#include "m_drv_mc36xx.h"
#include "arch_console.h"
#include "user_accelerometer.h"
#include "syscntl.h"

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
* Globally reserved GPIOs reservation
*/

/*
* Application specific GPIOs reservation. Used only in Development mode (#if DEVELOPMENT_DEBUG)

i.e.
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);    //Reserve P_01 as Generic Purpose I/O
*/

#ifdef CFG_PRINTF_UART2
    RESERVE_GPIO(UART2_TX, UART2_TX_GPIO_PORT, UART2_TX_GPIO_PIN, PID_UART2_TX);
    RESERVE_GPIO(UART2_RX, UART2_RX_GPIO_PORT, UART2_RX_GPIO_PIN, PID_UART2_RX);
#endif
}
#endif //DEVELOPMENT_DEBUG

void set_pad_functions(void)        // set gpio port function mode
{
/*
* Configure application ports.
i.e.
    GPIO_ConfigurePin( GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false ); // Set P_01 as Generic purpose Output
*/

#ifdef __DA14586__
    // disallow spontaneous flash wake-up
    GPIO_ConfigurePin(SPI_EN_GPIO_PORT, SPI_EN_GPIO_PIN, OUTPUT, PID_GPIO, true);
#endif

#ifdef CFG_PRINTF_UART2
    GPIO_ConfigurePin(UART2_TX_GPIO_PORT, UART2_TX_GPIO_PIN, OUTPUT, PID_UART2_TX, false);
    GPIO_ConfigurePin(UART2_RX_GPIO_PORT, UART2_RX_GPIO_PIN, INPUT, PID_UART2_RX, false);
#endif
}

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

    //rom patch
    patch_func();

    //Init pads
    set_pad_functions();

    // (Re)Initialize peripherals
    // i.e.
    //  uart_init(UART_BAUDRATE_115K2, UART_FRAC_BAUDRATE_115K2, UART_CHARFORMAT_8);

#ifdef CFG_PRINTF_UART2
    SetBits16(CLK_PER_REG, UART2_ENABLE, 1);
    uart2_init(UART_BAUDRATE_115K2, UART_FRAC_BAUDRATE_115K2, UART_CHARFORMAT_8);
#endif

   // Enable the pads
    SetBits16(SYS_CTRL_REG, PAD_LATCH_EN, 1);

    RESERVE_GPIO(,I2C_GPIO_PORT, I2C_SDA_PIN, PID_I2C_SDA);
    RESERVE_GPIO(,I2C_GPIO_PORT, I2C_SCL_PIN, PID_I2C_SCL);
    // Init I2C
    GPIO_ConfigurePin(I2C_GPIO_PORT, I2C_SCL_PIN, INPUT, PID_I2C_SCL, false);
    GPIO_ConfigurePin(I2C_GPIO_PORT, I2C_SDA_PIN, INPUT, PID_I2C_SDA, false);

    m_drv_i2c_init();

    M_DRV_MC36XX_Init();
#ifdef FIFO_DEMO
    M_DRV_MC36XX_EnableFIFO(E_M_DRV_MC36XX_FIFO_CTL_ENABLE, E_M_DRV_MC36XX_FIFO_MODE_WATERMARK, FIFO_THRESHOLD);
#endif
    M_DRV_MC36XX_SetMode(E_M_DRV_MC36XX_MODE_CWAKE);
}
