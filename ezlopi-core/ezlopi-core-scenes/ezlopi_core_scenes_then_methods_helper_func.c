#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>
#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_device_group.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_scenes_expressions.h"
#include "ezlopi_core_scenes_then_methods_helper_func.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"
#include "EZLOPI_USER_CONFIG.h"

#define STR_SIZE(str) ((NULL != str) ? (strlen(str)) : 0)

/**
 * @brief Frees an address => '*ptr', pointing to an occupied address in heap.
 */
#define FREE_PTR_IF_NOT_NULL(ptr)           \
    {                                       \
        if (ptr)                            \
        {                                   \
            ezlopi_free(__FUNCTION__, ptr); \
            ptr = NULL;                     \
        }                                   \
    }

 /**
  * @brief This funtion is called, only to reallocate a '*header' of custom_structure 's_ezlopi_core_http_mbedtls_t'
  *
  * @param tmp_http_data     [ Pointer to (s_ezlopi_core_http_mbedtls_t*) block of memory. ]
  * @param append_size       [ Size of 'string' to be appended. ]
  * @param append_str        [ 'string_literal' to be appended. ]
  * @return int [ Fail ==> returns Old-size / Success ==> returns New-size ]
  */
static int __ezlopi_core_scenes_then_sendhttp_relloc_header(s_ezlopi_core_http_mbedtls_t *tmp_http_data, int append_size, const char *append_str)
{
    int ret = (int)tmp_http_data->header_maxlen; // Assign Old-block size as default
    int new_size = append_size + (ret + 1);

    uint8_t retry = 5;
    do
    {
        if (EZPI_SUCCESS == ezlopi_core_http_dyna_relloc(&(tmp_http_data->header), new_size)) // rellocate: 'tmp_http_data->header' with  'new_size'
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

#if 0
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
                length = (end - start);
                if (length > 0)
                {
                    length++;                                         // include null character
                    char *tmp_string = ezlopi_malloc(__FUNCTION__, length); // tmp_string != NULL
                    if (tmp_string)
                    {
                        bzero(tmp_string, length);
                        snprintf(tmp_string, length, "%s", start);
                        // TRACE_I("web_host_name : %s", tmp_string);
                        tmp_http_data->web_server_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->web_server), tmp_string);
                        ezlopi_free(__FUNCTION__, tmp_string);
                    }
                }
            }
            else
            {
                const char *ptr = field_value_string;
                length = (int)strlen(field_value_string) - (int)(start - ptr);
                if (length > 0)
                {
                    length++;                                         // include null character
                    char *tmp_string = ezlopi_malloc(__FUNCTION__, length); // tmp_string != NULL
                    if (tmp_string)
                    {
                        bzero(tmp_string, length);
                        snprintf(tmp_string, length, "%s", (ptr + ((int)(start - ptr))));
                        // TRACE_I("web_host_name : %s", tmp_string);
                        tmp_http_data->web_server_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->web_server), tmp_string);
                        ezlopi_free(__FUNCTION__, tmp_string);
                    }
                }
            }
        }
    }
}
#endif

/**
 * @brief Function to Clear and Malloc the header_member (within 's_ezlopi_core_http_mbedtls_t') only.
 * @return Size of content in 's_ezlopi_core_http_mbedtls_t'->header
 */
static int __ezlopi_core_scenes_then_create_fresh_header(s_ezlopi_core_http_mbedtls_t *tmp_http_data)
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
static void __ezlopi_core_scenes_then_append_to_header(s_ezlopi_core_http_mbedtls_t *tmp_http_data, const char *str1, const char *str2)
{
    int append_size = (STR_SIZE(str1) + 4 + STR_SIZE(str2)) + 6;
    int max_allowed = ezlopi_core_http_calc_empty_bufsize(tmp_http_data->header, (tmp_http_data->header_maxlen), append_size);
    if (max_allowed > 0)
    {
        snprintf(tmp_http_data->header + STR_SIZE(tmp_http_data->header), max_allowed, "%s: %s\r\n", str1, str2);
    }
    else // We reallocate:- 'tmp_http_data->header'
    {
        char *append_str = ezlopi_malloc(__FUNCTION__, append_size); // append_str != NULL
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
            ezlopi_free(__FUNCTION__, append_str);
        }
    }
}

//------------------------------- ezlopi_scene_then_sendhttp_request -----------------------------------------------
void parse_http_request_type(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field)
{
    const char *field_value_string = curr_field->field_value.u_value.value_string;
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
void parse_http_url(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field)
{
    const char *field_value_string = curr_field->field_value.u_value.value_string;
    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != field_value_string))
    {
        // TRACE_W("Here! fresh url");
        tmp_http_data->url_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->url), field_value_string);
        if (tmp_http_data->url_maxlen > 0)
        {
            //--------------------------------------------
            // tmp_http_data->web_port = ((NULL != strstr(field_value_string, "https")) ? 443 : 80);
            // __ezlopi_core_scenes_then_sendhttp_parse_host_name(tmp_http_data, field_value_string);
            //--------------------------------------------

            // tests[0] = "http://www.testhttp.com:8090/foo_page";
            // tests[1] = "https://www.testhttp.com:8090/foo_page";
            // tests[2] = "http://www.testhttp.com/foo_page";
            // tests[3] = "https://www.testhttp.com/foo_page";
            // tests[4] = "http://www.testhttp.com:8090";
            // tests[5] = "https://www.testhttp.com:8090";
            // tests[6] = "http://www.testhttp.com";
            // tests[7] = "https://www.testhttp.com";
            int succ_parsing = 0; // Whether the parsing has been
            int port = 80;        // Port field of the HTTP uri if found
            char host[100] = { 0 }; // IP field of the HTTP uri
            char page[200] = { 0 }; // Page field of the uri if found
            if (sscanf(field_value_string, "http://%99[^:]:%i/%199[^\n]", host, &port, page) == 3)
            {
                succ_parsing = 1;
            }
            else if (sscanf(field_value_string, "https://%99[^:]:%i/%199[^\n]", host, &port, page) == 3)
            {
                // port = 443;
                succ_parsing = 1;
            }
            else if (sscanf(field_value_string, "http://%99[^:]:%i[^\n]", host, &port) == 2)
            {
                succ_parsing = 1;
            }
            else if (sscanf(field_value_string, "https://%99[^:]:%i[^\n]", host, &port) == 2)
            {
                // port = 443;
                succ_parsing = 1;
            }
            else if (sscanf(field_value_string, "http://%99[^/]/%199[^\n]", host, page) == 2)
            {
                succ_parsing = 1;
            }
            else if (sscanf(field_value_string, "https://%99[^/]/%199[^\n]", host, page) == 2)
            {
                port = 443;
                succ_parsing = 1;
            }
            else if (sscanf(field_value_string, "http://%99[^\n]", host) == 1)
            {
                succ_parsing = 1;
            }
            else if (sscanf(field_value_string, "https://%99[^\n]", host) == 1)
            {
                port = 443;
                succ_parsing = 1;
            }
            else if (sscanf(field_value_string, "%*[^:]%*[:/]%[^:]:%d%s", host, &port, page) == 3)
            {
                succ_parsing = 1;
            }
            else
            {
                TRACE_E("ERROR !! Unknown REQUEST-format. ");
            }

            TRACE_W("url scan status: %d\n", succ_parsing);

            if (succ_parsing)
            {
                TRACE_D("*source = '%s'", field_value_string);
                TRACE_S("host = '%s'", host);
                TRACE_S("port = '%d'", port);
                TRACE_S("page = '%s'", page);

                tmp_http_data->web_server_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->web_server), host);
                tmp_http_data->targe_page_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->target_page), page);
                tmp_http_data->web_port = port;
            }
            else
            {
                TRACE_E("failed to parse URL");
            }

            int content_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
            if (content_size > 0) // if this characters exsists in the 'tmp_http_data->header'
            {
                // 1. adding 'host' to header-buffer
                // TRACE_W("Appending!! webserver -> header");
                __ezlopi_core_scenes_then_append_to_header(tmp_http_data, "Host", host);
            }
            else
            {
                TRACE_E("Failed Creating header for 'Host:'");
            }
        }
    }
}
void parse_http_content_type(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field)
{
    const char *field_value_string = curr_field->field_value.u_value.value_string;
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
void parse_http_content(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field)
{
    const char *field_value_string = curr_field->field_value.u_value.value_string;
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
                    char str[10 + 1];
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
void parse_http_headers(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field)
{
    const cJSON *cj_value = curr_field->field_value.u_value.cj_value;
    if ((EZLOPI_VALUE_TYPE_DICTIONARY == curr_field->value_type) && cJSON_IsObject(cj_value))
    {
        // TRACE_W("Here! headers");
        int content_size = __ezlopi_core_scenes_then_create_fresh_header(tmp_http_data);
        if (content_size > 0) // if this characters exsists in the 'tmp_http_data->header'
        {
            // TRACE_W("Appending!! headers -> header");
            cJSON *header = (cj_value->child);
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
void parse_http_skipsecurity(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field)
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
            __ezlopi_core_scenes_then_append_to_header(tmp_http_data, "skipSecurity", ((curr_field->field_value.u_value.value_bool) ? ezlopi_true_str : ezlopi_false_str));
        }
        else
        {
            TRACE_E("Failed Creating header for 'skipSecurity:'");
        }
    }
}
void parse_http_creds(s_ezlopi_core_http_mbedtls_t *tmp_http_data, l_fields_v2_t *curr_field)
{
    const cJSON *cj_value = curr_field->field_value.u_value.cj_value;
    if ((EZLOPI_VALUE_TYPE_CREDENTIAL == curr_field->value_type) && cJSON_IsObject(cj_value))
    {
        cJSON *userItem = cJSON_GetObjectItem(__FUNCTION__, cj_value, "user");
        cJSON *passwordItem = cJSON_GetObjectItem(__FUNCTION__, cj_value, "password");
        if ((NULL != userItem) && (NULL != passwordItem))
        {
            const char *userValue = cJSON_GetStringValue(userItem);
            const char *passValue = cJSON_GetStringValue(passwordItem);

            // TRACE_W("Here! credential");
            tmp_http_data->username_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->username), userValue);
            tmp_http_data->password_maxlen = (uint16_t)ezlopi_core_http_mem_malloc(&(tmp_http_data->password), passValue);
        }
    }
}
void free_http_mbedtls_struct(s_ezlopi_core_http_mbedtls_t *config)
{
    FREE_PTR_IF_NOT_NULL(config->url);
    FREE_PTR_IF_NOT_NULL(config->web_server);
    FREE_PTR_IF_NOT_NULL(config->target_page);
    FREE_PTR_IF_NOT_NULL(config->header);
    FREE_PTR_IF_NOT_NULL(config->content);
    FREE_PTR_IF_NOT_NULL(config->username);
    FREE_PTR_IF_NOT_NULL(config->password);
    FREE_PTR_IF_NOT_NULL(config->response);
}

//------------------------------ SetExpression / SetVariable -------------------------------------------------------
ezlopi_error_t ezlopi_core_scene_then_helper_setexpression_setvariable(char *expression_name, const char *code_str, const char *value_type, cJSON *cj_metadata, cJSON *cj_params, l_fields_v2_t *var_value)
{
    ezlopi_error_t ret = EZPI_FAILED;
    s_ezlopi_expressions_t *curr_expr = EZPI_scenes_expressions_get_node_by_name(expression_name);
    if (curr_expr)
    {
        // 1. set new code
        if (code_str)
        {
            if (NULL != curr_expr->code)
            {
                ezlopi_free(__FUNCTION__, curr_expr->code);
                curr_expr->code = NULL;
            }
            curr_expr->code = ezlopi_malloc(__FUNCTION__, strlen(code_str) + 1);
            if (curr_expr->code)
            {
                snprintf(curr_expr->code, strlen(code_str) + 1, "%.*s", strlen(code_str), code_str);
            }
        }

        // 2. set new valueType
        if (value_type)
        {
            curr_expr->value_type = ezlopi_scene_get_scene_value_type_enum(value_type);
        }

        // 3. replace metadata CJSON
        if (cj_metadata)
        {
            if (NULL != curr_expr->meta_data)
            {
                cJSON_Delete(__FUNCTION__, curr_expr->meta_data);
                curr_expr->meta_data = NULL;
            }

            curr_expr->meta_data = cJSON_Duplicate(__FUNCTION__, cj_metadata, 1);
        }

        // 4. replace Params CJSON
        if (cj_params)
        {
            cJSON *cj_new_items = NULL;
            if ((NULL != curr_expr->items) && (NULL != (cj_new_items = cJSON_GetObjectItem(__FUNCTION__, cj_params, "items"))))
            {
                if (curr_expr->items)
                {
                    EZPI_scenes_expressions_delete_items(curr_expr->items);
                    curr_expr->items->next = NULL;
                    curr_expr->items = NULL;
                }

                EZPI_scenes_expressions_populate_items(curr_expr, cj_new_items);
            }
            cJSON *cj_new_device_item_names = NULL;
            if ((NULL != curr_expr->device_item_names) && (NULL != (cj_new_device_item_names = cJSON_GetObjectItem(__FUNCTION__, cj_params, "device_item_names"))))
            {
                if (curr_expr->device_item_names)
                {
                    EZPI_scenes_expressions_delete_devitem_names(curr_expr->device_item_names);
                    curr_expr->device_item_names->next = NULL;
                    curr_expr->device_item_names = NULL;
                }
                EZPI_scenes_expressions_populate_devitem_names(curr_expr, cj_new_device_item_names);
            }
        }

        // 4. replace the variable value ( if its variable_expression )
        if (NULL != var_value)
        {
            switch (var_value->field_value.e_type)
            {
            case VALUE_TYPE_NUMBER:
            {
                if (EXPRESSION_VALUE_TYPE_NUMBER == curr_expr->exp_value.type)
                {
                    curr_expr->exp_value.u_value.number_value = var_value->field_value.u_value.value_double;
                }

                break;
            }
            case VALUE_TYPE_STRING:
            {
                if (EXPRESSION_VALUE_TYPE_STRING == curr_expr->exp_value.type)
                {
                    if (NULL != curr_expr->exp_value.u_value.str_value)
                    {
                        ezlopi_free(__FUNCTION__, curr_expr->exp_value.u_value.str_value);
                        curr_expr->exp_value.u_value.str_value = NULL;
                    }

                    const char *variable_str = var_value->field_value.u_value.value_string;

                    curr_expr->exp_value.u_value.str_value = ezlopi_malloc(__FUNCTION__, strlen(variable_str) + 1);
                    if (curr_expr->exp_value.u_value.str_value)
                    {
                        snprintf(curr_expr->exp_value.u_value.str_value, strlen(variable_str) + 1, "%.*s", strlen(variable_str), variable_str);
                    }
                }
                break;
            }
            case VALUE_TYPE_BOOL:
            {
                if (EXPRESSION_VALUE_TYPE_BOOL == curr_expr->exp_value.type)
                {
                    curr_expr->exp_value.u_value.boolean_value = var_value->field_value.u_value.value_bool;
                }
                break;
            }
            case VALUE_TYPE_CJSON:
            {
                if (EXPRESSION_VALUE_TYPE_CJ == curr_expr->exp_value.type)
                {
                    if (NULL != curr_expr->exp_value.u_value.cj_value)
                    {
                        cJSON_Delete(__FUNCTION__, curr_expr->exp_value.u_value.cj_value);
                    }

                    curr_expr->exp_value.u_value.cj_value = cJSON_Duplicate(__FUNCTION__, var_value->field_value.u_value.cj_value, 1);
                }
                break;
            }
            default:
                break;
            }
        }

        // TRACE_S("-------------- Updated_exp_name : %s  ------------", expression_name);
        // EZPI_scenes_expressions_print(EZPI_scenes_expressions_get_node_by_name(expression_name));
        // TRACE_S("----------------- ll --------------------");

        // 5. Now to edit in nvs
        // A. read from  nvs
        char *exp_id_list_str = ezlopi_nvs_read_scenes_expressions();
        if (exp_id_list_str)
        {
            TRACE_D("exp_id_list_str: %s", exp_id_list_str);
            cJSON *cj_exp_id_list = cJSON_Parse(__FUNCTION__, exp_id_list_str);
            if (cj_exp_id_list)
            {
                cJSON *cj_exp_id = NULL;
                cJSON_ArrayForEach(cj_exp_id, cj_exp_id_list)
                {
                    if (cj_exp_id && cj_exp_id->valuestring)
                    {
                        uint32_t exp_id = strtoul(cj_exp_id->valuestring, NULL, 16);
                        if (exp_id == curr_expr->exp_id)
                        {
                            TRACE_S("Found [%#x] in nvs ; req[%#x]", exp_id, curr_expr->exp_id);
                            ret = EZPI_scenes_expressions_update_nvs(cj_exp_id->valuestring, EZPI_scenes_expressions_get_cjson(curr_expr));
                            break;
                        }
                    }
                }
                cJSON_Delete(__FUNCTION__, cj_exp_id_list);
            }
            ezlopi_free(__FUNCTION__, exp_id_list_str);
        }
    }
    return ret;
}

//------------------------------ toggleValue + grouptoggleValue -------------------------------------------------------

int ezlopi_core_scene_then_helper_toggleValue(uint32_t item_id, const char *item_id_str)
{
    int ret = 0;
    if (item_id && item_id_str)
    {
        l_ezlopi_item_t *curr_item = ezlopi_device_get_item_by_id(item_id);
        if ((curr_item) && (EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT == curr_item->interface_type))
        {
            cJSON *cj_tmp_value = cJSON_CreateObject(__FUNCTION__);
            if (cj_tmp_value)
            {
                if (curr_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item, (void *)cj_tmp_value, NULL))
                {
                    CJSON_TRACE("present_bool_gpio_value", cj_tmp_value); /*value formatted & value only*/

                    cJSON *cj_val = cJSON_GetObjectItem(__FUNCTION__, cj_tmp_value, ezlopi_value_str);
                    if (cj_val)
                    {
                        cJSON *cj_result_value = cJSON_CreateObject(__FUNCTION__);
                        if (cj_result_value)
                        {
                            cJSON_AddStringToObject(__FUNCTION__, cj_result_value, ezlopi__id_str, item_id_str);

                            if ((EZPI_STRNCMP_IF_EQUAL(curr_item->cloud_properties.value_type, value_type_bool, strlen(curr_item->cloud_properties.value_type) + 1, 5)) && cJSON_IsBool(cj_val))
                            {
                                // TRACE_S("1. getting 'item_id[%d]' ; bool_value = %s ", item_id, (cj_val->type == cJSON_True) ? ezlopi_true_str : ezlopi_false_str); // ezlopi_false_str or ezlopi_true_str
                                if (cj_val->type == cJSON_True)
                                {
                                    cJSON_AddBoolToObject(__FUNCTION__, cj_result_value, ezlopi_value_str, false);
                                }
                                else if (cj_val->type == cJSON_False)
                                {
                                    cJSON_AddBoolToObject(__FUNCTION__, cj_result_value, ezlopi_value_str, true);
                                }
                                ret = 1;
                                curr_item->func(EZLOPI_ACTION_SET_VALUE, curr_item, cj_result_value, curr_item->user_arg);
                            }
                            else if ((EZPI_STRNCMP_IF_EQUAL(curr_item->cloud_properties.value_type, value_type_int, strlen(curr_item->cloud_properties.value_type) + 1, 4)) && cJSON_IsNumber(cj_val))
                            {
                                // TRACE_S("2. getting 'item_id[%d]' ; int_value = %d ", item_id, (int)cj_val->valuedouble);
                                if (cj_val->valuedouble == 0) // either  '0' or '1'.
                                {
                                    cJSON_AddNumberToObject(__FUNCTION__, cj_result_value, ezlopi_value_str, 1);
                                }
                                else if (cj_val->valuedouble == 1)
                                {
                                    cJSON_AddNumberToObject(__FUNCTION__, cj_result_value, ezlopi_value_str, 0);
                                }
                                ret = 1;
                                curr_item->func(EZLOPI_ACTION_SET_VALUE, curr_item, cj_result_value, curr_item->user_arg);
                            }
                            else
                            {
                                ret = 0;
                                TRACE_E(" 'item_id[%d]' neither 'boolean' nor 'int' ;  Value-type mis-matched!  ", item_id);
                            }

                            cJSON_Delete(__FUNCTION__, cj_result_value);
                        }
                    }
                }

                cJSON_Delete(__FUNCTION__, cj_tmp_value);
            }
        }
    }
    return ret;
}

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS