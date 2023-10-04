#include "cJSON.h"
#include <stdlib.h>

#include "ezlopi_cloud_constants.h"
#include "scenes_scripts.h"
#include "ezlopi_scenes_scripts.h"

void scenes_scripts_list(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

        cJSON *cj_result = cJSON_AddObjectToObject(cj_response, "result");
        if (cj_result)
        {
            cJSON *cj_script_array = cJSON_AddArrayToObject(cj_result, "scripts");
            if (cj_script_array)
            {
                l_ezlopi_scenes_script_t *script_nodes = ezlopi_scenes_scripts_get_head();
                while (script_nodes)
                {
                    cJSON *cj_script = cJSON_CreateObject();
                    if (cj_script)
                    {
                        char script_id_str[32];
                        snprintf(script_id_str, sizeof(script_id_str), "%08x", script_nodes->id);
                        cJSON_AddStringToObject(cj_script, ezlopi__id_str, script_id_str);
                        cJSON_AddStringToObject(cj_script, "name", script_nodes->name);

                        if (!cJSON_AddItemToArray(cj_script_array, cj_script))
                        {
                            cJSON_Delete(cj_script);
                        }
                    }

                    script_nodes = script_nodes->next;
                }
            }
        }
    }
}

void scenes_scripts_get(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

        cJSON *cj_result = cJSON_AddObjectToObject(cj_response, "result");
        if (cj_result)
        {
            uint32_t script_id_num = 0;
            cJSON *cj_script_id = NULL;
            cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params);
            if (cj_params)
            {
                cj_script_id = cJSON_GetObjectItem(cj_params, ezlopi__id_str);
                if (cj_script_id && cj_script_id->valuestring)
                {
                    script_id_num = strtoul(cj_script_id->valuestring, NULL, 16);
                }
            }

            if (script_id_num)
            {
                l_ezlopi_scenes_script_t *script_nodes = ezlopi_scenes_scripts_get_head();
                while (script_nodes)
                {
                    if (script_id_num == script_nodes->id)
                    {
                        cJSON_AddItemReferenceToObject(cj_result, ezlopi__id_str, cj_script_id);
                        cJSON_AddStringToObject(cj_result, "name", script_nodes->name);
                        cJSON_AddStringToObject(cj_result, "code", script_nodes->code);

                        break;
                    }

                    script_nodes = script_nodes->next;
                }
            }
        }
    }
}

void scenes_scripts_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params);
    if (cj_params)
    {
        uint32_t script_id = ezlopi_scenes_scripts_add_to_head(0, cj_params);

        cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
        if (cj_result)
        {
            if (script_id)
            {
                char tmp_id_str[32];
                snprintf(tmp_id_str, sizeof(tmp_id_str), "%08x", script_id);
                cJSON_AddStringToObject(cj_result, ezlopi__id_str, tmp_id_str);
            }
        }
    }
}

void scenes_scripts_delete(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
        cJSON_AddObjectToObject(cj_response, "result");

        uint32_t script_id_num = 0;
        cJSON *cj_script_id = NULL;

        cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params);
        if (cj_params)
        {
            cj_script_id = cJSON_GetObjectItem(cj_params, ezlopi__id_str);
            if (cj_script_id && cj_script_id->valuestring)
            {
                script_id_num = strtoul(cj_script_id->valuestring, NULL, 16);
                ezlopi_scenes_scripts_delete_by_id(script_id_num);
            }
        }
    }
}
