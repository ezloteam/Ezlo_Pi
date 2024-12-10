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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef _EZLOPI_CORE_BLE_CONFIG_H_
#define _EZLOPI_CORE_BLE_CONFIG_H_

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "esp_gatts_api.h"

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
    typedef enum e_gatt_status
    {
        GATT_STATUS_NONE = 0,
        GATT_STATUS_PROCESSING,
        GATT_STATUS_DONE,
        GATT_STATUS_MAX
    } e_gatt_status_t;

    typedef void (*f_upcall_t)(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);

    typedef struct s_gatt_descr
    {
        uint16_t handle;
        esp_bt_uuid_t uuid;
        e_gatt_status_t status;
        // esp_attr_value_t *val;
        f_upcall_t read_upcall;
        f_upcall_t write_upcall;
        f_upcall_t write_exce_upcall;
        esp_gatt_perm_t permission;
        esp_attr_control_t* control;
        struct s_gatt_descr* next;
    } s_gatt_descr_t;

    typedef struct s_gatt_char
    {
        uint16_t handle;
        esp_bt_uuid_t uuid;
        e_gatt_status_t status;
        esp_gatt_perm_t permission;
        esp_gatt_char_prop_t property;
        // esp_attr_value_t *char_val;
        f_upcall_t read_upcall;
        f_upcall_t write_upcall;
        f_upcall_t write_exce_upcall;
        esp_attr_control_t control;
        s_gatt_descr_t* descriptor;
        struct s_gatt_char* next;
    } s_gatt_char_t;

    typedef struct s_gatt_service
    {
        uint16_t app_id;
        uint16_t conn_id;
        uint16_t gatts_if;
        uint16_t num_handles; // Number of handles
        e_gatt_status_t status;
        uint16_t service_handle;
        esp_gatt_srvc_id_t service_id;
        struct s_gatt_char* characteristics;
        struct s_gatt_service* next;
    } s_gatt_service_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif  // CONFIG_EZPI_BLE_ENABLE

#endif // _EZLOPI_CORE_BLE_CONFIG_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
