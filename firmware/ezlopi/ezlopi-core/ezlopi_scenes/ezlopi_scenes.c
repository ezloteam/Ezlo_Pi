#include <string.h>
#include "cJSON.h"

#include "trace.h"

#include "ezlopi_scenes.h"
#include "ezlopi_devices.h"
#include "ezlopi_nvs.h"

static l_scenes_list_t *scenes_list_head = NULL;

static l_house_modes_t *__new_house_mode_create(cJSON *cj_house_mode);
static l_house_modes_t *__house_modes_add(cJSON *cj_house_modes);
static l_user_notification_t *__new_user_notification_create(cJSON *cj_user_notification);
static l_user_notification_t *__user_notifications_add(cJSON *cj_user_notifications);

static l_then_block_t *__then_blocks_add(cJSON *cj_then_blocks)
{
    l_then_block_t *tmp_then_block_head = NULL;
    if (cj_then_blocks)
    {
        int then_block_idx = cJSON_GetArraySize(cj_then_blocks);
        cJSON *cj_then_block = NULL;

        while (NULL != (cj_then_block = cJSON_GetArrayItem(cj_then_blocks, then_block_idx)))
        {
            TRACE_B("then_block-%d:", then_block_idx);

            if (tmp_then_block_head)
            {
                l_then_block_t *tmp_then_block = tmp_then_block_head;
                while (tmp_then_block->next)
                {
                    tmp_then_block = tmp_then_block->next;
                }

                tmp_then_block->next = __new_then_block_create(cj_then_block);
            }
            else
            {
                tmp_then_block_head = __new_then_block_create(cj_then_block);
            }
        }
    }

    return tmp_then_block_head;
}

static l_scenes_list_t *__new_scene_create(cJSON *cj_scene)
{
    l_scenes_list_t *new_scene = NULL;
    if (cj_scene)
    {
        new_scene = malloc(sizeof(l_scenes_list_t));
        if (new_scene)
        {
            uint32_t tmp_success_creating_scene = 1;
            memset(new_scene, 0, sizeof(l_scenes_list_t));

            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "_id", new_scene->_id);
            CJSON_GET_VALUE_INT(cj_scene, "enabled", new_scene->enabled);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "group_id", new_scene->_id);
            CJSON_GET_VALUE_INT(cj_scene, "is_group", new_scene->is_group);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "name", new_scene->_id);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "parent_id", new_scene->_id);

            {
                cJSON *cj_house_modes = cJSON_GetObjectItem(cj_scene, "house_modes");
                if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
                {
                    new_scene->house_modes = __house_modes_add(cj_house_modes);
                }
            }

            {
                cJSON *cj_user_notifications = cJSON_GetObjectItem(cj_scene, "user_notifications");
                if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
                {
                    new_scene->user_notifications = __user_notifications_add(cj_user_notifications);
                }
            }

            {
            }

            if (0 == tmp_success_creating_scene)
            {
                // delete new_scene
            }
        }
    }

    return new_scene;
}

void ezlopi_scene_create(cJSON *scene)
{
    if (scenes_list_head)
    {
        l_scenes_list_t *curr_scene = scenes_list_head;
        while (curr_scene->next)
        {
            curr_scene = curr_scene->next;
        }
    }
}

void ezlopi_scene_init(void)
{
    char *scenes_list = ezlopi_nvs_get_scenes();
    if (scenes_list)
    {
        cJSON *cj_scenes_list = cJSON_Parse(scenes_list);
        if (cj_scenes_list && (cJSON_Array == cj_scenes_list->type))
        {
            int scenes_idx = 0;
            cJSON *cj_scene = NULL;
            while (NULL != (cj_scene = cJSON_GetArrayItem(cj_scenes_list, scenes_idx)))
            {
                TRACE_B("Scene-%d:", scenes_idx);
                ezlopi_scene_create(cj_scene);
            }
        }
    }
}

void ezlopi_scene_delete(void)
{
}

static l_house_modes_t *__new_house_mode_create(cJSON *cj_house_mode)
{
    l_house_modes_t *new_house_mode = NULL;

    if (cj_house_mode && (cJSON_String == cj_house_mode->type))
    {
        new_house_mode = malloc(sizeof(l_house_modes_t));
        if (new_house_mode)
        {
            strncpy(new_house_mode->house_mode, cj_house_mode->valuestring, sizeof(new_house_mode->house_mode));
            new_house_mode->next = NULL;
        }
    }

    return new_house_mode;
}

static l_house_modes_t *__house_modes_add(cJSON *cj_house_modes)
{
    l_house_modes_t *tmp_house_mode_head = NULL;
    if (cj_house_modes)
    {
        int house_mode_idx = cJSON_GetArraySize(cj_house_modes);
        cJSON *cj_house_mode = NULL;

        while (NULL != (cj_house_mode = cJSON_GetArrayItem(cj_house_modes, house_mode_idx)))
        {
            TRACE_B("house_mode-%d:", house_mode_idx);

            if (tmp_house_mode_head)
            {
                l_house_modes_t *tmp_house_mode = tmp_house_mode_head;
                while (tmp_house_mode->next)
                {
                    tmp_house_mode = tmp_house_mode->next;
                }

                tmp_house_mode->next = __new_house_mode_create(cj_house_mode);
            }
            else
            {
                tmp_house_mode_head = __new_house_mode_create(cj_house_mode);
            }
        }
    }

    return tmp_house_mode_head;
}

static l_user_notification_t *__new_user_notification_create(cJSON *cj_user_notification)
{
    l_user_notification_t *new_user_notification = NULL;

    if (cj_user_notification)
    {
        new_user_notification = malloc(sizeof(l_user_notification_t));
        if (new_user_notification)
        {
            snprintf(new_user_notification->user_id, sizeof(new_user_notification->user_id, "%s", cj_user_notification->valuestring));
            new_user_notification->next = NULL;
        }
    }

    return new_user_notification;
}

static l_user_notification_t *__user_notifications_add(cJSON *cj_user_notifications)
{
    l_user_notification_t *tmp_user_notifications_head = NULL;
    if (cj_user_notifications)
    {
        int user_notifications_idx = cJSON_GetArraySize(cj_user_notifications);
        cJSON *cj_user_notification = NULL;

        while (NULL != (cj_user_notification = cJSON_GetArrayItem(cj_user_notifications, user_notifications_idx)))
        {
            if (tmp_user_notifications_head)
            {
                l_user_notification_t *curr_user_notification = tmp_user_notifications_head;
                while (curr_user_notification->next)
                {
                    curr_user_notification = curr_user_notification->next;
                }
                curr_user_notification->next = __new_user_notification_create(cj_user_notification);
            }
            else
            {
                tmp_user_notifications_head = __new_user_notification_create(cj_user_notification);
            }
        }
    }

    return tmp_user_notifications_head;
}
