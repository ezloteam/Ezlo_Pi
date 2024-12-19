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
 * @file    main.h
 * @brief   perform some function on data
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
*/

#ifndef _EZLOPI_CORE_DELETE_H_
#define _EZLOPI_CORE_DELETE_H_

/*******************************************************************************
*                          Include Files
*******************************************************************************/
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
    //--------------SCENE_DELETE--------------------------------------------------------
 /**
  * @brief This function is used to delete a node from 'scene_linked_list'
  *
  * @param scenes_list Pointer to a scene-node.
  */
    void EZPI_core_scenes_delete(l_scenes_list_v2_t *scenes_list);
    /**
     * @brief This function deletes 'field_block' from the scene_node.
     *
     * @param field Pointer to the 'field_block' of a scene-node.
     */
    void EZPI_core_scenes_delete_field_value(l_fields_v2_t *field);
    /**
     * @brief This function deletes 'when_blocks' from the scene_node.
     *
     * @param when_blocks Pointer to the 'when_blocks' of a scene-node.
     */
    void EZPI_core_scenes_delete_when_blocks(l_when_block_v2_t *when_blocks);
    /**
     * @brief This function deletes 'house_modes' from the scene_node.
     *
     * @param house_modes Pointer to the 'house_modes' of a scene-node.
     */
    void EZPI_core_scenes_delete_house_modes(l_house_modes_v2_t *house_modes);
    /**
     * @brief This function deletes 'action_blocks' from the scene_node.
     *
     * @param action_blocks Pointer to the 'action_blocks' of a scene-node.
     */
    void EZPI_core_scenes_delete_action_blocks(l_action_block_v2_t *action_blocks);
    /**
     * @brief This function deletes 'user_notifications' from the scene_node.
     *
     * @param user_notifications Pointer to the 'user_notifications' of a scene-node.
     */
    void EZPI_core_scenes_delete_user_notifications(l_user_notification_v2_t *user_notifications);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_DELETE_H_

/*******************************************************************************
*                          End of File
*******************************************************************************/
