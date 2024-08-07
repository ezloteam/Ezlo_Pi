#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>
#include "ezlopi_util_trace.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_scenes_expressions.h"
#include "ezlopi_core_scenes_then_methods_helper_func.h"
#include "EZLOPI_USER_CONFIG.h"

#define STR_SIZE(str) ((NULL != str) ? (strlen(str)) : 0)

/**
 * @brief Frees an address => '*ptr', pointing to an occupied address in heap.
 */
#define FREE_PTR_IF_NOT_NULL(ptr) \
    {                             \
        if (ptr)                  \
        {                         \
            ezlopi_free(__FUNCTION__, ptr);            \
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

#if 0
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
                    char* tmp_string = ezlopi_malloc(__FUNCTION__, length); // tmp_string != NULL
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
                const char* ptr = field_value_string;
                length = (int)strlen(field_value_string) - (int)(start - ptr);
                if (length > 0)
                {
                    length++;                                         // include null character
                    char* tmp_string = ezlopi_malloc(__FUNCTION__, length); // tmp_string != NULL
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
        char* append_str = ezlopi_malloc(__FUNCTION__, append_size); // append_str != NULL
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
            int port = 80;  // Port field of the HTTP uri if found
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
        cJSON* userItem = cJSON_GetObjectItem(__FUNCTION__, cj_value, "user");
        cJSON* passwordItem = cJSON_GetObjectItem(__FUNCTION__, cj_value, "password");
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
    FREE_PTR_IF_NOT_NULL(config->target_page);
    FREE_PTR_IF_NOT_NULL(config->header);
    FREE_PTR_IF_NOT_NULL(config->content);
    FREE_PTR_IF_NOT_NULL(config->username);
    FREE_PTR_IF_NOT_NULL(config->password);
    FREE_PTR_IF_NOT_NULL(config->response);
}


//------------------------------ SetExpression / SetVariable -------------------------------------------------------
int ezlopi_core_scene_then_helper_setexpression_setvariable(char * expression_name, const char * code_str, const char * value_type, cJSON * cj_metadata, cJSON * cj_params, l_fields_v2_t * var_value)
{
    int ret = 0;
    s_ezlopi_expressions_t* curr_expr = ezlopi_scenes_get_expression_node_by_name(expression_name);
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
            }

            curr_expr->meta_data = cJSON_Duplicate(__FUNCTION__, cj_metadata, 1);
        }

        // 4. replace Params CJSON
        if (cj_params)
        {
            cJSON* cj_new_items = NULL;
            if ((NULL != curr_expr->items) && (NULL != (cj_new_items = cJSON_GetObjectItem(__FUNCTION__, cj_params, "items"))))
            {
                ezlopi_scenes_expressions_delete_exp_item(curr_expr->items);
                __get_expressions_items(curr_expr, cj_new_items);
            }
            cJSON* cj_new_device_item_names = NULL;
            if ((NULL != curr_expr->device_item_names) && (NULL != (cj_new_device_item_names = cJSON_GetObjectItem(__FUNCTION__, cj_params, "device_item_names"))))
            {
                ezlopi_scenes_expressions_delete_exp_device_item_names(curr_expr->device_item_names);
                __get_expressions_device_item_names(curr_expr, cj_new_device_item_names);
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

                    const char * variable_str = var_value->field_value.u_value.value_string;

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
        // ezlopi_scenes_expressions_print(ezlopi_scenes_get_expression_node_by_name(expression_name));
        // TRACE_S("----------------- ll --------------------");

        // 5. Now to edit in nvs
        // A. read from  nvs
        char* exp_id_list_str = ezlopi_nvs_read_scenes_expressions();
        if (exp_id_list_str)
        {
            TRACE_D("exp_id_list_str: %s", exp_id_list_str);
            cJSON* cj_exp_id_list = cJSON_Parse(__FUNCTION__, exp_id_list_str);
            if (cj_exp_id_list)
            {
                uint32_t exp_idx = 0;
                cJSON* cj_exp_id = NULL;
                while (NULL != (cj_exp_id = cJSON_GetArrayItem(cj_exp_id_list, exp_idx++)))
                {
                    if (cj_exp_id && cj_exp_id->valuestring)
                    {
                        uint32_t exp_id = strtoul(cj_exp_id->valuestring, NULL, 16);
                        if (exp_id == curr_expr->exp_id)
                        {
                            TRACE_S("Found [%#x] in nvs ; req[%#x]", exp_id, curr_expr->exp_id);
                            ret = ezlopi_scenes_expressions_update_nvs(cj_exp_id->valuestring, generate_expression_node_in_cjson(curr_expr));
#if 0
                            char* exp_str = ezlopi_nvs_read_str(cj_exp_id->valuestring); // modify and store this 'cj_exp'
                            if (exp_str)
                            {
                                // This new 'cj_nvs_exp' holds expression data
                                cJSON* cj_nvs_exp = cJSON_Parse(__FUNCTION__, exp_str);
                                if (cj_nvs_exp)
                                {
                                    // 1. code
                                    if (code_str)
                                    {
                                        cJSON  * get_cj_code = cJSON_GetObjectItem(__FUNCTION__, cj_nvs_exp, "code");
                                        if (get_cj_code && get_cj_code->valuestring)
                                        {
                                            cJSON_Delete(__FUNCTION__, get_cj_code);
                                        }
                                        cJSON_AddStringToObject(__FUNCTION__, cj_nvs_exp, "code", code_str);
                                    }

                                    TRACE_D("here!");
                                    // 2. valueType
                                    if (value_type)
                                    {
                                        cJSON  * get_cj_valueType = cJSON_GetObjectItem(__FUNCTION__, cj_nvs_exp, "valueType");
                                        if (get_cj_valueType && get_cj_valueType->valuestring)
                                        {
                                            cJSON_Delete(__FUNCTION__, get_cj_valueType);
                                        }
                                        cJSON_AddStringToObject(__FUNCTION__, cj_nvs_exp, "valueType", value_type);
                                    }

                                    TRACE_D("here!");
                                    // 3. metadata
                                    if (cj_metadata)
                                    {
                                        cJSON  * get_cj_metadata = cJSON_GetObjectItem(__FUNCTION__, cj_nvs_exp, "metadata");
                                        if (get_cj_metadata && cJSON_IsObject(get_cj_metadata))
                                        {
                                            cJSON_Delete(__FUNCTION__, get_cj_metadata);
                                        }
                                        cJSON_AddItemToObject(__FUNCTION__, cj_nvs_exp, "metadata", cJSON_Duplicate(__FUNCTION__, cj_metadata, 1));
                                    }

                                    TRACE_D("here!");
                                    // 4. params
                                    if (cj_params)
                                    {
                                        cJSON  * get_cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_nvs_exp, "params");
                                        if (get_cj_params && cJSON_IsObject(get_cj_params))
                                        {
                                            cJSON_Delete(__FUNCTION__, get_cj_params);
                                        }
                                        cJSON_AddItemToObject(__FUNCTION__, cj_nvs_exp, "params", cJSON_Duplicate(__FUNCTION__, cj_params, 1));
                                    }

                                    TRACE_D("here!");
                                    // 4. value (int/bool/string/cjson)
                                    if (NULL != var_value)
                                    {
                                        int req_type_nvs = 0;
                                        cJSON  * get_cj_var_value = cJSON_GetObjectItem(__FUNCTION__, cj_nvs_exp, "value");
                                        if (get_cj_var_value && cJSON_IsObject(get_cj_var_value))
                                        {
                                            req_type_nvs = get_cj_var_value->type;
                                            cJSON_Delete(__FUNCTION__, get_cj_var_value);
                                        }

                                        switch (req_type_nvs)
                                        {
                                        case cJSON_Number:
                                        {
                                            cJSON_AddNumberToObject(__FUNCTION__, cj_nvs_exp, "value", var_value->field_value.u_value.value_double);
                                            break;
                                        }
                                        case cJSON_String:
                                        {
                                            cJSON_AddStringToObject(__FUNCTION__, cj_nvs_exp, "value", var_value->field_value.u_value.value_string);
                                            break;
                                        }
                                        case cJSON_False:
                                        case cJSON_True:
                                        {
                                            cJSON_AddBoolToObject(__FUNCTION__, cj_nvs_exp, "value", var_value->field_value.u_value.value_bool);
                                            break;
                                        }
                                        case cJSON_Object:
                                        case cJSON_Array:
                                        {
                                            cJSON_AddItemToObject(__FUNCTION__, cj_nvs_exp, "value", cJSON_Duplicate(__FUNCTION__, var_value->field_value.u_value.cj_value, 1));
                                            break;
                                        }
                                        default:
                                            break;
                                        }
                                    }

                                    // printing the new/modified expression 'cj_nvs_exp'
                                    // CJSON_TRACE("updated_expr", cj_nvs_exp);

                                    TRACE_D("here!");

                                    // store the  modified expression into nvs
                                    char* updated_exp_string = cJSON_PrintBuffered(__FUNCTION__, cj_nvs_exp, 2 * 1024, false);
                                    TRACE_D("length of 'updated_exp_string': %d", strlen(updated_exp_string));
                                    if (updated_exp_string)
                                    {
                                        // delete the expression from nvs ; 

                                        ezlopi_nvs_delete_stored_data_by_name(cj_exp_id->valuestring);  // 0x...exp_id
                                        if (ezlopi_nvs_write_str(updated_exp_string, strlen(updated_exp_string), cj_exp_id->valuestring))
                                        {
                                            ret = 1;
                                            TRACE_S("successfully saved/modified expression in nvs");
                                        }
                                        ezlopi_free(__FUNCTION__, updated_exp_string);
                                    }



                                    cJSON_Delete(__FUNCTION__, cj_nvs_exp);
                                }


                                ezlopi_free(__FUNCTION__, exp_str);
                            }
#endif     

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
#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS