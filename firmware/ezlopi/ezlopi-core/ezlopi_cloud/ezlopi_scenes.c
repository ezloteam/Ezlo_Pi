#include <string.h>
#include "cJSON.h"

#include "trace.h"

#include "ezlopi_scenes.h"
#include "ezlopi_devices.h"
#include "ezlopi_nvs.h"

static l_scenes_list_t *scenes_list_head = NULL;

static l_house_modes_t *__house_mode_create(cJSON *cj_house_mode)
{
    l_house_modes_t *new_house_mode = NULL;

    if (cj_house_mode && (cJSON_Number == cj_house_mode->type))
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

static l_house_modes_t *__house_mode_add(cJSON *cj_house_modes)
{
    l_house_modes_t *tmp_house_mode_head = NULL;
    if (cj_house_modes)
    {
        int house_mode_idx = 0;
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

                tmp_house_mode->next = __house_mode_create(cj_house_mode);
            }
            else
            {
                tmp_house_mode_head = __house_mode_create(cj_house_mode);
            }
        }
    }

    return tmp_house_mode_head;
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

            cJSON *cj_house_modes = cJSON_GetObjectItem(cj_scene, "house_modes");
            if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
            {
                new_scene->house_modes = __house_mode_add(cj_house_modes);
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

void ezlopi_scene_delete(void)
{
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