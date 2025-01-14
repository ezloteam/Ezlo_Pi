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
 * @file    ezlopi_core_scenes_print.h
 * @brief   hese function print scene information
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef __EZLOPI_CORE_SCENES_PRINT_H__
#define __EZLOPI_CORE_SCENES_PRINT_H__

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_core_scenes_v2.h"

#define ENABLE_SCENES_PRINT 0
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
     * @brief This function prints block-options
     *
     * @param block_options Source 'block_options'k to extract info from.
     * @param fields Source 'field-block' to extract info from.
     * @param tab The starting 'tab-space' for scenes prints.
     */
    void EZPI_print_block_options(s_block_options_v2_t *block_options, l_fields_v2_t *fields, const char *tab);
    /**
     * @brief This function prints fields information
     *
     * @param fields Source 'field-block' to extract info from.
     * @param tab  The starting 'tab-space' for scenes prints.
     */
    void EZPI_print_fields(l_fields_v2_t *fields, const char *tab);
    /**
     * @brief This function prints House-mode information
     *
     * @param house_modes Source 'house_modes-block' to extract info from.
     */
    void EZPI_print_house_modes(l_house_modes_v2_t *house_modes);
    /**
     * @brief This function prints 'user_notification' information
     *
     * @param user_notification Source 'user_notification' to extract info from.
     */
    void EZPI_print_user_notifications(l_user_notification_v2_t *user_notification);
    /**
     * @brief This function prints 'when_blocks' information
     *
     * @param when_blocks Source 'when_blocks' to extract info from.
     * @param tab
     */
    void EZPI_print_when_blocks(l_when_block_v2_t *when_blocks, const char *tab);
    /**
     * @brief This function prints 'action_blocks' information
     *
     * @param action_block Source 'action_blocks' to extract info from.
     */
    void EZPI_print_action_blocks(l_action_block_v2_t *action_block);
    /**
     * @brief This function prints 'scene_link_list' information
     *
     * @param scene_link_list Source 'scene_link_list' to extract info from.
     */
    void EZPI_scenes_print(l_scenes_list_v2_t *scene_link_list);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // __EZLOPI_CORE_SCENES_PRINT_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/