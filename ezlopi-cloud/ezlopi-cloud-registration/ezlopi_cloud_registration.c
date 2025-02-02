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
 * @file    ezlopi_cloud_registration.c
 * @brief    Definitions for cloud registration functions
 * @authors Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    August 15th, 2022 11:57 AM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <time.h>
#include <string.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_util_version.h"

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_errors.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_registration.h"

#include "ezlopi_service_loop.h"
#include "ezlopi_service_webprov.h"
/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
/**
 * @brief Loop funtion for device registration
 *
 * @param arg Loop function argument
 */
static void EZPI_reg_loop(void *arg);
/**
 * @brief Function to create device registration packets
 *
 */
static void EZPI_create_reg_packet(void);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static cJSON *cj_reg_data = NULL;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_registration_init(void)
{
    EZPI_service_loop_add("reg-loop", EZPI_reg_loop, 5000, NULL);
}

void EZPI_register_repeat(cJSON *cj_request, cJSON *cj_response)
{
    EZPI_service_loop_add("reg-loop", EZPI_reg_loop, 5000, NULL);
}

void EZPI_registered(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_reg_data)
    {
        cJSON_Delete(__FUNCTION__, cj_reg_data);
        cj_reg_data = NULL;
    }
    EZPI_service_loop_remove(EZPI_reg_loop);
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
static void EZPI_create_reg_packet(void)
{
    if (NULL == cj_reg_data)
    {
        cj_reg_data = cJSON_CreateObject(__FUNCTION__);

        if (cj_reg_data)
        {
            char mac_str[18];
            uint8_t mac_addr[6];

            esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
            snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

            cJSON_AddStringToObject(__FUNCTION__, cj_reg_data, ezlopi_id_str, "__ID__");
            cJSON_AddStringToObject(__FUNCTION__, cj_reg_data, ezlopi_method_str, ezlopi_register_str);

            cJSON *cj_params = cJSON_AddObjectToObject(__FUNCTION__, cj_reg_data, ezlopi_params_str);

            if (cj_params)
            {
                char __id_str[32];

                unsigned long long __id = EZPI_core_factory_info_v3_get_id();
                snprintf(__id_str, sizeof(__id_str), "%llu", __id);
                cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_serial_str, __id_str);

                cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_firmware_str, VERSION_STR);
                cJSON_AddNumberToObject(__FUNCTION__, cj_params, ezlopi_timeOffset_str, 20700);
                cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_media_str, ezlopi_radio_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_hubType_str, "32.1");
                // cJSON_AddStringToObject(__FUNCTION__, cj_params, "mac_address", "11:22:33:44:55:66");

                char *__device_uuid = EZPI_core_factory_info_v3_get_device_uuid();
                if (__device_uuid)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_controller_uuid_str, __device_uuid);
                    ezlopi_free(__FUNCTION__, __device_uuid);
                }

                cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_mac_address_str, mac_str);
                cJSON_AddNumberToObject(__FUNCTION__, cj_params, ezlopi_maxFrameSize_str, (20 * 1024));
            }
        }
    }
}

static void EZPI_reg_loop(void *arg)
{
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    TRACE_D("reg-loop");
#endif

    ezlopi_error_t reg_event = EZPI_core_event_group_wait_for_event(EZLOPI_EVENT_NMA_REG, 0, false);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    TRACE_D("reg-event: %d", reg_event);
#endif

    if (reg_event != ESP_OK)
    {
        EZPI_create_reg_packet();

        cJSON *cj_register_dup = cJSON_CreateObjectReference(__FUNCTION__, cj_reg_data->child);
        if (cj_register_dup)
        {
            if (EZPI_SUCCESS != EZPI_core_broadcast_add_to_queue(cj_register_dup, EZPI_core_sntp_get_current_time_sec()))
            {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                TRACE_E("Error adding to broadcast queue!");
#endif
                cJSON_Delete(__FUNCTION__, cj_register_dup);
            }
        }
    }
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
