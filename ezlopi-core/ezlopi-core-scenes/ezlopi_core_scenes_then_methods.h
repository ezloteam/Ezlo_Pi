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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef _EZLOPI_CORE_SCENES_THEN_METHODS_H_
#define _EZLOPI_CORE_SCENES_THEN_METHODS_H_

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
    /**
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */
    int ezlopi_scene_then_set_item_value(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_group_set_item_value(l_scenes_list_v2_t* curr_scene, void* arg);
    int ezlopi_scene_then_set_device_armed(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_group_set_device_armed(l_scenes_list_v2_t* curr_scene, void* arg);
    int ezlopi_scene_then_send_cloud_abstract_command(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_switch_house_mode(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_send_http_request(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_run_custom_script(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_run_plugin_script(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_run_scene(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_set_scene_state(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_reset_latch(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_reset_scene_latches(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_reboot_hub(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_reset_hub(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_cloud_api(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_set_expression(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_set_variable(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_toggle_value(l_scenes_list_v2_t* scene_node, void* arg);
    int ezlopi_scene_then_group_toggle_value(l_scenes_list_v2_t* curr_scene, void* arg);

#ifdef __cplusplus
}
#endif

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_THEN_METHODS_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
