#include "cJSON.h"
#include "string.h"
#include "stdlib.h"

#include "trace.h"

#include "ezlopi_cloud.h"
#include "ezlopi_nvs.h"
#include "ezlopi_scenes_scripts.h"

static l_ezlopi_scenes_script_t *script_head = NULL;

static void ezlopi_scenes_scripts_nvs_parse(void);
static l_ezlopi_scenes_script_t *ezlopi_scenes_scripts_create_node(uint32_t script_id, cJSON *cj_script);

cJSON *test(void)
{
    TRACE_E("Here");
    cJSON *cj_new_scipt = cJSON_CreateObject();
    if (cj_new_scipt)
    {
        TRACE_E("Here");
        cJSON_AddStringToObject(cj_new_scipt, "name", "script-1");
        cJSON_AddStringToObject(cj_new_scipt, "code", "print(\"Hello World!\")");
    }

    return cj_new_scipt;
}

void ezlopi_scenes_scripts_init(void)
{
    ezlopi_scenes_scripts_nvs_parse();
}

uint32_t ezlopi_scenes_scripts_add_to_head(uint32_t script_id, cJSON *cj_script)
{
    uint32_t new_script_id = 0;
    if (script_head)
    {
        TRACE_E("Here");
        l_ezlopi_scenes_script_t *curr_script = script_head;
        while (curr_script->next)
        {
            TRACE_E("Here");
            curr_script = curr_script->next;
        }

        curr_script->next = ezlopi_scenes_scripts_create_node(script_id, cj_script);
        new_script_id = curr_script->next->id;
    }
    else
    {
        TRACE_E("Here");
        script_head = ezlopi_scenes_scripts_create_node(script_id, cj_script);
        new_script_id = script_head->id;
    }

    return new_script_id;
}

static void ezlopi_scenes_scripts_add_script_id(uint32_t script_id)
{
    bool free_str = true;
    char *script_ids_str = ezlopi_nvs_read_scenes_scripts();
    if (!script_ids_str)
    {
        TRACE_E("Here");
        free_str = false;
        script_ids_str = "[]";
    }

    TRACE_E("script_ids_str: %s", script_ids_str);

    cJSON *cj_script_ids = cJSON_Parse(script_ids_str);
    if (cj_script_ids)
    {
        TRACE_E("Here");
        cJSON *cj_script_id = cJSON_CreateNumber(script_id);
        if (cj_script_id)
        {
            TRACE_E("Here");
            if (cJSON_AddItemToArray(cj_script_ids, cj_script_id))
            {
                TRACE_E("Here");
                char *script_ids_str_updated = cJSON_Print(cj_script_ids);
                if (script_ids_str_updated)
                {
                    TRACE_E("Here");
                    ezlopi_nvs_write_scenes_scripts(script_ids_str_updated);
                }
            }
            else
            {
                TRACE_E("Here");
                cJSON_Delete(cj_script_id);
            }
        }
    }

    if (free_str)
    {
        TRACE_E("Here");
        free(script_ids_str);
    }
}

static void ezlopi_scenes_scripts_nvs_parse(void)
{
    char *script_ids = ezlopi_nvs_read_scenes_scripts();

    if (script_ids)
    {
        TRACE_E("Here");
        cJSON *cj_script_ids = cJSON_Parse(script_ids);
        if (cj_script_ids)
        {
            TRACE_E("Here");
            int array_size = cJSON_GetArraySize(cj_script_ids);
            for (int i = 0; i < array_size; i++)
            {
                TRACE_E("Here");
                cJSON *cj_script_id = cJSON_GetArrayItem(cj_script_ids, i);
                if (cj_script_id && cj_script_id->valuedouble)
                {
                    uint32_t script_id = (uint32_t)(cj_script_id->valuedouble);
                    TRACE_D("%d: %08x", i, script_id);

                    char script_id_str[32];
                    snprintf(script_id_str, sizeof(script_id_str), "%08x", script_id);

                    char *script = ezlopi_nvs_read_str(script_id_str);
                    if (script)
                    {
                        TRACE_D("script: %s", script);
                        cJSON *cj_script = cJSON_Parse(script);
                        if (cj_script)
                        {
                            TRACE_E("Here");
                            ezlopi_scenes_scripts_add_to_head(script_id, cj_script);
                        }
                    }
                    else
                    {
                        TRACE_E("Script parse failed!");
                    }
                }
            }

            cJSON_Delete(cj_script_ids);
        }
    }
    else
    {
        TRACE_W("Scenes-scripts not available!");
    }
}

static l_ezlopi_scenes_script_t *ezlopi_scenes_scripts_create_node(uint32_t script_id, cJSON *cj_script)
{
    l_ezlopi_scenes_script_t *new_script = NULL;

    if (cj_script)
    {
        TRACE_E("Here");
        cJSON *cj_script_name = cJSON_GetObjectItem(cj_script, "name");
        cJSON *cj_script_code = cJSON_GetObjectItem(cj_script, "code");

        if (cj_script_name && cj_script_name->valuestring && cj_script_code && cj_script_code->string)
        {
            TRACE_E("Here");
            uint32_t is_new_script = 0;
            if (script_id)
            {
                TRACE_E("Here");
                ezlopi_cloud_update_script_id(script_id);
            }
            else
            {
                TRACE_E("Here");
                script_id = ezlopi_cloud_generate_script_id();
                char *script_str = cJSON_Print(cj_script);
                if (script_str)
                {
                    TRACE_E("Here");
                    char scrpt_id_str[32];
                    snprintf(scrpt_id_str, sizeof(scrpt_id_str), "%08x", script_id);
                    ezlopi_nvs_write_str(script_str, strlen(script_str), scrpt_id_str);
                    free(script_str);
                    TRACE_E("Here");
                    ezlopi_scenes_scripts_add_script_id(script_id);
                    TRACE_E("Here");
                }
            }

            new_script = (l_ezlopi_scenes_script_t *)malloc(sizeof(l_ezlopi_scenes_script_t));
            if (new_script)
            {
                TRACE_E("Here");
                new_script->id = script_id;

                uint32_t script_name_size = strlen(cj_script_name->valuestring) + 1;
                new_script->name = (char *)malloc(script_name_size);
                if (new_script->name)
                {
                    snprintf(new_script->name, script_name_size, "%s", cj_script_name->valuestring);
                }
                TRACE_E("Here");

                uint32_t script_code_size = strlen(cj_script_code->valuestring) + 1;
                new_script->code = (char *)malloc(script_code_size);
                if (new_script->code)
                {
                    snprintf(new_script->code, script_code_size, "%s", cj_script_code->valuestring);
                }

                new_script->next = NULL;
                TRACE_E("Here");
            }
        }
        else
        {
            TRACE_E("Error");
        }
    }

    return new_script;
}
