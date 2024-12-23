

/**
 * @file    ezlopi_cloud_ota.c
 * @brief
 * @author
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
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

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_ENABLE_OTA

#include <string.h>
#include <ctype.h>

#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_util_version.h"

#include "ezlopi_core_ota.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_factory_info.h"

#include "ezlopi_cloud_data.h"
#include "ezlopi_cloud_constants.h"

void EZPI_firmware_update_start(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddNullToObject(__FUNCTION__, cj_response, ezlopi_error_str);
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *version = NULL;
    cJSON *source_urls = NULL;
    cJSON *params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (params)
    {
        version = cJSON_GetObjectItem(__FUNCTION__, params, ezlopi_version_str);
        TRACE_D("OTA - version: %s", (version && version->valuestring) ? version->valuestring : ezlopi_null_str);

        source_urls = cJSON_GetObjectItem(__FUNCTION__, params, ezlopi_urls_str);
        if (source_urls)
        {
            cJSON *firmware_url = cJSON_GetObjectItem(__FUNCTION__, source_urls, ezlopi_firmware_str);
            TRACE_D("OTA - source: %s", (firmware_url && firmware_url->valuestring) ? firmware_url->valuestring : ezlopi_null_str);

            if (firmware_url)
            {
                EZPI_core_ota_start(firmware_url);
            }

#warning "Checksum logic is not provided in document, needs to find it and implement it!"
            // https://confluence.mios.com/display/EPD/EzloPI+Firmware+Update+Support+v.0
        }
        else
        {
            // EZPI_firmware_send_firmware_query_to_nma_server();
            // send "cloud.firmware.info.get"
            EZPI_core_event_group_set_event(EZLOPI_EVENT_OTA);
        }
    }
}

void EZPI_firmware_info_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddNullToObject(__FUNCTION__, cj_response, ezlopi_error_str);
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *result = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_result_str);
    if (result)
    {
        cJSON *version = NULL;
        version = cJSON_GetObjectItem(__FUNCTION__, result, ezlopi_version_str);
        if (version != NULL)
        {
            TRACE_S("version: %s", version->valuestring);
            TRACE_D("Upgrading to version: %s", (version && version->valuestring) ? version->valuestring : ezlopi_null_str);

            cJSON *source_urls = NULL;
            source_urls = cJSON_GetObjectItem(__FUNCTION__, result, ezlopi_urls_str);
            if (source_urls)
            {
                cJSON *firmware_url = cJSON_GetObjectItem(__FUNCTION__, source_urls, ezlopi_firmware_str);
                TRACE_D("OTA - source: %s", (source_urls && source_urls->valuestring) ? source_urls->valuestring : ezlopi_null_str);

                if (firmware_url)
                {
                    EZPI_core_ota_start(firmware_url);
                }

#warning "Checksum logic is not provided in document, needs to find it and implement it!"
                // https://confluence.mios.com/display/EPD/EzloPI+Firmware+Update+Support+v.0
            }
            else
            {
                // send "cloud.firmware.info.get"
                // EZPI_core_event_group_set_event(EZLOPI_EVENT_OTA);
            }
            // # warning "since it deletes the cjson pointed by version itself ; thus creating redundant 'CJSON_delete' when exiting this funtion [EZPI_firmware_info_get]";
            // cJSON_Delete(__FUNCTION__, version);
        }
        // cJSON_Delete(__FUNCTION__, result);
    }
}

cJSON *EZPI_firmware_send_firmware_query_to_nma_server(uint32_t message_count)
{
    cJSON *cj_request = cJSON_CreateObject(__FUNCTION__);
    if (NULL != cj_request)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi_method_str, method_cloud_firmware_info_get);
        cJSON_AddNumberToObject(__FUNCTION__, cj_request, ezlopi_id_str, message_count);
        cJSON *cj_params = cJSON_AddObjectToObject(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            char firmware_version_str[20];
            snprintf(firmware_version_str, sizeof(firmware_version_str), "%s.%d", VERSION_STR, BUILD);
            cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_firmware_version_str, firmware_version_str);

            const char *device_type = EZPI_core_factory_info_v3_get_device_type();
            if (device_type)
            {
                if (isalpha(device_type[0]))
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_firmware_type_str, device_type); //
                }
                else
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_firmware_type_str, ezlopi_generic_str);
                }
            }
            else
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_firmware_type_str, ezlopi_generic_str);
            }

            cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_firmware_hardware_str, CONFIG_IDF_TARGET);
        }
    }

    return cj_request;
}

#endif // CONFIG_EZPI_ENABLE_OTA

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
