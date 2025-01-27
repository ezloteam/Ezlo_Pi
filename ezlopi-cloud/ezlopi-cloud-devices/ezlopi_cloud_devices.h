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
 * @file    ezlopi_cloud_devices.h
 * @brief   Definitions for cloud device functions
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    May 24th, 2023 4:58 PM
 */

#ifndef __HUB_DEVICE_LIST_H__
#define __HUB_DEVICE_LIST_H__
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
     * @brief Function to respond to hub.devices.list method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_devices_list_v3(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.device.name.set method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_name_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.device.room.set method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_room_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.devices.list method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_armed_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to response device updated with method hub.device.updated
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_updated(cJSON *cj_request, cJSON *cj_response);

    // char *devices_settings_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    // char *devices_name_set(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);

    //--------- device-groups-------
    /**
     * @brief Function to respond to hub.device.group.create method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_group_create(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.device.group.get method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_group_get(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.device.group.delete method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_group_delete(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.device.group.update method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_group_update(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.device.groups.list method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_groups_list(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.device.group.find method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_group_find(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.device.item.group.expand method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_group_devitem_expand(cJSON *cj_request, cJSON *cj_response);

    ///////////// device-grp updaters
    /**
     * @brief Function to send group created update with method hub.device.group.created
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_group_created(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to send group deleted update with method hub.device.group.deleted
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_group_deleted(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to send group updates with method hub.device.group.updated
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_device_group_updated(cJSON *cj_request, cJSON *cj_response);

    //--------- item-groups-------
    /**
     * @brief Function to respond to hub.item.group.create method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_item_group_create(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.item.group.get method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_item_group_get(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.item.group.delete method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_item_group_delete(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.item.group.update method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_item_group_update(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.item.groups.list method
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_item_groups_list(cJSON *cj_request, cJSON *cj_response);
    ///////////// item-grp updaters
    /**
     * @brief Function to send group created update with method hub.item.group.created
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_item_group_created(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to send group deleted update with method hub.item.group.deleted
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_item_group_deleted(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to send group updates with method hub.item.group.updated
     *
     * @param cj_request Incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_item_group_updated(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // __HUB_DEVICE_LIST_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
