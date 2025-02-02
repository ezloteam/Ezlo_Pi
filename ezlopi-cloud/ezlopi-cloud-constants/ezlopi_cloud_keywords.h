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
 * @file    ezlopi_cloud_device_types_str.c
 * @brief   Declarations for cloud keywords
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    October 23rd, 2022 9:53 PM
 */

#ifndef __EZLOPI_CLOUD_KEYWORDS_H__
#define __EZLOPI_CLOUD_KEYWORDS_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "stdio.h"
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
    extern const char *ezlopi_ESP32_str;
    extern const char *ezlopi_ESP32S3_str;
    extern const char *ezlopi_ESP32S2_str;
    extern const char *ezlopi_ESP32C3_str;
    extern const char *ezlopi_ezlopi_str;
    extern const char *ezlopi_EzloPI_str;
    extern const char *ezlopi__str;
    extern const char *ezlopi_id_str;
    extern const char *ezlopi_ids_str;
    extern const char *ezlopi__id_str;
    extern const char *ezlopi_deviceId_str;
    extern const char *ezlopi_itemId_str;
    extern const char *ezlopi_device_ids_str;
    extern const char *ezlopi_sender_str;
    extern const char *ezlopi_name_str;
    extern const char *ezlopi_user_str;
    extern const char *ezlopi_value_str;
    extern const char *ezlopi_method_str;
    extern const char *ezlopi_function_str;
    extern const char *ezlopi_ruleTrigger_str;
    extern const char *ezlopi_msg_id_str; // message counter, number of data transferred to ezlopi-cloud
    extern const char *ezlopi_msg_subclass_str;
    extern const char *ezlopi_ui_broadcast_str;
    extern const char *ezlopi_result_str;
    extern const char *ezlopi_params_str;
    extern const char *ezlopi_severity_str;
    extern const char *ezlopi_gateway_id_str;
    extern const char *ezlopi_scene_id_str;
    extern const char *ezlopi_scene_name_str;
    extern const char *ezlopi_status_str;
    extern const char *ezlopi_userNotification_str;
    extern const char *ezlopi_notifications_str;
    extern const char *ezlopi_syncNotification_str;
    extern const char *ezlopi_room_id_str;
    extern const char *ezlopi_room_name_str;
    extern const char *ezlopi_error_str;
    extern const char *ezlopi_expression_str;
    extern const char *ezlopi_expressions_str;
    extern const char *ezlopi_is_group_str;
    extern const char *ezlopi_user_notifications_str;
    extern const char *ezlopi_house_modes_str;
    extern const char *ezlopi_block_enable_str;
    extern const char *ezlopi_block_str;
    extern const char *ezlopi_blocks_str;
    extern const char *ezlopi_blockId_str;
    extern const char *ezlopi_blockOptions_str;
    extern const char *ezlopi_blockName_str;
    extern const char *ezlopi_version_str;
    extern const char *ezlopi_args_str;
    extern const char *ezlopi_delay_str;
    extern const char *ezlopi_fields_str;
    extern const char *ezlopi_include_str;
    extern const char *ezlopi_blockType_str;
    extern const char *ezlopi_when_str;
    extern const char *ezlopi_enabled_str;
    extern const char *ezlopi_enable_str;
    extern const char *ezlopi_group_id_str;
    extern const char *ezlopi_parent_id_str;
    extern const char *ezlopi_group_str;
    extern const char *ezlopi_scene_str;
    extern const char *ezlopi_then_str;
    extern const char *ezlopi_thenGroup_str;
    extern const char *ezlopi_thenGroups_str;
    extern const char *ezlopi_else_str;
    extern const char *ezlopi_hasGetter_str;
    extern const char *ezlopi_hasSetter_str;
    extern const char *ezlopi_show_str;
    extern const char *ezlopi_hwaddr_str;
    extern const char *ezlopi_internetAvailable_str;
    extern const char *ezlopi_network_str;
    extern const char *ezlopi_wan_str;
    extern const char *ezlopi_wifi_str;
    extern const char *ezlopi_ethernet_str;
    extern const char *ezlopi_up_str;
    extern const char *ezlopi_down_str;
    extern const char *ezlopi_interfaces_str;
    extern const char *ezlopi_auto_str;
    extern const char *ezlopi_type_str;
    extern const char *ezlopi_ip_str;
    extern const char *ezlopi_mask_str;
    extern const char *ezlopi_gateway_str;
    extern const char *ezlopi_mode_str;
    extern const char *ezlopi_dhcp_str;
    extern const char *ezlopi_ipv4_str;
    extern const char *ezlopi_gateways_str;
    extern const char *ezlopi_label_str;
    extern const char *ezlopi_yes_str;
    extern const char *ezlopi_no_str;
    extern const char *ezlopi_ready_str;
    extern const char *ezlopi_not_ready_str;
    extern const char *ezlopi_settings_str;
    extern const char *ezlopi_fahrenheit_str;
    extern const char *ezlopi_valueType_str;
    extern const char *ezlopi_valueTypeFamily_str;
    extern const char *ezlopi_metadata_str;
    extern const char *ezlopi_meta_str;
    extern const char *ezlopi_code_str;
    extern const char *ezlopi_data_str;
    extern const char *ezlopi_message_str;
    extern const char *ezlopi_Unknown_method_str;
    extern const char *ezlopi_firmware_str;
    extern const char *ezlopi_null_str;
    extern const char *ezlopi_firmware_version_str;
    extern const char *ezlopi_uptime_str;
    extern const char *ezlopi_mac_str;
    extern const char *ezlopi_base___mac_str;
    extern const char *ezlopi_uuid_str;
    extern const char *ezlopi_cloud_uuid_str;
    extern const char *ezlopi_config_version_str;
    extern const char *ezlopi_firmware_type_str;
    extern const char *ezlopi_deviceTypeId_str;
    extern const char *ezlopi_parentDeviceId_str;
    extern const char *ezlopi_category_str;
    extern const char *ezlopi_categories_str;
    extern const char *ezlopi_subcategory_str;
    extern const char *ezlopi_gatewayId_str;
    extern const char *ezlopi_batteryPowered_str;
    extern const char *ezlopi_reachable_str;
    extern const char *ezlopi_persistent_str;
    extern const char *ezlopi_serviceNotification_str;
    extern const char *ezlopi_roomId_str;
    extern const char *ezlopi_roomsId_str;
    extern const char *ezlopi_security_str;
    extern const char *ezlopi_synced_str;
    extern const char *ezlopi_info_str;
    extern const char *ezlopi_generic_str;
    extern const char *ezlopi_firmware_hardware_str;
    extern const char *ezlopi_plugin_id_str;
    extern const char *ezlopi_reason_str;
    extern const char *ezlopi_unreachable_reasons_str;
    extern const char *ezlopi_unreachable_actions_str;
    extern const char *ezlopi_manual_device_adding_str;
    extern const char *ezlopi_enum_str;

    extern const char *ezlopi_has_getter_str;
    extern const char *ezlopi_has_setter_str;
    extern const char *ezlopi_scale_str;
    extern const char *ezlopi_value_type_str;
    extern const char *ezlopi_device_id_str;
    extern const char *ezlopi_child_linked_parent_id_str;
    extern const char *ezlopi_none_str;
    extern const char *ezlopi_role_str;
    extern const char *ezlopi_lifeSafety_str;

    extern const char *ezlopi_offlineInsecureAccess_str;
    extern const char *ezlopi_offlineAnonymousAccess_str;

    extern const char *ezlopi_stateOfCharge_str;
    extern const char *ezlopi_remainingTime_str;

    extern const char *ezlopi_health_str;
    extern const char *ezlopi_localtime_str;

    extern const char *ezlopi_device_type_id_str;
    extern const char *ezlopi_parent_device_id_str;
    extern const char *ezlopi_manufacturer_str;
    extern const char *ezlopi_ezlopi_device_type_str;
    extern const char *ezlopi_model_str;
    extern const char *ezlopi_architecture_str;
    extern const char *ezlopi_kernel_str;
    extern const char *ezlopi_FreeRTOS_str;
    extern const char *ezlopi_device_name_str;
    extern const char *ezlopi_dev_name_str;
    extern const char *ezlopi_brand_str;
    extern const char *ezlopi_build__date_str;
    extern const char *ezlopi_hash_str;
    extern const char *ezlopi_commit_str;
    extern const char *ezlopi_branch_str;
    extern const char *ezlopi_developer_str;
    extern const char *ezlopi_battery_powered_str;
    extern const char *ezlopi_house_modes_options_str;
    extern const char *ezlopi_parent_room_str;
    extern const char *ezlopi_protect_config_str;
    extern const char *ezlopi_valueFormatted_str;
    extern const char *ezlopi_gpio_scl_str;
    extern const char *ezlopi_gpio_sda_str;
    extern const char *ezlopi_dev_type_str;
    extern const char *ezlopi_gpio_str;
    extern const char *ezlopi_false_str;
    extern const char *ezlopi_true_str;
    extern const char *ezlopi_gpio_out_str;
    extern const char *ezlopi_gpio_in_str;
    extern const char *ezlopi_dev_detail_str;
    extern const char *ezlopi_id_item_str;
    extern const char *ezlopi_showCode_str;
    extern const char *ezlopi_showValue_str;
    extern const char *ezlopi_deviceName_str;
    extern const char *ezlopi_items_str;
    extern const char *ezlopi_itemName_str;
    extern const char *ezlopi_itemNames_str;
    extern const char *ezlopi_device_item_names_str;
    extern const char *ezlopi_variable_str;
    extern const char *ezlopi_names_str;
    extern const char *ezlopi_filterTypes_str;
    extern const char *ezlopi_deviceCategory_str;
    extern const char *ezlopi_deviceSubcategory_str;
    extern const char *ezlopi_roomName_str;
    extern const char *ezlopi_is_ip_str;
    extern const char *ezlopi_ip_inv_str;
    extern const char *ezlopi_val_ip_str;
    extern const char *ezlopi_pullup_ip_str;
    extern const char *ezlopi_op_inv_str;
    extern const char *ezlopi_val_op_str;
    extern const char *ezlopi_pullup_op_str;
    extern const char *ezlopi_wifi_ssid_str;
    extern const char *ezlopi_wifi_connection_status_str;
    extern const char *ezlopi_internet_status_str;
    extern const char *ezlopi_build_date_str;
    extern const char *ezlopi_unknown_str;
    extern const char *ezlopi_manufacturer_name_str;
    extern const char *ezlopi_build_str;
    extern const char *ezlopi_firmware_build_str;
    extern const char *ezlopi_hardware_str;
    extern const char *ezlopi_builder_str;
    extern const char *ezlopi_chip_str;
    extern const char *ezlopi_provisioned_status_str;
    extern const char *ezlopi_provisioned_str;
    extern const char *ezlopi_model_number_str;
    extern const char *ezlopi_serial_str;
    extern const char *ezlopi_provisioning_uuid_str;
    extern const char *ezlopi_uuid_provisioning_str;
    extern const char *ezlopi_provision_server_str;
    extern const char *ezlopi_cloud_server_str;
    extern const char *ezlopi_coordinator_url_str;
    extern const char *ezlopi_provision_token_str;
    extern const char *ezlopi_device_type_ezlopi_str;
    extern const char *ezlopi_ssl_private_key_str;
    extern const char *ezlopi_ssl_public_key_str;
    extern const char *ezlopi_ssl_shared_key_str;
    extern const char *ezlopi_signing_ca_certificate_str;
    extern const char *ezlopi_len_str;
    extern const char *ezlopi_total_len_str;
    extern const char *ezlopi_sequence_str;
    extern const char *ezlopi_user_id_str;
    extern const char *ezlopi_userId_str;
    extern const char *ezlopi_userIds_str;
    extern const char *ezlopi_coinfig_time_str;
    extern const char *ezlopi_wifi_password_str;
    extern const char *ezlopi_ca_cert_str;
    extern const char *ezlopi_config_id_str;
    extern const char *ezlopi_config_time_str;
    extern const char *ezlopi_ssid_str;
    extern const char *ezlopi_dev_type_ezlopi_str;
    extern const char *ezlopi_flash_size_str;
    extern const char *ezlopi_dev_free_flash_str;
    extern const char *ezlopi_sta_connection_str;
    extern const char *ezlopi_ip_sta_str;
    extern const char *ezlopi_ip_nmask_str;
    extern const char *ezlopi_ip_gw_str;
    extern const char *ezlopi_pass_str;
    extern const char *ezlopi_cmd_str;
    extern const char *ezlopi_sub_cmd_str;
    extern const char *ezlopi_uuid_prov_str;
    extern const char *ezlopi_status_write_str;
    extern const char *ezlopi_status_connect_str;
    extern const char *ezlopi_first_start_str;

    extern const char *ezlopi_gpio1_str;
    extern const char *ezlopi_gpio2_str;
    extern const char *ezlopi_gpio3_str;
    extern const char *ezlopi_gpio4_str;
    extern const char *ezlopi_gpio5_str;
    extern const char *ezlopi_gpio6_str;
    extern const char *ezlopi_gpio7_str;
    extern const char *ezlopi_gpio8_str;

    extern const char *ezlopi_gpio_tx_str;
    extern const char *ezlopi_gpio_rx_str;
    extern const char *ezlopi_baud_str;
    extern const char *ezlopi_sceneId_str;
    extern const char *ezlopi_changed_by_str;

    extern const char *scene_status_started_str;
    extern const char *scene_status_finished_str;
    extern const char *scene_status_partially_finished_str;
    extern const char *scene_status_failed_str;
    extern const char *scene_status_stopped_str;
    extern const char *ezlopi_armed_str;
    extern const char *ezlopi_subtype_str;

    extern const char *ezlopi_valueDefault_str;
    extern const char *ezlopi_text_str;
    extern const char *ezlopi_label_str;
    extern const char *ezlopi_lang_tag_str;
    extern const char *ezlopi_description_str;
    extern const char *ezlopi_value_type_str;
    extern const char *ezlopi_valueMin_str;
    extern const char *ezlopi_valueMax_str;
    extern const char *ezlopi_no_str;
    extern const char *ezlopi_idle_str;
    extern const char *ezlopi_is_active_str;

    extern const char *ezlopi_timezone_str;
    extern const char *ezlopi_location_str;
    extern const char *ezlopi_locations_str;
    extern const char *ezlopi_custom_timezone_str;
    extern const char *ezlopi_default_str;
    extern const char *ezlopi_gmt0_str;
    extern const char *ezlopi_modeId_str;
    extern const char *ezlopi_current_str;
    extern const char *ezlopi_switchTo_str;
    extern const char *ezlopi_timeIsLeftToSwitch_str;
    extern const char *ezlopi_timeIsLeftToAlarm_str;
    extern const char *ezlopi_switchToDelay_str;
    extern const char *ezlopi_alarmDelay_str;
    extern const char *ezlopi_entryDelay_str;
    extern const char *ezlopi_pendingDelay_str;
    extern const char *ezlopi_phase_str;
    extern const char *ezlopi_bypass_str;
    extern const char *ezlopi_main_str;
    extern const char *ezlopi_done_str;
    extern const char *ezlopi_begin_str;
    extern const char *ezlopi_canceled_str;
    extern const char *ezlopi_changed_str;
    extern const char *ezlopi_normal_str;
    extern const char *ezlopi_long_extended_str;
    extern const char *ezlopi_extended_str;
    extern const char *ezlopi_instant_str;
    extern const char *ezlopi_abortWindow_str;
    extern const char *ezlopi_minimum_str;
    extern const char *ezlopi_maximum_str;

    extern const char *ezlopi_modes_str;
    extern const char *ezlopi_disarmedDefault_str;
    extern const char *ezlopi_disarmedDevices_str;
    extern const char *ezlopi_alarmsOffDevice_str;
    extern const char *ezlopi_alarmsOffDevices_str;
    extern const char *ezlopi_camerasOffDevices_str;
    extern const char *ezlopi_bypassDevices_str;
    extern const char *ezlopi_protect_str;
    extern const char *ezlopi_devices_str;
    extern const char *ezlopi_disabledDevices_str;
    extern const char *ezlopi_deviceGroup_str;
    extern const char *ezlopi_deviceGroups_str;
    extern const char *ezlopi_deviceGroupIds_str;
    extern const char *ezlopi_deviceGroupId_str;
    extern const char *ezlopi_itemGroup_str;
    extern const char *ezlopi_itemGroupId_str;
    extern const char *ezlopi_showItems_str;
    extern const char *ezlopi_deviceFlag_str;

    extern const char *ezlopi_alarms_str;
    extern const char *ezlopi_cameras_str;
    extern const char *ezlopi_protectButtons_str;
    extern const char *ezlopi_service_str;
    extern const char *ezlopi_alarmed_str;
    extern const char *ezlopi_timeIsLeft_str;
    extern const char *ezlopi_sources_str;
    extern const char *ezlopi_timestamp_str;
    extern const char *ezlopi_Home_str;
    extern const char *ezlopi_Away_str;
    extern const char *ezlopi_Night_str;
    extern const char *ezlopi_Vacation_str;
    extern const char *ezlopi_houseMode_str;
    extern const char *ezlopi_disarmedDevice_str;

    extern const char *ezlopi_client_id_str;
    extern const char *ezlopi_source_str;

    extern const char *ezlopi_endValue_str;
    extern const char *ezlopi_startValue_str;
    extern const char *ezlopi_item_str;
    extern const char *ezlopi_comparator_str;
    extern const char *ezlopi_scenes_str;

    extern const char *ezlopi_logic_inv_str;
    extern const char *ezlopi_duty_cycle_str;
    extern const char *ezlopi_freq_hz_str;
    extern const char *ezlopi_slave_addr_str;
    extern const char *ezlopi_pull_up_str;

    extern const char *ezlopi_red_str;
    extern const char *ezlopi_green_str;
    extern const char *ezlopi_blue_str;
    extern const char *ezlopi_cwhite_str;

    extern const char *ezlopi_minValue_str;
    extern const char *ezlopi_maxValue_str;

    extern const char *ezlopi_confidence_level_str;
    extern const char *ezlopi_elementType_str;

    extern const char *ezlopi_min_move_distance_str;
    extern const char *ezlopi_max_move_distance_str;
    extern const char *ezlopi_min_still_distance_str;
    extern const char *ezlopi_max_still_distance_str;
    extern const char *ezlopi_timeout_str;

    extern const char *ezlopi_latitude_str;
    extern const char *ezlopi_longitude_str;

    extern const char *ezlopi_url_str;
    extern const char *ezlopi_urls_str;
    extern const char *ezlopi_credential_str;
    extern const char *ezlopi_contentType_str;
    extern const char *ezlopi_content_str;
    extern const char *ezlopi_headers_str;
    extern const char *ezlopi_skipSecurity_str;

    extern const char *ezlopi_parity_str;
    extern const char *ezlopi_start_bits_str;
    extern const char *ezlopi_stop_bits_str;
    extern const char *ezlopi_frame_size_str;
    extern const char *ezlopi_flow_control_str;

    extern const char *ezlopi_chipset_str;
    extern const char *ezlopi_internet_str;
    extern const char *ezlopi_local_key_str;

    extern const char *ezlopi_fwTimestampMs_str;
    extern const char *ezlopi_followEntry_str;
    extern const char *ezlopi_packageId_str;
    extern const char *ezlopi_favorites_str;
    extern const char *ezlopi_exceptions_str;
    extern const char *ezlopi_silent_str;
    extern const char *ezlopi_empty_str;
    extern const char *ezlopi_switchToDelayRunsOut_str;
    extern const char *ezlopi_interfaceId_str;
    extern const char *ezlopi_wlan0_str;
    extern const char *ezlopi_process_str;
    extern const char *ezlopi_networks_str;

    extern const char *ezlopi_latch_str;
    extern const char *ezlopi_for_str;
    extern const char *ezlopi_repeat_str;
    extern const char *ezlopi_follow_str;
    extern const char *ezlopi_pulse_str;
    extern const char *ezlopi_state_str;

    extern const char *ezlopi_connected_str;
    extern const char *ezlopi_disconnected_str;
    extern const char *ezlopi_notbetween_str;
    extern const char *ezlopi_any_result_str;
    extern const char *ezlopi_scene_enabled_str;
    extern const char *ezlopi_scene_disabled_str;
    extern const char *ezlopi_operation_str;

    extern const char *ezlopi_sunrise_str;
    extern const char *ezlopi_sunset_str;
    extern const char *ezlopi_weekdays_str;
    extern const char *ezlopi_weeks_str;
    extern const char *ezlopi_days_str;
    extern const char *ezlopi_range_str;

    extern const char *ezlopi_startDay_str;
    extern const char *ezlopi_startMonth_str;
    extern const char *ezlopi_startYear_str;
    extern const char *ezlopi_endDay_str;
    extern const char *ezlopi_endMonth_str;
    extern const char *ezlopi_endYear_str;

    extern const char *ezlopi_hours_str;
    extern const char *ezlopi_minutes_str;
    extern const char *ezlopi_seconds_str;
    extern const char *ezlopi_at_str;
    extern const char *ezlopi_after_str;
    extern const char *ezlopi_before_str;

    extern const char *ezlopi_day_str;
    extern const char *ezlopi_month_str;
    extern const char *ezlopi_year_str;
    extern const char *ezlopi_intime_str;
    extern const char *ezlopi_time_str;
    extern const char *ezlopi_times_str;
    extern const char *ezlopi_timeScenes_str;
    extern const char *ezlopi_httpAnswerCode_str;

    extern const char *ezlopi_less_str;
    extern const char *ezlopi_least_str;

    extern const char *ezlopi_start_str;
    extern const char *ezlopi_finish_str;
    extern const char *ezlopi_blockMeta_str;

    extern const char *ezlopi_sceneName_str;
    extern const char *ezlopi_methodName_str;
    extern const char *ezlopi_executionDate_str;

    extern const char *ezlopi_when_category_time_str;
    extern const char *ezlopi_when_category_modes_str;
    extern const char *ezlopi_when_category_logic_str;
    extern const char *ezlopi_when_category_function_str;

    extern const char *ezlopi_converter_str;
    extern const char *ezlopi_scales_str;
    extern const char *ezlopi_list_str;
    extern const char *ezlopi_numeric_str;
    extern const char *ezlopi_family_str;
    extern const char *ezlopi_field_str;
    extern const char *ezlopi_methods_str;
    extern const char *ezlopi_strings_str;
    extern const char *ezlopi_valuesWithLess_str;
    extern const char *ezlopi_valuesWithoutLess_str;
    extern const char *ezlopi_families_str;
    extern const char *ezlopi_op_str;
    extern const char *ezlopi_dataSource_str;
    extern const char *ezlopi_options_str;
    extern const char *ezlopi_compareNumberRange_str;
    extern const char *ezlopi_compareNumbers_str;
    extern const char *ezlopi_compareStrings_str;
    extern const char *ezlopi_compareValues_str;
    extern const char *ezlopi_inArray_str;
    extern const char *ezlopi_isDeviceItemGroup_str;
    extern const char *ezlopi_isDeviceState_str;
    extern const char *ezlopi_isItemState_str;
    extern const char *ezlopi_isItemStateChanged_str;
    extern const char *ezlopi_stringOperation_str;
    extern const char *ezlopi_schema_version_str;
    extern const char *ezlopi_dataTarget_str;
    extern const char *ezlopi_execution_str;
    extern const char *ezlopi_typeSystem_str;
    extern const char *ezlopi_itemValueTypes_str;
    extern const char *ezlopi_sideEffects_str;
    extern const char *ezlopi_action_str;
    extern const char *ezlopi_saveResult_str;

    extern const char *ezlopi_Info_str;
    extern const char *ezlopi_setItemValue_str;
    extern const char *ezlopi_setDeviceArmed_str;
    extern const char *ezlopi_sendCloudAbstractCommand_str;
    extern const char *ezlopi_switchHouseMode_str;
    extern const char *ezlopi_sendHttpRequest_str;
    extern const char *ezlopi_runCustomScript_str;
    extern const char *ezlopi_runPluginScript_str;
    extern const char *ezlopi_runScene_str;
    extern const char *ezlopi_stopScene_str;
    extern const char *ezlopi_setSceneState_str;
    extern const char *ezlopi_rebootHub_str;
    extern const char *ezlopi_cloudAPI_str;
    extern const char *ezlopi_resetHub_str;
    extern const char *ezlopi_resetLatch_str;
    extern const char *ezlopi_setVariable_str;
    extern const char *ezlopi_resetSceneLatches_str;
    extern const char *ezlopi_setExpression_str;
    extern const char *ezlopi_toggleValue_str;
    extern const char *ezlopi_groupSetItemValue_str;
    extern const char *ezlopi_groupToggleValue_str;
    extern const char *ezlopi_groupSetDeviceArmed_str;

    extern const char *ezlopi_boot_count_str;
    extern const char *ezlopi_boot_reason_str;
    extern const char *ezlopi_wifi_mac_str;
    extern const char *ezlopi_ble_mac_str;
    extern const char *ezlopi_wifi_mode_str;
    extern const char *ezlopi_cloud_str;
    extern const char *ezlopi_api_str;
    extern const char *ezlopi_ezlopi_cloud_str;
    extern const char *ezlopi_oem_str;

    //////////// otel keywords
    extern const char *ezlopi_values_str;
    extern const char *ezlopi_arrayValue_str;
    extern const char *ezlopi_stringValue_str;
    extern const char *ezlopi_doubleValue_str;
    extern const char *ezlopi_boolValue_str;
    extern const char *ezlopi_key_str;
    extern const char *ezlopI_service___name_str;
    extern const char *ezlopi_attributes_str;
    extern const char *ezlopi_resource_str;
    extern const char *ezlopi_resourceSpans_str;
    extern const char *ezlopi_scopeSpans_str;
    extern const char *ezlopi_scope_str;
    extern const char *ezlopi_spans_str;
    extern const char *ezlopi_kind_str;
    extern const char *ezlopi_endTimeUnixNano_str;
    extern const char *ezlopi_startTimeUnixNano_str;
    extern const char *ezlopi_parentSpanId_str;
    extern const char *ezlopi_spanId_str;
    extern const char *ezlopi_traceId_str;
    extern const char *ezlopi_startTime_str;
    extern const char *ezlopi_endTime_str;
    extern const char *ezlopi_logTime_str;
    extern const char *ezlopi_severityNumber_str;
    extern const char *ezlopi_severityText_str;
    extern const char *ezlopi_TRACE_str;
    extern const char *ezlopi_DEBUG_str;
    extern const char *ezlopi_INFO_str;
    extern const char *ezlopi_WARN_str;
    extern const char *ezlopi_ERROR_str;
    extern const char *ezlopi_fileName_str;
    extern const char *ezlopi_lineNumber_str;
    extern const char *ezlopi_log_str;
    extern const char *ezlopi_request_str;
    extern const char *ezlopi_resourceLogs_str;
    extern const char *ezlopi_scopeLogs_str;
    extern const char *ezlopi_logRecords_str;
    extern const char *ezlopi_timeUnixNano_str;
    extern const char *ezlopi_observedTimeUnixNano_str;
    extern const char *ezlopi_body_str;
    extern const char *ezlopi_otel_c_str;
    extern const char *ezlopi_trace_str;
    extern const char *ezlopi_otel_version_str;
    extern const char *ezlopi_timeOffset_str;
    extern const char *ezlopi_register_str;
    extern const char *ezlopi_media_str;
    extern const char *ezlopi_radio_str;
    extern const char *ezlopi_hubType_str;
    extern const char *ezlopi_controller_uuid_str;
    extern const char *ezlopi_mac_address_str;
    extern const char *ezlopi_maxFrameSize_str;

    extern const char *ezlopi_uart_str;
    extern const char *ezlopi_bluetooth_str;
    extern const char *ezlopi_websocket_server_str;
    extern const char *ezlopi_websocket_client_str;

    extern const char *ezlopi_hits_limit_str;
    extern const char *ezlopi_inactivity_window_str;

    extern const char *ezlopi_mmddyy_str;
    extern const char *ezlopi_ddmmyy_str;

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_CLOUD_KEYWORDS_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
