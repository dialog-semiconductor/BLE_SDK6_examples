/**
 ****************************************************************************************
 *
 * @file user_con_timeout.c
 *
 * @brief Source file containing the functionality to terminate a connection after a
 *				specific amount of time for a specific connection id. 
 *
 * Copyright (C) 2018 Dialog Semiconductor. This computer program or computer programs 
 * included in this package ("Software") include confidential, proprietary information 
 * of Dialog Semiconductor. All Rights Reserved.
 * 
 * THIS SOFTWARE IS AN UNOFFICIAL RELEASE FROM DIALOG SEMICONDUCTOR ('DIALOG') AND MAY
 * ONLY BE USED BY RECIPIENT AT ITS OWN RISK AND WITHOUT SUPPORT OF ANY KIND. THIS 
 * SOFTWARE IS SOLELY FOR USE ON AUTHORIZED DIALOG PRODUCTS AND PLATFORMS. RECIPIENT 
 * SHALL NOT TRANSMIT ANY SOFTWARE SOURCE CODE TO ANY THIRD PARTY WITHOUT DIALOG'S PRIOR 
 * WRITTEN PERMISSION.
 * 
 * UNLESS SET FORTH IN A SEPARATE AGREEMENT, RECIPIENT ACKNOWLEDGES AND UNDERSTANDS THAT 
 * TO THE FULLEST EXTENT PERMITTED BY LAW, THE SOFTWARE IS DELIVERED 'AS IS', WITHOUT 
 * WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT 
 * LIMITED TO, ANY IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR PURPOSE, 
 * MERCHANTABILITY, TITLE OR NON-INFRINGEMENT, AND ALL WARRANTIES THAT MAY ARISE FROM 
 * COURSE OF DEALING, CUSTOM OR USAGE IN TRADE. FOR THE SAKE OF CLARITY, DIALOG AND ITS
 * AFFILIATES AND ITS AND THEIR SUPPLIERS DO NOT WARRANT, GUARANTEE OR MAKE ANY 
 * REPRESENTATIONS (A) REGARDING THE USE, OR THE RESULTS OF THE USE, OF THE LICENSED 
 * SOFTWARE IN TERMS OF CORRECTNESS, COMPLETENESS, ACCURACY, RELIABILITY OR OTHERWISE, 
 * AND (B) THAT THE LICENSED SOFTWARE HAS BEEN TESTED FOR COMPLIANCE WITH ANY REGULATORY 
 * OR INDUSTRY STANDARD, INCLUDING, WITHOUT LIMITATION, ANY SUCH STANDARDS PROMULGATED 
 * BY THE FCC OR OTHER LIKE AGENCIES.
 * 
 * IN NO EVENT SHALL DIALOG BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 ****************************************************************************************
 */
 
 #include "user_con_timeout.h"
 #include "app.h"							// dependencency on disconnection functionality
 #include "app_easy_timer.h"	// dependencency on timer functionality
 #include "systick.h"
 
 #ifdef CFG_PRINTF
		#include "arch_console.h"
 #endif
 
static struct con_timeout_t
{
	uint8_t connection_id;
	uint8_t timer_id;
	uint32_t delay;
	bool active;
}con_timeout = {0,0,0, false};
 
static void user_con_timeout_cb(void);
static bool user_con_timeout_is_active(void);

void user_con_timeout_init(uint32_t delay, uint8_t connection_id)
{
		//Stop timer if one is running
	  if (user_con_timeout_is_active())
		{
				app_easy_timer_cancel(con_timeout.timer_id);
		}
		
		con_timeout.timer_id = app_easy_timer(delay, user_con_timeout_cb);
		con_timeout.active = true;
		con_timeout.connection_id = connection_id;
		con_timeout.delay = delay;
}

void user_con_timeout_cancel(void)
{
		if(user_con_timeout_is_active())
		{
				app_easy_timer_cancel(con_timeout.timer_id);
				con_timeout.active = false;
		}
}

void user_con_timeout_reset(void)
{
		if (user_con_timeout_is_active())
		{
				user_con_timeout_cancel();
				user_con_timeout_init(con_timeout.delay, con_timeout.connection_id);
		}		
}

static void user_con_timeout_cb(void)
{
#ifdef CFG_PRINTF
		arch_printf("Timer with id 0x%x expired.\n\r Disconnecting...", con_timeout.timer_id);
#endif	
		app_easy_gap_disconnect(con_timeout.connection_id);
		con_timeout.active = false;
}

static bool user_con_timeout_is_active(void)
{
		return (con_timeout.active && con_timeout.timer_id);
}

