#include <cJSON.h>

#include "trace.h"

#include "ezlopi_nvs.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_cjson_macros.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_scenes_methods.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_meshbot_service.h"
#include "ezlopi_scenes_when_methods.h"
#include "ezlopi_scenes_then_methods.h"
#include "ezlopi_scenes_notifications.h"
#include "ezlopi_scenes_status_changed.h"

void ezlopi_scenes_notifications_add(cJSON *cj_notifications)
{
    if (cj_notifications)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(cj_notifications, ezlopi_notifications_str);
        cJSON *cj_user_id = cJSON_GetObjectItem(cj_notifications, ezlopi_userId_str);

        if (cj_scene_id && cj_scene_id->valuestring && cj_user_id && cj_user_id->valuestring)
        {
            char *scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON *cj_scene = cJSON_Parse(scene_str);
                free(scene_str);

                if (cj_scene)
                {
                    cJSON *cj_user_notifications = cJSON_GetObjectItem(cj_scene, ezlopi_user_notifications_str);
                    if (cj_user_notifications)
                    {
                        cJSON_AddItemReferenceToArray(cj_user_notifications, cj_user_id);
                    }

                    CJSON_TRACE("updated-scene", cj_scene);
                    char *updated_scene_str = cJSON_Print(cj_scene);
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
                l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_head_v2();
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

void ezlopi_scenes_notifications_remove(cJSON *cj_notifications)
{
    if (cj_notifications)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(cj_notifications, ezlopi_notifications_str);
        cJSON *cj_user_id_del = cJSON_GetObjectItem(cj_notifications, ezlopi_userId_str);

        if (cj_scene_id && cj_scene_id->valuestring && cj_user_id_del && cj_user_id_del->valuestring)
        {
            char *scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON *cj_scene = cJSON_Parse(scene_str);
                free(scene_str);

                if (cj_scene)
                {
                    cJSON *cj_user_notifications = cJSON_GetObjectItem(cj_scene, ezlopi_user_notifications_str);
                    if (cj_user_notifications && cj_user_id_del)
                    {
                        uint32_t idx = 0;
                        cJSON *cj_user_id = NULL;
                        while (NULL != (cj_user_id = cJSON_GetArrayItem(cj_user_notifications, idx)))
                        {
                            if (0 == strcmp(cj_user_id->valuestring, cj_user_id_del->valuestring))
                            {
                                cJSON_DeleteItemFromArray(cj_user_notifications, idx);
                            }
                            idx++;
                        }
                    }

                    CJSON_TRACE("updated-scene", cj_scene);
                    char *updated_scene_str = cJSON_Print(cj_scene);
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
                l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_head_v2();
                while (scene_node)
                {
                    if (scene_node->_id == scene_id)
                    {
                        if (0 == strcmp(scene_node->user_notifications->user_id, cj_user_id_del->valuestring))
                        {
                            l_user_notification_v2_t *user_id_del = scene_node->user_notifications;
                            scene_node->user_notifications = scene_node->user_notifications->next;
                            user_id_del->next = NULL;
                            ezlopi_scenes_delete_user_notifications(user_id_del);
                        }
                        else
                        {
                            l_user_notification_v2_t *user_node = scene_node->user_notifications;
                            while (user_node->next)
                            {
                                if (0 == strcmp(user_node->user_id, cj_user_id_del->valuestring))
                                {
                                    l_user_notification_v2_t *user_id_del = user_node;
                                    user_node = user_node->next;
                                    user_id_del->next = NULL;
                                    ezlopi_scenes_delete_user_notifications(user_id_del);
                                    break;
                                }

                                user_node = user_node->next;
                            }
                        }

                        break;
                    }
                    scene_node = scene_node->next;
                }
            }
        }
    }
}
