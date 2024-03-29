#include <cJSON.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

#include "corelib.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_scenes_scripts.h"
#include "ezlopi_core_scenes_scripts_custom_libs_includes.h"

#include "ezlopi_cloud_constants.h"

static l_ezlopi_scenes_script_t *script_head = NULL;

typedef struct s_lua_scripts_modules
{
    char *name;
    lua_CFunction func;
} s_lua_scripts_modules_t;

static void __scripts_nvs_parse(void);
static void __script_process(void *arg);
static void __scripts_process_runner(void);
static void __load_custom_libs(lua_State *lua_state);
static void __run_script(l_ezlopi_scenes_script_t *script_node);
static void __scripts_remove_id_and_update_list(uint32_t script_id);
static const char *__script_report(lua_State *lua_state, int status);
static void __exit_script_hook(lua_State *lua_state, lua_Debug *ar);
static l_ezlopi_scenes_script_t *__scripts_create_node(uint32_t script_id, cJSON *cj_script);

l_ezlopi_scenes_script_t *ezlopi_scenes_scripts_get_head(void)
{
    return script_head;
}

void ezlopi_scenes_scripts_stop(l_ezlopi_scenes_script_t *script_node)
{
    if (script_node && script_node->script_handle)
    {
        lua_sethook(script_node->script_handle, __exit_script_hook, LUA_MASKCOUNT, 1);
    }
}

void ezlopi_scenes_scripts_stop_by_id(uint32_t script_id)
{
    l_ezlopi_scenes_script_t *script_node = script_head;
    while (script_node)
    {
        if (script_node->id == script_id)
        {
            ezlopi_scenes_scripts_stop(script_node);
            break;
        }
        script_node = script_node->next;
    }
}

void ezlopi_scenes_scripts_delete_by_id(uint32_t script_id)
{
    l_ezlopi_scenes_script_t *script_to_delete = NULL;
    if (script_head->id == script_id)
    {
        script_to_delete = script_head;
        script_head = script_head->next;
    }
    else
    {
        l_ezlopi_scenes_script_t *curr_script = script_head;
        while (curr_script->next)
        {
            if (curr_script->next->id == script_id)
            {
                script_to_delete = curr_script->next;
                curr_script->next = curr_script->next->next;
                break;
            }
            curr_script = curr_script->next;
        }
    }

    if (script_to_delete)
    {
        ezlopi_scenes_scripts_stop(script_to_delete);
        ezlopi_nvs_delete_stored_data_by_id(script_to_delete->id); // deleting script from nvs
        __scripts_remove_id_and_update_list(script_to_delete->id);

        if (script_to_delete->code)
        {
            free(script_to_delete->code);
        }
        if (script_to_delete->name)
        {
            free(script_to_delete->name);
        }
        free(script_to_delete);
    }
}

uint32_t ezlopi_scenes_scripts_add_to_head(uint32_t script_id, cJSON *cj_script)
{
    uint32_t new_script_id = 0;
    if (script_head)
    {
        l_ezlopi_scenes_script_t *curr_script = script_head;
        while (curr_script->next)
        {
            curr_script = curr_script->next;
        }

        curr_script->next = __scripts_create_node(script_id, cj_script);
        if (curr_script)
        {
            new_script_id = curr_script->next->id;
        }
    }
    else
    {
        script_head = __scripts_create_node(script_id, cj_script);
        if (script_head)
        {
            new_script_id = script_head->id;
        }
    }

    return new_script_id;
}

void ezlopi_scenes_scripts_run_by_id(uint32_t script_id)
{
    l_ezlopi_scenes_script_t *curr_script = script_head;
    while (curr_script)
    {
        if (script_id == curr_script->id)
        {
            if (SCRIPT_STATE_RUNNING != curr_script->state)
            {
                __run_script(curr_script);
            }
            break;
        }
        curr_script = curr_script->next;
    }
}

void ezlopi_scenes_scripts_update(cJSON *cj_script)
{
    cJSON *cj_script_id = cJSON_DetachItemFromObject(cj_script, ezlopi__id_str);

    if (cj_script_id)
    {
        if (cj_script_id->valuestring)
        {
            uint32_t script_id = strtoul(cj_script_id->valuestring, NULL, 16);

            l_ezlopi_scenes_script_t *script_node = script_head;
            while (script_node)
            {
                if (script_id == script_node->id)
                {
                    if (script_node->code)
                    {
                        free(script_node->code);
                    }

                    if (script_node->name)
                    {
                        free(script_node->name);
                    }

                    {
                        cJSON *cj_name = cJSON_GetObjectItem(cj_script, ezlopi_name_str);
                        if (cj_name && cj_name->valuestring)
                        {
                            uint32_t len = strlen(cj_name->valuestring) + 1;
                            script_node->name = malloc(len);
                            snprintf(script_node->name, len, "%s", cj_name->valuestring);
                        }
                    }

                    {
                        cJSON *cj_code = cJSON_GetObjectItem(cj_script, ezlopi_code_str);
                        if (cj_code && cj_code->valuestring)
                        {
                            uint32_t len = strlen(cj_code->valuestring) + 1;
                            script_node->code = malloc(len);
                            snprintf(script_node->code, len, "%s", cj_code->valuestring);
                        }
                    }

                    char *script_to_update = cJSON_Print(cj_script);
                    if (script_to_update)
                    {
                        cJSON_Minify(script_to_update);
                        ezlopi_nvs_write_str(script_to_update, strlen(script_to_update), cj_script_id->valuestring);
                        free(script_to_update);
                    }

                    break;
                }

                script_node = script_node->next;
            }
        }

        cJSON_Delete(cj_script_id);
    }
}

void ezlopi_scenes_scripts_init(void)
{
    __scripts_nvs_parse();
    __scripts_process_runner();
}

static void __run_script(l_ezlopi_scenes_script_t *script_node)
{
    if (script_node->code)
    {
        xTaskCreate(__script_process, script_node->name, 2048 * 2, script_node, 3, NULL);
    }
}

static void __scripts_process_runner(void)
{
    l_ezlopi_scenes_script_t *script_node = script_head;
    while (script_node)
    {
        __run_script(script_node);
        script_node = script_node->next;
    }
}

static void __script_process(void *arg)
{
    l_ezlopi_scenes_script_t *script_node = (l_ezlopi_scenes_script_t *)arg;
    lua_State *lua_state = luaL_newstate();
    if (lua_state)
    {
        luaL_openlibs(lua_state);
        __load_custom_libs(lua_state);

        script_node->state = SCRIPT_STATE_RUNNING;

        int tmp_ret = luaL_loadstring(lua_state, script_node->code);
        if (tmp_ret)
        {
#warning "Krishna needs to check this."
            const char *script_report = __script_report(lua_state, tmp_ret);
            if (script_report)
            {
                TRACE_E("Error in '%s' -> %s", script_node->name, script_report);
            }
        }

        tmp_ret = lua_pcall(lua_state, 0, 1, 0);
        if (tmp_ret)
        {
            const char *script_report = __script_report(lua_state, tmp_ret);
            if (script_report)
            {
                TRACE_E("Error in '%s' -> %s", script_node->name, script_report);
            }
        }

        script_node->state = SCRIPT_STATE_NOT_RUNNING;
        script_node->script_handle = NULL;
        lua_close(lua_state);
    }
    else
    {
        TRACE_E("Couldn't create lua state for '%s'", script_node->name);
    }

    TRACE_W("%s -> {state: %d} -> Stopped", script_node->name, script_node->state);
    vTaskDelete(NULL);
}

static void __scripts_add_script_id(uint32_t script_id)
{
    uint32_t script_ids_str_free = true;
    char *script_ids_str = ezlopi_nvs_read_scenes_scripts();
    if (NULL == script_ids_str)
    {
        script_ids_str = "[]"; // don't free in this case
        script_ids_str_free = false;
    }

    cJSON *cj_script_ids = cJSON_Parse(script_ids_str);
    if (cj_script_ids)
    {
        cJSON *cj_script_id = cJSON_CreateNumber(script_id);
        if (cj_script_id)
        {
            if (cJSON_AddItemToArray(cj_script_ids, cj_script_id))
            {
                char *script_ids_str_updated = cJSON_Print(cj_script_ids);
                if (script_ids_str_updated)
                {
                    cJSON_Minify(script_ids_str_updated);
                    ezlopi_nvs_write_scenes_scripts(script_ids_str_updated);
                    free(script_ids_str_updated);
                }
            }
            else
            {
                cJSON_Delete(cj_script_id);
            }
        }

        cJSON_Delete(cj_script_ids);
    }

    if (script_ids_str_free) // only free in case of read from nvs otehrwise do not free
    {
        free(script_ids_str);
    }
}

static void __scripts_remove_id_and_update_list(uint32_t script_id)
{
    char *scripts_ids_str = ezlopi_nvs_read_scenes_scripts();
    if (scripts_ids_str)
    {
        cJSON *cj_scripts_ids = cJSON_Parse(scripts_ids_str);
        if (cj_scripts_ids)
        {
            int array_size = cJSON_GetArraySize(cj_scripts_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *cj_script_id = cJSON_GetArrayItem(cj_scripts_ids, i);
                if (cj_script_id && cj_script_id->valuedouble)
                {
                    if (script_id == cj_script_id->valuedouble)
                    {
                        TRACE_D("Removing (%d: %08x) script from list!", i, script_id);
                        cJSON_DeleteItemFromArray(cj_scripts_ids, i);

                        char *scripts_ids_str_updated = cJSON_Print(cj_scripts_ids);
                        if (scripts_ids_str_updated)
                        {
                            cJSON_Minify(scripts_ids_str_updated);
                            ezlopi_nvs_write_scenes_scripts(scripts_ids_str_updated);

                            free(scripts_ids_str_updated);
                        }

                        break;
                    }
                }
            }

            cJSON_Delete(cj_scripts_ids);
        }

        free(scripts_ids_str);
    }
}

static void __scripts_nvs_parse(void)
{
    char *script_ids = ezlopi_nvs_read_scenes_scripts();

    if (script_ids)
    {
        cJSON *cj_script_ids = cJSON_Parse(script_ids);
        if (cj_script_ids)
        {
            int array_size = cJSON_GetArraySize(cj_script_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *cj_script_id = cJSON_GetArrayItem(cj_script_ids, i);
                if (cj_script_id && cj_script_id->valuedouble)
                {
                    uint32_t script_id = (uint32_t)(cj_script_id->valuedouble);
                    TRACE_D("%d: %08x", i, script_id);

                    char script_id_str[32];
                    snprintf(script_id_str, sizeof(script_id_str), "%08x", script_id);

                    char *script_str = ezlopi_nvs_read_str(script_id_str);
                    if (script_str)
                    {
                        cJSON *cj_script = cJSON_Parse(script_str);
                        if (cj_script)
                        {
                            ezlopi_scenes_scripts_add_to_head(script_id, cj_script);
                        }

                        free(script_str);
                    }
                    else
                    {
                        TRACE_E("Script parse failed!");
                    }
                }
            }

            cJSON_Delete(cj_script_ids);
        }

        free(script_ids);
    }
}

static l_ezlopi_scenes_script_t *__scripts_create_node(uint32_t script_id, cJSON *cj_script)
{
    l_ezlopi_scenes_script_t *new_script = NULL;

    if (cj_script)
    {
        cJSON *cj_script_name = cJSON_GetObjectItem(cj_script, ezlopi_name_str);
        cJSON *cj_script_code = cJSON_GetObjectItem(cj_script, ezlopi_code_str);

        if (cj_script_name && cj_script_name->valuestring && cj_script_code && cj_script_code->string)
        {
            if (script_id)
            {
                ezlopi_cloud_update_script_id(script_id);
            }
            else
            {
                script_id = ezlopi_cloud_generate_script_id();
                char *script_str = cJSON_Print(cj_script);
                if (script_str)
                {
                    cJSON_Minify(script_str);
                    char scrpt_id_str[32];
                    snprintf(scrpt_id_str, sizeof(scrpt_id_str), "%08x", script_id);
                    ezlopi_nvs_write_str(script_str, strlen(script_str), scrpt_id_str);
                    free(script_str);
                    __scripts_add_script_id(script_id);
                }
            }

            new_script = (l_ezlopi_scenes_script_t *)malloc(sizeof(l_ezlopi_scenes_script_t));
            if (new_script)
            {
                memset(new_script, 0, sizeof(l_ezlopi_scenes_script_t));
                new_script->id = script_id;
                new_script->script_handle = NULL;
                new_script->state = SCRIPT_STATE_NONE;

                uint32_t script_name_size = strlen(cj_script_name->valuestring) + 1;
                new_script->name = (char *)malloc(script_name_size);
                if (new_script->name)
                {
                    snprintf(new_script->name, script_name_size, "%s", cj_script_name->valuestring);
                }

                uint32_t script_code_size = strlen(cj_script_code->valuestring) + 1;
                new_script->code = (char *)malloc(script_code_size);
                if (new_script->code)
                {
                    snprintf(new_script->code, script_code_size, "%s", cj_script_code->valuestring);
                }

                new_script->next = NULL;
            }
        }
    }

    return new_script;
}

static void __exit_script_hook(lua_State *lua_state, lua_Debug *ar)
{
    lua_sethook(lua_state, __exit_script_hook, LUA_MASKLINE, 0);
    luaL_error(lua_state, "Exited from software call");
}

static const char *__script_report(lua_State *lua_state, int status)
{
    if (status == LUA_OK)
    {
        return NULL;
    }

    const char *msg = lua_tostring(lua_state, -1);
    lua_pop(lua_state, 1);
    return msg;
}

static s_lua_scripts_modules_t lua_scripts_modules[] = {
#define SCRIPTS_CUSTOM_LIB(module_name, module_func) {.name = module_name, .func = module_func},
#include "ezlopi_core_scenes_scripts_custom_libs.h"
#undef SCRIPTS_CUSTOM_LIB
    {.name = NULL, .func = NULL},
};

static void __load_custom_libs(lua_State *lua_state)
{
    uint32_t idx = 0;
    while (lua_scripts_modules[idx].name && lua_scripts_modules[idx].func)
    {
        TRACE_D("loading custom lib -> %s : %p", lua_scripts_modules[idx].name, lua_scripts_modules[idx].func);
        luaL_requiref(lua_state, lua_scripts_modules[idx].name, lua_scripts_modules[idx].func, 1);
        lua_pop(lua_state, 1);
        idx++;
    }
}