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
 * @file    ezlopi_core_scenes_v2.h
 * @brief   These functions perform  scene creation operations
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_SCENES_V2_H_
#define _EZLOPI_CORE_SCENES_V2_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdbool.h>
// #include <stdint.h>
#include <ctype.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_core_errors.h"
#include "ezlopi_core_scenes_v2_type_declaration.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    // ----- # below function are for APIs # ---------
    /**
     * @brief Sets/resets block enable flag of a scene [for scene-API].
     *
     * @param sceneId_str Id of scene
     * @param blockId_str Id of block of a scene
     * @param enable_status Flag value
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_scenes_block_enable_set_reset(const char *sceneId_str, const char *blockId_str, bool enable_status);
    /**
     * @brief Sets/resets latch-state of a scene [for scene-API].
     *
     * @param sceneId_str Id of scene
     * @param blockId_str Id of block of a scene
     * @return int
     */
    int EZPI_core_scenes_reset_latch_state(const char *sceneId_str, const char *blockId_str);
    /**
     * @brief Resets when-block of a scene [for scene-API].
     *
     * @param sceneId_str Id of scene
     * @param blockId_str Id of block of a scene
     * @return int
     */
    int EZPI_core_scenes_reset_when_block(const char *sceneId_str, const char *blockId_str);
    /**
     * @brief Resets when-block of a scene [for scene-API].
     *
     * @param sceneId_str Id of scene
     * @param blockId_str Id of block of a scene
     * @param cj_meta cjson meta info to set
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_scenes_set_meta_by_id(const char *sceneId_str, const char *blockId_str, cJSON *cj_meta);
    /**
     * @brief Get the time based scenes_list from ll
     *
     * @param cj_scenes_array Array of scenes to extract from
     * @return int
     */
    int EZPI_core_scenes_get_time_list(cJSON *cj_scenes_array);
    /**
     * @brief Add scene_group_id to a new scene-node
     *
     * @param cj_new_scene Destination scene-node
     * @return int
     */
    int EZPI_core_scenes_add_group_id_if_reqd(cJSON *cj_new_scene);
    /**
     * @brief Add when-block Id to a perticular scene-node
     *
     * @param cj_new_scene Pointer to the target scene-node
     * @return int
     */
    int EZPI_core_scenes_add_when_blockId_if_reqd(cJSON *cj_new_scene);
    /**
     * @brief Return the when-block of perticular scene
     *
     * @param scene_id Id of scene
     * @param group_id Id of group of the scene-node
     * @return l_when_block_v2_t*
     */
    l_when_block_v2_t *EZPI_core_scenes_get_group_block(uint32_t scene_id, uint32_t group_id);

    /**
     * @brief Depopulate a scene-node from the ll by '_id'
     *
     * @param _id Target sceneId
     */
    void EZPI_core_scenes_depopulate_by_id_v2(uint32_t _id);
    /**
     * @brief Enable/Disable scene-node activation
     *
     * @param _id Target Scene_id
     * @param enabled_flag Scene activation flag
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_scenes_enable_disable_scene_by_id_v2(uint32_t _id, bool enabled_flag);
    /**
     * @brief Remove perticulat scene-node by _id
     *
     * @param _id Target Scene_id
     */
    void EZPI_core_scenes_remove_id_from_list_v2(uint32_t _id);
    /**
     * @brief Remove a scene_node from ll
     *
     * @param _id Target Scene_id
     * @return l_scenes_list_v2_t*
     */
    l_scenes_list_v2_t *EZPI_core_scenes_pop_by_id_v2(uint32_t _id);
    /**
     * @brief Edit scene_node by _id
     *
     * @param scene_id Target Scene_id
     * @param cj_scene Pointer to scene-node
     * @return int
     */
    int EZPI_core_scenes_edit_by_id(uint32_t scene_id, cJSON *cj_scene);
    /**
     * @brief Append new users into notification cjson_array for a scene
     *
     * @param scene_node Target Scene_node
     * @param cj_user Pointer to new user info
     */
    void EZPI_core_scenes_add_users_in_notifications(l_scenes_list_v2_t *scene_node, cJSON *cj_user);
    /**
     * @brief Store new scene node info into nvs
     *
     * @param cj_new_scene Pointer to new scene-node
     * @return uint32_t
     */
    uint32_t EZPI_core_scenes_store_new_scene_v2(cJSON *cj_new_scene);
    /**
     * @brief Get the list of scene created till now
     *
     * @param cj_scenes_array Pointer to array of scenes
     * @return uint32_t
     */
    uint32_t EZPI_core_scenes_get_list_v2(cJSON *cj_scenes_array);
    /**
     * @brief Get the scenes head node
     *
     * @return l_scenes_list_v2_t*
     */
    l_scenes_list_v2_t *EZPI_core_scenes_get_scene_head_v2(void);
    /**
     * @brief Get the scene_node by '_id'
     *
     * @param _id Target Scene_id
     * @return l_scenes_list_v2_t*
     */
    l_scenes_list_v2_t *EZPI_core_scenes_get_by_id_v2(uint32_t _id);
    /**
     * @brief Populate and return pointer to new scene_node
     *
     * @param cj_new_scene Destination obj where new scene_node is appended
     * @param scene_id New Scene_id
     * @return l_scenes_list_v2_t*
     */
    l_scenes_list_v2_t *EZPI_core_scenes_new_scene_populate(cJSON *cj_new_scene, uint32_t scene_id);
    /**
     * @brief Return function ptr to desired scenes method type
     *
     * @param scene_method_type Desired scene-method-type
     * @return f_scene_method_v2_t
     */
    f_scene_method_v2_t EZPI_core_scenes_get_method_v2(e_scene_method_type_t scene_method_type);

    /**
     * @brief Function to initialize scenes-task
     *
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_scenes_init_v2(void);

    // e_scene_value_type_v2_t EZPI_core_scenes_get_value_type(cJSON *cj_field);
    // e_scene_value_type_v2_t EZPI_core_scenes_get_expressions_value_type(cJSON *cj_value_type);
    //----------------------------------------------------------------------------------------

#if 0 // for future usage
//-------------------------------- Only for latch operations  ----------------------------------------
/**
 * @brief This function checks for 'latch' struct within nvs_scenes. The scenes are filtered out using 'sceneId[necessary]' & 'blockId[optional]'
 *
 * @param sceneId_str contains required sceneId value
 * @param blockId_str contains required blockId value (when-condition). If (blockID == NULL) ; means to delete all latches contained within sceneId.
 * @param enable_status enable [true or false] -> [1 or 0]
 * @return successful reset => 1 / else => 0.
 */
    int EZPI_core_scenes_set_reset_latch_enable(const char *sceneId_str, const char *blockId_str, bool enable_status);
#endif

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_V2_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/