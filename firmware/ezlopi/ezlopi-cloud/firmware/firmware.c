#include <string.h>
#include <ctype.h>

#include "cJSON.h"
#include "data.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"

#include "version.h"
#include "ezlopi_ota.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_event_group.h"

void firmware_update_start(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddNullToObject(cj_response, "error");
    cJSON_AddObjectToObject(cj_response, "result");

    cJSON *version = NULL;
    cJSON *source_urls = NULL;
    cJSON *params = cJSON_GetObjectItem(cj_request, "params");
    if (params)
    {
        version = cJSON_GetObjectItem(params, "version");
        TRACE_D("OTA - version: %s", (version && version->valuestring) ? version->valuestring : "null");

        source_urls = cJSON_GetObjectItem(params, "urls");
        if (source_urls)
        {
            cJSON *firmware_url = cJSON_GetObjectItem(source_urls, "firmware");
            TRACE_D("OTA - source: %s", (source_urls && source_urls->valuestring) ? source_urls->valuestring : "null");

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

void firmware_info_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddNullToObject(cj_response, "error");
    cJSON_AddObjectToObject(cj_response, "result");

    cJSON *params = cJSON_GetObjectItem(cj_request, "result");
    if (params)
    {
        cJSON *version = NULL;
        version = cJSON_GetObjectItem(params, "version");
        TRACE_D("Upgrading to version: %s", (version && version->valuestring) ? version->valuestring : "null");

        cJSON *source_urls = NULL;
        source_urls = cJSON_GetObjectItem(params, "urls");
        if (source_urls)
        {
            cJSON *firmware_url = cJSON_GetObjectItem(source_urls, "firmware");
            TRACE_D("OTA - source: %s", (source_urls && source_urls->valuestring) ? source_urls->valuestring : "null");

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
        }
    }
}

cJSON *firmware_send_firmware_query_to_nma_server(uint32_t message_count)
{
    cJSON *cj_request = cJSON_CreateObject();
    if (NULL != cj_request)
    {
        cJSON_AddStringToObject(cj_request, "method", "cloud.firmware.info.get");
        cJSON_AddNumberToObject(cj_request, "id", message_count);
        cJSON *cj_params = cJSON_AddObjectToObject(cj_request, "params");
        if (cj_params)
        {
            cJSON_AddStringToObject(cj_params, "firmware_version", VERSION_STR);

            char *device_type = ezlopi_factory_info_v2_get_device_type();
            if (device_type)
            {
                if (isalpha(device_type[0]))
                {
                    cJSON_AddStringToObject(cj_params, "firmware_type", device_type);
                }
                else
                {
                    cJSON_AddStringToObject(cj_params, "firmware_type", "generic");
                }
                free(device_type);
            }
            else
            {
                cJSON_AddStringToObject(cj_params, "firmware_type", "generic");
            }
            cJSON_AddStringToObject(cj_params, "firmware_hardware", "ezlopi");
        }

        char *str_request = cJSON_Print(cj_request);
        cJSON_Minify(str_request);
        if (str_request)
        {
            TRACE_I("firmware status request: \n%s", str_request);
            free(str_request);
        }
    }

    return cj_request;
}
