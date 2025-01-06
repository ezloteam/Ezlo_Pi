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
 * @file    ezlopi_core_scenes_when_methods.h
 * @brief   Functions that operates on scene-when-methods
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
*/

#ifndef _EZLOPI_CORE_SCENES_WHEN_METHODS_H_
#define _EZLOPI_CORE_SCENES_WHEN_METHODS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
*                          Include Files
*******************************************************************************/
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
    /* When methods */
    /**
     * @brief Triggers when-condtion for :
     *
     * @param scene_node Target scene-node
     * @param arg User-Arg
     * @return int
     */
    int EZPI_core_scenes_when_is_item_state(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_interva
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_interval(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : item_state_changed
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_item_state_changed(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_button_state
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_button_state(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_sun_state
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_sun_state(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_date
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_date(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_once
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_once(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_date_range
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_date_range(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_user_lock_operation
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_user_lock_operation(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_house_mode_changed_to
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_house_mode_changed_to(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_house_mode_changed_from
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_house_mode_changed_from(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_house_mode_alarm_phase_range
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_house_mode_alarm_phase_range(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_house_mode_switch_to_range
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_house_mode_switch_to_range(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_device_state
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_device_state(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_network_state
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_network_state(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_scene_state
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_scene_state(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_group_state
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_group_state(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_cloud_state
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_cloud_state(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_battery_state
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_battery_state(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_battery_level
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_battery_level(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : when_compare_numbrers
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_compare_numbers(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : when_compare_num_range
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_compare_number_range(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : when_compare_strings
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_compare_strings(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : when_string_operation
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_string_operation(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : when_in_array
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_in_array(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : when_compare_values
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_compare_values(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : has_atleast_one_dictionary_value
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_has_atleast_one_dictionary_value(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_firmware_update_state
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_firmware_update_state(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_dictionary_changed
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_dictionary_changed(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_detected_in_hot_zone
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_detected_in_hot_zone(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : AND
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_and(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : NOT
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_not(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : OR
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_or(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : XOR
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_xor(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : function-condition
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scene_when_function(l_scenes_list_v2_t *scene_node, void *arg);
    /**
    * @brief Triggers when-condtion for : is_device_item_group
    *
    * @param scene_node Target scene-node
    * @param arg User-Arg
    * @return int
    */
    int EZPI_core_scenes_when_is_device_item_group(l_scenes_list_v2_t *scene_node, void *arg);



#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_WHEN_METHODS_H_

/*******************************************************************************
*                          End of File
*******************************************************************************/





