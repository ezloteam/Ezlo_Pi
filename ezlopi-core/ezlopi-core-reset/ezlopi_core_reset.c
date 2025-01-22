/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
 * @file    ezlopi_core_reset.c
 * @brief   perform some function on system-reset
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include <esp_system.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_service_webprov.h"
#include "ezlopi_service_ws_server.h"

#include "ezlopi_util_trace.h"
/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

void EZPI_core_reset_reboot(void)
{
#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    EZPI_service_web_provisioning_deinit();
#endif // CONFIG_EZPI_WEBSOCKET_CLIENT

#ifdef CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    EZPI_service_ws_server_stop();
#endif // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER

    esp_restart();
}

void EZPI_core_reset_factory_restore(void)
{
    ezlopi_error_t ret = EZPI_core_factory_info_v3_factory_reset();
    if (EZPI_SUCCESS == ret)
    {
        TRACE_I("FLASH RESET WAS DONE SUCCESSFULLY");
    }

    ret = EZPI_core_nvs_factory_reset();
    if (EZPI_SUCCESS == ret)
    {
        TRACE_I("NVS-RESET WAS DONE SUCCESSFULLY");
    }

    TRACE_S("factory reset done, rebooting now .............................................");
    vTaskDelay(2000 / portTICK_RATE_MS);
    EZPI_core_reset_reboot();
}
/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/