#include <cJSON.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_notifications.h"

#include "ezlopi_cloud_constants.h"

void ezlopi_scenes_notifications_add(cJSON* cj_notifications)
{
    if (cj_notifications)
    {
        cJSON* cj_scene_id = cJSON_GetObjectItem(cj_notifications, ezlopi_notifications_str);
        cJSON* cj_user_id = cJSON_GetObjectItem(cj_notifications, ezlopi_userId_str);

        if (cj_scene_id && cj_scene_id->valuestring && cj_user_id && cj_user_id->valuestring)
        {
            char* scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON* cj_scene = cJSON_Parse(scene_str);
                free(scene_str);

                if (cj_scene)
                {
                    cJSON* cj_user_notifications = cJSON_GetObjectItem(cj_scene, ezlopi_user_notifications_str);
                    if (cj_user_notifications)
                    {
                        cJSON_AddItemReferenceToArray(cj_user_notifications, cj_user_id);
                    }

                    CJSON_TRACE("updated-scene", cj_scene);
                    char* updated_scene_str = cJSON_PrintBuffered(cj_scene, 4096, false);
                    TRACE_D("length of 'updated_scene_str': %d", strlen(updated_scene_str));

                    cJSON_Delete(cj_scene);

                    if (updated_scene_str)
                    {
                        ezlopi_nvs_write_str(updated_scene_str, strlen(updated_scene_str), cj_scene_id->valuestring);
                        free(updated_scene_str);
                    }
                }
            }

            uint32_t scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);

            if (scene_id)
            {
                l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_scenes_head_v2();
                while (scene_node)
                {
                    if (scene_id == scene_node->_id)
                    {
                        ezlopi_scene_add_users_in_notifications(scene_node, cj_user_id);
                        break;
                    }
                    scene_node = scene_node->next;
                }
            }
        }
    }
}

void ezlopi_scenes_notifications_remove(cJSON* cj_notifications)
{
    if (cj_notifications)
    {
    }
}
