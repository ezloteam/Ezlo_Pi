#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_ezlopi_broadcast.h"
#include "ezlopi_core_scenes_status_changed.h"

#include "ezlopi_service_webprov.h"

int ezlopi_scenes_status_change_broadcast(l_scenes_list_v2_t* scene_node, const char* status_str)
{
    int ret = 0;
    if (scene_node)
    {
        cJSON* cj_response = cJSON_CreateObject();
        if (cj_response)
        {
            cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
            cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass_str, method_hub_scene_run_progress);
            cJSON* cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
            if (cj_result)
            {
                char tmp_str[32];
                snprintf(tmp_str, sizeof(tmp_str), "%08x", scene_node->_id);
                cJSON_AddStringToObject(cj_result, ezlopi_scene_id_str, tmp_str);
                cJSON_AddStringToObject(cj_result, ezlopi_scene_name_str, scene_node->name);
                cJSON_AddStringToObject(cj_result, ezlopi_status_str, status_str ? status_str : scene_status_failed_str);
                cJSON* cj_notifications = cJSON_AddArrayToObject(cj_result, ezlopi_notifications_str);

                if (scene_node->user_notifications && cj_notifications)
                {
                    cJSON_AddTrueToObject(cj_result, ezlopi_userNotification_str);
                    l_user_notification_v2_t* user_notification_node = scene_node->user_notifications;
                    while (user_notification_node)
                    {
                        cJSON* cj_notf = cJSON_CreateString(user_notification_node->user_id);
                        if (!cJSON_AddItemToArray(cj_notifications, cj_notf))
                        {
                            cJSON_Delete(cj_notf);
                        }

                        user_notification_node = user_notification_node->next;
                    }
                }
                else
                {
                    cJSON_AddFalseToObject(cj_result, ezlopi_userNotification_str);
                }

                cJSON_AddStringToObject(cj_result, ezlopi_room_id_str, ezlopi__str);
                cJSON_AddStringToObject(cj_result, ezlopi_room_name_str, ezlopi__str);
            }

            char* data_to_send = cJSON_Print(cj_response);
            cJSON_Delete(cj_response);

            if (data_to_send)
            {
                cJSON_Minify(data_to_send);
                ret = ezlopi_service_web_provisioning_send_str_data_to_nma_websocket(data_to_send, TRACE_TYPE_D);
                if (0 == ezlopi_core_ezlopi_broadcast_methods_send_to_queue(data_to_send)) {
                    free(data_to_send);
                }
            }
        }
    }

    return ret;
}

const char* ezlopi_scenes_status_to_string(e_scene_status_v2_t scene_status)
{
    const char* ret = "NULL";
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
    default:
    {
        ret = "EZLOPI_SCENE_STATUS_NONE";
        break;
    }
    }

    return ret;
}