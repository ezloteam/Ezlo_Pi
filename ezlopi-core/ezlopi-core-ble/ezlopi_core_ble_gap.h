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
 * @file    main.h
 * @brief   perform some function on data
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */
#ifndef _EZLOPI_CORE_BLE_GAP_H_
#define _EZLOPI_CORE_BLE_GAP_H_

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "esp_gap_ble_api.h"
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
     * @brief This function configures ble advertisement
     */
    void EZPI_core_ble_setup_adv_config(void);
    /**
     * @brief This funcion sets security passkey value
     *
     * @param passkey Key value
     */
    void EZPI_core_ble_gap_set_passkey(uint32_t passkey);
    /**
     * @brief This function dissociates the ble-device
     *
     */
    void EZPI_core_ble_gap_dissociate_bonded_devices(void);
    /**
     * @brief This function configures advertisement data
     *
     */
    void EZPI_core_ble_gap_config_adv_data(void);
    /**
     * @brief This function configures scan properties of rsp_data
     *
     */
    void EZPI_core_ble_gap_config_scan_rsp_data(void);
    /**
     * @brief This function start ble advertisement operation
     *
     */
    void EZPI_core_ble_gap_start_advertising(void);
    /**
     * @brief This function handles ble-gap events
     *
     * @param event ble-gap event information
     * @param param Pointer to ble-gap-params
     */
    void EZPI_core_ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_BLE_ENABLE

#endif //  _EZLOPI_CORE_BLE_GAP_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
