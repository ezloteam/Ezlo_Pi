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
 * @file    ezlopi_cloud_scenes.h
 * @brief
 * @author
 * @version
 * @date
 */
#ifndef __HUB_SCENES_LIST_H__
#define __HUB_SCENES_LIST_H__

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "cjext.h"

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
     * @brief Function that responds to hub.scenes.list method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_list(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.create method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_create(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.get method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_get(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.edit method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_edit(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.delete method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_delete(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.status.get method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_status_get(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.blocks.list method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_blocks_list(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.block.data.list method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_block_data_list(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.run method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_run(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.enable.set method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_enable_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.notification.add method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_notification_add(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.notification.remove method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_notification_remove(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.room.set method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_room_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.time.list method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_time_list(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.trigger.device.list method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_trigger_device_list(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.block.enabled.set method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_block_enabled_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.house_modes.set method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_house_modes_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.action.block.test method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_action_block_test(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.block.status.reset method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_block_status_reset(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.meta.set method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_meta_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.block.meta.set method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_blockmeta_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.stop method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_stop(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that responds to hub.scenes.clone method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scenes_clone(cJSON *cj_request, cJSON *cj_response);

    ///////////// updaters
    /**
     * @brief Updater function with method hub.scene.changed
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scene_changed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function with method hub.scene.added
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scene_added(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function with method hub.scene.deleted
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_scene_deleted(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // __HUB_SCENES_LIST_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
