#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_edit.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_populate.h"

#include "ezlopi_service_meshbot.h"

static void _edit_scene(l_scenes_list_v2_t *scene_node, cJSON *cj_scene);

int ezlopi_core_scene_edit_store_updated_to_nvs(cJSON *cj_updated_scene)
{
    int ret = 0;
    if (cj_updated_scene)
    {
        char *update_scene_str = cJSON_Print(cj_updated_scene);
        if (update_scene_str)
        {
            cJSON *cj_scene_id = cJSON_GetObjectItem(cj_updated_scene, ezlopi__id_str);
            if (cj_scene_id && cj_scene_id->valuestring)
            {
                ezlopi_nvs_delete_stored_data_by_name(cj_scene_id->valuestring);
                cJSON_Minify(update_scene_str);
                ezlopi_nvs_write_str(update_scene_str, strlen(update_scene_str), cj_scene_id->valuestring);
            }

            free(update_scene_str);
        }
    }

    return ret;
}

int ezlopi_core_scene_edit_update_id(uint32_t scene_id, cJSON *cj_updated_scene)
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
                if (scene_node->enabled == true)
                {
                    ezlopi_meshbot_service_start_scene(scene_node);
                }
                ret = 1;
                break;
            }

            scene_node = scene_node->next;
        }
    }

    return ret;
}

static void _edit_scene(l_scenes_list_v2_t *scene_node, cJSON *cj_scene)
{
    CJSON_GET_VALUE_DOUBLE(cj_scene, ezlopi_enabled_str, scene_node->enabled);
    CJSON_GET_VALUE_DOUBLE(cj_scene, ezlopi_is_group_str, scene_node->is_group);
    CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_group_id_str, scene_node->group_id);

    CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_name_str, scene_node->name);
    CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_parent_id_str, scene_node->parent_id);

    {
        cJSON *cj_user_notifications = cJSON_GetObjectItem(cj_scene, ezlopi_user_notifications_str);
        if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
        {
            ezlopi_scenes_delete_user_notifications(scene_node->user_notifications);
            scene_node->user_notifications = ezlopi_scenes_populate_user_notifications(cj_user_notifications);
        }
    }

    {
        cJSON *cj_house_modes = cJSON_GetObjectItem(cj_scene, ezlopi_house_modes_str);
        if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
        {
            ezlopi_scenes_delete_house_modes(scene_node->house_modes);
            scene_node->house_modes = ezlopi_scenes_populate_house_modes(cj_house_modes);
        }
    }

    {
        cJSON *cj_then_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_then_str);
        if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
        {
            ezlopi_scenes_delete_action_blocks(scene_node->then_block);
            scene_node->then_block = ezlopi_scenes_populate_action_blocks(cj_then_blocks, SCENE_BLOCK_TYPE_THEN);
        }
    }

    {
        cJSON *cj_when_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_when_str);
        if (cj_when_blocks && (cJSON_Array == cj_when_blocks->type))
        {
            ezlopi_scenes_delete_when_blocks(scene_node->when_block);
            scene_node->when_block = ezlopi_scenes_populate_when_blocks(cj_when_blocks);
        }
    }

    {
        cJSON *cj_else_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_else_str);
        if (cj_else_blocks && (cJSON_Array == cj_else_blocks->type))
        {
            ezlopi_scenes_delete_action_blocks(scene_node->else_block);
            scene_node->else_block = ezlopi_scenes_populate_action_blocks(cj_else_blocks, SCENE_BLOCK_TYPE_ELSE);
        }
    }
}
