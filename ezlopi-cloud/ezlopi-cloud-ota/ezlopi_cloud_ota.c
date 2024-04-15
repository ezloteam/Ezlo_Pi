#include <string.h>
#include <ctype.h>
#include "cjext.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_util_version.h"

#include "ezlopi_cloud_data.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_ota.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_factory_info.h"

void firmware_update_start(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddNullToObject(cj_response, ezlopi_error_str);
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON* version = NULL;
    cJSON* source_urls = NULL;
    cJSON* params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (params)
    {
        version = cJSON_GetObjectItem(params, ezlopi_version_str);
        TRACE_D("OTA - version: %s", (version && version->valuestring) ? version->valuestring : ezlopi_null_str);

        source_urls = cJSON_GetObjectItem(params, ezlopi_urls_str);
        if (source_urls)
        {
            cJSON* firmware_url = cJSON_GetObjectItem(source_urls, ezlopi_firmware_str);
            TRACE_D("OTA - source: %s", (firmware_url && firmware_url->valuestring) ? firmware_url->valuestring : ezlopi_null_str);

            if (firmware_url)
            {
                ezlopi_ota_start(firmware_url);
            }

            #warning "Checksum logic is not provided in document, needs to find it and implement it!"
                // https://confluence.mios.com/display/EPD/EzloPI+Firmware+Update+Support+v.0
        }
        else
        {
            // firmware_send_firmware_query_to_nma_server();
            // send "cloud.firmware.info.get"
            ezlopi_event_group_set_event(EZLOPI_EVENT_OTA);
        }
    }
}

void firmware_info_get(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddNullToObject(cj_response, ezlopi_error_str);
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON* result = cJSON_GetObjectItem(cj_request, ezlopi_result_str);
    if (result)
    {
        cJSON* version = NULL;
        version = cJSON_GetObjectItem(result, ezlopi_version_str);
        if (version != NULL)
        {
            TRACE_S("version: %s", version->valuestring);
            TRACE_D("Upgrading to version: %s", (version && version->valuestring) ? version->valuestring : ezlopi_null_str);

            cJSON* source_urls = NULL;
            source_urls = cJSON_GetObjectItem(result, ezlopi_urls_str);
            if (source_urls)
            {
                cJSON* firmware_url = cJSON_GetObjectItem(source_urls, ezlopi_firmware_str);
                TRACE_D("OTA - source: %s", (source_urls && source_urls->valuestring) ? source_urls->valuestring : ezlopi_null_str);

                if (firmware_url)
                {
                    ezlopi_ota_start(firmware_url);
                }

                #warning "Checksum logic is not provided in document, needs to find it and implement it!"
                    // https://confluence.mios.com/display/EPD/EzloPI+Firmware+Update+Support+v.0
            }
            else
            {
                // send "cloud.firmware.info.get"
                // ezlopi_event_group_set_event(EZLOPI_EVENT_OTA);
            }
        }
    }
}

cJSON* firmware_send_firmware_query_to_nma_server(uint32_t message_count)
{
    cJSON* cj_request = cJSON_CreateObject();
    if (NULL != cj_request)
    {
        cJSON_AddStringToObject(cj_request, ezlopi_method_str, method_cloud_firmware_info_get);
        cJSON_AddNumberToObject(cj_request, ezlopi_id_str, message_count);
        cJSON* cj_params = cJSON_AddObjectToObject(cj_request, ezlopi_params_str);
        if (cj_params)
        {
            char firmware_version_str[20];
            snprintf(firmware_version_str, sizeof(firmware_version_str), "%s.%d", VERSION_STR, BUILD);
            cJSON_AddStringToObject(cj_params, ezlopi_firmware_version_str, firmware_version_str);

            const char* device_type = ezlopi_factory_info_v3_get_device_type();
            if (device_type)
            {
                if (isalpha(device_type[0]))
                {
                    cJSON_AddStringToObject(cj_params, ezlopi_firmware_type_str, device_type); //
                }
                else
                {
                    cJSON_AddStringToObject(cj_params, ezlopi_firmware_type_str, ezlopi_generic_str);
                }
            }
            else
            {
                cJSON_AddStringToObject(cj_params, ezlopi_firmware_type_str, ezlopi_generic_str);
            }

            cJSON_AddStringToObject(cj_params, ezlopi_firmware_hardware_str, CONFIG_IDF_TARGET);
        }
    }

    return cj_request;
}
