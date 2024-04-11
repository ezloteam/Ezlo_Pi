#include <string.h>
#include "ezlopi_util_trace.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_then_methods_helper_func.h"

#define STR_SIZE(str) ((NULL != str) ? (strlen(str)) : 0)

/**
 * @brief Frees an address => '*ptr', pointing to an occupied address in heap.
 */
#define FREE_PTR_IF_NOT_NULL(ptr) \
    {                             \
        if (ptr)                  \
        {                         \
            free(ptr);            \
            ptr = NULL;           \
        }                         \
    }

 /**
  * @brief This funtion is called, only to reallocate a '*header' of custom_structure 's_ezlopi_core_http_mbedtls_t'
  *
  * @param tmp_http_data     [ Pointer to (s_ezlopi_core_http_mbedtls_t*) block of memory. ]
  * @param append_size       [ Size of 'string' to be appended. ]
  * @param append_str        [ 'string_literal' to be appended. ]
  * @return int [ Fail ==> returns Old-size / Success ==> returns New-size ]
  */
static int __ezlopi_core_scenes_then_sendhttp_relloc_header(s_ezlopi_core_http_mbedtls_t* tmp_http_data, int append_size, const char* append_str)
{
    int ret = (int)tmp_http_data->header_maxlen; // Assign Old-block size as default
    int new_size = append_size + (ret + 1);

    uint8_t retry = 5;
    do
    {
        if (1 == ezlopi_core_http_dyna_relloc(&(tmp_http_data->header), new_size)) // rellocate: 'tmp_http_data->header' with  'new_size'
        {
            snprintf((tmp_http_data->header) + strlen(tmp_http_data->header), append_size, "%s", append_str);
            ret = new_size; // return new memory-block size
            // TRACE_I("Append Successful: Header[size: %d , occupied: %d]", ret, STR_SIZE(tmp_http_data->header));
            break;
        }
        else
        {
            TRACE_E("Append_Failed... ; returning original header_contents [RETRY:%d]", retry);
            retry--;
        }
    } while (retry > 0);
    return ret;
}
/**
 * @brief Function to extract "web_host" from "field_value_string".
 */
static void __ezlopi_core_scenes_then_sendhttp_parse_host_name(s_ezlopi_core_http_mbedtls_t* tmp_http_data, const char* field_value_string)
{
    if (NULL != field_value_string)
    {
        const char* start = strstr(field_value_string, "://");
        if (start != NULL)
        {
            // TRACE_W("Here! fresh webserver");
            start += 3;
            int length = 0;
            char* end = strchr(start, '/');
            if (end != NULL)
            {
                length = (end - start);
                if (length > 0)
                {
                    length++;                                         // include null character
                    char* tmp_string = malloc(length); // tmp_string != NULL
                    if (tmp_string)
                    {
                        bzero(tmp_string, length);
                        snprintf(tmp_string, length, "%s", start);
                        // TRACE_I("web_host_name : %s", tmp_string);
                        tmp_http_data->web_server_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->web_server), tmp_string);
                        free(tmp_string);
                    }
                }
            }
            else
            {
                const char* ptr = field_value_string;
                length = (int)strlen(field_value_string) - (int)(start - ptr);
                if (length > 0)
                {
                    length++;                                         // include null character
                    char* tmp_string = malloc(length); // tmp_string != NULL
                    if (tmp_string)
                    {
                        bzero(tmp_string, length);
                        snprintf(tmp_string, length, "%s", (ptr + ((int)(start - ptr))));
                        // TRACE_I("web_host_name : %s", tmp_string);
                        tmp_http_data->web_server_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->web_server), tmp_string);
                        free(tmp_string);
                    }
                }
            }
        }
    }
}
/**
 * @brief Function to Clear and Malloc the header_member (within 's_ezlopi_core_http_mbedtls_t') only.
 * @return Size of content in 's_ezlopi_core_http_mbedtls_t'->header
 */
static int __ezlopi_core_scenes_then_create_fresh_header(s_ezlopi_core_http_mbedtls_t* tmp_http_data)
{
    int ret = STR_SIZE(tmp_http_data->header);

    if ((NULL == tmp_http_data->header) && (0 == ret))
    {
        // TRACE_W("Here! fresh header init");
        tmp_http_data->header_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->header), "\r\0");
        ret = STR_SIZE(tmp_http_data->header);
        // TRACE_W("Here!! Created fresh header-> [capacity: %d] , [occupied: %d]", tmp_http_data->header_maxlen, ret);
    }
    return ret;
}
/**
 * @brief Function to append values to header_member (within 's_ezlopi_core_http_mbedtls_t') only.
 */
static void __ezlopi_core_scenes_then_append_to_header(s_ezlopi_core_http_mbedtls_t* tmp_http_data, const char* str1, const char* str2)
{
    int append_size = (STR_SIZE(str1) + 4 + STR_SIZE(str2)) + 6;
    int max_allowed = ezlopi_core_http_calc_empty_bufsize(tmp_http_data->header, (tmp_http_data->header_maxlen), append_size);
    if (max_allowed > 0)
    {
        snprintf(tmp_http_data->header + STR_SIZE(tmp_http_data->header), max_allowed, "%s: %s\r\n", str1, str2);
    }
    else // We reallocate:- 'tmp_http_data->header'
    {
        char* append_str = malloc(append_size); // append_str != NULL
        if (append_str)
        {
            bzero(append_str, append_size);
            snprintf(append_str, append_size, "%s: %s\r\n", str1, str2);

            //-----------------------------------------------------------------------------------
            // TRACE_D("Append_str: %s[%d] ", append_str, append_size);
            // TRACE_D("-> Before => Realloc_Header:-[capacity:%d (occupied:%d)]->[needed:%d]", tmp_http_data->header_maxlen, STR_SIZE(tmp_http_data->header), append_size);
            tmp_http_data->header_maxlen = (uint16_t)__ezlopi_core_scenes_then_sendhttp_relloc_header(tmp_http_data, append_size, append_str);
            // TRACE_D("-> After => Realloc_Header:-[capacity:%d (occupied:%d)]", tmp_http_data->header_maxlen, STR_SIZE(tmp_http_data->header));
            //-----------------------------------------------------------------------------------
            free(append_str);
        }
    }
}

//------------------------------- ezlopi_scene_then_sendhttp_request -----------------------------------------------
void parse_http_request_type(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field)
{
    const char* field_value_string = curr_field->field_value.u_value.value_string;
    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != field_value_string))
    {
        if (0 == strncmp(curr_field->field_value.u_value.value_string, "GET", 4))
        {
            tmp_http_data->method = HTTP_METHOD_GET;
        }
        else if (0 == strncmp(curr_field->field_value.u_value.value_string, "POST", 5))
        {
            tmp_http_data->method = HTTP_METHOD_POST;
        }
        else if (0 == strncmp(curr_field->field_value.u_value.value_string, "PUT", 4))
        {
            tmp_http_data->method = HTTP_METHOD_PUT;
        }
        else if (0 == strncmp(curr_field->field_value.u_value.value_string, "DELETE", 7))
        {
            tmp_http_data->method = HTTP_METHOD_DELETE;
        }
        else
        {
            TRACE_E("The given http_req method is not implemented yet");
        }
    }
}
void parse_http_url(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field)
{
    const char* field_value_string = curr_field->field_value.u_value.value_string;
    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != field_value_string))
    {
        // TRACE_W("Here! fresh url");
        tmp_http_data->url_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->url), field_value_string);
        if (tmp_http_data->url_maxlen > 0)
        {
            tmp_http_data->web_port = ((NULL != strstr(field_value_string, "https")) ? 443 : 80);
            //--------------------------------------------
            __ezlopi_core_scenes_then_sendhttp_parse_host_name(tmp_http_data, field_value_string);
            //--------------------------------------------

            int content_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
            if (content_size > 0) // if this characters exsists in the 'tmp_http_data->header'
            {
                // 1. adding 'host' to header-buffer
                // TRACE_W("Appending!! webserver -> header");
                __ezlopi_core_scenes_then_append_to_header(tmp_http_data, "Host", tmp_http_data->web_server);
            }
            else
            {
                TRACE_E("Failed Creating header for 'Host:'");
            }
        }
    }
}
void parse_http_content_type(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field)
{
    const char* field_value_string = curr_field->field_value.u_value.value_string;
    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != field_value_string))
    {
        // TRACE_W("Here! content-type");
        if (STR_SIZE(field_value_string) > 0)
        {
            int content_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
            if (content_size > 0) // if this characters exsists in the 'tmp_http_data->header'
            {
                // 2. adding 'Content-Type' to header-buffer
                // TRACE_W("Appending!! content-type -> header");
                __ezlopi_core_scenes_then_append_to_header(tmp_http_data, "Content-Type", field_value_string);
            }
            else
            {
                TRACE_E("Failed Creating header for 'Content-Type:'");
            }
        }
        // else
        // {
        //     TRACE_E("ERR : Content-Type ==> STRLEN[%d]", STR_SIZE(field_value_string));
        // }
    }
}
void parse_http_content(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field)
{
    const char* field_value_string = curr_field->field_value.u_value.value_string;
    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != field_value_string))
    {
        // TRACE_W("Here! fresh content");
        tmp_http_data->content_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->content), field_value_string);
        if (tmp_http_data->content_maxlen > 0)
        {
            int content_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
            if (content_size > 0) // if this characters exsists in the 'tmp_http_data->header'
            {
                // TRACE_W("Appending!! content_length -> header");
                uint16_t i = (int)strlen(field_value_string);
                if (i > 0)
                {
                    char str[10];
                    snprintf(str, 10, "%d", (int)i); // write length value in string
                    str[10] = '\0';
                    // 3. adding 'Content-Length' to header-buffer
                    __ezlopi_core_scenes_then_append_to_header(tmp_http_data, "Content-Length", str);
                }
            }
            else
            {
                TRACE_E("Failed Creating header for 'Content-Length:'");
            }
        }
    }
}
void parse_http_headers(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field)
{
    const cJSON* cj_value = curr_field->field_value.u_value.cj_value;
    if ((EZLOPI_VALUE_TYPE_DICTIONARY == curr_field->value_type) && cJSON_IsObject(cj_value))
    {
        // TRACE_W("Here! headers");
        int content_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
        if (content_size > 0) // if this characters exsists in the 'tmp_http_data->header'
        {
            // TRACE_W("Appending!! headers -> header");
            cJSON* header = (cj_value->child);
            while (header)
            {
                if ((NULL != header->string) && (NULL != header->valuestring))
                {
                    // 4. adding 'remaining' to header-buffer
                    __ezlopi_core_scenes_then_append_to_header(tmp_http_data, header->string, header->valuestring);
                }
                header = header->next;
            }
        }
        else
        {
            TRACE_E("Failed Creating header for 'Headers:'");
        }
    }
}
void parse_http_skipsecurity(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field)
{
    // TRACE_W("Here! skipsecurity");
    if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
    {
        tmp_http_data->skip_cert_common_name_check = curr_field->field_value.u_value.value_bool;
        int content_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
        if (content_size > 0) // if this characters exsists in the 'tmp_http_data->header'
        {
            // 5. adding 'skip_security' to header-buffer
            // TRACE_W("Appending!! skipsecurity -> header");
            __ezlopi_core_scenes_then_append_to_header(tmp_http_data, "skipSecurity", ((curr_field->field_value.u_value.value_bool) ? "true" : "false"));
        }
        else
        {
            TRACE_E("Failed Creating header for 'skipSecurity:'");
        }
    }
}
void parse_http_creds(s_ezlopi_core_http_mbedtls_t* tmp_http_data, l_fields_v2_t* curr_field)
{
    const cJSON* cj_value = curr_field->field_value.u_value.cj_value;
    if ((EZLOPI_VALUE_TYPE_CREDENTIAL == curr_field->value_type) && cJSON_IsObject(cj_value))
    {
        cJSON* userItem = cJSON_GetObjectItem(cj_value, "user");
        cJSON* passwordItem = cJSON_GetObjectItem(cj_value, "password");
        if ((NULL != userItem) && (NULL != passwordItem))
        {
            const char* userValue = cJSON_GetStringValue(userItem);
            const char* passValue = cJSON_GetStringValue(passwordItem);

            // TRACE_W("Here! credential");
            tmp_http_data->username_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->username), userValue);
            tmp_http_data->password_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->password), passValue);
        }
    }
}
void free_http_mbedtls_struct(s_ezlopi_core_http_mbedtls_t* config)
{
    FREE_PTR_IF_NOT_NULL(config->url);
    FREE_PTR_IF_NOT_NULL(config->web_server);
    FREE_PTR_IF_NOT_NULL(config->header);
    FREE_PTR_IF_NOT_NULL(config->content);
    FREE_PTR_IF_NOT_NULL(config->username);
    FREE_PTR_IF_NOT_NULL(config->password);
    FREE_PTR_IF_NOT_NULL(config->response);
}
