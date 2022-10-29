#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sdkconfig.h"
#include "items.h"
#include "trace.h"
#include "frozen.h"
#include "devices_common.h"
#include "interface_common.h"
#include "dht.h"
#include "mpu6050.h"
#include "web_provisioning.h"

#include "cJSON.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_devices_list.h"

extern float dht11_service_get_temperature(void);
static void parse_item_id(char *data, uint32_t len, char *item_id);

char *items_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    char *string_response = NULL;

    cJSON *cjson_request = cJSON_ParseWithLength(payload, len);
    if (cjson_request)
    {
        cJSON *id = cJSON_GetObjectItem(cjson_request, ezlopi_id_str);
        cJSON *sender = cJSON_GetObjectItem(cjson_request, ezlopi_sender_str);

        cJSON *cjson_response = cJSON_CreateObject();
        if (cjson_response)
        {
            cJSON_AddStringToObject(cjson_response, ezlopi_key_method_str, method_hub_items_list);
            cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, msg_count);
            cJSON_AddItemReferenceToObject(cjson_response, ezlopi_id_str, id);
            cJSON_AddItemReferenceToObject(cjson_response, ezlopi_sender_str, sender);
            cJSON_AddNullToObject(cjson_response, "error");

            cJSON *cjson_result = cJSON_CreateObject();
            if (cjson_result)
            {
                cJSON *cjson_items_array = cJSON_CreateArray();
                if (cjson_items_array)
                {

                    // s_ezlopi_device_t *ezlopi_device_list = ezlopi_devices_list_get_list();
                    // if (ezlopi_device_list)
                    // {
                    //     int dev_idx = 0;
                    l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
                    while (NULL != registered_device)
                    {
                        if (NULL != registered_device->properties)
                        {
                            cJSON *cjson_properties = cJSON_CreateObject();
                            if (cjson_properties)
                            {
                                char tmp_string[64];
                                snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.item_id);
                                cJSON_AddStringToObject(cjson_properties, "_id", tmp_string);
                                snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.device_id);
                                cJSON_AddStringToObject(cjson_properties, "deviceId", tmp_string);
                                cJSON_AddStringToObject(cjson_properties, "deviceName", registered_device->properties->ezlopi_cloud.device_name);
                                cJSON_AddTrueToObject(cjson_properties, "deviceArmed");
                                cJSON_AddBoolToObject(cjson_properties, "hasGetter", registered_device->properties->ezlopi_cloud.has_getter);
                                cJSON_AddBoolToObject(cjson_properties, "hasSetter", registered_device->properties->ezlopi_cloud.has_setter);
                                cJSON_AddStringToObject(cjson_properties, "name", registered_device->properties->ezlopi_cloud.item_name);
                                cJSON_AddTrueToObject(cjson_properties, "show");
                                cJSON_AddStringToObject(cjson_properties, "valueType", registered_device->properties->ezlopi_cloud.value_type);

                                registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cjson_properties);
                                cJSON_AddStringToObject(cjson_properties, "valueFormatted", "false");
                                cJSON_AddStringToObject(cjson_properties, "status", "synced");

                                if (!cJSON_AddItemToArray(cjson_items_array, cjson_properties))
                                {
                                    cJSON_Delete(cjson_properties);
                                }
                            }
                        }

                        registered_device = registered_device->next;

                        // dev_idx++;
                    }
                    // }

                    if (!cJSON_AddItemToObjectCS(cjson_result, "items", cjson_items_array))
                    {
                        cJSON_Delete(cjson_items_array);
                    }
                }

                if (!cJSON_AddItemToObjectCS(cjson_response, "result", cjson_result))
                {
                    cJSON_Delete(cjson_result);
                }
            }

            string_response = cJSON_Print(cjson_response);
            if (string_response)
            {
                printf("'hub.items.list' response:\r\n%s\r\n", string_response);
                cJSON_Minify(string_response);
            }

            cJSON_Delete(cjson_response);
        }

        cJSON_Delete(cjson_request);
    }

    return string_response;
}

char *items_set_value(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    char *string_response = NULL;
    cJSON *cjson_response = cJSON_CreateObject();

    cJSON *cjson_request = cJSON_ParseWithLength(payload, len);
    if (cjson_request)
    {
        cJSON *cjson_id = cJSON_GetObjectItem(cjson_request, ezlopi_id_str);
        cJSON *cjson_sender = cJSON_GetObjectItem(cjson_request, "sender");
        cJSON *cjson_params = cJSON_GetObjectItem(cjson_request, "params");
        cJSON *cjson_methods = cJSON_GetObjectItem(cjson_request, "method");

        if (cjson_params)
        {
            char *item_id_str = 0;
            CJSON_GET_VALUE_STRING(cjson_params, ezlopi__id_str, item_id_str);
            int item_id = strtol(item_id_str, NULL, 16);
            TRACE_I("item_id: %d | %X", item_id, item_id);

            // s_ezlopi_device_t *device_list = ezlopi_devices_list_get_list();
            // if (device_list)
            // {
            //     int idx = 0;
            l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
            while (NULL != registered_device)
            {
                if (registered_device->properties)
                {
                    if (item_id == registered_device->properties->ezlopi_cloud.item_id)
                    {
                        registered_device->device->func(EZLOPI_ACTION_SET_VALUE, registered_device->properties, cjson_params);
                    }
                }

                registered_device = registered_device->next;
            }
        }

        if (cjson_response)
        {
            cJSON_AddStringToObject(cjson_response, "method", method_hub_item_value_set);
            cJSON_AddNumberToObject(cjson_response, "msg_id", msg_count);
            cJSON_AddNullToObject(cjson_response, "error");
            cJSON_AddItemReferenceToObject(cjson_response, "id", cjson_id);
            cJSON_AddItemReferenceToObject(cjson_response, "sender", cjson_sender);
            cJSON_AddObjectToObject(cjson_response, "result");

            string_response = cJSON_Print(cjson_response);
            if (string_response)
            {
                cJSON_Minify(string_response);
                TRACE_I("hub.item.value.set - response: %s", string_response);
            }

            cJSON_Delete(cjson_response);
        }

        cJSON_Delete(cjson_request);
    }

    return string_response;
}

char *items_update(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    char *string_response = NULL;
    cJSON *cjson_request = cJSON_ParseWithLength(payload, len);
    if (cjson_request)
    {
        cJSON *cjson_id = cJSON_GetObjectItem(cjson_request, ezlopi_id_str);
        cJSON *cjson_sender = cJSON_GetObjectItem(cjson_request, "sender");
        cJSON *cjson_params = cJSON_GetObjectItem(cjson_request, "params");
        cJSON *cjson_method = cJSON_GetObjectItem(cjson_request, "method");
        char *item_id_str = 0;

        if (cjson_params)
        {
            CJSON_GET_VALUE_STRING(cjson_params, ezlopi__id_str, item_id_str);
            int item_id = strtol(item_id_str, NULL, 16);
            TRACE_I("item_id: %d | %X", item_id, item_id);
            l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();

            while (NULL != registered_device)
            {
                if (NULL != registered_device->properties)
                {
                    if (item_id == registered_device->properties->ezlopi_cloud.item_id)
                    {
                        cJSON *cjson_response = cJSON_CreateObject();
                        if (cjson_response)
                        {
                            cJSON_AddStringToObject(cjson_response, "msg_subclass", method_hub_item_updated);
                            cJSON_AddNumberToObject(cjson_response, "msg_id", msg_count);
                            cJSON_AddStringToObject(cjson_response, "id", "ui_broadcast");
                            cJSON *cjson_result = cJSON_AddObjectToObject(cjson_response, "result");
                            if (cjson_result)
                            {
                                cJSON_AddStringToObject(cjson_result, "_id", item_id_str);
                                char tmp_string[64];
                                snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.device_id);
                                cJSON_AddStringToObject(cjson_result, "deviceId", tmp_string);
                                cJSON_AddStringToObject(cjson_result, "deviceName", registered_device->properties->ezlopi_cloud.device_name);
                                cJSON_AddStringToObject(cjson_result, "deviceCategory", registered_device->properties->ezlopi_cloud.category);
                                cJSON_AddStringToObject(cjson_result, "deviceSubcategory", registered_device->properties->ezlopi_cloud.subcategory);
                                cJSON_AddStringToObject(cjson_result, "roomName", registered_device->properties->ezlopi_cloud.room_name);
                                cJSON_AddFalseToObject(cjson_result, "serviceNotification");
                                cJSON_AddFalseToObject(cjson_result, "userNotification");
                                cJSON_AddNullToObject(cjson_result, "notifications");
                                cJSON_AddStringToObject(cjson_result, "name", registered_device->properties->ezlopi_cloud.item_name);
                                registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cjson_result);
                                cJSON_AddStringToObject(cjson_result, "valueType", registered_device->properties->ezlopi_cloud.value_type);
                            }

                            string_response = cJSON_Print(cjson_response);
                            if (string_response)
                            {
                                TRACE_B("'hub.items.update' response: %s", string_response);
                                cJSON_Minify(string_response);
                            }

                            cJSON_Delete(cjson_response);
                        }

                        break;
                    }
                }

                registered_device = registered_device->next;
            }
        }

        cJSON_Delete(cjson_request);
    }

    return string_response;
}

#if 0
char *items_update(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    return items_update_with_device_index(payload, len, method, msg_count, UINT32_MAX);
}

char *items_update_from_sensor(int device_index, char *updated_value)
{
    uint32_t buf_len = 1024;
    char *send_buf = malloc(buf_len);

    if (send_buf)
    {
        if (device_index < MAX_DEV)
        {

            static const char *update_frmt = "{\"id\":\"ui_broadcast\",\"msg_id\":\"%d\",\"msg_subclass\":\method_hub_item_updated",\"result\":{\"_id\":\"%.*s\",\"deviceId\":\"%.*s\","
                                             "\"deviceName\":\"%.*s\",\"deviceCategory\":\"%.*s\",\"deviceSubcategory\":\"%.*s\",\"roomName\":\"%.*s\","
                                             "\"serviceNotification\":false,\"userNotification\":false,\"notifications\":null,\"name\":\"%.*s\",\"valueType\":\"%s\","
                                             "\"value\":%s}}";

            s_device_properties_t *dev_list = devices_common_device_list();

            snprintf(send_buf, buf_len, update_frmt, web_provisioning_get_message_count(),
                     sizeof(dev_list[device_index].item_id), dev_list[device_index].item_id,
                     sizeof(dev_list[device_index].device_id), dev_list[device_index].device_id,
                     sizeof(dev_list[device_index].name), dev_list[device_index].name,
                     sizeof(dev_list[device_index].category), dev_list[device_index].category,
                     sizeof(dev_list[device_index].subcategory), dev_list[device_index].subcategory,
                     sizeof(dev_list[device_index].roomName), dev_list[device_index].roomName,
                     sizeof(dev_list[device_index].item_name), dev_list[device_index].item_name,
                     dev_list[device_index].value_type, updated_value);
        }
        else
        {
            TRACE_E("Item not found in device list!");
            strcpy(send_buf, "{\"id\":\"_ID_\",\"error\":{\"code\":-32600,\"data\":\"rpc.params.empty._id\",\"description\":\"Missing item_id\"},\"result\":{}}");
        }
    }

    return send_buf;
}

char *items_update_with_device_index(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count, int device_index)
{
    uint32_t buf_len = 1024;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        int device_idx = 0;
        char item_id[20] = {'\0', '\0'};
        struct json_token value = JSON_INVALID_TOKEN;
        struct json_token params = JSON_INVALID_TOKEN;

        memset(send_buf, 0, buf_len);

        if (payload && len > 10)
        {
            json_scanf(payload, len, "{params:%T}", &params);
            json_scanf(params.ptr, params.len, "{value:%T}", &value);

            TRACE_W("params: %.*s", params.len, params.ptr);
            TRACE_W("value: %.*s", value.len, value.ptr);

            parse_item_id((char *)params.ptr, params.len, item_id);
            device_idx = devices_common_get_device_by_item_id(item_id);
        }
        else
        {
            device_idx = device_index;
        }

        static const char *update_frmt = "{\"id\":\"ui_broadcast\",\"msg_id\":\"%d\",\"msg_subclass\":\method_hub_item_updated",\"result\":{\"_id\":\"%.*s\",\"deviceId\":\"%.*s\","
                                         "\"deviceName\":\"%.*s\",\"deviceCategory\":\"%.*s\",\"deviceSubcategory\":\"%.*s\",\"roomName\":\"%.*s\","
                                         "\"serviceNotification\":false,\"userNotification\":false,\"notifications\":null,\"name\":\"%.*s\",\"valueType\":\"%s\","
                                         "\"value\":%s}}";

        TRACE_D("device idx: %d", device_idx);

        if (device_idx < MAX_DEV)
        {
            s_device_properties_t *dev_list = devices_common_device_list();

            char dev_value[100];
            memset(dev_value, 0, sizeof(dev_value));

            switch (dev_list[device_idx].dev_type)
            {
#warning "need to replace with id_item"
            case EZPI_DEV_TYPE_ONE_WIRE:
            {
                // snprintf(dev_value, sizeof(dev_value), "\"%.02f\",\"scale\":\"celsius\"", dht11_service_get_temperature());
                float val = dht11_service_get_temperature();
                snprintf(dev_value, sizeof(dev_value), "%.02f,\"valueFormatted\":\"%.02f\",\"scale\":\"celsius\",\"syncNotification\":false", val, val);
                break;
            }
            case EZPI_DEV_TYPE_I2C:
            {
                static int count = 0;
                switch (count)
                {
                    uint16_t val;
                case 0:
                    count++;
                    val = accel_x_value_read();
                    snprintf(dev_value, sizeof(dev_value), "%d,\"scale\":\"meter_per_second_square\",\"syncNotification\":false", val);
                    break;
                case 1:
                    count++;
                    val = accel_y_value_read();
                    snprintf(dev_value, sizeof(dev_value), "%d,\"scale\":\"meter_per_second_square\",\"syncNotification\":false", val);
                    break;
                case 2:
                    count = 0;
                    val = accel_z_value_read();
                    snprintf(dev_value, sizeof(dev_value), "%d,\"scale\":\"meter_per_second_square\",\"syncNotification\":false", val);
                    break;

                default:
                    break;
                }

                break;
            }
            case EZPI_DEV_TYPE_DIGITAL_OP:
            case EZPI_DEV_TYPE_DIGITAL_IP:
            {
                uint32_t current_state = interface_common_gpio_get_output_state(dev_list[device_idx].out_gpio);
                snprintf(dev_value, sizeof(dev_value), "%s", current_state ? "true" : "false");
                break;
            }
            case EZPI_DEV_TYPE_OTHER:
            {
                extern int hall_sensor_value_get(void);
                int current_state = hall_sensor_value_get();
                snprintf(dev_value, sizeof(dev_value), "%s", (current_state > 60 || current_state < 10) ? "\"dw_is_closed\"" : "\"dw_is_opened\"");
                break;
            }
            default:
            {
                snprintf(dev_value, sizeof(dev_value), "0");
                break;
            }
            }

            snprintf(send_buf, buf_len, update_frmt, msg_count,
                     sizeof(dev_list[device_idx].item_id), dev_list[device_idx].item_id,
                     sizeof(dev_list[device_idx].device_id), dev_list[device_idx].device_id,
                     sizeof(dev_list[device_idx].name), dev_list[device_idx].name,
                     sizeof(dev_list[device_idx].category), dev_list[device_idx].category,
                     sizeof(dev_list[device_idx].subcategory), dev_list[device_idx].subcategory,
                     sizeof(dev_list[device_idx].roomName), dev_list[device_idx].roomName,
                     sizeof(dev_list[device_idx].item_name), dev_list[device_idx].item_name,
                     dev_list[device_idx].value_type, dev_value);
            //  "142F6C69", "588b7eb528b12d03be86f36f", "SWITCH1",   "dining room", "switch",   "bool",       value ? "true" : "false");
            //   item_id,    device_id,                  device_name  room_name      item_name   valute_type  value
        }
        else
        {
            if ('\0' == item_id[0])
            {
                TRACE_E("Missing Item_id");
                strcpy(send_buf, "{\"id\":\"_ID_\",\"error\":{\"code\":-32600,\"data\":\"rpc.params.empty._id\",\"description\":\"Missing item_id\"},\"result\":{}}");
            }
            else
            {
                TRACE_E("Device Not found: device_idx: %d", device_idx);
            }
        }

        TRACE_B(">> WS Tx - method_hub_item_updated [%d]\r\n%s", strlen(send_buf), send_buf);
    }

    return send_buf;
}

static void parse_item_id(char *data, uint32_t len, char *item_id)
{
    char *param_ptr = data;
    char param_c = param_ptr[len];
    param_ptr[len] = '\0';
    const char *needle = "\"_id\":\"";
    char *_id_start = strstr(param_ptr, needle);
    _id_start += strlen(needle);
    char *_id_end = strchr(_id_start, '"');
    param_ptr[len] = param_c;

    if (_id_start && _id_end && (_id_end - _id_start))
    {
        snprintf(item_id, _id_end - _id_start + 2, "%.*s", _id_end - _id_start, _id_start);
        TRACE_D("## item_id: %s", item_id);
    }
    else
    {
        TRACE_E("item_id not found!");
    }
}
#endif