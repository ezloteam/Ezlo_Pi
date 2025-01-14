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
 * @file    ezlopi_cloud_modes_updaters.h
 * @brief   Declerations for cloud modes updater functions
 * @author  ezlopi_team_np
 * @version 1.0
 * @date    January 20, 2024
 */
#ifndef __EZLOPI_CLOUD_MODES_UPDATERS_H__
#define __EZLOPI_CLOUD_MODES_UPDATERS_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "cjext.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

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
     * @brief Updater function for modes switched
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_switched(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes alarmed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_alarmed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes changed alarmed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_changed_alarmed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes notifications notify all
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_notifications_notify_all(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes notifications added
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_notifications_added(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes notifications removed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_notifications_removed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes disarmed devices added
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_disarmed_devices_added(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes disarmed devices removed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_disarmed_devices_removed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes alarms off added
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_alarms_off_added(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes alarms off removed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_alarms_off_removed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes cameras off added
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_cameras_off_added(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes cameras off removed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_cameras_off_removed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes bypass added
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_bypass_devices_added(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes bypass removed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_bypass_devices_removed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes changed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_changed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes protect buttons added
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_protect_buttons_added(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes protect buttons updated
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_protect_buttons_updated(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes protect buttons removed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_protect_buttons_removed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes protect button set broadcast
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_protect_button_set_broadcast(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes protect devices added
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_protect_devices_added(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes protect devices removed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_protect_devices_removed(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Updater function for modes entry delay changed
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_cloud_modes_entry_delay_changed(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MODES

#endif // __EZLOPI_CLOUD_MODES_UPDATERS_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
