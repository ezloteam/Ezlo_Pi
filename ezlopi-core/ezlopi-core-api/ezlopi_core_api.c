#include <time.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_api.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_api_methods.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_cjson_macros.h"

static int __check_for_no_error(cJSON *cj_request);
static cJSON *__execute_method(cJSON *cj_request, f_method_func_t method_func);

///////////////////////////////////////////////////////////////////////////////////
cJSON *ezlopi_core_api_consume(const char *who, const char *payload, uint32_t len)
{
    cJSON *cj_response = NULL;

    if (payload && len)
    {
        cJSON *cj_request = cJSON_ParseWithLength(who, payload, len);
        // cJSON *cj_request = cJSON_ParseWithRefWithLength(who, payload, len);
        if (cj_request)
        {
            cj_response = ezlopi_core_api_consume_cjson(who, cj_request);
            cJSON_Delete(__FUNCTION__, cj_request);
        }
    }
    return cj_response;
}

cJSON *ezlopi_core_api_consume_cjson(const char *who, cJSON *cj_request)
{
    cJSON *cj_response = NULL;

    if (cj_request)
    {
        if (__check_for_no_error(cj_request))
        {
            cJSON *cj_id = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_id_str);
            cJSON *cj_sender = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_sender_str);
            cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str);

            uint32_t method_id = ezlopi_core_ezlopi_methods_search_in_list(cj_method);

            if (UINT32_MAX != method_id)
            {
                f_method_func_t method = ezlopi_core_ezlopi_methods_get_by_id(method_id);
                if (method)
                {
                    cj_response = __execute_method(cj_request, method);

                    if (cj_response)
                    {
                        cJSON_AddNullToObject(__FUNCTION__, cj_response, ezlopi_error_str);
                        cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_id_str, cJSON_Duplicate(__FUNCTION__, cj_id, true));
                        cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_sender_str, cJSON_Duplicate(__FUNCTION__, cj_sender, true));
                        cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_method_str, cJSON_Duplicate(__FUNCTION__, cj_method, true));
                    }
                }

                f_method_func_t updater = ezlopi_core_ezlopi_methods_get_updater_by_id(method_id);
                if (updater)
                {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                    TRACE_W("updater function: %p", updater);
#endif

                    cJSON *cj_update_response = __execute_method(cj_request, updater);

                    if (cj_update_response)
                    {
                        time_t now = 0;
                        time(&now);
                        cJSON_AddNumberToObject(__FUNCTION__, cj_update_response, ezlopi_startTime_str, now);
                        // cJSON_AddNullToObject(__FUNCTION__, cj_update_response, ezlopi_error_str);

                        if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(cj_update_response))
                        {
                            cJSON_Delete(__FUNCTION__, cj_update_response);
                        }
                    }
                }
            }
            else
            {
                cj_response = __execute_method(cj_request, ezlopi_core_ezlopi_methods_rpc_method_notfound);

                if (cj_response)
                {
                    cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_id_str, cJSON_Duplicate(__FUNCTION__, cj_id, true));
                    cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_sender_str, cJSON_Duplicate(__FUNCTION__, cj_sender, true));
                    cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_method_str, cJSON_Duplicate(__FUNCTION__, cj_method, true));
                }

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                CJSON_TRACE("x-cj_response", cj_response);
#endif
            }
        }

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        else
        {
            cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str);
            // TRACE_E("## WS Rx <<<<<<<<<< '%s' \n %.*s", (NULL != cj_method) ? (cj_method->valuestring ? cj_method->valuestring : ezlopi__str) : ezlopi__str, len, payload);
        }
#endif
    }

    return cj_response;
}

static cJSON *__execute_method(cJSON *cj_request, f_method_func_t method_func)
{
    cJSON *cj_response = NULL;
    if (method_func)
    {
        if (ezlopi_core_elzlopi_methods_check_method_register(method_func))
        {
            method_func(cj_request, NULL);
            ezlopi_event_group_set_event(EZLOPI_EVENT_NMA_REG);
        }
        else
        {
            cj_response = cJSON_CreateObject(__FUNCTION__);
            if (NULL != cj_response)
            {
                method_func(cj_request, cj_response);
            }
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            else
            {
                TRACE_E("Error - cj_response: %d", (uint32_t)cj_response);
            }
#endif
        }
    }

    return cj_response;
}

static uint32_t __register_fail_count = 0;

static int __check_for_no_error(cJSON *cj_request)
{
    int ret = 0;

    if (cj_request)
    {
        cJSON *cj_error = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_error_str);
        if ((NULL == cj_error) || (cJSON_NULL == cj_error->type) ||
            ((NULL != cj_error->valuestring) && (0 == strncmp(cj_error->valuestring, ezlopi_null_str, 4))))
        {
            ret = 1;
        }
#if (1 == ENABLE_TRACE)
        else
        {
            TRACE_E("cj_error: %p, cj_error->type: %u, cj_error->value_string: %s", cj_error, cj_error->type, cj_error ? (cj_error->valuestring ? cj_error->valuestring : ezlopi_null_str) : ezlopi_null_str);
        }
#endif
        cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str);
        if (cj_method && cj_method->valuestring)
        {
            if (0 == strncmp(cj_method->valuestring, "register", cj_method->str_value_len > 8 ? cj_method->str_value_len : 8))
            {
                __register_fail_count++;
                if (__register_fail_count > 5)
                {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                    TRACE_E("Registration failed for 5 times! Rebooting device!");
#endif
                    esp_restart();
                }
            }
        }
    }

    return ret;
}