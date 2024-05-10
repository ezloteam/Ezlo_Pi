#include <stdio.h>
#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "ezlopi_core_processes.h"

static const char *prg =
"count = 0\n"
"v = {'value1', 'value2', 1.21, 'gigawatts'}\n"
"for i = 1, #v do\n"
"  print(v[i])\n"
"  print(count)\n"
"  count = count + 1\n"
"end\n";

static int report(lua_State *L, int status)
{
    if (status != LUA_OK)
    {
        const char *msg = lua_tostring(L, -1);
        lua_writestring(msg, strlen(msg));
        lua_writeline();
        lua_pop(L, 1); /* remove message */
    }
    return status;
}

void test(void *arg)
{
    printf("\n\n%s\n\n", prg);
    printf("Start, heap: %" PRIu32 "\n", xPortGetFreeHeapSize());

    lua_State *L = luaL_newstate();
    if (!L)
    {
        printf("Could not create state....\n");
        while (1)
        {
            vTaskDelay(1000);
        }
    }

    printf("State ready, heap: %" PRIu32 "\n", xPortGetFreeHeapSize());

    luaL_openlibs(L);

    printf("Libs ready, heap: %" PRIu32 "\n", xPortGetFreeHeapSize());

    int r = luaL_loadstring(L, prg);
    if (r)
    {
        report(L, r);
        while (1)
        {
            vTaskDelay(1000);
        }
    }

    printf("Prg loaded, heap: %" PRIu32 "\n", xPortGetFreeHeapSize());

    printf("-------------------------------------------\n");
    r = lua_pcall(L, 0, 0, 0);
    if (r)
        report(L, r);
    printf("-------------------------------------------\n");

    printf("Prg done, heap: %" PRIu32 "\n", xPortGetFreeHeapSize());

    lua_close(L);

    printf("State closed, heap: %" PRIu32 "\n", xPortGetFreeHeapSize());
    while (1)
    {
        printf(".");
        fflush(NULL);
        vTaskDelay(100);
    }
}

void app_main()
{
    TaskHandle_t ezlopi_component_lua_simple_test_task_handle = NULL;
    xTaskCreate(test, "test", EZLOPI_COMPONENT_LUA_SIMPLE_TEST_TASK_DEPTH, NULL, 5, &ezlopi_component_lua_simple_test_task_handle);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_COMPONENT_LUA_SIMPLE_TEST_TASK, &ezlopi_component_lua_simple_test_task_handle, EZLOPI_COMPONENT_LUA_SIMPLE_TEST_TASK_DEPTH);
}
