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
// typedef enum e_http_methods
// {
//     HTTP_METHOD_GET = 0, /*!< HTTP GET Method */
//     HTTP_METHOD_POST,    /*!< HTTP POST Method */
//     HTTP_METHOD_PUT,     /*!< HTTP PUT Method */
//     HTTP_METHOD_PATCH,   /*!< HTTP PATCH Method */
//     HTTP_METHOD_DELETE,  /*!< HTTP DELETE Method */
//     MAX
// } e_http_methods_t;

typedef struct ezlopi_scenes_then_methods_send_http
{
    char *url; //"https://ezlo.com/ HTTP/1.1\\r\\n",
    esp_http_client_method_t method;
    char *host; //= "10.101.101.10";
    char *username;
    char *password;
    char content_type[20];
    char content[100];
    uint32_t port;                    //= 80;
    bool skip_cert_common_name_check; // bool
    cJSON *header;

} ezlopi_scenes_then_methods_send_http_t;

static void __http_request_api(ezlopi_scenes_then_methods_send_http_t *config)
{
    esp_http_client_config_t http_config = {
        .url = config->url,
        .method = config->method,
        .skip_cert_common_name_check = config->skip_cert_common_name_check,
        .host = config->host, //"10.101.101.10",
        .port = config->port,
        .timeout_ms = 30000, // 30sec
        .max_redirection_count = 10,
    };

    char *e_cloud_server = NULL;
    char *e_ca_certificate = NULL;
    char *e_ssl_shared_key = NULL;
    char *e_ssl_private_key = NULL;

    e_cloud_server = ezlopi_factory_info_v2_get_cloud_server();
    e_ca_certificate = ezlopi_factory_info_v2_get_ca_certificate();
    e_ssl_shared_key = ezlopi_factory_info_v2_get_ssl_shared_key();
    e_ssl_private_key = ezlopi_factory_info_v2_get_ssl_private_key();

    TRACE_W("!HERE");
    // esp_http_client_handle_t client = esp_http_client_init(&http_config);
    // if (client)
    // {
    switch (config->method)
    {
    case HTTP_METHOD_GET:
    {
        // GET

        // esp_err_t err = esp_http_client_perform(client);
        // if (err == ESP_OK)
        // {
        //     ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %" PRId64,
        //              esp_http_client_get_status_code(client),
        //              esp_http_client_get_content_length(client));
        // }
        // else
        // {
        //     ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        // }
        // ESP_LOG_BUFFER_HEX(TAG, local_response_buffer, strlen(local_response_buffer));

        s_ezlopi_http_data_t *http_reply = (s_ezlopi_http_data_t *)malloc(sizeof(s_ezlopi_http_data_t));
        if (http_reply)
        {
            memset(http_reply, 0, sizeof(s_ezlopi_http_data_t));
            http_reply = ezlopi_http_get_request(config->url, e_ssl_private_key, e_ssl_shared_key, e_ca_certificate);

            TRACE_D("HTTP GET Status_resonse = %s, Status_code = %d",
                    http_reply->response,
                    http_reply->status_code);
        }
        break;
    }
#if 0
        case HTTP_METHOD_POST:
        { // POST
            const char *post_data = "{\"field1\":\"value1\"}";
            // esp_http_client_set_url(client, "http://" CONFIG_EXAMPLE_HTTP_ENDPOINT "/post");
            esp_http_client_set_method(client, HTTP_METHOD_POST);
            esp_http_client_set_header(client, "Content-Type", "application/json");
            esp_http_client_set_post_field(client, post_data, strlen(post_data));
            err = esp_http_client_perform(client);
            if (err == ESP_OK)
            {
                ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %" PRId64,
                         esp_http_client_get_status_code(client),
                         esp_http_client_get_content_length(client));
            }
            else
            {
                ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
            }
            break;
        }
        case HTTP_METHOD_PUT:
        { // PUT
            esp_http_client_set_url(client, "http://" CONFIG_EXAMPLE_HTTP_ENDPOINT "/put");
            esp_http_client_set_method(client, HTTP_METHOD_PUT);
            err = esp_http_client_perform(client);
            if (err == ESP_OK)
            {
                ESP_LOGI(TAG, "HTTP PUT Status = %d, content_length = %" PRId64,
                         esp_http_client_get_status_code(client),
                         esp_http_client_get_content_length(client));
            }
            else
            {
                ESP_LOGE(TAG, "HTTP PUT request failed: %s", esp_err_to_name(err));
            }
            break;
        }
        case HTTP_METHOD_DELETE:
        { // DELETE
            // esp_http_client_set_url(client, "http://" CONFIG_EXAMPLE_HTTP_ENDPOINT "/delete");
            esp_http_client_set_method(client, HTTP_METHOD_DELETE);
            err = esp_http_client_perform(client);
            if (err == ESP_OK)
            {
                ESP_LOGI(TAG, "HTTP DELETE Status = %d, content_length = %" PRId64,
                         esp_http_client_get_status_code(client),
                         esp_http_client_get_content_length(client));
            }
            else
            {
                ESP_LOGE(TAG, "HTTP DELETE request failed: %s", esp_err_to_name(err));
            }
            break;
        }
#endif
    default:
        break;
    }

    //     esp_http_client_cleanup(client);
    // }
}

int ezlopi_scene_then_send_http_request(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // /**
    //  * @brief        Get /mysite/index.html HTTP/1.1\r\n
    //  *               Host: 10.101.101.10\r\n
    //  *               Accept: */*\r\n
    //  *               \r\n
    //  *
    //  */
    char *request_type = NULL;
    char *content_type = NULL;
    char *skipSecurity_header = NULL;
    char *credential = NULL;
    static char request_line[80];
    static char http_request[100];
    // TRACE_W("Warning: then-method not implemented!");
    int ret = 0;

    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
    if (curr_then)
    {
        // configuration for http_request
        ezlopi_scenes_then_methods_send_http_t *user_data = (ezlopi_scenes_then_methods_send_http_t *)malloc(sizeof(ezlopi_scenes_then_methods_send_http_t));
        if (user_data)
        {

            memset(user_data, 0, sizeof(ezlopi_scenes_then_methods_send_http_t));
            user_data->username = NULL;
            user_data->password = NULL;
            user_data->host = "192.168.01.10";
            user_data->port = 8080;

            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field) // fields
            {
                // create a  requrest line
                if (0 == strncmp(curr_field->name, "request", strlen("request") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                    {
                        if (0 == strncmp(curr_field->name, "GET", strlen("GET") + 1))
                        {
                            user_data->method = HTTP_METHOD_GET;
                            // memset(http_request, '\0', sizeof(http_request));
                            // snprintf(http_request, sizeof("GET"), "%s", request_line);
                            strcat(http_request, "GET");
                            TRACE_W("http_request GET: %s", http_request);
                        }
                        if (0 == strncmp(curr_field->name, "POST", strlen("POST") + 1))
                        {
                            user_data->method = HTTP_METHOD_POST;
                            // memset(http_request, '\0', sizeof(http_request));
                            // snprintf(http_request, sizeof("POST"), "%s", request_line);
                            strcat(http_request, "POST");
                            TRACE_W("http_request POST: %s", http_request);
                        }
                        // if (0 == strncmp(curr_field->name, "PUT", strlen("PUT") + 1))
                        // {
                        //     user_data->method = HTTP_METHOD_PUT;
                        //     memset(http_request, '\0', sizeof(http_request));
                        //     strncpy(http_request, "PUT", 4);
                        // }
                        // if (0 == strncmp(curr_field->name, "DELETE", strlen("DELETE") + 1))
                        // {
                        //     user_data->method = HTTP_METHOD_DELETE;
                        //     memset(http_request, '\0', sizeof(http_request));
                        //     strncpy(http_request, "DELETE", 7);
                        // }
                        TRACE_W("http_request : %s", http_request);
                    }
                }
                else if (0 == strncmp(curr_field->name, "url", strlen("url") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                    {
                        memset(request_line, '\0', sizeof(request_line));
                        snprintf(request_line + strlen(request_line) + 1, strlen(curr_field->value.value_string) + 1 >= 60 ? 60 : strlen(curr_field->value.value_string) + 1, "%s HTTP/1.1\\r\\n", curr_field->value.value_string); //"https://ezlo.com/ HTTP/1.1\\r\\n"
                        TRACE_W("URL : %s", request_line);
                    }
                }
                // else if (0 == strncmp(curr_field->name, "credential", strlen("credential") + 1))
                // {
                //     if (EZLOPI_VALUE_TYPE_CREDENTIAL == curr_field->value_type)
                //     {
                //         user_data->username = curr_field->value.value_cred.username;
                //         user_data->password = curr_field->value.value_cred.password;
                //     }
                // }
                else if (0 == strncmp(curr_field->name, "contentType", strlen("contentType") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                    {
                        snprintf(user_data->content_type, (strlen(curr_field->value.value_string) + 1 >= 20 ? 20 : (strlen(curr_field->value.value_string) + 1)), "%s", curr_field->value.value_string);
                        TRACE_W("content_type : %s", user_data->content_type);
                    }
                }
                else if (0 == strncmp(curr_field->name, "content", strlen("content") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                    {
                        snprintf(user_data->content, (strlen(curr_field->value.value_string) + 1 >= 100 ? 100 : (strlen(curr_field->value.value_string) + 1)), "%s", curr_field->value.value_string);
                        TRACE_W("content : %s", user_data->content);
                    }
                }
                // else if (0 == strncmp(curr_field->name, "headers", strlen("headers") + 1))
                // {
                //     if (EZLOPI_VALUE_TYPE_DICTIONARY == curr_field->value_type)
                //     {
                //         //cJSON *headersItem=cJSON_GetObjectItem(cjson , "header");
                //          user_data->header=  headersItem ? cJSON_PrintUnformatted(headersItem) : NULL;
                //     }
                // }
                else if (0 == strncmp(curr_field->name, "skipSecurity", strlen("skipSecurity") + 1))
                {
                    if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                    {
                        user_data->skip_cert_common_name_check = curr_field->value.value_bool;
                        TRACE_W("skip_cert : %s", (user_data->skip_cert_common_name_check) ? "true" : "false");
                    }
                }

                curr_field = curr_field->next;
            }
            // Append ->   http_request[] = "GET.." + "https://ezlo.com/ HTTP/1.1\\r\\n"
            snprintf(http_request + strlen(http_request), strlen(http_request), "%s", request_line);
            // strncat(&http_request, &request_line, strlen(request_line));
            TRACE_I("http_request: %s", http_request);

            user_data->url = http_request;
            __http_request_api(user_data);

            free(user_data);
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