#include "ezlopi_cloud_ota.h"
#include "ezlopi_cloud_data.h"
#include "ezlopi_cloud_info.h"
#include "ezlopi_cloud_room.h"
#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_modes.h"
#include "ezlopi_cloud_scenes.h"
#include "ezlopi_cloud_devices.h"
#include "ezlopi_cloud_network.h"
#include "ezlopi_cloud_location.h"
#include "ezlopi_cloud_favorite.h"
#include "ezlopi_cloud_gateways.h"
#include "ezlopi_cloud_settings.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_coordinates.h"
#include "ezlopi_cloud_offline_login.h"
#include "ezlopi_cloud_modes_updaters.h"
#include "ezlopi_cloud_scenes_scripts.h"
#include "ezlopi_cloud_scenes_expressions.h"
#include "ezlopi_cloud_registration.h"
#include "ezlopi_cloud_status.h"
#include "ezlopi_cloud_log.h"
#include "ezlopi_cloud_setting_commands.h"
#include "ezlopi_cloud_hub_data_list.h"


#include "ezlopi_core_reset.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_api_methods.h"

static const s_method_list_v2_t method_list_v2[] = {
#define CLOUD_METHOD(name, _method, _updater) {.method_name = name, .method = _method, .updater = _updater},
#include "ezlopi_core_api_macros.h"
#undef CLOUD_METHOD
    {.method_name = NULL, .method = NULL, .updater = NULL},
};

f_method_func_t ezlopi_core_ezlopi_methods_get_by_id(uint32_t method_id)
{
    return method_list_v2[method_id].method;
}

f_method_func_t ezlopi_core_ezlopi_methods_get_updater_by_id(uint32_t method_id)
{
    return method_list_v2[method_id].updater;
}

char* ezlopi_core_ezlopi_methods_get_name_by_id(uint32_t method_id)
{
    return method_list_v2[method_id].method_name;
}

uint32_t ezlopi_core_ezlopi_methods_search_in_list(cJSON* cj_method)
{
    uint32_t found_method = 0;
    uint32_t idx = 0;

    if (cj_method && cj_method->valuestring)
    {
        while (method_list_v2[idx].method_name)
        {
            if (EZPI_STRNCMP_IF_EQUAL(cj_method->valuestring, method_list_v2[idx].method_name, strlen(cj_method->valuestring), strlen(method_list_v2[idx].method_name)))
            {
                found_method = 1;
                break;
            }

            idx++;
        }
    }

    return (found_method ? idx : UINT32_MAX);
}

void ezlopi_core_ezlopi_methods_rpc_method_notfound(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cjson_error = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_error_str);
    if (cjson_error)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cjson_error, ezlopi_code_str, -32602);
        cJSON_AddStringToObject(__FUNCTION__, cjson_error, ezlopi_data_str, ezlopi_rpc_method_notfound_str);
        cJSON_AddStringToObject(__FUNCTION__, cjson_error, ezlopi_message_str, ezlopi_Unknown_method_str);
    }

    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void ezlopi_core_ezlopi_methods_registration_init(void)
{
    registration_init();
}

bool ezlopi_core_elzlopi_methods_check_method_register(f_method_func_t method)
{
    bool ret = false;

    if (registered == method)
    {
        ret = true;
    }

    return ret;
}

void ezlopi_core_ezlopi_methods_reboot(cJSON* cj_request, cJSON* cj_response)
{
    EZPI_CORE_reset_reboot();
}
