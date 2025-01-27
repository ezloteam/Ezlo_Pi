/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    ezlopi_core_scenes_notifications.c
 * @brief   These function performes operation on scene-notifications
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    December 31st, 2023 11:52 AM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "cjext.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_v2_type_declaration.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_notifications.h"

#include "ezlopi_cloud_constants.h"
// #include "ezlopi_util_trace.h"

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
void EZPI_scenes_notifications_add(cJSON *cj_notifications)
{
    if (cj_notifications)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_notifications, ezlopi_notifications_str);
        cJSON *cj_user_id = cJSON_GetObjectItem(__FUNCTION__, cj_notifications, ezlopi_userId_str);

        if (cj_scene_id && cj_scene_id->valuestring && cj_user_id && cj_user_id->valuestring)
        {
            char *scene_str = EZPI_core_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);

                if (cj_scene)
                {
                    cJSON *cj_user_notifications = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_user_notifications_str);
                    if (cj_user_notifications)
                    {
                        cJSON_AddItemReferenceToArray(__FUNCTION__, cj_user_notifications, cj_user_id);
                    }

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                    CJSON_TRACE("updated-scene", cj_scene);
#endif

                    char *updated_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 4096, false);
                    cJSON_Delete(__FUNCTION__, cj_scene);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                    TRACE_D("length of 'updated_scene_str': %d", strlen(updated_scene_str));
#endif

                    if (updated_scene_str)
                    {
                        EZPI_core_nvs_write_str(updated_scene_str, strlen(updated_scene_str), cj_scene_id->valuestring);
                        ezlopi_free(__FUNCTION__, updated_scene_str);
                    }
                }
            }

            uint32_t scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);

            if (scene_id)
            {
                l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_scene_head_v2();
                while (scene_node)
                {
                    if (scene_id == scene_node->_id)
                    {
                        EZPI_core_scenes_add_users_in_notifications(scene_node, cj_user_id);
                        break;
                    }
                    scene_node = scene_node->next;
                }
            }
        }
    }
}

// void EZPI_scenes_notifications_remove(cJSON *cj_notifications)
// {
//     if (cj_notifications)
//     {
//     }
// }

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
