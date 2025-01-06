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
 * @file    ezlopi_core_ble_profile.h
 * @brief   perform some function on ble-profile information
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_BLE_PROFILE_H_
#define _EZLOPI_CORE_BLE_PROFILE_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE
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
     * @brief This function return ble-profile by app_id
     *
     * @return s_gatt_service_t *
     */
    s_gatt_service_t *EZPI_core_ble_profile_get_by_app_id(uint16_t app_id);
    /**
     * @brief Function to return 'gatt_service_info' using 'gatts_if'
     *
     * @param gatts_if Interface of the gatts services
     * @return s_gatt_service_t*
     */
    s_gatt_service_t *EZPI_core_ble_profile_get_service_by_gatts_if(uint16_t gatts_if);
    /**
     * @brief Function to get initiating descriptors
     *
     * @param characteristic Pointer to ble-gatt-characteristics
     * @return s_gatt_descr_t*
     */
    s_gatt_descr_t *EZPI_core_ble_profile_get_initiating_descriptor(s_gatt_char_t *characteristic);
    /**
     * @brief Function to initialize and return ble-gatt descriptors
     *
     * @param characteristic Pointer to gatt characteristics
     * @return s_gatt_descr_t*
     */
    s_gatt_descr_t *EZPI_core_ble_profile_get_descriptor_to_init(s_gatt_char_t *characteristic);
    /**
     * @brief Function to initialize ble profile characteristics
     *
     * @param service pointer to ble-services
     * @return s_gatt_char_t*
     */
    s_gatt_char_t *EZPI_core_ble_profile_get_initiating_characterstics(s_gatt_service_t *service);
    /**
     * @brief Function to initialize and get ble profile characteristics
     *
     * @param service Pointer to gatt services
     * @return s_gatt_char_t*
     */
    s_gatt_char_t *EZPI_core_ble_profile_get_characterstics_to_init(s_gatt_service_t *service);
    /**
     * @brief Function to create ble gatt service
     *
     * @param app_id ble-app-id
     * @param service_uuid ble-service_uuid
     * @return s_gatt_service_t*
     */
    s_gatt_service_t *EZPI_core_ble_gatt_create_service(uint16_t app_id, esp_bt_uuid_t *service_uuid);
    /**
     * @brief Function to add descriptor in ble gatt
     *
     * @param charcteristic Pointer to new characteristics
     * @param uuid Pointer to ble-gatt uuid info
     * @param permission Pointer to ble-gatt permission info
     * @param read_func Pointer to target read function
     * @param write_func Pointer to target write function
     * @param write_exec_func Pointer to target write-execution function
     * @return s_gatt_descr_t*
     */
    s_gatt_descr_t *EZPI_core_ble_gatt_add_descriptor(s_gatt_char_t *charcteristic, esp_bt_uuid_t *uuid, esp_gatt_perm_t permission,
                                                      f_upcall_t read_func, f_upcall_t write_func, f_upcall_t write_exec_func);
    /**
     * @brief Function to add characteristics in ble-gatt
     *
     * @param service_obj Pointer to new service
     * @param uuid Pointer to ble-gatt uuid info
     * @param permission Pointer to ble-gatt permission info
     * @param properties Pointer to ble-gatt properties info
     * @param read_func Pointer to target read function
     * @param write_func Pointer to target write function
     * @param write_exec_func Pointer to target write-execution function
     * @return s_gatt_char_t*
     */
    s_gatt_char_t *EZPI_core_ble_gatt_add_characteristic(s_gatt_service_t *service_obj, esp_bt_uuid_t *uuid, esp_gatt_perm_t permission, esp_gatt_char_prop_t properties,
                                                         f_upcall_t read_func, f_upcall_t write_func, f_upcall_t write_exec_func);
    /**
     * @brief Function to return ble-profile head-node from ll
     *
     * @return s_gatt_service_t*
     */
    s_gatt_service_t *EZPI_core_ble_profile_get_head(void);
    /**
     * @brief Function to return total no of ble-service active
     *
     * @return int
     */
    int EZPI_core_ble_gatt_number_of_services(void);
    /**
     * @brief Funtion to print ble-descriptor info
     *
     * @param descriptor pointer to ble-descriptors info
     */
    void EZPI_core_ble_gatt_print_descriptor(s_gatt_descr_t *descriptor);
    /**
     * @brief Funtion to print ble-characteristics info
     *
     * @param characteristic pointer to print ble-characteristics info
     */
    void EZPI_core_ble_gatt_print_characteristic(s_gatt_char_t *characteristic);
    /**
     * @brief Funtion to print ble-service info
     *
     * @param service pointer to service
     */
    void EZPI_core_ble_gatt_print_service(s_gatt_service_t *service);
    /**
     * @brief Function to print the ble-gatt-uuid
     *
     * @param uuid pointer to ble_uuid_struct
     * @param msg Pointer to message
     */
    void EZPI_core_ble_gatt_print_uuid(esp_bt_uuid_t *uuid, char *msg);
    /**
     * @brief Funtion to print ble-profile info
     *
     */
    void EZPI_core_ble_profile_print(void);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_BLE_ENABLE

#endif // _EZLOPI_CORE_BLE_PROFILE_H_
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
