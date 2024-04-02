#include <string.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_gateway.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_cloud_gateways.h"
#include "gateways_commands_str.h"

void gateways_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *cjson_gateways_array = cJSON_AddArrayToObject(cjson_result, ezlopi_gateways_str);
        if (cjson_gateways_array)
        {

            cJSON *cjson_gateway = cJSON_CreateObject();
            if (cjson_gateway)
            {
                s_ezlopi_gateway_t *gateway_prop = ezlopi_gateway_get();
                if (gateway_prop)
                {
                    char tmp_buffer[32];
                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", gateway_prop->_id);

                    cJSON_AddStringToObject(cjson_gateway, ezlopi__id_str, tmp_buffer);
                    cJSON_AddStringToObject(cjson_gateway, gw_addItemDictionaryValueCommand_str, gateway_prop->add_item_dictionary_value_command ? gateway_prop->add_item_dictionary_value_command : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_checkDeviceCommand_str, gateway_prop->check_device_command ? gateway_prop->check_device_command : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_clearItemDictionaryCommand_str, gateway_prop->clear_item_dictionary_command ? gateway_prop->clear_item_dictionary_command : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_forceRemoveDeviceCommand_str, gateway_prop->force_remove_device_command ? gateway_prop->force_remove_device_command : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, ezlopi_label_str, gateway_prop->label ? gateway_prop->label : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_manualDeviceAdding_str, gateway_prop->manual_device_adding ? ezlopi_yes_str : ezlopi_no_str);
                    cJSON_AddStringToObject(cjson_gateway, ezlopi_name_str, gateway_prop->name ? gateway_prop->name : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_pluginId_str, gateway_prop->pluginid ? gateway_prop->pluginid : ezlopi__str);
                    cJSON_AddBoolToObject(cjson_gateway, ezlopi_ready_str, gateway_prop->ready ? true : false);
                    cJSON_AddStringToObject(cjson_gateway, gw_removeItemDictionaryValueCommand_str, gateway_prop->remove_item_dictionary_value_command ? gateway_prop->remove_item_dictionary_value_command : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_resetSettingCommand_str, gateway_prop->reset_setting_command ? gateway_prop->reset_setting_command : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_setGatewaySettingValueCommand_str, gateway_prop->set_gateway_setting_vlaue_command ? gateway_prop->set_gateway_setting_vlaue_command : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_setItemDictionaryValueCommand_str, gateway_prop->set_item_dictionary_value_command ? gateway_prop->set_item_dictionary_value_command : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_setItemValueCommand_str, gateway_prop->set_item_value_command ? gateway_prop->set_item_value_command : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_setSettingDictionaryValueCommand_str, gateway_prop->set_setting_dictionary_value_command ? gateway_prop->set_setting_dictionary_value_command : ezlopi__str);
                    cJSON_AddStringToObject(cjson_gateway, gw_setSettingValueCommand_str, gateway_prop->set_setting_value_command ? gateway_prop->set_setting_value_command : ezlopi__str);
                    cJSON_AddNumberToObject(cjson_gateway, ezlopi_settings_str, gateway_prop->settings);
                    cJSON_AddStringToObject(cjson_gateway, ezlopi_status_str, gateway_prop->ready ? ezlopi_ready_str : ezlopi_not_ready_str);
                    cJSON_AddStringToObject(cjson_gateway, gw_updateDeviceFirmwareCommand_str, gateway_prop->update_device_firmware_command ? gateway_prop->update_device_firmware_command : ezlopi__str);
                }

                if (!cJSON_AddItemToArray(cjson_gateways_array, cjson_gateway))
                {
                    cJSON_Delete(cjson_gateway);
                }
            }
        }
    }
}
