#include <string.h>

#include "gateways.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_cloud.h"
#include "ezlopi_gateway.h"
#include "ezlopi_cloud_constants.h"

void gateways_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *cjson_gateways_array = cJSON_AddArrayToObject(cjson_result, "gateways");
        if (cjson_gateways_array)
        {

            cJSON *cjson_gateway = cJSON_CreateObject();
            if (cjson_gateway)
            {
                s_ezlopi_gateway_t *gateway_prop = ezlopi_gateway_get();
                if (gateway_prop)
                {
                    char *tmp_buffer[32];
                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", gateway_prop->_id);
                    // cJSON_AddNumberToObject(cjson_gateway, "_id", ezlopi_cloud_get_gateway_id());

                    cJSON_AddStringToObject(cjson_gateway, "_id", tmp_buffer);
                    cJSON_AddStringToObject(cjson_gateway, "addItemDictionaryValueCommand", gateway_prop->add_item_dictionary_value_command ? gateway_prop->add_item_dictionary_value_command : "");
                    cJSON_AddStringToObject(cjson_gateway, "checkDeviceCommand", gateway_prop->check_device_command ? gateway_prop->check_device_command : "");
                    cJSON_AddStringToObject(cjson_gateway, "clearItemDictionaryCommand", gateway_prop->clear_item_dictionary_command ? gateway_prop->clear_item_dictionary_command : "");
                    cJSON_AddStringToObject(cjson_gateway, "forceRemoveDeviceCommand", gateway_prop->force_remove_device_command ? gateway_prop->force_remove_device_command : "");
                    cJSON_AddStringToObject(cjson_gateway, "label", gateway_prop->label ? gateway_prop->label : "");
                    cJSON_AddStringToObject(cjson_gateway, "manualDeviceAdding", gateway_prop->manual_device_adding ? "yes" : "no");
                    cJSON_AddStringToObject(cjson_gateway, "name", gateway_prop->name ? gateway_prop->name : "");
                    cJSON_AddStringToObject(cjson_gateway, "pluginId", gateway_prop->pluginid ? gateway_prop->pluginid : "");
                    cJSON_AddBoolToObject(cjson_gateway, "ready", gateway_prop->ready ? true : false);
                    cJSON_AddStringToObject(cjson_gateway, "removeItemDictionaryValueCommand", gateway_prop->remove_item_dictionary_value_command ? gateway_prop->remove_item_dictionary_value_command : "");
                    cJSON_AddStringToObject(cjson_gateway, "resetSettingCommand", gateway_prop->reset_setting_command ? gateway_prop->reset_setting_command : "");
                    cJSON_AddStringToObject(cjson_gateway, "setGatewaySettingValueCommand", gateway_prop->set_gateway_setting_vlaue_command ? gateway_prop->set_gateway_setting_vlaue_command : "");
                    cJSON_AddStringToObject(cjson_gateway, "setItemDictionaryValueCommand", gateway_prop->set_item_dictionary_value_command ? gateway_prop->set_item_dictionary_value_command : "");
                    cJSON_AddStringToObject(cjson_gateway, "setItemValueCommand", gateway_prop->set_item_value_command ? gateway_prop->set_item_value_command : "");
                    cJSON_AddStringToObject(cjson_gateway, "setSettingDictionaryValueCommand", gateway_prop->set_setting_dictionary_value_command ? gateway_prop->set_setting_dictionary_value_command : "");
                    cJSON_AddStringToObject(cjson_gateway, "setSettingValueCommand", gateway_prop->set_setting_value_command ? gateway_prop->set_setting_value_command : "");
                    cJSON_AddNumberToObject(cjson_gateway, "settings", gateway_prop->settings);
                    cJSON_AddStringToObject(cjson_gateway, "status", gateway_prop->ready ? "ready" : "not ready");
                    cJSON_AddStringToObject(cjson_gateway, "updateDeviceFirmwareCommand", gateway_prop->update_device_firmware_command ? gateway_prop->update_device_firmware_command : "");
                }

                if (!cJSON_AddItemToArray(cjson_gateways_array, cjson_gateway))
                {
                    cJSON_Delete(cjson_gateway);
                }
            }
        }
    }
}
