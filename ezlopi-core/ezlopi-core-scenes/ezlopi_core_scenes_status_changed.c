/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_scenes_status_changed.c
 * @brief   These function operates on scene status info
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS
#include <time.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_status_changed.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
int EZPI_core_scenes_status_change_broadcast(l_scenes_list_v2_t *scene_node, const char *status_str, time_t time_stamp)
{
    int ret = 0;
    if (scene_node)
    {
        cJSON *cj_response = cJSON_CreateObject(__FUNCTION__);
        if (cj_response)
        {
            // cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_startTime_str, EZPI_core_sntp_get_current_time_sec());

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
                        if (NULL != user_notification_node->user_id)
                        {
                            cJSON *cj_notf = cJSON_CreateString(__FUNCTION__, user_notification_node->user_id);
                            if (!cJSON_AddItemToArray(cj_notifications, cj_notf))
                            {
                                cJSON_Delete(__FUNCTION__, cj_notf);
                            }
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

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
#endif

            ret = EZPI_core_broadcast_add_to_queue(cj_response, time_stamp);

            if (0 != ret)
            {
                cJSON_Delete(__FUNCTION__, cj_response);
            }
        }
    }

    return ret;
}

const char *EZPI_core_scenes_status_to_string(e_scene_status_v2_t scene_status)
{
    const char *ret = ezlopi__str;
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

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/