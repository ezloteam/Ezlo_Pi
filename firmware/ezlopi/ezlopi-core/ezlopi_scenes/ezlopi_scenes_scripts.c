#include "cJSON.h"

#include "trace.h"

#include "ezlopi_nvs.h"
#include "ezlopi_scenes_scripts.h"

static s_ezlopi_lua_script_t *script_head = NULL;

void ezlopi_scenes_scripts_init(void)
{
    char *script_name_list = ezlopi_nvs_read_scenes_scripts();
    if (!script_name_list)
    {
        script_name_list = "[\"script-1\", \"script-2\", \"script-3\"]";
    }

    if (script_name_list)
    {
        cJSON *root = cJSON_Parse(script_name_list);
        if (root)
        {
            int array_size = cJSON_GetArraySize(root);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *elem = cJSON_GetArrayItem(root, i);
                if (elem && elem->valuestring)
                {
                    TRACE_D("%d: %s", i, elem->valuestring);

                }
            }
        }
    }
}



void ezlopi_scenes_scripts_add(void)
{
}
