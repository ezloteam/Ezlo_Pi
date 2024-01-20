#include "trace.h"
#include "string.h"

#include "ezlopi_nvs.h"
#include "ezlopi_http.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_scenes_scripts.h"
#include "ezlopi_scenes_expressions.h"
#include "ezlopi_scenes_then_methods.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_factory_info.h"
#include "web_provisioning.h"

//---------------------------------------------------------------------------------------
// #define
//---------------------------------------------------------------------------------------

int ezlopi_scene_then_set_item_value(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    uint32_t item_id = 0;
    // uint32_t value = 0;

    cJSON *cj_params = cJSON_CreateObject();

    if (cj_params)
    {
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                if (0 == strncmp(curr_field->name, "item", 4))
                {
                    cJSON_AddStringToObject(cj_params, "_id", curr_field->value.value_string);
                    item_id = strtoul(curr_field->value.value_string, NULL, 16);
                    // TRACE_D("item_id: %s", curr_field->value.value_string);
                }
                else if (0 == strncmp(curr_field->name, "value", 5))
                {
                    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                    {
                        cJSON_AddNumberToObject(cj_params, "value", curr_field->value.value_int);
                        // TRACE_D("value: %f", curr_field->value.value_double);
                    }
                    else if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                    {
                        cJSON_AddBoolToObject(cj_params, "value", curr_field->value.value_bool);
                        // TRACE_D("value: %s", curr_field->value.value_bool ? "true" : "false");
                    }
                    else if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                    {
                        cJSON_AddStringToObject(cj_params, "value", curr_field->value.value_string);
                        // TRACE_D("value: %s", curr_field->value.value_string);
                    }
                }

                curr_field = curr_field->next;
            }

            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
            uint32_t found_item = 0;
            while (curr_device)
            {
                l_ezlopi_item_t *curr_item = curr_device->items;
                while (curr_item)
                {
                    if (item_id == curr_item->cloud_properties.item_id)
                    {
                        curr_item->func(EZLOPI_ACTION_SET_VALUE, curr_item, cj_params, curr_item->user_arg);
                        found_item = 1;
                        break;
                    }
                    curr_item = curr_item->next;
                }
                if (found_item)
                {
                    TRACE_D("\r\n\r\n FOUND DEVICE \r\n\r\n");
                    break;
                }
                curr_device = curr_device->next;
            }
        }

        cJSON_Delete(cj_params);
    }

    return ret;
}
int ezlopi_scene_then_set_device_armed(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_send_cloud_abstract_command(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_switch_house_mode(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}

int ezlopi_scene_then_send_http_request(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
    if (curr_then)
    {
        s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data = (s_ezlopi_scenes_then_methods_send_http_t *)malloc(sizeof(s_ezlopi_scenes_then_methods_send_http_t));
        if (tmp_http_data)
        {
            memset(tmp_http_data, 0, sizeof(s_ezlopi_scenes_then_methods_send_http_t));
            l_fields_v2_t *curr_field = curr_then->fields;
            while (NULL != curr_field) // fields
            {
                if (0 == strncmp(curr_field->name, "request", 8))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                    {
                        if (0 == strncmp(curr_field->value.value_string, "GET", 4))
                        {
                            tmp_http_data->method = HTTP_METHOD_GET;
                        }
                        else if (0 == strncmp(curr_field->value.value_string, "POST", 5))
                        {
                            tmp_http_data->method = HTTP_METHOD_POST;
                        }
                        else if (0 == strncmp(curr_field->value.value_string, "PUT", 4))
                        {
                            tmp_http_data->method = HTTP_METHOD_PUT;
                        }
                        else if (0 == strncmp(curr_field->value.value_string, "DELETE", 7))
                        {
                            tmp_http_data->method = HTTP_METHOD_DELETE;
                        }
#warning "Some http-methods are still remaining" // Document mentions only four of them.
                    }
                }
                else if (0 == strncmp(curr_field->name, "url", 4))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                    {
                        ezlopi_http_scenes_then_parse_url(tmp_http_data, curr_field->value.value_string); // extracts : url, Host_name & Port_num.
                    }
                }
                else if (0 == strncmp(curr_field->name, "credential", 11))
                {
                    if ((EZLOPI_VALUE_TYPE_CREDENTIAL == curr_field->value_type) && (cJSON_IsObject(curr_field->value.value_json)))
                    {
                        ezlopi_http_scenes_then_parse_username_password(tmp_http_data, curr_field->value.value_json);
                    }
                }
                else if (0 == strncmp(curr_field->name, "contentType", 12))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                    {
                        ezlopi_http_scenes_then_parse_content_type(tmp_http_data, curr_field->value.value_string);
                    }
                }
                else if (0 == strncmp(curr_field->name, "content", 8))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                    {
                        ezlopi_http_scenes_then_parse_content(tmp_http_data, curr_field->value.value_string);
                    }
                }
                else if (0 == strncmp(curr_field->name, "headers", 8))
                {
                    if ((EZLOPI_VALUE_TYPE_DICTIONARY == curr_field->value_type) && (cJSON_IsObject(curr_field->value.value_json)))
                    {
                        ezlopi_http_scenes_then_parse_headers(tmp_http_data, curr_field->value.value_json);
                    }
                }
                else if (0 == strncmp(curr_field->name, "skipSecurity", 12))
                {
                    if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                    {
                        ezlopi_http_scenes_then_parse_skipsecurity(tmp_http_data, curr_field->value.value_bool);
                    }
                }
                curr_field = curr_field->next;
            }

            ezlopi_http_scenes_then_sendhttp_request(tmp_http_data);

            //-----------------------------------------------------------------
            if (tmp_http_data->url)
            {
                free(tmp_http_data->url);
                // TRACE_W("#url [%p] : %d ==> %s [%d]\n", tmp_http_data->url, (int)((tmp_http_data->url) ? strlen(tmp_http_data->url) : 0), (tmp_http_data->url), tmp_http_data->url_maxlen);
                tmp_http_data->url = NULL;
            }
            if (tmp_http_data->web_server)
            {
                free(tmp_http_data->web_server);
                // TRACE_W("#web_server [%p] : %d ==> %s [%d]\n", tmp_http_data->web_server, (int)((tmp_http_data->web_server) ? strlen(tmp_http_data->web_server) : 0), (tmp_http_data->web_server), tmp_http_data->web_server_maxlen);
                tmp_http_data->web_server = NULL;
            }
            if (tmp_http_data->header)
            {
                free(tmp_http_data->header);
                // TRACE_W("#header [%p] : %d ==> %s [%d]\n", tmp_http_data->header, (int)((tmp_http_data->header) ? strlen(tmp_http_data->header) : 0), (tmp_http_data->header), tmp_http_data->header_maxlen);
                tmp_http_data->header = NULL;
            }
            if (tmp_http_data->content)
            {
                free(tmp_http_data->content);
                // TRACE_W("#content [%p] : %d ==> %s [%d]\n", tmp_http_data->content, (int)((tmp_http_data->content) ? strlen(tmp_http_data->content) : 0), (tmp_http_data->content), tmp_http_data->content_maxlen);
                tmp_http_data->content = NULL;
            }
            if (tmp_http_data->username)
            {
                free(tmp_http_data->username);
                // TRACE_W("#username [%p] : %d ==> %s [%d]\n", tmp_http_data->username, (int)((tmp_http_data->username) ? strlen(tmp_http_data->username) : 0), (tmp_http_data->username), tmp_http_data->username_maxlen);
                tmp_http_data->username = NULL;
            }
            if (tmp_http_data->password)
            {
                free(tmp_http_data->password);
                // TRACE_W("#password [%p] : %d ==> %s [%d]\n", tmp_http_data->password, (int)((tmp_http_data->password) ? strlen(tmp_http_data->password) : 0), (tmp_http_data->password), tmp_http_data->password_maxlen);
                tmp_http_data->password = NULL;
            }

            free(tmp_http_data);
        }
    }

    return ret;
}

//---------------------------------------------------------------------------------------

int ezlopi_scene_then_run_custom_script(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_run_plugin_script(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_run_scene(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_set_scene_state(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reset_latch(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reset_scene_latches(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reboot_hub(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    cJSON *cj_params = cJSON_CreateObject();

    if (cj_params)
    {
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            TRACE_E("Rebooting ESP......................... ");
            web_provisioning_deinit();
            esp_restart();
        }

        cJSON_Delete(cj_params);
    }
    return ret;
}

int ezlopi_scene_then_reset_hub(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    cJSON *cj_params = cJSON_CreateObject();

    if (cj_params)
    {
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                if (0 == strncmp(curr_field->name, "type", 5))
                {
                    if (EZLOPI_VALUE_TYPE_ENUM == curr_field->value_type)
                    {
                        TRACE_I("value: %s", curr_field->value.value_string);
                        if (0 == strncmp(curr_field->value.value_string, "factory", 8))
                        {
                            TRACE_E("Factory Reseting ESP......................... ");
                            // clear the settings realated to scenes, devices, items, rooms,etc
                            ezlopi_scenes_scripts_factory_info_reset();
                            ezlopi_device_factory_info_reset();
                            ezlopi_nvs_factory_info_reset();  // 'nvs' partitions
                            ezlopi_factory_info_soft_reset(); // 'ID' partition :- 'wifi' sector
                            web_provisioning_deinit();
                            esp_restart();
                        }
                        if (0 == strncmp(curr_field->value.value_string, "soft", 5))
                        {

                            ezlopi_nvs_soft_reset();
                            ezlopi_factory_info_soft_reset(); // only affects wifi sector
                            TRACE_E("Rebooting ESP......................... ");
                            vTaskDelay(1000 / portTICK_PERIOD_MS);
                            web_provisioning_deinit();
                            esp_restart();
                        }
                    }
                }

                curr_field = curr_field->next;
            }
        }

        cJSON_Delete(cj_params);
    }
    return ret;
}

int ezlopi_scene_then_cloud_api(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_set_expression(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_set_variable(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_toggle_value(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}