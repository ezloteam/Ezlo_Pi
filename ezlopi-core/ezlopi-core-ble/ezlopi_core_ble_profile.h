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

#ifndef _EZLOPI_CORE_BLE_PROFILE_H_
#define _EZLOPI_CORE_BLE_PROFILE_H_

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

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
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */
    s_gatt_service_t* ezlopi_ble_profile_get_by_app_id(uint16_t app_id);
    s_gatt_service_t* ezlopi_ble_profile_get_service_by_gatts_if(uint16_t gatts_if);

    s_gatt_descr_t* ezlopi_ble_profile_get_initiating_descriptor(s_gatt_char_t* characteristic);
    // s_gatt_descr_t *ezlopi_ble_profile_get_initiating_descriptor(s_gatt_service_t *service);
    s_gatt_descr_t* ezlopi_ble_profile_get_descriptor_to_init(s_gatt_char_t* characteristic);

    // s_gatt_char_t *ezlopi_ble_profile_get_characterstics_to_init(void);
    s_gatt_char_t* ezlopi_ble_profile_get_initiating_characterstics(s_gatt_service_t* service);
    s_gatt_char_t* ezlopi_ble_profile_get_characterstics_to_init(s_gatt_service_t* service);

    s_gatt_service_t* ezlopi_ble_gatt_create_service(uint16_t app_id, esp_bt_uuid_t* service_uuid);
    s_gatt_descr_t* ezlopi_ble_gatt_add_descriptor(s_gatt_char_t* charcteristic, esp_bt_uuid_t* uuid, esp_gatt_perm_t permission,
        f_upcall_t read_func, f_upcall_t write_func, f_upcall_t write_exec_func);
    s_gatt_char_t* ezlopi_ble_gatt_add_characteristic(s_gatt_service_t* service_obj, esp_bt_uuid_t* uuid, esp_gatt_perm_t permission, esp_gatt_char_prop_t properties,
        f_upcall_t read_func, f_upcall_t write_func, f_upcall_t write_exec_func);
    s_gatt_service_t* ezlopi_ble_profile_get_head(void);

    int ezlopi_ble_gatt_number_of_services(void);

    void ezlopi_ble_gatt_print_descriptor(s_gatt_descr_t* descriptor);
    void ezlopi_ble_gatt_print_characteristic(s_gatt_char_t* characteristic);
    void ezlopi_ble_gatt_print_service(s_gatt_service_t* service);
    void ezlopi_ble_gatt_print_uuid(esp_bt_uuid_t* uuid, char* msg);
    void ezlopi_ble_profile_print(void);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_BLE_ENABLE

#endif // _EZLOPI_CORE_BLE_PROFILE_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
