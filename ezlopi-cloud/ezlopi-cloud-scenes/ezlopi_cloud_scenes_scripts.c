#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "cjext.h"
#include <stdlib.h>

#include "ezlopi_cloud_scenes_scripts.h"
#include "ezlopi_core_scenes_scripts.h"
#include "ezlopi_cloud_constants.h"

void scenes_scripts_list(cJSON* cj_request, cJSON* cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON* cj_result = cJSON_AddArrayToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            l_ezlopi_scenes_script_t* script_nodes = ezlopi_scenes_scripts_get_head();
            while (script_nodes)
            {
                cJSON* cj_script = cJSON_CreateObject(__FUNCTION__);
                if (cj_script)
                {
                    char script_id_str[32];
                    snprintf(script_id_str, sizeof(script_id_str), "%08x", script_nodes->id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_script, ezlopi__id_str, script_id_str);
                    cJSON_AddStringToObject(__FUNCTION__, cj_script, ezlopi_name_str, script_nodes->name);

                    if (!cJSON_AddItemToArray(cj_result, cj_script))
                    {
                        cJSON_Delete(__FUNCTION__, cj_script);
                    }
                }

                script_nodes = script_nodes->next;
            }
        }
    }
}

void scenes_scripts_get(cJSON* cj_request, cJSON* cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            uint32_t script_id_num = 0;
            cJSON* cj_script_id = NULL;
            cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cj_script_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
                if (cj_script_id && cj_script_id->valuestring)
                {
                    script_id_num = strtoul(cj_script_id->valuestring, NULL, 16);
                }
            }

            if (script_id_num)
            {
                l_ezlopi_scenes_script_t* script_nodes = ezlopi_scenes_scripts_get_head();
                while (script_nodes)
                {
                    if (script_id_num == script_nodes->id)
                    {
                        cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi__id_str, cJSON_Duplicate(__FUNCTION__, cj_script_id, true));
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, script_nodes->name);
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_code_str, script_nodes->code);

                        break;
                    }

                    script_nodes = script_nodes->next;
                }
            }
        }
    }
}

void scenes_scripts_add(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        uint32_t script_id = ezlopi_scenes_scripts_add_to_head(0, cj_params);

        cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            if (script_id)
            {
                char tmp_id_str[32];
                snprintf(tmp_id_str, sizeof(tmp_id_str), "%08x", script_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, tmp_id_str);
            }
        }
    }
}

void scenes_scripts_delete(cJSON* cj_request, cJSON* cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

        uint32_t script_id_num = 0;
        cJSON* cj_script_id = NULL;

        cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cj_script_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
            if (cj_script_id && cj_script_id->valuestring)
            {
                script_id_num = strtoul(cj_script_id->valuestring, NULL, 16);
                ezlopi_scenes_scripts_stop_by_id(script_id_num);
                ezlopi_scenes_scripts_delete_by_id(script_id_num);
            }
        }
    }
}

void scenes_scripts_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        ezlopi_scenes_scripts_update(cj_params);
    }
}

void scenes_scripts_run(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_script_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (cj_script_id && cj_script_id->valuestring)
        {
            uint32_t script_id = strtoul(cj_script_id->valuestring, NULL, 16);
            if (script_id)
            {
                ezlopi_scenes_scripts_run_by_id(script_id);
            }
        }
    }
}
#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS