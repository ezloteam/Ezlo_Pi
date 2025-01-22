/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_scenes_scripts.c
 * @brief   Perform operations on Scene-scipts
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "cjext.h"
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

#include "corelib.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_scenes_scripts.h"
// #include "ezlopi_core_errors.h"
// #include "ezlopi_core_scenes_scripts_custom_libs_includes.h"
#include "ezlopi_core_processes.h"

#include "ezlopi_cloud_constants.h"
#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
typedef struct s_lua_scripts_modules
{
    char *name;
    lua_CFunction func;
} s_lua_scripts_modules_t;
/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static ezlopi_error_t __scripts_nvs_parse(void);
static void __script_process(void *arg);
static void __scripts_process_runner(void);
static void __scripts_add_script_id(uint32_t script_id);
static void __load_custom_libs(lua_State *lua_state);
static void __run_script(l_ezlopi_scenes_script_t *script_node);
static void __scripts_remove_id_and_update_list(uint32_t script_id);
static void __exit_script_hook(lua_State *lua_state, lua_Debug *ar);
static const char *__script_report(lua_State *lua_state, int status);
static l_ezlopi_scenes_script_t *__scripts_create_node(uint32_t script_id, cJSON *cj_script);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static l_ezlopi_scenes_script_t *script_head = NULL;

static s_lua_scripts_modules_t lua_scripts_modules[] = {
#define SCRIPTS_CUSTOM_LIB(module_name, module_func) {.name = module_name, .func = module_func},
#include "ezlopi_core_scenes_scripts_custom_libs.h"
#undef SCRIPTS_CUSTOM_LIB
    {.name = NULL, .func = NULL},
};
/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
l_ezlopi_scenes_script_t *EZPI_scenes_scripts_get_head(void)
{
    return script_head;
}

void EZPI_scenes_scripts_stop(l_ezlopi_scenes_script_t *script_node)
{
    if (script_node && script_node->script_handle)
    {
        lua_sethook(script_node->script_handle, __exit_script_hook, LUA_MASKCOUNT, 1);
    }
}

void EZPI_scenes_scripts_stop_by_id(uint32_t script_id)
{
    l_ezlopi_scenes_script_t *script_node = script_head;
    while (script_node)
    {
        if (script_node->id == script_id)
        {
            EZPI_scenes_scripts_stop(script_node);
            break;
        }
        script_node = script_node->next;
    }
}

void EZPI_scenes_scripts_delete_by_id(uint32_t script_id)
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
        EZPI_scenes_scripts_stop(script_to_delete);
        EZPI_core_nvs_delete_stored_data_by_id(script_to_delete->id); // deleting script from nvs
        __scripts_remove_id_and_update_list(script_to_delete->id);

        if (script_to_delete->code)
        {
            ezlopi_free(__FUNCTION__, script_to_delete->code);
        }

        ezlopi_free(__FUNCTION__, script_to_delete);
    }
}

void EZPI_scenes_scripts_factory_info_reset(void)
{
    l_ezlopi_scenes_script_t *curr_script = EZPI_scenes_scripts_get_head();

    while (curr_script)
    {
        EZPI_scenes_scripts_delete_by_id(curr_script->id); // auto-script_head shift
        curr_script = EZPI_scenes_scripts_get_head();
    }
}

uint32_t EZPI_scenes_scripts_add_to_head(uint32_t script_id, cJSON *cj_script)
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

void EZPI_scenes_scripts_run_by_id(uint32_t script_id)
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

void EZPI_scenes_scripts_update(cJSON *cj_script)
{
    cJSON *cj_script_id = cJSON_DetachItemFromObject(__FUNCTION__, cj_script, ezlopi__id_str);

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
                        ezlopi_free(__FUNCTION__, script_node->code);
                    }

                    {
                        cJSON *cj_name = cJSON_GetObjectItem(__FUNCTION__, cj_script, ezlopi_name_str);
                        if (cj_name && cj_name->valuestring)
                        {
                            // uint32_t len = strlen(cj_name->valuestring) + 1;
                            snprintf(script_node->name, sizeof(script_node->name), "%s", cj_name->valuestring);
                        }
                    }

                    {
                        cJSON *cj_code = cJSON_GetObjectItem(__FUNCTION__, cj_script, ezlopi_code_str);
                        if (cj_code && cj_code->valuestring)
                        {
                            uint32_t len = strlen(cj_code->valuestring) + 1;
                            script_node->code = ezlopi_malloc(__FUNCTION__, len);
                            if (script_node->code)
                            {
                                snprintf(script_node->code, len, "%s", cj_code->valuestring);
                            }
                        }
                    }

                    char *script_to_update = cJSON_PrintBuffered(__FUNCTION__, cj_script, 4096, false);
                    TRACE_D("length of 'script_to_update': %d", strlen(script_to_update));

                    if (script_to_update)
                    {
                        EZPI_core_nvs_write_str(script_to_update, strlen(script_to_update), cj_script_id->valuestring);
                        ezlopi_free(__FUNCTION__, script_to_update);
                    }

                    break;
                }

                script_node = script_node->next;
            }
        }

        cJSON_Delete(__FUNCTION__, cj_script_id);
    }
}

void EZPI_scenes_scripts_init(void)
{
    __scripts_nvs_parse();
    __scripts_process_runner();
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

static void __run_script(l_ezlopi_scenes_script_t *script_node)
{
    if (script_node->code)
    {
        TaskHandle_t ezlopi_core_scenes_script_process_task_handle = NULL;
        xTaskCreate(__script_process, script_node->name, EZLOPI_CORE_SCENES_SCRIPT_PROCESS_TASK_DEPTH, script_node, 3, &ezlopi_core_scenes_script_process_task_handle);
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
        EZPI_core_process_set_process_info(ENUM_EZLOPI_CORE_SCENES_SCRIPT_PROCESS_TASK, &ezlopi_core_scenes_script_process_task_handle, EZLOPI_CORE_SCENES_SCRIPT_PROCESS_TASK_DEPTH);
#endif
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
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_is_deleted(ENUM_EZLOPI_CORE_SCENES_SCRIPT_PROCESS_TASK);
#endif
    vTaskDelete(NULL);
}

static void __scripts_add_script_id(uint32_t script_id)
{
    uint32_t script_ids_str_free = true;
    char *script_ids_str = EZPI_core_nvs_read_scenes_scripts();
    if (NULL == script_ids_str)
    {
        script_ids_str = "[]"; // don't free in this case
        script_ids_str_free = false;
    }

    cJSON *cj_script_ids = cJSON_Parse(__FUNCTION__, script_ids_str);
    if (cj_script_ids)
    {
        cJSON *cj_script_id = cJSON_CreateNumber(__FUNCTION__, script_id);
        if (cj_script_id)
        {
            if (cJSON_AddItemToArray(cj_script_ids, cj_script_id))
            {
                char *script_ids_str_updated = cJSON_PrintBuffered(__FUNCTION__, cj_script_ids, 1024, false);
                TRACE_D("length of 'script_ids_str_updated': %d", strlen(script_ids_str_updated));

                if (script_ids_str_updated)
                {
                    EZPI_core_nvs_write_scenes_scripts(script_ids_str_updated);
                    ezlopi_free(__FUNCTION__, script_ids_str_updated);
                }
            }
            else
            {
                cJSON_Delete(__FUNCTION__, cj_script_id);
            }
        }

        cJSON_Delete(__FUNCTION__, cj_script_ids);
    }

    if (script_ids_str_free) // only free in case of read from nvs otehrwise do not free
    {
        ezlopi_free(__FUNCTION__, script_ids_str);
    }
}

static void __scripts_remove_id_and_update_list(uint32_t script_id)
{
    char *scripts_ids_str = EZPI_core_nvs_read_scenes_scripts();
    if (scripts_ids_str)
    {
        cJSON *cj_scripts_ids = cJSON_Parse(__FUNCTION__, scripts_ids_str);
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
                        cJSON_DeleteItemFromArray(__FUNCTION__, cj_scripts_ids, i);

                        char *scripts_ids_str_updated = cJSON_PrintBuffered(__FUNCTION__, cj_scripts_ids, 1024, false);
                        TRACE_D("length of 'scripts_ids_str_updated': %d", strlen(scripts_ids_str_updated));

                        if (scripts_ids_str_updated)
                        {
                            EZPI_core_nvs_write_scenes_scripts(scripts_ids_str_updated);

                            ezlopi_free(__FUNCTION__, scripts_ids_str_updated);
                        }

                        break;
                    }
                }
            }

            cJSON_Delete(__FUNCTION__, cj_scripts_ids);
        }

        ezlopi_free(__FUNCTION__, scripts_ids_str);
    }
}

static ezlopi_error_t __scripts_nvs_parse(void)
{
    ezlopi_error_t error = EZPI_ERR_JSON_PARSE_FAILED;
    char *script_ids = EZPI_core_nvs_read_scenes_scripts();
    if (script_ids)
    {
        cJSON *cj_script_ids = cJSON_Parse(__FUNCTION__, script_ids);
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

                    char *script_str = EZPI_core_nvs_read_str(script_id_str);
                    if (script_str)
                    {
                        cJSON *cj_script = cJSON_Parse(__FUNCTION__, script_str);
                        if (cj_script)
                        {
                            EZPI_scenes_scripts_add_to_head(script_id, cj_script);
                        }

                        ezlopi_free(__FUNCTION__, script_str);
                    }
                    else
                    {
                        TRACE_E("Script parse failed!");
                    }
                }
            }
            error = EZPI_SUCCESS;
            cJSON_Delete(__FUNCTION__, cj_script_ids);
        }

        ezlopi_free(__FUNCTION__, script_ids);
    }
    return error;
}

static l_ezlopi_scenes_script_t *__scripts_create_node(uint32_t script_id, cJSON *cj_script)
{
    l_ezlopi_scenes_script_t *new_script = NULL;

    if (cj_script)
    {
        cJSON *cj_script_name = cJSON_GetObjectItem(__FUNCTION__, cj_script, ezlopi_name_str);
        cJSON *cj_script_code = cJSON_GetObjectItem(__FUNCTION__, cj_script, ezlopi_code_str);

        if (cj_script_name && cj_script_name->valuestring && cj_script_code && cj_script_code->string)
        {
            if (script_id)
            {
                EZPI_core_cloud_update_script_id(script_id);
            }
            else
            {
                script_id = EZPI_core_cloud_generate_script_id();
                char *script_str = cJSON_PrintBuffered(__FUNCTION__, cj_script, 4096, false);
                TRACE_D("length of 'script_str': %d", strlen(script_str));

                if (script_str)
                {
                    char scrpt_id_str[32];
                    snprintf(scrpt_id_str, sizeof(scrpt_id_str), "%08x", script_id);
                    EZPI_core_nvs_write_str(script_str, strlen(script_str), scrpt_id_str);
                    ezlopi_free(__FUNCTION__, script_str);
                    __scripts_add_script_id(script_id);
                }
            }

            new_script = (l_ezlopi_scenes_script_t *)ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_scenes_script_t));
            if (new_script)
            {
                memset(new_script, 0, sizeof(l_ezlopi_scenes_script_t));
                new_script->id = script_id;
                new_script->script_handle = NULL;
                new_script->state = SCRIPT_STATE_NONE;

                // uint32_t script_name_size = strlen(cj_script_name->valuestring) + 1;
                snprintf(new_script->name, sizeof(new_script->name), "%s", cj_script_name->valuestring);

                uint32_t script_code_size = strlen(cj_script_code->valuestring) + 1;
                new_script->code = (char *)ezlopi_malloc(__FUNCTION__, script_code_size);
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

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/