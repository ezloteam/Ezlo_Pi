#include "cJSON.h"
#include "string.h"
#include "stdlib.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"

#include "ezlopi_cloud.h"
#include "ezlopi_nvs.h"
#include "ezlopi_scenes_scripts.h"
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

static l_ezlopi_scenes_script_t *script_head = NULL;

static void ezlopi_scenes_process_scripts(void);
static void __exit_script_hook(lua_State *lua_state, lua_Debug *ar);
static void ezlopi_scenes_scripts_nvs_parse(void);
static l_ezlopi_scenes_script_t *ezlopi_scenes_scripts_create_node(uint32_t script_id, cJSON *cj_script);

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
    if (script_head->id == script_id)
    {
        script_head = script_head->next;
    }
    else
    {
        l_ezlopi_scenes_script_t *curr_script = script_head;
        while (curr_script->next)
        {
            if (curr_script->next->id == script_id)
            {
                l_ezlopi_scenes_script_t *script_to_free = curr_script->next;
                curr_script->next = curr_script->next->next;

                ezlopi_scenes_scripts_stop(script_to_free);
                if (script_to_free->code)
                {
                    free(script_to_free);
                }
                if (script_to_free->name)
                {
                    free(script_to_free->name);
                }
                free(script_to_free);
            }
        }
    }
}

void ezlopi_scenes_scripts_init(void)
{
    ezlopi_scenes_scripts_nvs_parse();
    ezlopi_scenes_process_scripts();
}

static char *__script_report(lua_State *lua_state, int status)
{
    if (status == LUA_OK)
    {
        return;
    }

    const char *msg = lua_tostring(lua_state, -1);
    lua_pop(lua_state, 1);
    return msg;
}

static void __exit_script_hook(lua_State *lua_state, lua_Debug *ar)
{
    lua_sethook(lua_state, __exit_script_hook, LUA_MASKLINE, 0);
    luaL_error(lua_state, "Exited from software call");
}

typedef struct s_lua_scripts_modules
{
    char *name;
    lua_CFunction func;
} s_lua_scripts_modules_t;

static s_lua_scripts_modules_t lua_scripts_modules[] = {
#define SCRIPTS_CUSTOM_LIB(module_name, module_func) {.name = module_name, .func = module_func},
#include "ezlopi_scenes_scripts_custom_libs.h"
#undef SCRIPTS_CUSTOM_LIB
    {.name = NULL, .func = NULL},
};

static void __load_custom_libs(lua_State *lua_state)
{
    uint32_t idx = 0;
    while (lua_scripts_modules[idx].name && lua_scripts_modules[idx].func)
    {
        luaL_requiref(lua_state, lua_scripts_modules[idx].name, lua_scripts_modules[idx].func, 1);
        lua_pop(lua_state, 1);
        idx++;
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
            char *script_report = __script_report(lua_state, tmp_ret);
            if (script_report)
            {
                TRACE_E("Error in '%s' -> %s", script_node->name, script_report);
            }
        }

        tmp_ret = lua_pcall(lua_state, 0, 0, 0);
        if (tmp_ret)
        {
            char *script_report = __script_report(lua_state, tmp_ret);
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

    TRACE_W("%s -> {state: %d}", script_node->name, script_node->state);
    vTaskDelete(NULL);
}

static void ezlopi_scenes_process_scripts(void)
{
    l_ezlopi_scenes_script_t *script_node = script_head;
    while (script_node)
    {
        if (script_node->code)
        {
            xTaskCreate(__script_process, script_node->name, 2048 * 2, script_node, 3, NULL);
        }

        script_node = script_node->next;
    }
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
