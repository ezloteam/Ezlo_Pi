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

static int _parse_web_host_name(char *dest_addr, int dest_limit, char *src_addr)
{
    int ret = 0;
    char *start = strstr(src_addr, "://");
    if (start != NULL)
    {
        start += 3;
        int buf_size = dest_limit;
        int length = 0;
        char *end = strchr(start, '/');
        if (end != NULL)
        {
            length = end - start;
            if ((length + 1) < buf_size)
            {
                snprintf(dest_addr, length + 1, "%s", start);
            }
        }
        else
        {
            char *ptr = src_addr;
            length = strlen(src_addr) - (int)(start - ptr);
            if ((length + 1) < buf_size)
            {
                snprintf(dest_addr, length + 1, "%s", (ptr + ((int)(start - ptr))));
            }
        }
        dest_addr[buf_size] = '\0';
        ret = 1;
    }
    else
    {
        TRACE_E("Cannot find web_server/host_name");
    }
    return ret;
}
int ezlopi_scene_then_send_http_request(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
    if (curr_then)
    {
        // configuration for http_request
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
#warning "Some http-methods are still remaining"
                    }
                }
                else if (0 == strncmp(curr_field->name, "url", 4))
                {
#warning "add url parser in ezlopi_core.ezlopi_http"
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                    {
                        snprintf(tmp_http_data->url, sizeof(tmp_http_data->url), "%s", curr_field->value.value_string);
#warning "web_port -> use number instead of string"
                        snprintf(tmp_http_data->web_port, sizeof(tmp_http_data->web_port), "%s", (NULL != strstr(curr_field->value.value_string, "https")) ? "443" : "80");

                        _parse_web_host_name(tmp_http_data->web_server, sizeof(tmp_http_data->web_server), curr_field->value.value_string);

// 1. adding 'User-Agent & host' to header-buffer
#warning "name 'limit' sounds incomplete"
                        int max_allowed_len = ezlopi_http_limit_size_check(tmp_http_data->header, sizeof(tmp_http_data->header), (6 + (strlen(tmp_http_data->web_server) + 1)) + 3);
                        if (max_allowed_len > 0)
                        {
                            snprintf((tmp_http_data->header) + (strlen(tmp_http_data->header)),
                                     max_allowed_len,
                                     "Host: %s\r\n",
                                     tmp_http_data->web_server);
                        }
                    }
                }
                else if (0 == strncmp(curr_field->name, "credential", 11))
                {
                    if (EZLOPI_VALUE_TYPE_CREDENTIAL == curr_field->value_type)
                    {
                        if (NULL != curr_field->value.value_json)
                        {
                            char *cj_ptr = cJSON_Print(curr_field->value.value_json);
                            if (cj_ptr)
                            {
                                // TRACE_W("-user/pass sent:-\n%s\n", cj_ptr);
                                cJSON_free(cj_ptr);

                                cJSON *userItem = cJSON_GetObjectItem(curr_field->value.value_json, "user");
                                cJSON *passwordItem = cJSON_GetObjectItem(curr_field->value.value_json, "password");
                                if ((userItem != NULL) && (passwordItem != NULL))
                                {
                                    const char *userValue = cJSON_GetStringValue(userItem);
                                    const char *passValue = cJSON_GetStringValue(passwordItem);
                                    snprintf(tmp_http_data->username, sizeof(tmp_http_data->username), "%s", userValue);
                                    snprintf(tmp_http_data->password, sizeof(tmp_http_data->password), "%s", passValue);
                                }
                            }
                            else
                            {
                                TRACE_E("Missing 'username' or 'password' field in credential");
                            }
                        }
                    }
                }
                else if (0 == strncmp(curr_field->name, "contentType", 12))
                {
#warning "place it in ezlopi_core.ezlopi_http"
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                    {
                        // cJSON_AddStringToObject(cj_header, "Content-Type", curr_field->value.value_string);
                        // 2. adding 'Content-Type' to header-buffer
                        int max_allowed_len = ezlopi_http_limit_size_check(tmp_http_data->header, sizeof(tmp_http_data->header), (14 + strlen(curr_field->value.value_string)) + 3);
                        if (max_allowed_len > 0)
                        {
                            snprintf((tmp_http_data->header) + (strlen(tmp_http_data->header)),
                                     max_allowed_len,
                                     "Content-Type: %s\r\n",
                                     curr_field->value.value_string);
                        }
                    }
                }
                else if (0 == strncmp(curr_field->name, "content", 8))
                {
#warning "place it in ezlopi_core.ezlopi_http"
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                    {
                        snprintf(tmp_http_data->content, sizeof(tmp_http_data->content), "%s\r\n", curr_field->value.value_string);
                        uint32_t i = 0; // variable to store 'content-length'
                        for (; i < strlen(curr_field->value.value_string); i++)
                        {
                            if ('\0' == tmp_http_data->content[i])
                                break;
                        }
                        if (i > 0)
                        {
                            char str[i];
                            snprintf(str, sizeof(str), "%d", i);
                            // cJSON_AddStringToObject(cj_header, "Content-Length", str);
                            // 3. adding 'Content-Length' to header-buffer
                            int max_allowed_len = ezlopi_http_limit_size_check(tmp_http_data->header, sizeof(tmp_http_data->header), (16 + strlen(str)) + 3);
                            if (max_allowed_len > 0)
                            {
                                snprintf((tmp_http_data->header) + strlen(tmp_http_data->header),
                                         max_allowed_len,
                                         "Content-Length: %s\r\n",
                                         str);
                            }
                        }
                    }
                }
                else if (0 == strncmp(curr_field->name, "headers", 8))
                {
#warning "place it in ezlopi_core.ezlopi_http"
                    if (EZLOPI_VALUE_TYPE_DICTIONARY == curr_field->value_type)
                    {
                        if (NULL != curr_field->value.value_json)
                        {
                            int max_allowed_len = 0;
                            cJSON *header = (curr_field->value.value_json->child);
                            while (header)
                            {
                                // cJSON_AddStringToObject(cj_header, header->string, header->valuestring);
                                // 4. adding 'remaining' to header-buffer
                                max_allowed_len = ezlopi_http_limit_size_check(tmp_http_data->header, sizeof(tmp_http_data->header), ((strlen(header->string) + 1) + 2 + (strlen(header->valuestring) + 1)) + 3);
                                if (max_allowed_len > 0)
                                {
                                    snprintf((tmp_http_data->header) + (strlen(tmp_http_data->header)),
                                             max_allowed_len,
                                             "%s: %s\r\n",
                                             header->string, header->valuestring);
                                }
                                header = header->next;
                            }
                        }
                    }
                }
                else if (0 == strncmp(curr_field->name, "skipSecurity", 12))
                {
                    if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                    {
                        tmp_http_data->skip_cert_common_name_check = curr_field->value.value_bool;
                        // 4. adding 'remaining' to header-buffer
                        int max_allowed_len = ezlopi_http_limit_size_check(tmp_http_data->header, sizeof(tmp_http_data->header), (14 + (strlen((curr_field->value.value_bool) ? "true" : "false"))) + 3);
                        if (max_allowed_len > 0)
                        {
                            snprintf((tmp_http_data->header) + strlen(tmp_http_data->header),
                                     max_allowed_len,
                                     "skipSecurity: %s\r\n",
                                     ((curr_field->value.value_bool) ? "true" : "false"));
                        }
                    }
                }
                curr_field = curr_field->next;
            }
            // function to add the credential field in url or content-body
            if ((0 < strlen(tmp_http_data->username)) && (0 < strlen(tmp_http_data->password)))
            {
                char cred[96] = {'\0'};
                if (HTTP_METHOD_GET == tmp_http_data->method)
                {
                    snprintf(cred, sizeof(cred), "?username=%s&password=%s", tmp_http_data->username, tmp_http_data->password);
#warning "its not how strncat should be used, fiind the better way"
                    strncat(tmp_http_data->url, cred, strlen(cred));
                }
                else
                {
                    snprintf(cred, sizeof(cred), "user:%s\r\npassword:%s\r\n", tmp_http_data->username, tmp_http_data->password);
#warning "its not how strncat should be used, fiind the better way"
                    strncat(tmp_http_data->content, cred, strlen(cred));
                }
            }
            ezlopi_http_scenes_sendhttp_request_api(tmp_http_data, NULL);
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
            // stop web_provisioning
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
                            // stop web_provisioning
                            esp_restart();
                        }
                        if (0 == strncmp(curr_field->value.value_string, "soft", 5))
                        {

                            ezlopi_nvs_soft_reset();
                            ezlopi_factory_info_soft_reset(); // only affects wifi sector
                            TRACE_E("Rebooting ESP......................... ");
                            vTaskDelay(1000 / portTICK_PERIOD_MS);
                            // stop web_provisioning
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