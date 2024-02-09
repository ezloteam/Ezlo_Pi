#include "ezlopi_util_trace.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_scenes_then_methods_helper_func.h"

#define STR_SIZE(str) ((NULL != str) ? (1 + strlen(str)) : 0)

/**
 * @brief Frees an address => '*__dest_ptr', pointing to an occupied address in heap.
 */
static void __free_custom_ptr(char *__dest_ptr)
{
    if (NULL != __dest_ptr)
    {
        free(__dest_ptr);
        TRACE_D("#freed [%p] : %d ==> %s ", __dest_ptr, STR_SIZE(__dest_ptr), (__dest_ptr));
        __dest_ptr = NULL;
    }
}
/**
 * @brief This funtion is called, only to reallocate a '*header' of custom_structure 's_ezlopi_core_http_mbedtls_t'
 *
 * @param tmp_http_data     [ Pointer to (s_ezlopi_core_http_mbedtls_t*) block of memory. ]
 * @param prev_size         [ Original size of the '*header' memory-block. ]
 * @param append_size       [ Size of 'string' to be appended. ]
 * @param append_str        [ 'string_literal' to be appended. ]
 * @return int [ Fail ==> returns Old-size / Success ==> returns New-size ]
 */
static int __ezlopi_core_scenes_then_sendhttp_relloc_header(s_ezlopi_core_http_mbedtls_t *tmp_http_data, int prev_size, int append_size, const char *append_str)
{
    int ret = (int)(tmp_http_data->header_maxlen); // Assign Old-block size as default
    int new_size = prev_size + append_size;        // this size count only :- 'characters' after combining them
    int tmp_maxlen = (((int)tmp_http_data->header_maxlen > 16) ? tmp_http_data->header_maxlen : 16);
    while (tmp_maxlen <= new_size)
    {
        tmp_maxlen += (tmp_http_data->header_maxlen); // possible constraint :- if(tmp_maxlen > (4096*sizeof(char))) break;
    }
    new_size = tmp_maxlen;
    if (__ezlopi_core_http_mem_relloc(&(tmp_http_data->header), new_size)) // now rellocate: 'tmp_http_data->header' with suitable 'new_size'
    {
        snprintf((tmp_http_data->header) + strlen(tmp_http_data->header), append_size, "%s", append_str);
        TRACE_D("Append Successful: Header[%d]:\n%s[%d]", (tmp_http_data->header_maxlen), tmp_http_data->header, strlen(tmp_http_data->header));
        ret = new_size; // return new memory-block size
    }
    return ret;
}
/**
 * @brief Function to extract "web_host" from "field_value_string".
 */
static void __ezlopi_core_scenes_then_sendhttp_parse_host_name(s_ezlopi_core_http_mbedtls_t *tmp_http_data, const char *field_value_string)
{
    if (NULL != field_value_string)
    {
        const char *start = strstr(field_value_string, "://");
        if (start != NULL)
        {
            // TRACE_W("Here! fresh webserver");
            start += 3;
            int length = 0;
            char *end = strchr(start, '/');
            if (end != NULL)
            {
                length = end - start;
                if (length > 0)
                {
                    char *tmp_string = malloc(sizeof(char) * (length + 1)); // tmp_string != NULL
                    if (tmp_string)
                    {
                        bzero(tmp_string, sizeof(char) * (length + 1));
                        snprintf(tmp_string, (length + 1), "%s", start);
                        TRACE_I("web_host_name : %s", tmp_string);
                        tmp_http_data->web_server_maxlen = __ezlopi_core_http_mem_malloc(&(tmp_http_data->web_server), tmp_string);
                        free(tmp_string);
                    }
                }
            }
            else
            {
                const char *ptr = field_value_string;
                length = (int)strlen(field_value_string) - (int)(start - ptr);
                if (length > 0)
                {
                    char *tmp_string = malloc(sizeof(char) * (length + 1)); // tmp_string != NULL
                    if (tmp_string)
                    {
                        bzero(tmp_string, sizeof(char) * (length + 1));
                        snprintf(tmp_string, (length + 1), "%s", (ptr + ((int)(start - ptr))));
                        TRACE_I("web_host_name : %s", tmp_string);
                        tmp_http_data->web_server_maxlen = __ezlopi_core_http_mem_malloc(&(tmp_http_data->web_server), tmp_string);
                        free(tmp_string);
                    }
                }
            }
        }
    }
}
/**
 * @brief Function to Clear and Malloc the header_member (within 's_ezlopi_core_http_mbedtls_t') only.
 */
static int __ezlopi_core_scenes_then_create_fresh_header(s_ezlopi_core_http_mbedtls_t *tmp_http_data)
{
    int ret = STR_SIZE(tmp_http_data->header);
    if (0 == ret)
    {
        // TRACE_W("Here! fresh header");
        tmp_http_data->header_maxlen = __ezlopi_core_http_mem_malloc(&(tmp_http_data->header), "\0");
        ret = STR_SIZE(tmp_http_data->header);
    }
    return ret;
}
/**
 * @brief Function to append values to header_member (within 's_ezlopi_core_http_mbedtls_t') only.
 */
static void __ezlopi_core_scenes_then_append_to_header(s_ezlopi_core_http_mbedtls_t *tmp_http_data, const char *str1, const char *str2, int prev_size)
{
    int append_size = (STR_SIZE(str1) + 2 + STR_SIZE(str2)) + 3;
    int max_allowed = __ezlopi_core_http_calc_empty_bufsize(tmp_http_data->header, (tmp_http_data->header_maxlen), append_size);
    if (max_allowed > 0)
    {
        snprintf((tmp_http_data->header) + (strlen(tmp_http_data->header)), max_allowed, "%s: %s\r\n", str1, str2);
    }
    else // if there is no space left ; we reallocate:- 'tmp_http_data->header'
    {
        char *append_str = malloc(sizeof(char) * (append_size + 1)); // append_str != NULL
        if (append_str)
        {
            bzero(append_str, sizeof(char) * (append_size + 1));
            snprintf(append_str, (append_size + 1), "%s: %s\r\n", str1, str2);
            append_str[append_size + 1] = '\0';
            //-----------------------------------------------------------------------------------
            TRACE_E("Here!");
            tmp_http_data->header_maxlen = __ezlopi_core_scenes_then_sendhttp_relloc_header(tmp_http_data, prev_size, append_size, append_str);
            //-----------------------------------------------------------------------------------
            free(append_str);
        }
    }
}

//------------------------------- ezlopi_scene_then_sendhttp_request -----------------------------------------------
void parse_http_url(s_ezlopi_core_http_mbedtls_t *tmp_http_data, const char *field_value_string)
{
    if (NULL != field_value_string)
    {
        // TRACE_W("Here! fresh url");
        tmp_http_data->url_maxlen = __ezlopi_core_http_mem_malloc(&(tmp_http_data->url), field_value_string);
        tmp_http_data->web_port = ((NULL != strstr(field_value_string, "https")) ? 443 : 80);

        //--------------------------------------------
        __ezlopi_core_scenes_then_sendhttp_parse_host_name(tmp_http_data, field_value_string);
        //--------------------------------------------

        int prev_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
        if (prev_size > 0) // if this characters exsists in the 'tmp_http_data->header'
        {
            // 1. adding 'host' to header-buffer
            // TRACE_W("Here! webserver -> header");
            __ezlopi_core_scenes_then_append_to_header(tmp_http_data, "Host", tmp_http_data->web_server, prev_size);
        }
    }
}
void parse_http_content_type(s_ezlopi_core_http_mbedtls_t *tmp_http_data, const char *field_value_string)
{
    if (NULL != field_value_string)
    {
        // TRACE_W("Here! content-type");
        int prev_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
        if (prev_size > 0) // if this characters exsists in the 'tmp_http_data->header'
        {
            // 2. adding 'Content-Type' to header-buffer
            // TRACE_W("Here! content-type -> header");
            __ezlopi_core_scenes_then_append_to_header(tmp_http_data, "Content-Type", field_value_string, prev_size);
        }
    }
}
void parse_http_content(s_ezlopi_core_http_mbedtls_t *tmp_http_data, const char *field_value_string)
{
    if (NULL != field_value_string)
    {
        // TRACE_W("Here! fresh content");
        tmp_http_data->content_maxlen = __ezlopi_core_http_mem_malloc(&(tmp_http_data->content), field_value_string);

        int prev_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
        if (prev_size > 0) // if this characters exsists in the 'tmp_http_data->header'
        {
            // TRACE_W("Here! content_length -> header");
            uint16_t i = (int)strlen(field_value_string);
            if (i > 0)
            {
                char str[10];
                snprintf(str, 10, "%d", (int)i); // write length value in string
                str[10] = '\0';
                // 3. adding 'Content-Length' to header-buffer
                __ezlopi_core_scenes_then_append_to_header(tmp_http_data, "Content-Length", str, prev_size);
            }
        }
    }
}
void parse_http_headers(s_ezlopi_core_http_mbedtls_t *tmp_http_data, cJSON *cj_value)
{
    if (cJSON_IsObject(cj_value))
    {
        // TRACE_W("Here! headers");
        int prev_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
        if (prev_size > 0) // if this characters exsists in the 'tmp_http_data->header'
        {
            // TRACE_W("Here! headers -> header");
            cJSON *header = (cj_value->child);
            while (header)
            {
                if ((NULL != header->string) && (NULL != header->valuestring))
                {
                    // 4. adding 'remaining' to header-buffer
                    __ezlopi_core_scenes_then_append_to_header(tmp_http_data, header->string, header->valuestring, prev_size);
                }
                header = header->next;
            }
        }
    }
}
void parse_http_skipsecurity(s_ezlopi_core_http_mbedtls_t *tmp_http_data, bool value_bool)
{
    // TRACE_W("Here! skipsecurity");
    tmp_http_data->skip_cert_common_name_check = value_bool;
    int prev_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
    if (prev_size > 0) // if this characters exsists in the 'tmp_http_data->header'
    {
        // 5. adding 'skip_security' to header-buffer
        // TRACE_W("Here! skipsecurity -> header");
        __ezlopi_core_scenes_then_append_to_header(tmp_http_data, "skipSecurity", ((value_bool) ? "true" : "false"), prev_size);
    }
}
void parse_http_creds(s_ezlopi_core_http_mbedtls_t *tmp_http_data, cJSON *cj_value)
{
    if (cJSON_IsObject(cj_value))
    {
        cJSON *userItem = cJSON_GetObjectItem(cj_value, "user");
        cJSON *passwordItem = cJSON_GetObjectItem(cj_value, "password");
        if ((NULL != userItem) && (NULL != passwordItem))
        {
            const char *userValue = cJSON_GetStringValue(userItem);
            const char *passValue = cJSON_GetStringValue(passwordItem);

            // TRACE_W("Here! fresh username");
            tmp_http_data->username_maxlen = __ezlopi_core_http_mem_malloc(&(tmp_http_data->username), userValue);
            // TRACE_W("Here! fresh password");
            tmp_http_data->password_maxlen = __ezlopi_core_http_mem_malloc(&(tmp_http_data->password), passValue);
        }
    }
}
void free_http_mbedtls_struct(s_ezlopi_core_http_mbedtls_t *config)
{
    __free_custom_ptr(config->url);
    __free_custom_ptr(config->web_server);
    __free_custom_ptr(config->header);
    __free_custom_ptr(config->content);
    __free_custom_ptr(config->username);
    __free_custom_ptr(config->password);
}
