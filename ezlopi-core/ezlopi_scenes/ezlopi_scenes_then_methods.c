#include "trace.h"
#include "ezlopi_http.h"
#include "ezlopi_devices.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_scenes_then_methods.h"

int ezlopi_scene_then_set_item_value(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    uint32_t item_id = 0;
    uint32_t value = 0;

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
                        cJSON_AddBoolToObject(cj_params, "value", curr_field->value.value_string);
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

//---------------------------------------------------------------------------------------
typedef struct s_ezlopi_scenes_then_methods_send_http
{
    char url[100]; //"https://ezlo.com/",
    esp_http_client_method_t method;
    char *host; //= "10.101.101.10";
    char *username;
    char *password;
    char content_type[20];
    char content[100];
    uint32_t port;                    //= 80;
    bool skip_cert_common_name_check; // bool
    cJSON *header;
    void *user_data;

} s_ezlopi_scenes_then_methods_send_http_t;

// helper functions
// static void __add_data_src_dest_array_to_object(cJSON *cj_method, char *array_name, s_data_source_n_target_object_t *data_list)
// {
//     cJSON *cj_data_source_n_target_list = cJSON_AddArrayToObject(cj_method, array_name);
//     if (cj_data_source_n_target_list)
//     {
//         uint32_t idx = 0;
//         while (data_list[idx].types || data_list[idx].field)
//         {
//             cJSON *cj_arr_object = cJSON_CreateObject();
//             if (cj_arr_object)
//             {
//                 cJSON_AddNumberToObject(cj_arr_object, "index", idx);
//                 if (data_list[idx].types)
//                 {
//                     cJSON_AddRawToObject(cj_arr_object, "types", data_list[idx].types);
//                 }
//                 if (data_list[idx].field)
//                 {
//                     cJSON_AddStringToObject(cj_arr_object, "field", data_list[idx].field);
//                 }
//                 if (!cJSON_AddItemToArray(cj_data_source_n_target_list, cj_arr_object))
//                 {
//                     cJSON_Delete(cj_arr_object);
//                 }
//             }
//             idx++;
//         }
//     }
// }

static void __http_request_api(s_ezlopi_scenes_then_methods_send_http_t *config)
{

    // POST / HTTP/1.1
    // Host: ezlo.com
    // Content-Type: text/plain
    // Content-Length: 13

    // Request check

    /* needed URI */
    // POST%20%2F%20HTTP%2F1.1%0AHost%3A%20ezlo.com%0AContent-Type%3A%20text%2Fplain%0AContent-Length%3A%2013%0A%0ARequest%20check

    char *tmp_cloud_server = ezlopi_factory_info_v2_get_cloud_server();
    char *tmp_ca_certificate = ezlopi_factory_info_v2_get_ca_certificate();
    char *tmp_ssl_shared_key = ezlopi_factory_info_v2_get_ssl_shared_key();
    char *tmp_ssl_private_key = ezlopi_factory_info_v2_get_ssl_private_key();

    esp_http_client_config_t http_config = {
        .url = config->url,
        .method = config->method,
        .cert_pem = tmp_ca_certificate,
        .client_cert_pem = tmp_ssl_shared_key,
        .client_key_pem = tmp_ssl_private_key,
        .skip_cert_common_name_check = config->skip_cert_common_name_check,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,

        .user_data = NULL,   // 'my_data' will be filled in 'ezlopi_http_event_handler'
        .timeout_ms = 30000, // 30sec
        .max_redirection_count = 10,
        .auth_type = HTTP_AUTH_TYPE_NONE,
    };

    TRACE_W("URI :- '%s' [%d]", config->url, strlen(config->url));
    TRACE_W("content_type : %s", config->content_type);
    TRACE_W("content : %s", config->content);
    TRACE_W("skip_cert : %s", (config->skip_cert_common_name_check) ? "true" : "false");

    esp_http_client_handle_t client = esp_http_client_init(&http_config);
    if (client)
    {
        switch (config->method)
        {
        case HTTP_METHOD_GET:
        {
            // GET

            esp_err_t err = esp_http_client_perform(client);
            if (err == ESP_OK)
            {
                TRACE_I("HTTP GET Status = %d, content_length = %d",
                        esp_http_client_get_status_code(client),
                        esp_http_client_get_content_length(client));
                while (!esp_http_client_is_complete_data_received(client))
                {
                    vTaskDelay(50 / portTICK_RATE_MS);
                }
            }
            else
            {
                TRACE_E("HTTP GET request failed: %d", (err));
            }

            // s_ezlopi_http_data_t *http_reply = NULL;
            // http_reply = ezlopi_http_get_request(config->url, tmp_ssl_private_key, tmp_ssl_shared_key, tmp_ca_certificate);
            // if (http_reply)
            // {
            //     TRACE_D("HTTP GET Status_resonse = %s, Status_code = %d",
            //             http_reply->response,
            //             http_reply->status_code);

            //     if (http_reply->response)
            //     {
            //         free(http_reply->response);
            //     }
            //     free(http_reply);
            // }
            break;
        }
        case HTTP_METHOD_POST:
        { // POST
          // const char *post_data = "{\"field1\":\"value1\"}";

            const char *post_data = "field1=value1&field2=value2";

            esp_http_client_set_method(client, HTTP_METHOD_POST);
            esp_http_client_set_post_field(client, post_data, strlen(post_data));

            esp_err_t err = esp_http_client_perform(client);
            if (err == ESP_OK)
            {
                TRACE_I("HTTP GET Status = %d, content_length = %d",
                        esp_http_client_get_status_code(client),
                        esp_http_client_get_content_length(client));
                while (!esp_http_client_is_complete_data_received(client))
                {
                    vTaskDelay(50 / portTICK_RATE_MS);
                }
            }
            else
            {
                TRACE_E("HTTP POST request failed: %s", esp_err_to_name(err));
            }

            // s_ezlopi_http_data_t *http_reply = NULL;
            // http_reply = ezlopi_http_post_request(config->url, NULL, NULL, tmp_ssl_private_key, tmp_ssl_shared_key, tmp_ca_certificate);
            // if (http_reply)
            // {
            //     TRACE_D("HTTP GET Status_resonse = %s, Status_code = %d",
            //             http_reply->response,
            //             http_reply->status_code);
            //     if (http_reply->response)
            //     {
            //         free(http_reply->response);
            //     }
            //     free(http_reply);
            // }

            break;
        }
#if 0
        case HTTP_METHOD_PUT:
        { // PUT
            esp_http_client_set_url(http_config, config->url);
            esp_http_client_set_method(http_config, HTTP_METHOD_PUT);
            err = esp_http_client_perform(http_config);
            if (err == ESP_OK)
            {
                TRACE_I("HTTP PUT Status = %d, content_length = %d",
                        esp_http_client_get_status_code(http_config),
                        esp_http_client_get_content_length(http_config));
            }
            else
            {
                TRACE_I("HTTP PUT request failed: %d", (err));
            }
            break;
        }
        case HTTP_METHOD_DELETE:
        { // DELETE
            esp_http_client_set_url(http_config, config->url);
            esp_http_client_set_method(http_config, HTTP_METHOD_DELETE);
            err = esp_http_client_perform(http_config);
            if (err == ESP_OK)
            {
                TRACE_I("HTTP DELETE Status = %d, content_length = %d",
                        esp_http_client_get_status_code(http_config),
                        esp_http_client_get_content_length(http_config));
            }
            else
            {
                TRACE_I("HTTP DELETE request failed: %d", (err));
            }
            break;
        }
#endif
        default:

            break;
        }

        esp_http_client_cleanup(client);
    }
}

int ezlopi_scene_then_send_http_request(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W("Warning: then-method not implemented!");
    // char http_request[100] = {'\0'};
    char request_line[80] = {'\0'};
    int ret = 0;

    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
    if (curr_then)
    {
        // configuration for http_request
        s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data = (s_ezlopi_scenes_then_methods_send_http_t *)malloc(sizeof(s_ezlopi_scenes_then_methods_send_http_t));
        if (tmp_http_data)
        {

            memset(tmp_http_data, 0, sizeof(s_ezlopi_scenes_then_methods_send_http_t));
            tmp_http_data->username = NULL;
            tmp_http_data->password = NULL;
            tmp_http_data->host = "192.168.01.10";
            tmp_http_data->port = 8080;

            l_fields_v2_t *curr_field = curr_then->fields;
            while (NULL != curr_field) // fields
            {
                // create a  requrest line
                if (0 == strncmp(curr_field->name, "request", strlen("request") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                    {
                        if (0 == strncmp(curr_field->value.value_string, "GET", 4))
                        {
                            tmp_http_data->method = HTTP_METHOD_GET;
                        }
                        if (0 == strncmp(curr_field->value.value_string, "POST", 5))
                        {
                            tmp_http_data->method = HTTP_METHOD_POST;
                        }
                        if (0 == strncmp(curr_field->value.value_string, "PUT", 4))
                        {
                            tmp_http_data->method = HTTP_METHOD_PUT;
                        }
                        if (0 == strncmp(curr_field->value.value_string, "DELETE", 7))
                        {
                            tmp_http_data->method = HTTP_METHOD_DELETE;
                        }

                        // TRACE_I("http_request : %s ", http_request);
                    }
                }
                else if (0 == strncmp(curr_field->name, "url", strlen("url") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                    {
                        memset(request_line, '\0', sizeof(request_line));
                        snprintf(request_line, strlen(curr_field->value.value_string) + 1, "%s", curr_field->value.value_string);
                        // snprintf(request_line, strlen(curr_field->value.value_string) + 1, " %s HTTP/1.1\r\n", curr_field->value.value_string);
                        TRACE_I("URI : '%s' [%d]", request_line, strlen(request_line));
                    }
                }
                // else if (0 == strncmp(curr_field->name, "credential", strlen("credential") + 1))
                // {
                // if (EZLOPI_VALUE_TYPE_CREDENTIAL == curr_field->value_type)
                //     {
                //         tmp_http_data->username = curr_field->value.value_credential.username;
                //         tmp_http_data->password = curr_field->value.value_credential.password;
                //         TRACE_W("->username : %s", tmp_http_data->username);
                //         TRACE_W("->password : %s", tmp_http_data->password);
                //     }
                // }
                else if (0 == strncmp(curr_field->name, "contentType", strlen("contentType") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                    {
                        snprintf(tmp_http_data->content_type, sizeof(tmp_http_data->content_type), "%s", curr_field->value.value_string);
                    }
                }
                else if (0 == strncmp(curr_field->name, "content", strlen("content") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                    {
                        snprintf(tmp_http_data->content, sizeof(tmp_http_data->content), "%s", curr_field->value.value_string);
                    }
                }
                else if (0 == strncmp(curr_field->name, "headers", strlen("headers") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_DICTIONARY == curr_field->value_type)
                    {
                        cJSON *headers = cJSON_Parse(curr_field->value.value_json);
                        while (headers)
                        {
                            /* code */
                        }
                    }
                }
                else if (0 == strncmp(curr_field->name, "skipSecurity", strlen("skipSecurity") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                    {
                        tmp_http_data->skip_cert_common_name_check = curr_field->value.value_bool;
                    }
                }

                curr_field = curr_field->next;
            }

            snprintf(tmp_http_data->url, sizeof(tmp_http_data->url), "%s", request_line);

            __http_request_api(tmp_http_data);

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
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reset_hub(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
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