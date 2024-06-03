/**
 ****************************************************************************************
 *
 * @file user_modules_config.h
 *
 * @brief User modules configuration file.
 *
 * Copyright (C) 2015-2023 Renesas Electronics Corporation and/or its affiliates.
 * All rights reserved. Confidential Information.
 *
 * This software ("Software") is supplied by Renesas Electronics Corporation and/or its
 * affiliates ("Renesas"). Renesas grants you a personal, non-exclusive, non-transferable,
 * revocable, non-sub-licensable right and license to use the Software, solely if used in
 * or together with Renesas products. You may make copies of this Software, provided this
 * copyright notice and disclaimer ("Notice") is included in all such copies. Renesas
 * reserves the right to change or discontinue the Software at any time without notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS". RENESAS DISCLAIMS ALL WARRANTIES OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. TO THE
 * MAXIMUM EXTENT PERMITTED UNDER LAW, IN NO EVENT SHALL RENESAS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE, EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES. USE OF THIS SOFTWARE MAY BE SUBJECT TO TERMS AND CONDITIONS CONTAINED IN
 * AN ADDITIONAL AGREEMENT BETWEEN YOU AND RENESAS. IN CASE OF CONFLICT BETWEEN THE TERMS
 * OF THIS NOTICE AND ANY SUCH ADDITIONAL LICENSE AGREEMENT, THE TERMS OF THE AGREEMENT
 * SHALL TAKE PRECEDENCE. BY CONTINUING TO USE THIS SOFTWARE, YOU AGREE TO THE TERMS OF
 * THIS NOTICE.IF YOU DO NOT AGREE TO THESE TERMS, YOU ARE NOT PERMITTED TO USE THIS
 * SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef _USER_MODULES_CONFIG_H_
#define _USER_MODULES_CONFIG_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief User modules configuration.
 *
 * @{
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/***************************************************************************************/
/* Exclude or not a module in user's application code.                                 */
/*                                                                                     */
/* (0) - The module is included. The module's messages are handled by the SDK.         */
/*                                                                                     */
/* (1) - The module is excluded. The user must handle the module's messages.           */
/*                                                                                     */
/* Note:                                                                               */
/*      This setting has no effect if the respective module is a BLE Profile           */
/*      that is not used included in the user's application.                           */
/***************************************************************************************/
#define EXCLUDE_DLG_GAP             (0)
#define EXCLUDE_DLG_TIMER           (0)
#define EXCLUDE_DLG_MSG             (1)
#define EXCLUDE_DLG_SEC             (1)
#define EXCLUDE_DLG_DISS            (0)
#define EXCLUDE_DLG_PROXR           (1)
#define EXCLUDE_DLG_BASS            (1)
#define EXCLUDE_DLG_FINDL           (1)
#define EXCLUDE_DLG_FINDT           (1)
#define EXCLUDE_DLG_SUOTAR          (1)
#define EXCLUDE_DLG_CUSTS1          (0)
#define EXCLUDE_DLG_CUSTS2          (1)

/// @} APP

#endif // _USER_MODULES_CONFIG_H_
