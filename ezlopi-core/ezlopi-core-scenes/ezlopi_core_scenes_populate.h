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
 * @file    ezlopi_core_scenes_populate.h
 * @brief   These function perfrom scenes populate operations
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_SCENES_POPULATE_H_
#define _EZLOPI_CORE_SCENES_POPULATE_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
// #include "cjext.h"
// #include <stdint.h>

#include "ezlopi_core_scenes_v2.h"

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
    /**
     * @brief This function populates new field values from 'cj_field'
     *
     * @param cj_field source cjson-obj to extract data from.
     * @return l_fields_v2_t *
     */
    l_fields_v2_t *EZPI_scenes_populate_fields(cJSON *cj_fields);
    /**
     * @brief This function populates field values from 'cj_field' to 'new_field' struct
     *
     * @param new_field Target field struct
     * @param cj_field source cjson-obj to extract data from.
     */
    void EZPI_scenes_populate_assign_field(l_fields_v2_t *new_field, cJSON *cj_field);
    /**
     * @brief This function populate the 'action_delay' values of a scene
     *
     * @param action_delay Structure to populate
     * @param cj_delay source 'cj_delay' to extract data from.
     */
    void EZPI_scenes_populate_assign_action_delay(s_action_delay_v2_t *action_delay, cJSON *cj_delay);
    /**
     * @brief This function populates the blockOptions of a scene
     *
     * @param p_block_options Destination struct to populate
     * @param cj_block_options Source cjson to extract data from.
     */
    void EZPI_scenes_populate_assign_block_options(s_block_options_v2_t *p_block_options, cJSON *cj_block_options);
    /**
     * @brief This function populates the method-info of a scene
     *
     * @param p_method Destination struct to populate
     * @param cj_method Source cjson to extract data from.
     */
    void EZPI_scenes_populate_assign_method(s_method_v2_t *p_method, cJSON *cj_method);
    /**
     * @brief The funciton returns populated when-block of a scene
     *
     * @param cj_when_blocks Source CJSON used to extract the data from.
     * @return l_when_block_v2_t*
     */
    l_when_block_v2_t *EZPI_scenes_populate_when_blocks(cJSON *cj_when_blocks);
    /**
     * @brief This function populates the when-block of a scene
     *
     * @param new_when_block Destination when-block  which gets populated
     * @param cj_when_block Source CJSON used to extract the data from.
     */
    void EZPI_scenes_populate_assign_when_block(l_when_block_v2_t *new_when_block, cJSON *cj_when_block);
    /**
     * @brief This function returns populated action-block of a scene
     *
     * @param cj_then_blocks Source CJSON used to extract the data from.
     * @param block_type
     * @return l_action_block_v2_t*
     */
    l_action_block_v2_t *EZPI_scenes_populate_action_blocks(cJSON *cj_then_blocks, e_scenes_block_type_v2_t block_type);
    /**
     * @brief This function populates the when-block of a scene
     *
     * @param new_action_block Destination action-block  which gets populated
     * @param cj_action_block Source CJSON used to extract the data from.
     * @param block_type Enum to determine : THEN-block or ELSE-block
     */
    void EZPI_scenes_populate_assign_action_block(l_action_block_v2_t *new_action_block, cJSON *cj_action_block, e_scenes_block_type_v2_t block_type);
    /**
     * @brief
     *
     * @param cj_house_modes
     * @return l_house_modes_v2_t*
     */
    l_house_modes_v2_t *EZPI_scenes_populate_house_modes(cJSON *cj_house_modes);
    /**
     * @brief This function populates the 'house_mode' of a scene
     *
     * @param new_house_mode Destination 'house_mode' block which gets populated
     * @param cj_house_mode Source CJSON used to extract the data from.
     */
    void EZPI_scenes_populate_assign_house_mode(l_house_modes_v2_t *new_house_mode, cJSON *cj_house_mode);
    /**
     * @brief This function returns populated 'user-notification' block of a scene
     *
     * @param cj_user_notifications Source CJSON used to extract the data from.
     * @return l_user_notification_v2_t*
     */
    l_user_notification_v2_t *EZPI_scenes_populate_user_notifications(cJSON *cj_user_notifications);
    /**
     * @brief This function populates the 'user-notification' of a scene
     *
     * @param new_user_notification Destination 'user-notification' struct  which gets populated
     * @param cj_user_notification Source CJSON used to extract the data from.
     */
    void EZPI_scenes_populate_assign_user_notification(l_user_notification_v2_t *new_user_notification, cJSON *cj_user_notification);
    /**
     * @brief This function populates the new-scene node in linked-list
     *
     * @param new_scene Destination scene-node that gets populated
     * @param cj_scene Source CJSON used to extract the data from.
     * @param scene_id Id of new-scene.
     */
    void EZPI_scenes_populate_scene(l_scenes_list_v2_t *new_scene, cJSON *cj_scene, uint32_t scene_id);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_POPULATE_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
