#include <string.h>

#include "gateways.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_cloud.h"
#include "ezlopi_cloud_constants.h"

void gateways_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
    if (cjson_result)
    {
        cJSON *cjson_gateways_array = cJSON_AddArrayToObject(cjson_result, "gateways");
        if (cjson_gateways_array)
        {

            cJSON *cjson_gateway = cJSON_CreateObject();
            if (cjson_gateway)
            {
                cJSON_AddNumberToObject(cjson_gateway, "_id", ezlopi_cloud_generate_gateway_id());
                cJSON_AddStringToObject(cjson_gateway, "addItemDictionaryValueCommand", "HUB:test_plugin/scripts/add_item_dictionary_value");
                cJSON_AddStringToObject(cjson_gateway, "checkDeviceCommand", "");
                cJSON_AddStringToObject(cjson_gateway, "clearItemDictionaryCommand", "");
                cJSON_AddStringToObject(cjson_gateway, "forceRemoveDeviceCommand", "");
                cJSON_AddStringToObject(cjson_gateway, "label", "virtual gateway");
                cJSON_AddStringToObject(cjson_gateway, "manualDeviceAdding", "no");
                cJSON_AddStringToObject(cjson_gateway, "name", "test_plugin");
                cJSON_AddStringToObject(cjson_gateway, "pluginId", "test_plugin");
                cJSON_AddTrueToObject(cjson_gateway, "ready");
                cJSON_AddStringToObject(cjson_gateway, "removeItemDictionaryValueCommand", "HUB:test_plugin/scripts/remove_item_dictionary_value");
                cJSON_AddStringToObject(cjson_gateway, "resetSettingCommand", "");
                cJSON_AddStringToObject(cjson_gateway, "setGatewaySettingValueCommand", "HUB:test_plugin/scripts/set_gateway_setting_value");
                cJSON_AddStringToObject(cjson_gateway, "setItemDictionaryValueCommand", "HUB:test_plugin/scripts/set_item_dictionary_value");
                cJSON_AddStringToObject(cjson_gateway, "setItemValueCommand", "HUB:test_plugin/scripts/set_item_value");
                cJSON_AddStringToObject(cjson_gateway, "setSettingDictionaryValueCommand", "");
                cJSON_AddStringToObject(cjson_gateway, "setSettingValueCommand", "HUB:test_plugin/scripts/set_setting_value");
                cJSON_AddNumberToObject(cjson_gateway, "settings", 3);
                cJSON_AddStringToObject(cjson_gateway, "status", "ready");
                cJSON_AddStringToObject(cjson_gateway, "updateDeviceFirmwareCommand", "");

                if (!cJSON_AddItemToArray(cjson_gateways_array, cjson_gateway))
                {
                    cJSON_Delete(cjson_gateway);
                }
            }
        }
    }
}
