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
 * @file    ezlopi_core_ble_gatt.h
 * @brief   perform some function on ble-gatt operations
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 0.1
 * @date    January 22nd, 2024 1:12 AM
 */

#ifndef _EZLOPI_CORE_BLE_GATT_H_
#define _EZLOPI_CORE_BLE_GATT_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_core_ble_config.h"

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
     * @brief This function return max ble-data size
     *
     * @return uint16_t
     */
    uint16_t EZPI_core_ble_gatt_get_max_data_size(void);
    /**
     * @brief Function to handle ble-gatt event
     *
     * @param event ble-event struct
     * @param gatts_if ble-gatt interfaces
     * @param param pointer to gatt-cb-params
     */
    void EZPI_core_ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
    /**
     * @brief Function to notify ble-gatt characteristics
     *
     * @param service Pointer to ble-sevice-structure
     * @param characteristics Pointer to ble-characteristics
     * @param value Pointer to data value-structure
     */
    void EZPI_core_ble_gatts_characteristic_notify(s_gatt_service_t *service, s_gatt_char_t *characteristics, esp_gatt_value_t *value);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_BLE_ENABLE

#endif // _EZLOPI_CORE_BLE_GATT_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
