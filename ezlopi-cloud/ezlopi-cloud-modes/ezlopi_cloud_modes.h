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
 * @file    ezlopi_cloud_modes.h
 * @brief   Declerations for cloud modes functions
 * @author  ezlopi_team_np
 * @version 1.0
 * @date    January 20, 2024
 */
#ifndef __HUB_MODES_H__
#define __HUB_MODES_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>

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
     * @brief Function to respond to hub.modes.get method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_get(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.current.get method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_current_get(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.switch method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_switch(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.cancel_switch method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_cancel_switch(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.entry_delay.cancel method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_entry_delay_cancel(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.entry_delay.skip method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_entry_delay_skip(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.switch_to_delay.set method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_switch_to_delay_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.switch_to_delay.set method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_alarm_delay_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.notifications.set method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_notifications_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.disarmed_default.set method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_disarmed_default_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.disarmed_devices.add method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_disarmed_devices_add(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.disarmed_devices.remove method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_disarmed_devices_remove(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.alarms_off.add method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_alarms_off_add(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.alarms_off.remove method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_alarms_off_remove(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.cameras_off.add method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_cameras_off_add(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.cameras_off.remove method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_cameras_off_remove(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.bypass_devices.add method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_bypass_devices_add(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.bypass_devices.remove method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_bypass_devices_remove(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.protect.set method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_protect_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.protect_buttons.set method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_protect_buttons_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.protect.devices.add method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_protect_devices_add(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.protect.devices.remove method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_protect_devices_remove(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.entry_delay.set method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_entry_delay_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.modes.entry_delay.reset method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_entry_delay_reset(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // __HUB_MODES_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
