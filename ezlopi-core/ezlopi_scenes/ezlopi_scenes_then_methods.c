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

//---------------------------------------------------------------------------------------
typedef struct s_ezlopi_scenes_then_methods_send_http
{
    char url[100];         //"https://ezlo.com/",
    char encoded_url[250]; // POST%20%2F%20HTTP%2F1.1%0AHost%3A%20ezlo.com
    // char user_pass[64];
    char content_type[40];
    char content[100];
    uint32_t port;                    //= 80;
    bool skip_cert_common_name_check; // bool
    cJSON *header;
    esp_http_client_method_t method;

} s_ezlopi_scenes_then_methods_send_http_t;

static void __http_request_api(s_ezlopi_scenes_then_methods_send_http_t *config)
{

    // POST / HTTP/1.1
    // Host: ezlo.com
    // Content-Type: text/plain
    // Content-Length: 13

    // Request check

    /* needed URI */
    // POST%20%2F%20HTTP%2F1.1%0AHost%3A%20ezlo.com%0AContent-Type%3A%20text%2Fplain%0AContent-Length%3A%2013%0A%0ARequest%20check

    /**
     *  ' ' => %20
     *  '/' => %2F
     *  ':' => %3A
     *  '\r\n' => %0A
     */

    // char *tmp_cloud_server = ezlopi_factory_info_v2_get_cloud_server();
    char *tmp_ca_certificate = ezlopi_factory_info_v2_get_ca_certificate();
    char *tmp_ssl_shared_key = ezlopi_factory_info_v2_get_ssl_shared_key();
    char *tmp_ssl_private_key = ezlopi_factory_info_v2_get_ssl_private_key();
    TRACE_W("URI :- '%s' [%d]", config->url, strlen(config->url));
    TRACE_W("ENCODED_URI :- '%s' [%d]", config->encoded_url, strlen(config->encoded_url));
    TRACE_W("content-type : %s", config->content_type);
    TRACE_W("content : %s", config->content);
    TRACE_W("skip_cert : %s", (config->skip_cert_common_name_check) ? "true" : "false");

    switch (config->method)
    {
    case HTTP_METHOD_GET:
    {
        // GET
        s_ezlopi_http_data_t *http_reply = NULL;

        esp_http_client_config_t tmp_http_config = {
            // initialize with default values

            // -> Fill remaining members of this structure according to reqm
            // eg : port , host ...
            .timeout_ms = 30000,         // 30sec
            .max_redirection_count = 10, // default 0
            .skip_cert_common_name_check = config->skip_cert_common_name_check,
        };

        http_reply = ezlopi_http_get_request(config->url, tmp_ssl_private_key, tmp_ssl_shared_key, tmp_ca_certificate, NULL);
        if (http_reply)
        {
            TRACE_D("HTTP GET Status_resonse = %s, Status_code = %d",
                    http_reply->response,
                    http_reply->status_code);

            if (http_reply->response)
            {
                free(http_reply->response);
            }
            free(http_reply);
        }
        break;
    }
    case HTTP_METHOD_POST:
    { // POST
      // const char *post_data = "{\"field1\":\"value1\"}";

        const char *location = NULL;
        s_ezlopi_http_data_t *http_reply = NULL;

        esp_http_client_config_t tmp_http_config = {
            // initialize with default values

            // -> Fill remaining members of this structure according to reqm
            // eg : port , host ...
            .timeout_ms = 30000,         // 30sec
            .max_redirection_count = 10, // default 0
            .skip_cert_common_name_check = config->skip_cert_common_name_check,
        };

        http_reply = ezlopi_http_post_request(config->url, "", config->header, tmp_ssl_private_key, tmp_ssl_shared_key, tmp_ca_certificate, &tmp_http_config);
        if (http_reply)
        {
            TRACE_D("HTTP GET Status_resonse = %s, Status_code = %d",
                    http_reply->response,
                    http_reply->status_code);
            if (http_reply->response)
            {
                free(http_reply->response);
            }
            free(http_reply);
        }

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
}

char *urlEncode(const char *input, int *encoded_len)
{
    // Calculate the length of the encoded string
    int len = 0;
    for (const char *ptr = input; *ptr != '\0'; ++ptr)
    {
        if (' ' == *ptr || ':' == *ptr || '/' == *ptr || '\n' == *ptr)
        {
            len += 3; // Space (' ') becomes '%20', slash ('/') becomes '%2F', colon (':') becomes '%3A' , Newline ('\n') becomes '%0A'
        }
        else
        {
            len += 1; // Other characters remain as is
        }
    }
    *encoded_len = len;
    // Allocate memory for the encoded string
    char *encoded = (char *)malloc(len + 1); // +1 for the null terminator

    // Perform the encoding
    int j = 0;
    for (const char *ptr = input; *ptr != '\0'; ++ptr)
    {
        if (*ptr == ' ')
        {
            encoded[j++] = '%';
            encoded[j++] = '2';
            encoded[j++] = '0';
        }
        else if (*ptr == '/')
        {
            encoded[j++] = '%';
            encoded[j++] = '2';
            encoded[j++] = 'F';
        }
        else if (*ptr == ':')
        {
            encoded[j++] = '%';
            encoded[j++] = '3';
            encoded[j++] = 'A';
        }
        else if (*ptr == '\n')
        {
            encoded[j++] = '%';
            encoded[j++] = '0';
            encoded[j++] = 'A';
        }
        else
        {
            encoded[j++] = *ptr;
        }
    }
    // Null-terminate the encoded string
    encoded[j] = '\0';

    return encoded;
}

int ezlopi_scene_then_send_http_request(l_scenes_list_v2_t *curr_scene, void *arg)
{
    /* needed URI */
    // POST%20%2F%20HTTP%2F1.1%0AHost%3A%20ezlo.com%0AContent-Type%3A%20text%2Fplain%0AContent-Length%3A%2013%0A%0ARequest%20check

    char encoded_uri_line[100] = {'\0'};
    char encoded_content_line[100] = {'\0'};
    char encoded_content_type_line[50] = {'\0'};
    int ret = 0;

    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
    if (curr_then)
    {
        // configuration for http_request
        s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data = (s_ezlopi_scenes_then_methods_send_http_t *)malloc(sizeof(s_ezlopi_scenes_then_methods_send_http_t));
        if (tmp_http_data)
        {
            memset(tmp_http_data, 0, sizeof(s_ezlopi_scenes_then_methods_send_http_t));
            tmp_http_data->header = NULL;
            tmp_http_data->header = cJSON_CreateObject();
            if (tmp_http_data->header) //  headers
            {
                l_fields_v2_t *curr_field = curr_then->fields;
                while (NULL != curr_field) // fields
                {
                    // create a  requrest line
                    if (0 == strncmp(curr_field->name, "request", 8))
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
                    else if (0 == strncmp(curr_field->name, "url", 4))
                    {
                        // add to uri
                        if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                        {
                            snprintf(tmp_http_data->url, sizeof(tmp_http_data->url), "%s", curr_field->value.value_string);
                            snprintf(encoded_uri_line, sizeof(encoded_uri_line), "%s\n", curr_field->value.value_string);
                        }
                    }
                    // else if (0 == strncmp(curr_field->name, "credential", 11))
                    // {
                    //     if (EZLOPI_VALUE_TYPE_CREDENTIAL == curr_field->value_type)
                    //     {
                    //         // tmp_http_data->username = curr_field->value.value_credential.username;
                    //         // tmp_http_data->password = curr_field->value.value_credential.password;
                    //         cJSON *credential_object = cJSON_GetObjectItem(curr_field->value.value_json, "value");
                    //         if (credential_object)
                    //         {
                    //             snprintf(tmp_http_data->user_pass, strlen(curr_field->value.value_string) + 1, "%s", curr_field->value.value_string);
                    //
                    //             cJSON_Delete(credential_object);
                    //         }
                    //     }
                    // }
                    else if (0 == strncmp(curr_field->name, "contentType", 12))
                    {
                        // add to uri
                        if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                        {
                            snprintf(tmp_http_data->content_type, sizeof(tmp_http_data->content_type), "%s", curr_field->value.value_string);
                            snprintf(encoded_content_type_line, sizeof(encoded_content_type_line), "Content-Type: %s\n", (curr_field->value.value_string));
                        }
                    }
                    else if (0 == strncmp(curr_field->name, "content", 8))
                    {
                        // add to uri
                        if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                        {
                            memset(tmp_http_data->content, 0, sizeof(tmp_http_data->content));
                            snprintf(tmp_http_data->content, sizeof(tmp_http_data->content), "%s", curr_field->value.value_string);
                            uint8_t i = 0;
                            for (; i < sizeof(tmp_http_data->content); i++)
                            {
                                if (('\0' == tmp_http_data->content[i]) || (0 == tmp_http_data->content[i]))
                                    break;
                            }
                            snprintf(encoded_content_line, sizeof(encoded_content_line), "Content-Length: %d\n\n%s", i, (curr_field->value.value_string));
                        }
                    }
                    // else if (0 == strncmp(curr_field->name, "headers", 8))
                    // {
                    //     if (EZLOPI_VALUE_TYPE_DICTIONARY == curr_field->value_type)
                    //     {
                    //         if (cJSON_HasObjectItem(curr_field->value.value_json, "value"))
                    //         {
                    //             cJSON *tmp_header_item = cJSON_GetObjectItem(curr_field->value.value_json, "value");
                    //             if (tmp_header_item)
                    //             {
                    //                 cJSON_AddItemToObject(tmp_http_data->header, "headers", tmp_header_item);
                    //             }
                    //         }
                    //     }
                    // }
                    else if (0 == strncmp(curr_field->name, "skipSecurity", 12))
                    {
                        if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                        {
                            tmp_http_data->skip_cert_common_name_check = curr_field->value.value_bool;
                        }
                    }

                    curr_field = curr_field->next;
                }

                // if src is  'empty' no characters are coppied
                strncat(tmp_http_data->encoded_url, encoded_uri_line, (size_t)(sizeof(encoded_uri_line)));
                strncat(tmp_http_data->encoded_url, encoded_content_type_line, (size_t)(sizeof(encoded_content_type_line)));
                strncat(tmp_http_data->encoded_url, encoded_content_line, (size_t)(sizeof(encoded_content_line)));

                int encoded_len = 0;
                char *encoded = urlEncode(tmp_http_data->encoded_url, &encoded_len);
                if (encoded)
                {
                    TRACE_D("Original: %s", (tmp_http_data->url));
                    TRACE_D("Encoded : %s [%d]", encoded, encoded_len);
                    snprintf(tmp_http_data->encoded_url, sizeof(tmp_http_data->encoded_url), "%s", encoded);

                    free(encoded); // Don't forget to free the allocated memory
                    // TRACE_I("Encoded-url : '%s' [%d]", tmp_http_data->encoded_url, strlen(tmp_http_data->encoded_url));
                }

                __http_request_api(tmp_http_data);

                cJSON_Delete(tmp_http_data->header);
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