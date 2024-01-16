#include "ezlopi_scenes_v2.h"
#include "ezlopi_cjson_macros.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_meshbot_service.h"
#include "ezlopi_scenes_populate.h"

// static __edit_

static void __free_user_notifications(l_user_notification_v2_t *user_notifications)
{
    if (user_notifications)
    {
        __free_user_notifications(user_notifications->next);
        free(user_notifications);
    }
}

static void _edit_scene(l_scenes_list_v2_t *scene_node, cJSON *cj_scene)
{
    CJSON_GET_VALUE_INT(cj_scene, ezlopi_enabled_str, scene_node->enabled);
    CJSON_GET_VALUE_INT(cj_scene, ezlopi_is_group_str, scene_node->is_group);
    CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_group_id_str, scene_node->group_id);

    CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_name_str, scene_node->name);
    CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_parent_id_str, scene_node->parent_id);

    {
        cJSON *cj_user_notifications = cJSON_GetObjectItem(cj_scene, ezlopi_user_notifications_str);
        if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
        {
            __free_user_notifications(scene_node->user_notifications);
            scene_node->user_notifications = ezlopi_scenes_populate_user_notifications(cj_user_notifications);
        }
    }
}

int ezlopi_scenes_edit_by_id(uint32_t scene_id, cJSON *cj_updated_scene)
{
    int ret = 0;
    CJSON_TRACE("cj_updated_scene", cj_updated_scene);

    if (scene_id && cj_updated_scene)
    {
        l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_head_v2();
        while (scene_node)
        {
            if (scene_id == scene_node->_id)
            {
                ezlopi_meshobot_service_stop_scene(scene_node);
                _edit_scene(scene_node, cj_updated_scene);
                ret = 1;
                break;
            }

            scene_node = scene_node->next;
        }
    }

    return ret;
}