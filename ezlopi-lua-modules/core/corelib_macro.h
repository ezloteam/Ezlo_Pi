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
 * @file    main.c
 * @brief   perform some function on data
 * @author  
 * @version 0.1
 * @date    1st January 2024
 */

// #ifndef __CORELIB_MACRO_H__
// #define __CORELIB_MACRO_H__

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
    // Getters api
    CORELIB_FUNCTION("get_product_name", lcore_get_product_name)
    CORELIB_FUNCTION("get_gateways", lcore_get_gateways)
    CORELIB_FUNCTION("get_gateway", lcore_get_gateway)
    CORELIB_FUNCTION("get_device", lcore_get_device)
    CORELIB_FUNCTION("get_devices", lcore_get_devices)
    CORELIB_FUNCTION("get_devices_ids", lcore_get_devices_ids)
    CORELIB_FUNCTION("get_root_device_id", lcore_get_root_device_id)
    CORELIB_FUNCTION("get_item", lcore_get_item)
    CORELIB_FUNCTION("get_items", lcore_get_items)
    CORELIB_FUNCTION("get_items_by_device_id", lcore_get_items_by_device_id)
    CORELIB_FUNCTION("get_setting", lcore_get_setting)
    CORELIB_FUNCTION("get_setting_ids_by_device_id", lcore_get_setting_ids_by_device_id)
    CORELIB_FUNCTION("get_gateway_setting", lcore_get_gateway_setting)
    CORELIB_FUNCTION("get_gateway_setting_ids_by_gateway_id", lcore_get_gateway_setting_ids_by_gateway_id)
    CORELIB_FUNCTION("get_rooms", lcore_get_rooms)
    CORELIB_FUNCTION("get_room", lcore_get_room)

    // Adders api
    CORELIB_FUNCTION("add_device", lcore_add_device)
    CORELIB_FUNCTION("add_item", lcore_add_item)
    CORELIB_FUNCTION("add_setting", lcore_add_setting)
    CORELIB_FUNCTION("add_gateway_setting", lcore_add_gateway_setting)
    CORELIB_FUNCTION("add_vera_device_mapping", lcore_add_vera_device_mapping)

    // Removers api
    CORELIB_FUNCTION("remove_device", lcore_remove_device)
    CORELIB_FUNCTION("remove_device_sync", lcore_remove_device_sync)
    CORELIB_FUNCTION("remove_item", lcore_remove_item)
    CORELIB_FUNCTION("remove_gateway_devices", lcore_remove_gateway_devices)
    CORELIB_FUNCTION("remove_item_dictionary_value", lcore_remove_item_dictionary_value)
    CORELIB_FUNCTION("remove_setting", lcore_remove_setting)
    CORELIB_FUNCTION("remove_gateway_setting", lcore_remove_gateway_setting)

    // Updaters api
    CORELIB_FUNCTION("update_item_value", lcore_update_item_value)
    CORELIB_FUNCTION("update_item_value_with_min_max", lcore_update_item_value_with_min_max)
    CORELIB_FUNCTION("update_item_dictionary_value", lcore_update_item_dictionary_value)
    CORELIB_FUNCTION("update_user_code_restriction", lcore_update_user_code_restriction)
    CORELIB_FUNCTION("update_reachable_state", lcore_update_reachable_state)
    CORELIB_FUNCTION("update_ready_state", lcore_update_ready_state)
    CORELIB_FUNCTION("update_device_status", lcore_update_device_status)
    CORELIB_FUNCTION("update_gateway", lcore_update_gateway)
    CORELIB_FUNCTION("modify_device", lcore_modify_device)
    CORELIB_FUNCTION("modify_item", lcore_modify_item)
    CORELIB_FUNCTION("modify_setting", lcore_modify_setting)
    CORELIB_FUNCTION("set_setting_value", lcore_set_setting_value)
    CORELIB_FUNCTION("set_setting_status", lcore_set_setting_status)
    CORELIB_FUNCTION("set_setting_dictionary_value", lcore_set_setting_dictionary_value)
    CORELIB_FUNCTION("set_setting_dictionary_status", lcore_set_setting_dictionary_status)
    CORELIB_FUNCTION("set_gateway_setting_value", lcore_set_gateway_setting_value)
    CORELIB_FUNCTION("set_gateway_setting_status", lcore_set_gateway_setting_status)

    // Others api
    CORELIB_FUNCTION("notify_process_started", lcore_notify_process_started)
    CORELIB_FUNCTION("notify_process_stopped", lcore_notify_process_stopped)
    CORELIB_FUNCTION("get_current_notified_process", lcore_get_current_notified_process)
    CORELIB_FUNCTION("generate_item_dictionary_number_id", lcore_generate_item_dictionary_number_id)
    CORELIB_FUNCTION("send_ui_broadcast", lcore_send_ui_broadcast)
    CORELIB_FUNCTION("send_device_checked_broadcast", lcore_send_device_checked_broadcast)
    CORELIB_FUNCTION("send_response", lcore_send_response)
    CORELIB_FUNCTION("subscribe", lcore_subscribe)
    CORELIB_FUNCTION("set_subscription_filters", lcore_set_subscription_filters)
    CORELIB_FUNCTION("unsubscribe", lcore_unsubscribe)

    // External gateway api
    CORELIB_FUNCTION("notify_process_started", lcore_notify_process_started)
    CORELIB_FUNCTION("notify_process_stopped", lcore_notify_process_stopped)
    CORELIB_FUNCTION("get_current_notified_process", lcore_get_current_notified_process)
    CORELIB_FUNCTION("generate_item_dictionary_number_id", lcore_generate_item_dictionary_number_id)
    CORELIB_FUNCTION("send_ui_broadcast", lcore_send_ui_broadcast)
    CORELIB_FUNCTION("send_device_checked_broadcast", lcore_send_device_checked_broadcast)
    CORELIB_FUNCTION("send_response", lcore_send_response)
    CORELIB_FUNCTION("subscribe", lcore_subscribe)
    CORELIB_FUNCTION("set_subscription_filters", lcore_set_subscription_filters)
    CORELIB_FUNCTION("unsubscribe", lcore_unsubscribe)

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

// #endif // __CORELIB_MACRO_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
