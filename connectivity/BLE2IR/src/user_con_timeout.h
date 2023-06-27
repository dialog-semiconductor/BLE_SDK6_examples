/**
 ****************************************************************************************
 *
 * @file user_con_timeout.h
 *
 * @brief API containing the functionality for connection timeout after (user)inactivity
 * 				to save power. This includes, initializing, starting, stopping and resetting
 *				the timeout. 
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
**/

#ifndef _USR_CON_TIMEOUT_H_
#define _USR_CON_TIMEOUT_H_

#include "stdint.h"

/**
 ****************************************************************************************
 * @brief 		Initialization procudure for the connection timeout. This must be called
 *						first if the other routines are to have any effect. If there is another 
 *						connection timeout present it will be overridden. 
 * @param[in]	uint32_t delay: The time (in 10ms) before a connection timeout will occur.
 *						uint_8t connection_id: id of the connection to be terminated. 
 * @return 		void
 ****************************************************************************************
*/	void user_con_timeout_init(uint32_t delay, uint8_t connection_id);


/**
 ****************************************************************************************
 * @brief 		Procedure to cancel the timer.
 ****************************************************************************************
*/	void user_con_timeout_cancel(void);

/**
 ****************************************************************************************
 * @brief 		Procedure to reset the timer. This can be used to postpone the time-out when 
 *						called after (user)activity. 
 * @return 		void
 ****************************************************************************************
*/	void user_con_timeout_reset(void);

#endif
