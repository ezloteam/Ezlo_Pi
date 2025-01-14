/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_actions_list.h
 * @brief   These are MACROs for ezlopi-core-action operations
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
    EZLOPI_ACTION(NONE, "EZLOPI_ACTION_NONE")
    EZLOPI_ACTION(PREPARE, "EZLOPI_ACTION_PREPARE")
    EZLOPI_ACTION(INITIALIZE, "EZLOPI_ACTION_INITIALIZE")
    EZLOPI_ACTION(NOTIFY_50_MS, "EZLOPI_ACTION_NOTIFY_50_MS")
    EZLOPI_ACTION(NOTIFY_100_MS, "EZLOPI_ACTION_NOTIFY_100_MS")
    EZLOPI_ACTION(NOTIFY_200_MS, "EZLOPI_ACTION_NOTIFY_200_MS")
    EZLOPI_ACTION(NOTIFY_500_MS, "EZLOPI_ACTION_NOTIFY_500_MS")
    EZLOPI_ACTION(NOTIFY_1000_MS, "EZLOPI_ACTION_NOTIFY_1000_MS")
    EZLOPI_ACTION(NOTIFY_DEFAULT, "EZLOPI_ACTION_NOTIFY_DEFAULT") // default is of 1000 ms
    EZLOPI_ACTION(HUB_GET_ITEM, "EZLOPI_ACTION_HUB_GET_ITEM")
    EZLOPI_ACTION(HUB_GET_DEVICE, "EZLOPI_ACTION_HUB_GET_DEVICE")
    EZLOPI_ACTION(HUB_UPDATE, "EZLOPI_ACTION_HUB_GET_ITEM")
    EZLOPI_ACTION(GET_VALUE, "EZLOPI_ACTION_GET_VALUE")
    EZLOPI_ACTION(GET_EZLOPI_VALUE, "EZLOPI_ACTION_GET_EZLOPI_VALUE")
    EZLOPI_ACTION(SET_VALUE, "EZLOPI_ACTION_SET_VALUE")
    EZLOPI_ACTION(MAX, "EZLOPI_ACTION_MAX")
    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
