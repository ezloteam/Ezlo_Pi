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

#ifndef _EZLOPI_CORE_SCENES_POPULATE_H_
#define _EZLOPI_CORE_SCENES_POPULATE_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "cjext.h"
#include <stdint.h>

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
    l_fields_v2_t* ezlopi_scenes_populate_fields(cJSON* cj_fields);
    void ezlopi_scenes_populate_assign_field(l_fields_v2_t* new_field, cJSON* cj_field);

    void ezlopi_scenes_populate_assign_action_delay(s_action_delay_v2_t* action_delay, cJSON* cj_delay);
    void ezlopi_scenes_populate_assign_block_options(s_block_options_v2_t* p_block_options, cJSON* cj_block_options);
    void ezlopi_scenes_populate_assign_method(s_method_v2_t* p_method, cJSON* cj_method);

    l_when_block_v2_t* ezlopi_scenes_populate_when_blocks(cJSON* cj_when_blocks);
    void ezlopi_scenes_populate_assign_when_block(l_when_block_v2_t* new_when_block, cJSON* cj_when_block);

    l_action_block_v2_t* ezlopi_scenes_populate_action_blocks(cJSON* cj_then_blocks, e_scenes_block_type_v2_t block_type);
    void ezlopi_scenes_populate_assign_action_block(l_action_block_v2_t* new_action_block, cJSON* cj_action_block, e_scenes_block_type_v2_t block_type);

    l_house_modes_v2_t* ezlopi_scenes_populate_house_modes(cJSON* cj_house_modes);
    void ezlopi_scenes_populate_assign_house_mode(l_house_modes_v2_t* new_house_mode, cJSON* cj_house_mode);

    l_user_notification_v2_t* ezlopi_scenes_populate_user_notifications(cJSON* cj_user_notifications);
    void ezlopi_scenes_populate_assign_user_notification(l_user_notification_v2_t* new_user_notification, cJSON* cj_user_notification);

    void ezlopi_scenes_populate_scene(l_scenes_list_v2_t* new_scene, cJSON* cj_scene, uint32_t scene_id);

#ifdef __cplusplus
}
#endif

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_POPULATE_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
