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
 * @file    ezlopi_service_meshbot.h
 * @brief   Contains function declarations for meshbot related operations
 * @author  ezlopi_team_np
 * @version 1.0
 * @date    January 22, 2024
 */
#ifndef __EZLOPI_SERVICE_MESHBOT_H__
#define __EZLOPI_SERVICE_MESHBOT_H__

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
     * @brief Function to initialize meshbot service
     * @details Following operations are performed by the function
     *  - Prepares data structures for scene node
     *  - Adds function loop for meshbot
     *
     */
    void EZPI_scenes_meshbot_init(void);
    /**
     * @brief Function to run the scene identified by id
     *
     * @param[in] _id id of the scene to run
     * @return uint32_t
     */
    uint32_t EZPI_scenes_service_run_by_id(uint32_t _id);
    /**
     * @brief Function to start scene provided
     *
     * @param[in] scene_node Pointer to the scene node to run
     * @return uint32_t
     */
    uint32_t EZPI_meshbot_service_start_scene(l_scenes_list_v2_t *scene_node);
    /**
     * @brief Function to staop the meshbot service without broadcast sending to the cloud
     *
     * @param[in] scene_node Pointer to the scene node to run
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_meshbot_stop_without_broadcast(l_scenes_list_v2_t *scene_node);
    /**
     * @brief Function to stop meshbot service for specific scene identified by id
     *
     * @param[in] _id id of the scene to stop meshbot for
     * @return uint32_t
     */
    uint32_t EZPI_meshbot_service_stop_for_scene_id(uint32_t _id);
    /**
     * @brief Function to stop the meshbot service for the scene provided
     *
     * @param[in] scene_node Pointer to the scene node to run
     * @return uint32_t
     */
    uint32_t EZPI_meshobot_service_stop_scene(l_scenes_list_v2_t *scene_node);
    /**
     * @brief Function to execute else action group for a scene
     *
     * @param[in] scene_id Pointer to the scene node to run
     * @return uint32_t
     */
    uint32_t EZPI_meshbot_execute_scene_else_action_group(uint32_t scene_id);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // __EZLOPI_SERVICE_MESHBOT_H__
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
