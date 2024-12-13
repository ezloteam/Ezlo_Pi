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
* @file    ezlopi_core_scenes_edit.c
* @brief   This file contains function that edits scene data.
* @author  xx
* @version 0.1
* @date    12th DEC 2024
*/
/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_edit.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_populate.h"
#include "ezlopi_core_scenes_delete.h"
#include "ezlopi_core_errors.h"
#include "ezlopi_core_cloud.h"

#include "ezlopi_service_meshbot.h"

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
static void __edit_scene(l_scenes_list_v2_t *scene_node, cJSON *cj_scene);

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/
ezlopi_error_t EZPI_core_scenes_edit_store_updated_to_nvs(cJSON *cj_updated_scene)
{
    ezlopi_error_t error = EZPI_SUCCESS;
    if (cj_updated_scene)
    {
        if (EZPI_core_scenes_add_when_blockId_if_reqd(cj_updated_scene))
        {
            TRACE_S("==> Added new_blockIds (Edit) : SUCCESS");
        }

        if (EZPI_core_scenes_add_group_id_if_reqd(cj_updated_scene))
        {
            TRACE_S("==> Added new_group_id (Edit) : SUCCESS");
        }

        char *update_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_updated_scene, 4096, false);
        TRACE_D("length of 'update_scene_str': %d", strlen(update_scene_str));

        if (update_scene_str)
        {
            cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_updated_scene, ezlopi__id_str);
            if (cj_scene_id && cj_scene_id->valuestring)
            {
                ezlopi_nvs_delete_stored_data_by_name(cj_scene_id->valuestring);
                error = ezlopi_nvs_write_str(update_scene_str, strlen(update_scene_str), cj_scene_id->valuestring);
            }

            ezlopi_free(__FUNCTION__, update_scene_str);
        }
    }

    return error;
}

ezlopi_error_t EZPI_core_scenes_edit_update_id(uint32_t scene_id, cJSON *cj_updated_scene)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if (scene_id && cj_updated_scene)
    {
        l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_scene_head_v2();
        while (scene_node)
        {
            if (scene_id == scene_node->_id)
            {
                CJSON_TRACE("cj_updated_scene", cj_updated_scene);
                ezlopi_meshobot_service_stop_scene(scene_node);
                __edit_scene(scene_node, cj_updated_scene);

                // TRACE_S("HERE : scene_node->enabled = [%s]", (scene_node->enabled) ? ezlopi_true_str : ezlopi_false_str);
                if (scene_node->enabled == true)
                {
                    ezlopi_meshbot_service_start_scene(scene_node);
                }
                ret = EZPI_SUCCESS;

                // TRACE_D("----------------------------------------------");
                // TRACE_D("esp_get_free_heap_size - %f kB", esp_get_free_heap_size() / 1024.0);
                // TRACE_D("esp_get_minimum_free_heap_size: %f kB", esp_get_minimum_free_heap_size() / 1024.0);
                // TRACE_D("----------------------------------------------");

                break;
            }

            scene_node = scene_node->next;
        }
    }
    return ret;
}


/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/
static void __edit_scene(l_scenes_list_v2_t *scene_node, cJSON *cj_scene)
{
    CJSON_GET_VALUE_BOOL(cj_scene, ezlopi_enabled_str, scene_node->enabled);
    CJSON_GET_VALUE_BOOL(cj_scene, ezlopi_is_group_str, scene_node->is_group);

    {
        char tmp_grp_id[32] = { 0 };
        CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_group_id_str, tmp_grp_id);
        if (0 < strlen(tmp_grp_id))
        {
            scene_node->group_id = (uint32_t)strtoul(tmp_grp_id, NULL, 16);
            TRACE_S("new_group_id (edit): %08x", scene_node->group_id);
            ezlopi_cloud_update_group_id(scene_node->group_id);
        }
    }

    CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_name_str, scene_node->name);
    CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_parent_id_str, scene_node->parent_id);

    {
        cJSON *cj_meta = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_meta_str);
        if (cj_meta && (cJSON_Object == cj_meta->type))
        {
            if (scene_node->meta)
            {
                cJSON_Delete(__FUNCTION__, scene_node->meta);
                scene_node->meta = NULL;
            }
            scene_node->meta = cJSON_Duplicate(__FUNCTION__, cj_meta, 1);
        }
    }

    {
        cJSON *cj_user_notifications = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_user_notifications_str);
        if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
        {
            EZPI_scenes_delete_user_notifications(scene_node->user_notifications);
            scene_node->user_notifications = EZPI_scenes_populate_user_notifications(cj_user_notifications);
        }
    }

    {
        cJSON *cj_house_modes = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_house_modes_str);
        if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
        {
            EZPI_scenes_delete_house_modes(scene_node->house_modes);
            scene_node->house_modes = EZPI_scenes_populate_house_modes(cj_house_modes);
        }
    }

    {
        cJSON *cj_then_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_then_str);
        if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
        {
            EZPI_scenes_delete_action_blocks(scene_node->then_block);
            scene_node->then_block = EZPI_scenes_populate_action_blocks(cj_then_blocks, SCENE_BLOCK_TYPE_THEN);
        }
    }

    {
        cJSON *cj_when_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_when_str);
        if (cj_when_blocks && (cJSON_Array == cj_when_blocks->type))
        {
            EZPI_scenes_delete_when_blocks(scene_node->when_block);
            scene_node->when_block = EZPI_scenes_populate_when_blocks(cj_when_blocks);
        }
    }

    {
        cJSON *cj_else_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_else_str);
        if (cj_else_blocks && (cJSON_Array == cj_else_blocks->type))
        {
            EZPI_scenes_delete_action_blocks(scene_node->else_block);
            scene_node->else_block = EZPI_scenes_populate_action_blocks(cj_else_blocks, SCENE_BLOCK_TYPE_ELSE);
        }
    }
}

/*******************************************************************************
*                          End of File
*******************************************************************************/




#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS