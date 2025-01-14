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
 * @file    ezlopi_cloud_status.c
 * @brief
 * @author
 * @version
 * @date
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "esp_system.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_processes.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_status.h"

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
void EZPI_CLOUD_status_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(__FUNCTION__, cj_response, ezlopi_id_str, cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(__FUNCTION__, cj_response, ezlopi_method_str, cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *processes_array = cJSON_AddArrayToObject(__FUNCTION__, cjson_result, "processes");
        if (processes_array)
        {
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
            ezlopi_core_get_processes_details(processes_array);
#endif
        }
        cJSON *cj_ram = cJSON_AddObjectToObject(__FUNCTION__, cjson_result, "ram");
        if (cj_ram)
        {
            double free_heap_size = (esp_get_free_heap_size() / 1024.0);
            cJSON *cj_available = cJSON_AddObjectToObject(__FUNCTION__, cj_ram, "available");
            if (cj_available)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_available, ezlopi_scale_str, "Kb");
                cJSON_AddNumberToObject(__FUNCTION__, cj_available, ezlopi_value_str, free_heap_size);
            }
            cJSON *cj_total_available_ram = cJSON_AddObjectToObject(__FUNCTION__, cj_ram, "total");
            if (cj_total_available_ram)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_total_available_ram, ezlopi_scale_str, "Kb");
                cJSON_AddNumberToObject(__FUNCTION__, cj_total_available_ram, ezlopi_value_str, 400);
            }
            float available_pct = (free_heap_size / 400.0) * 100.0;
            cJSON_AddNumberToObject(__FUNCTION__, cj_ram, "availablePct", available_pct);
        }
    }
}
/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
