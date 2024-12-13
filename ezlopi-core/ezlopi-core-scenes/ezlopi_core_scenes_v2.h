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
 * @file    ezlopi_core_scenes_v2.h
 * @brief   These functions perform  scene creation operations
 * @author  xx
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
    void ezlopi_scenes_depopulate_by_id_v2(uint32_t _id);
    ezlopi_error_t ezlopi_scenes_enable_disable_scene_by_id_v2(uint32_t _id, bool enabled_flag);
    void ezlopi_scenes_remove_id_from_list_v2(uint32_t _id);
    l_scenes_list_v2_t *ezlopi_scenes_pop_by_id_v2(uint32_t _id);
    // ----- # below function are for APIs # ---------
    ezlopi_error_t ezlopi_core_scene_block_enable_set_reset(const char *sceneId_str, const char *blockId_str, bool enable_status);
    int ezlopi_core_scene_reset_latch_state(const char *sceneId_str, const char *blockId_str);
    int ezlopi_core_scene_reset_when_block(const char *sceneId_str, const char *blockId_str);
    // ----- # below function are for APIs # ---------
    ezlopi_error_t ezlopi_core_scene_meta_by_id(const char *sceneId_str, const char *blockId_str, cJSON *cj_meta);
    int ezlopi_core_scenes_get_time_list(cJSON *cj_scenes_array);
    // ----- # below function are called when 'creating' and 'editing' scene # ---------
    int ezlopi_core_scene_add_group_id_if_reqd(cJSON *cj_new_scene);
    int ezlopi_core_scene_add_when_blockId_if_reqd(cJSON *cj_new_scene);
    l_when_block_v2_t *ezlopi_core_scene_get_group_block(uint32_t scene_id, uint32_t group_id);


    uint32_t ezlopi_store_new_scene_v2(cJSON *cj_new_scene);
    uint32_t ezlopi_scenes_get_list_v2(cJSON *cj_scenes_array);
    int ezlopi_scene_edit_by_id(uint32_t scene_id, cJSON *cj_scene);
    l_scenes_list_v2_t *ezlopi_scenes_get_scenes_head_v2(void);
    f_scene_method_v2_t ezlopi_scene_get_method_v2(e_scene_method_type_t scene_method_type);
    l_scenes_list_v2_t *ezlopi_scenes_get_by_id_v2(uint32_t _id);
    l_scenes_list_v2_t *ezlopi_scenes_new_scene_populate(cJSON *cj_new_scene, uint32_t scene_id);

    // e_scene_value_type_v2_t ezlopi_scenes_get_value_type(cJSON *cj_field);
    // e_scene_value_type_v2_t ezlopi_scenes_get_expressions_value_type(cJSON *cj_value_type);

    void ezlopi_scene_add_users_in_notifications(l_scenes_list_v2_t *scene_node, cJSON *cj_user);

    ezlopi_error_t ezlopi_scenes_init_v2(void);
 

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
    int ezlopi_core_scene_set_reset_latch_enable(const char *sceneId_str, const char *blockId_str, bool enable_status);
#endif

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_V2_H_

/*******************************************************************************
*                          End of File
*******************************************************************************/