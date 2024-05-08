#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_util_trace.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_ezlopi_broadcast.h"
#include "ezlopi_core_scenes_status_changed.h"

int ezlopi_scenes_status_change_broadcast(l_scenes_list_v2_t* scene_node, const char* status_str)
{
    int ret = 0;
    if (scene_node)
    {
        cJSON *cj_response = cJSON_CreateObject(__FUNCTION__);
        if (cj_response)
        {
            vTaskDelay(5000);
            cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
            cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, method_hub_scene_run_progress);
            cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
            if (cj_result)
            {
                char tmp_str[32];
                snprintf(tmp_str, sizeof(tmp_str), "%08x", scene_node->_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_scene_id_str, tmp_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_scene_name_str, scene_node->name);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_status_str, status_str ? status_str : scene_status_failed_str);
                cJSON *cj_notifications = cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_notifications_str);

                if (scene_node->user_notifications && cj_notifications)
                {
                    cJSON_AddTrueToObject(__FUNCTION__, cj_result, ezlopi_userNotification_str);
                    l_user_notification_v2_t *user_notification_node = scene_node->user_notifications;
                    while (user_notification_node)
                    {
                        cJSON *cj_notf = cJSON_CreateString(__FUNCTION__, user_notification_node->user_id);
                        if (!cJSON_AddItemToArray(cj_notifications, cj_notf))
                        {
                            cJSON_Delete(__FUNCTION__, cj_notf);
                        }

                        user_notification_node = user_notification_node->next;
                    }
                }
                else
                {
                    cJSON_AddFalseToObject(__FUNCTION__, cj_result, ezlopi_userNotification_str);
                }

                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_room_id_str, ezlopi__str);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_room_name_str, ezlopi__str);
            }

            CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);

            ret = ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response);

            if (0 == ret)
            {
                cJSON_Delete(__FUNCTION__, cj_response);
            }
        }
    }

    return ret;
}

const char* ezlopi_scenes_status_to_string(e_scene_status_v2_t scene_status)
{
    const char* ret = "";
#if (1 == ENABLE_TRACE)
    switch (scene_status)
    {
    case EZLOPI_SCENE_STATUS_RUN:
    {
        ret = "EZLOPI_SCENE_STATUS_RUN";
        break;
    }
    case EZLOPI_SCENE_STATUS_RUNNING:
    {
        ret = "EZLOPI_SCENE_STATUS_RUNNING";
        break;
    }
    case EZLOPI_SCENE_STATUS_STOP:
    {
        ret = "EZLOPI_SCENE_STATUS_STOP";
        break;
    }
    case EZLOPI_SCENE_STATUS_STOPPED:
    {
        ret = "EZLOPI_SCENE_STATUS_STOPPED";
        break;
    }
    #warning "need to add status_failed";
    default:
    {
        ret = "EZLOPI_SCENE_STATUS_NONE";
        break;
    }
    }
#endif

    return ret;
}
#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS