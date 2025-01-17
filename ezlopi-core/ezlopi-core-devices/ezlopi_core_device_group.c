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
 * @file    ezlopi_core_device_group.c
 * @brief   These function perform operation on device-groups
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_device_group.h"

#include "ezlopi_cloud_constants.h"

#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
enum e_devicGrp_vs_itemGrp_choice
{
    DEVICE_GROUP_SELECTED = 0,
    ITEM_GROUP_SELECTED = 1
};

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void __EZPI_core_device_group_print(l_ezlopi_device_grp_t *device_grp_node);
static void __EZPI_core_item_group_print(l_ezlopi_item_grp_t *item_grp_node);
static void __edit_devgrp_from_ll(l_ezlopi_device_grp_t *req_devgrp_node, cJSON *cj_devgrp_new);
static void __edit_itemgrp_from_ll(l_ezlopi_item_grp_t *req_itemgrp_node, cJSON *cj_itemgrp_new);
static int __edit_and_update_ll_devgrp_by_id(uint32_t devgrp_id, cJSON *cj_devgrp_new);
static int __edit_and_update_ll_itemgrp_by_id(uint32_t itemgrp_id, cJSON *cj_itemgrp_new);
static ezlopi_error_t __edit_group_and_store_updated_to_nvs(uint32_t _id, cJSON *cj_grp_new);
static l_ezlopi_device_grp_t *__device_group_pop_by_id(uint32_t _id);
static l_ezlopi_item_grp_t *__item_group_pop_by_id(uint32_t _id);
static void __device_group_delete_node(l_ezlopi_device_grp_t *devgrp_node);
static void __item_group_delete_node(l_ezlopi_item_grp_t *itemgrp_node);
static l_ezlopi_device_grp_t *____device_grp_create_node(cJSON *cj_device_grp, uint32_t device_grp_id);
static l_ezlopi_item_grp_t *____item_grp_create_node(cJSON *cj_item_grp, uint32_t item_grp_id);
static l_ezlopi_device_grp_t *__device_group_populate(cJSON *cj_device_grp, uint32_t device_grp_id);
static l_ezlopi_item_grp_t *__item_group_populate(cJSON *cj_item_grp, uint32_t item_grp_id);
static void ____remove_id_from_group_list(uint32_t _id, bool choice_of_trigger);
static uint32_t __grp_get_list(cJSON *cj_grp_array, bool choice_of_trigger);
static uint32_t __store_new_grp_in_nvs(cJSON *cj_new_grp, bool choice_of_trigger);
static bool ____check_for_category_in_devGrp(cJSON *cj_curr_devGrp_node, const char *req_category_name);
static bool ____check_for_subcategory_in_devGrp(cJSON *cj_curr_devGrp_node, const char *req_subcategory_name);
static bool ____check_for_device_id_in_devGrp(cJSON *cj_curr_devGrp_node, const char *req_device_id_str);
static bool ____check_for_deviceGroupIds_list(cJSON *cj_curr_devGrp_node, cJSON *deviceGroupIds_list);
static bool ____check_for_deviceGroupId(cJSON *cj_curr_devGrp_node, const char *req_deviceGroupId);
static bool __check_devgroup_validity(cJSON *cj_curr_devgrp_node, cJSON *cj_params);
static void ______add_items_list_to_dest_array(cJSON *cj_main_device_list, l_ezlopi_device_t *curr_dev_node);
static void ____compare_when_itemgrpid_is_given(cJSON *cj_main_device_list, l_ezlopi_device_t *curr_dev_node, const char *device_id_str, const char *itemgrp_id_str);
static void __generate_device_list_with_specific_itemgroup_id(cJSON *cj_destination_array, cJSON *cj_devices_arr, cJSON *cj_params);
/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static l_ezlopi_device_grp_t *l_device_grp_head = NULL;
static l_ezlopi_item_grp_t *l_item_grp_head = NULL;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
l_ezlopi_device_grp_t *EZPI_core_device_group_get_head(void)
{
    return l_device_grp_head;
}
l_ezlopi_item_grp_t *EZPI_core_item_group_get_head(void)
{
    return l_item_grp_head;
}
//----------------------------------------------------------------------------------------------------------------------------------
l_ezlopi_device_grp_t *EZPI_core_device_group_get_by_id(uint32_t _id)
{
    l_ezlopi_device_grp_t *ret_devgrp_node = NULL;
    l_ezlopi_device_grp_t *curr_devgrp = l_device_grp_head;
    while (curr_devgrp)
    {
        if (_id == curr_devgrp->_id)
        {
            ret_devgrp_node = curr_devgrp;
            break;
        }
        curr_devgrp = curr_devgrp->next;
    }

    return ret_devgrp_node;
}
l_ezlopi_item_grp_t *EZPI_core_item_group_get_by_id(uint32_t _id)
{
    l_ezlopi_item_grp_t *ret_itemgrp_node = NULL;
    l_ezlopi_item_grp_t *curr_itemgrp = l_item_grp_head;
    while (curr_itemgrp)
    {
        if (_id == curr_itemgrp->_id)
        {
            ret_itemgrp_node = curr_itemgrp;
            break;
        }
        curr_itemgrp = curr_itemgrp->next;
    }

    return ret_itemgrp_node;
}
//----------------------------------------------------------------------------------------------------------------------------------
cJSON *EZPI_core_device_group_create_cjson(l_ezlopi_device_grp_t *devgrp_node)
{
    cJSON *cj_devgrp = NULL;
    if (devgrp_node)
    {
        cj_devgrp = cJSON_CreateObject(__FUNCTION__);
        if (cj_devgrp)
        {
            char tmp_str[16] = {0};
            snprintf(tmp_str, sizeof(tmp_str), "%08x", devgrp_node->_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi__id_str, tmp_str);
            cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi_name_str, devgrp_node->name);

            cJSON_AddItemToObject(__FUNCTION__, cj_devgrp, ezlopi_categories_str, cJSON_Duplicate(__FUNCTION__, devgrp_node->categories, cJSON_True));
            cJSON_AddItemToObject(__FUNCTION__, cj_devgrp, ezlopi_devices_str, cJSON_Duplicate(__FUNCTION__, devgrp_node->devices, cJSON_True));
            cJSON_AddItemToObject(__FUNCTION__, cj_devgrp, ezlopi_exceptions_str, cJSON_Duplicate(__FUNCTION__, devgrp_node->exceptions, cJSON_True));

            { // add 'entryDelay'
                switch (devgrp_node->entry_delay)
                {
                case EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi_entryDelay_str, ezlopi_normal_str);
                    break;
                }
                case EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi_entryDelay_str, ezlopi_long_extended_str);
                    break;
                }
                case EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi_entryDelay_str, ezlopi_extended_str);
                    break;
                }
                case EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi_entryDelay_str, ezlopi_instant_str);
                    break;
                }
                case EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE:
                default:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi_entryDelay_str, ezlopi_none_str);
                    break;
                }
                }
            }
            { // add 'role'
                switch (devgrp_node->role)
                {
                case EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi_role_str, ezlopi_house_modes_str);
                    break;
                }
                case EZLOPI_DEVICE_GRP_ROLE_USER:
                default:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi_role_str, ezlopi_user_str);
                    break;
                }
                }
            }

            cJSON_AddBoolToObject(__FUNCTION__, cj_devgrp, ezlopi_persistent_str, devgrp_node->persistent);
            cJSON_AddBoolToObject(__FUNCTION__, cj_devgrp, ezlopi_followEntry_str, devgrp_node->follow_entry);
            cJSON_AddStringToObject(__FUNCTION__, cj_devgrp, ezlopi_packageId_str, devgrp_node->package_id);
        }
    }

    return cj_devgrp;
}
cJSON *EZPI_core_item_group_create_cjson(l_ezlopi_item_grp_t *itemgrp_node)
{
    cJSON *cj_itemgrp = NULL;
    if (itemgrp_node)
    {
        cj_itemgrp = cJSON_CreateObject(__FUNCTION__);
        if (cj_itemgrp)
        {
            char tmp_str[16] = {0};
            snprintf(tmp_str, sizeof(tmp_str), "%08x", itemgrp_node->_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, ezlopi__id_str, tmp_str);
            cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, ezlopi_name_str, itemgrp_node->name);
            cJSON_AddBoolToObject(__FUNCTION__, cj_itemgrp, ezlopi_hasGetter_str, itemgrp_node->has_getter);
            cJSON_AddBoolToObject(__FUNCTION__, cj_itemgrp, ezlopi_hasSetter_str, itemgrp_node->has_setter);
            cJSON_AddBoolToObject(__FUNCTION__, cj_itemgrp, ezlopi_persistent_str, itemgrp_node->persistent);
            cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, ezlopi_valueType_str, itemgrp_node->value_type);
            cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, ezlopi_valueTypeFamily_str, itemgrp_node->value_type_family);
            // cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp, ezlopi_valueTypeFamily_str, cJSON_Duplicate(__FUNCTION__, itemgrp_node->value_type_family, cJSON_True));

            cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp, ezlopi_itemNames_str, cJSON_Duplicate(__FUNCTION__, itemgrp_node->item_names, cJSON_True));
            cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp, ezlopi_enum_str, cJSON_Duplicate(__FUNCTION__, itemgrp_node->enum_values, cJSON_True));
            cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp, ezlopi_info_str, cJSON_Duplicate(__FUNCTION__, itemgrp_node->info, cJSON_True));

            { // add 'role'
                switch (itemgrp_node->role)
                {
                case EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, ezlopi_role_str, ezlopi_lifeSafety_str);
                    break;
                }
                case EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, ezlopi_role_str, ezlopi_house_modes_str);
                    break;
                }
                case EZLOPI_ITEM_GRP_ROLE_EMPTY:
                default:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp, ezlopi_role_str, ezlopi_empty_str);
                    break;
                }
                }
            }
        }
    }

    return cj_itemgrp;
}
//----------------------------------------------------------------------------------------------------------------------------------
int EZPI_core_device_group_edit_by_id(uint32_t devgrp_id, cJSON *cj_devgrp_new)
{
    int ret = 0;

    cJSON *tmp_updated_cj = cJSON_Duplicate(__FUNCTION__, cj_devgrp_new, cJSON_True);
    if (tmp_updated_cj)
    {
        if (1 == __edit_and_update_ll_devgrp_by_id(devgrp_id, tmp_updated_cj)) // modifies the ll-node with, 'devgrp_id'
        {
            if (EZPI_SUCCESS == __edit_group_and_store_updated_to_nvs(devgrp_id, tmp_updated_cj))
            {
                ret = 1;
            }
        }

        cJSON_Delete(__FUNCTION__, tmp_updated_cj);
    }

    return ret;
}
int EZPI_core_item_group_edit_by_id(uint32_t itemgrp_id, cJSON *cj_itemgrp_new)
{
    int ret = 0;

    cJSON *tmp_updated_cj = cJSON_Duplicate(__FUNCTION__, cj_itemgrp_new, cJSON_True);
    if (tmp_updated_cj)
    {
        if (1 == __edit_and_update_ll_itemgrp_by_id(itemgrp_id, cj_itemgrp_new)) // modifies the ll-node with, 'itemgrp_id'
        {
            if (EZPI_SUCCESS == __edit_group_and_store_updated_to_nvs(itemgrp_id, tmp_updated_cj))
            {
                ret = 1;
            }
        }
        cJSON_Delete(__FUNCTION__, tmp_updated_cj);
    }

    return ret;
}
//----------------------------------------------------------------------------------------------------------------------------------
void EZPI_core_device_group_remove_id_from_list(uint32_t _id)
{
    ____remove_id_from_group_list(_id, DEVICE_GROUP_SELECTED);
}
void EZPI_core_item_group_remove_id_from_list(uint32_t _id)
{
    ____remove_id_from_group_list(_id, ITEM_GROUP_SELECTED);
}
//----------------------------------------------------------------------------------------------------------------------------------
void EZPI_core_device_group_depopulate_by_id_v2(uint32_t _id)
{
    __device_group_delete_node(__device_group_pop_by_id(_id));
}
void EZPI_core_item_group_depopulate_by_id_v2(uint32_t _id)
{
    __item_group_delete_node(__item_group_pop_by_id(_id));
}
//----------------------------------------------------------------------------------------------------------------------------------
uint32_t EZPI_core_device_group_get_list(cJSON *cj_devgrp_array)
{
    return __grp_get_list(cj_devgrp_array, DEVICE_GROUP_SELECTED);
}
uint32_t EZPI_core_item_group_get_list(cJSON *cj_itemgrp_array)
{
    return __grp_get_list(cj_itemgrp_array, ITEM_GROUP_SELECTED);
}
//----------------------------------------------------------------------------------------------------------------------------------
uint32_t EZPI_core_device_group_store_nvs_devgrp(cJSON *cj_new_device_grp)
{
    return __store_new_grp_in_nvs(cj_new_device_grp, DEVICE_GROUP_SELECTED);
}
uint32_t EZPI_core_item_group_store_nvs_itemgrp(cJSON *cj_new_item_grp)
{
    return __store_new_grp_in_nvs(cj_new_item_grp, ITEM_GROUP_SELECTED);
}
//----------------------------------------------------------------------------------------------------------------------------------
l_ezlopi_device_grp_t *EZPI_core_device_group_new_devgrp_populate(cJSON *cj_new_dev_grp, uint32_t new_device_grp_id)
{
    return __device_group_populate(cj_new_dev_grp, new_device_grp_id);
}
l_ezlopi_item_grp_t *EZPI_core_item_group_new_itemgrp_populate(cJSON *cj_new_item_grp, uint32_t new_item_grp_id)
{
    return __item_group_populate(cj_new_item_grp, new_item_grp_id);
}
//----------------------------------------------------------------------------------------------------------------------------------
uint32_t EZPI_core_device_group_find(cJSON *cj_destination_array, cJSON *cj_params)
{
    int ret = 0;
    char *devgrp_id_list = EZPI_core_nvs_read_device_groups();
    if (devgrp_id_list)
    {
        cJSON *cj_devgrp_id_list = cJSON_Parse(__FUNCTION__, devgrp_id_list);
        if (cj_devgrp_id_list)
        {
            cJSON *cj_devgrp_id = NULL;
            cJSON_ArrayForEach(cj_devgrp_id, cj_devgrp_id_list)
            {
                char devgrp_id_str[32];
                snprintf(devgrp_id_str, sizeof(devgrp_id_str), "%08x", (uint32_t)cj_devgrp_id->valuedouble); // convert to "0xc02e00.."

                char *devgrp_str = EZPI_core_nvs_read_str(devgrp_id_str); // to exxtract the dev_grp from nvs ; if exists
                if (devgrp_str)
                {
                    cJSON *cj_curr_devgrp_node = cJSON_Parse(__FUNCTION__, devgrp_str);
                    if (cj_curr_devgrp_node)
                    {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                        CJSON_TRACE(" Checking ---> ", cj_curr_devgrp_node);
#endif
                        //------------------------------------------------------------------------------------------------
                        // 2. if Yes create add object with fields "_id" & "name"
                        if (__check_devgroup_validity(cj_curr_devgrp_node, cj_params))
                        {
                            cJSON *cj_add_valid_devGrp = cJSON_CreateObject(__FUNCTION__);
                            if (cj_add_valid_devGrp)
                            {
                                // 1. add : _id
                                cJSON_AddStringToObject(__FUNCTION__, cj_add_valid_devGrp, ezlopi__id_str, devgrp_id_str);

                                // 2. add : name
                                cJSON *cj_name = cJSON_GetObjectItem(__FUNCTION__, cj_curr_devgrp_node, ezlopi_name_str);
                                if (cj_name && cj_name->valuestring)
                                {
                                    cJSON_AddStringToObject(__FUNCTION__, cj_add_valid_devGrp, ezlopi_name_str, cj_name->valuestring);
                                }
                                else
                                {
                                    cJSON_AddStringToObject(__FUNCTION__, cj_add_valid_devGrp, ezlopi_name_str, ezlopi__str);
                                }

                                // 3. delete if not added
                                if (!cJSON_AddItemToArray(cj_destination_array, cj_add_valid_devGrp))
                                {
                                    // TRACE_I(" --- DISCARD [%s] --- ", devgrp_id_str);
                                    cJSON_Delete(__FUNCTION__, cj_add_valid_devGrp);
                                }
                                else
                                {
                                    // TRACE_I(" --- ADDING [%s] --- ", devgrp_id_str);
                                    ret += 1; // return total number of device-grp added
                                }
                            }
                        }

                        cJSON_Delete(__FUNCTION__, cj_curr_devgrp_node);
                    }

                    ezlopi_free(__FUNCTION__, devgrp_str);
                }
            }

            cJSON_Delete(__FUNCTION__, cj_devgrp_id_list);
        }

        ezlopi_free(__FUNCTION__, devgrp_id_list);
    }

    return ret;
}
//--------------------------------------------------------------------------------------------------------------------------------
uint32_t EZPI_core_device_group_devitem_expand(cJSON *cj_destination_array, cJSON *cj_params)
{
    int ret = 0;
    if (cj_destination_array && cj_params)
    {
        cJSON *cj_deviceGroupId_param = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceGroupId_str);
        if (cj_deviceGroupId_param && cj_deviceGroupId_param->valuestring)
        {
            // Get the required device_group from NVS.
            char *devgrp_str = EZPI_core_nvs_read_str(cj_deviceGroupId_param->valuestring);
            if (devgrp_str)
            {
                cJSON *cj_curr_devgrp_node = cJSON_Parse(__FUNCTION__, devgrp_str);
                if (cj_curr_devgrp_node)
                {
                    //------------------------------------------------------------------------------------------------
                    // 2. if Yes create add object with fields "_id" & "name"

                    cJSON *cj_devices_arr = cJSON_GetObjectItem(__FUNCTION__, cj_curr_devgrp_node, ezlopi_devices_str); // array
                    if (cj_devices_arr && cJSON_IsArray(cj_devices_arr))
                    {
                        // Generate dev-item expansion list according to 'cj_params'
                        __generate_device_list_with_specific_itemgroup_id(cj_destination_array, cj_devices_arr, cj_params);
                    }

                    cJSON_Delete(__FUNCTION__, cj_curr_devgrp_node);
                }

                ezlopi_free(__FUNCTION__, devgrp_str);
            }
        }
    }
    return ret;
}
//------------------------------------------- Main Init_Function -----------------------------------------------------------------
void EZPI_device_group_init(void)
{
    // __remove_residue_ids_from_list(DEVICE_GROUP_SELECTED);
    char *devgrp_id_list_str = EZPI_core_nvs_read_device_groups();
    if (devgrp_id_list_str)
    {
        TRACE_D("devGrp_id_list_str : %s", devgrp_id_list_str);
        cJSON *cj_devgrp_ids = cJSON_Parse(__FUNCTION__, devgrp_id_list_str);
        if (cj_devgrp_ids)
        {
            int array_size = cJSON_GetArraySize(cj_devgrp_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *cj_devgrp_id = cJSON_GetArrayItem(cj_devgrp_ids, i);
                if (cj_devgrp_id && cj_devgrp_id->valuedouble)
                {
                    uint32_t tmp_devgrp_id = (uint32_t)cj_devgrp_id->valuedouble;

                    char tmp_buffer[32];
                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", tmp_devgrp_id);
                    EZPI_core_cloud_update_device_group_id((uint32_t)tmp_devgrp_id);

                    char *devgrp_str = EZPI_core_nvs_read_str(tmp_buffer);
                    if (devgrp_str)
                    {
                        cJSON *cj_devgrp = cJSON_Parse(__FUNCTION__, devgrp_str);
                        if (cj_devgrp)
                        {
                            l_ezlopi_device_grp_t *new_devgrp_node = __device_group_populate(cj_devgrp, tmp_devgrp_id);
                            cJSON_Delete(__FUNCTION__, cj_devgrp);

                            if (new_devgrp_node)
                            {
                                cJSON *cj_new_devgrp_node = EZPI_core_device_group_create_cjson(new_devgrp_node);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                                // CJSON_TRACE("new_devgrp_node", cj_new_devgrp_node);
#endif

                                cJSON_Delete(__FUNCTION__, cj_new_devgrp_node);
                            }
                        }
                        ezlopi_free(__FUNCTION__, devgrp_str);
                    }
                }
            }
            cJSON_Delete(__FUNCTION__, cj_devgrp_ids);
        }

        ezlopi_free(__FUNCTION__, devgrp_id_list_str);
    }
}
void EZPI_item_group_init(void)
{
    // __remove_residue_ids_from_list(ITEM_GROUP_SELECTED);
    char *itemgrp_id_list_str = EZPI_core_nvs_read_item_groups();
    if (itemgrp_id_list_str)
    {
        TRACE_D("itemGrp_id_list_str : %s", itemgrp_id_list_str);
        cJSON *cj_itemgrp_ids = cJSON_Parse(__FUNCTION__, itemgrp_id_list_str);
        if (cj_itemgrp_ids)
        {
            int array_size = cJSON_GetArraySize(cj_itemgrp_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *cj_itemgrp_id = cJSON_GetArrayItem(cj_itemgrp_ids, i);
                if (cj_itemgrp_id && cj_itemgrp_id->valuedouble)
                {
                    uint32_t tmp_itemgrp_id = (uint32_t)cj_itemgrp_id->valuedouble;

                    char tmp_buffer[32];
                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", tmp_itemgrp_id);
                    EZPI_core_cloud_update_item_group_id((uint32_t)tmp_itemgrp_id);

                    char *itemgrp_str = EZPI_core_nvs_read_str(tmp_buffer);
                    if (itemgrp_str)
                    {
                        cJSON *cj_itemgrp = cJSON_Parse(__FUNCTION__, itemgrp_str);
                        if (cj_itemgrp)
                        {
                            l_ezlopi_item_grp_t *new_itemgrp_node = __item_group_populate(cj_itemgrp, tmp_itemgrp_id);
                            cJSON_Delete(__FUNCTION__, cj_itemgrp);

                            if (new_itemgrp_node)
                            {
                                cJSON *cj_new_itemgrp_node = EZPI_core_item_group_create_cjson(new_itemgrp_node);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                                // CJSON_TRACE("new_itemgrp_node", cj_new_itemgrp_node);
#endif

                                cJSON_Delete(__FUNCTION__, cj_new_itemgrp_node);
                            }
                        }

                        ezlopi_free(__FUNCTION__, itemgrp_str);
                    }
                }
            }

            cJSON_Delete(__FUNCTION__, cj_itemgrp_ids);
        }

        ezlopi_free(__FUNCTION__, itemgrp_id_list_str);
    }
}
//--------------------------------------------------------------------------------------------------------------------------------

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

static void __EZPI_core_device_group_print(l_ezlopi_device_grp_t *device_grp_node)
{
#if (1 == ENABLE_TRACE)
    if (device_grp_node)
    {
        TRACE_D("\t-------------------- DEVICE-GROUP --------------------");
        TRACE_D("\t|-- Group_id: %08x", device_grp_node->_id);
        TRACE_D("\t|-- Group_name: %s", device_grp_node->name);
        CJSON_TRACE("\t|-- Categories", device_grp_node->categories);
        CJSON_TRACE("\t|-- Devices", device_grp_node->devices);
        CJSON_TRACE("\t|-- Exceptions", device_grp_node->exceptions);
        TRACE_D("\t|-- Persistent: %s", device_grp_node->persistent ? ezlopi_true_str : ezlopi_false_str);

        TRACE_D("\t|-- Entry_Delay: %s", (EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL == device_grp_node->entry_delay)          ? ezlopi_normal_str
                                         : (EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED == device_grp_node->entry_delay) ? ezlopi_long_extended_str
                                         : (EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED == device_grp_node->entry_delay)      ? ezlopi_extended_str
                                         : (EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT == device_grp_node->entry_delay)       ? ezlopi_instant_str
                                                                                                                        : ezlopi_none_str);

        TRACE_D("\t|-- Follow_Entry: %s", device_grp_node->follow_entry ? ezlopi_true_str : ezlopi_false_str);
        TRACE_D("\t|-- Role: %s", (EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES == device_grp_node->role) ? ezlopi_house_modes_str : ezlopi_user_str);
        TRACE_D("\t|-- Package_Id: %s", device_grp_node->package_id);
        TRACE_D("\t|-- ----------------- ----------- --------------------");
    }
#endif
}
static void __EZPI_core_item_group_print(l_ezlopi_item_grp_t *item_grp_node)
{
#if (1 == ENABLE_TRACE)
    if (item_grp_node)
    {
        TRACE_D("\t|------------------- ITEM-GROUP --------------------");
        TRACE_D("\t|-- Group_id: %08x", item_grp_node->_id);
        TRACE_D("\t|-- Group_name: %s", item_grp_node->name);
        TRACE_D("\t|-- Has_Getter: %s", item_grp_node->has_getter ? ezlopi_true_str : ezlopi_false_str);
        TRACE_D("\t|-- Has_Setter: %s", item_grp_node->has_setter ? ezlopi_true_str : ezlopi_false_str);
        TRACE_D("\t|-- Persistent: %s", item_grp_node->persistent ? ezlopi_true_str : ezlopi_false_str);
        CJSON_TRACE("\t|-- Item_Names", item_grp_node->item_names);
        TRACE_D("\t|-- Value_Type: %s", item_grp_node->value_type);
        CJSON_TRACE("\t|-- Enum", item_grp_node->enum_values);
        TRACE_D("\t|-- Value_Type_Family: %s", item_grp_node->value_type_family);

        TRACE_D("\t|-- Role: %s", (EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES == item_grp_node->role)   ? ezlopi_house_modes_str
                                  : (EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY == item_grp_node->role) ? ezlopi_lifeSafety_str
                                                                                              : ezlopi_empty_str);

        CJSON_TRACE("\t|-- Info", item_grp_node->info);
        TRACE_D("\t|-- ----------------- ----------- --------------------");
    }
#endif
}
/**
 * @brief This function edits "req_devgrp_node" using "cj_devgrp_new"
 *
 * @param req_devgrp_node target 'devgrp_node'
 * @param cj_devgrp_new new cjson
 */
static void __edit_devgrp_from_ll(l_ezlopi_device_grp_t *req_devgrp_node, cJSON *cj_devgrp_new)
{
    (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, ezlopi_name_str))
        ? (CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_devgrp_new, ezlopi_name_str, &req_devgrp_node->name))
        : cJSON_AddStringToObject(__FUNCTION__, cj_devgrp_new, ezlopi_packageId_str, req_devgrp_node->name);

    (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, ezlopi_persistent_str))
        ? (CJSON_GET_VALUE_BOOL(cj_devgrp_new, ezlopi_persistent_str, req_devgrp_node->persistent))
        : cJSON_AddBoolToObject(__FUNCTION__, cj_devgrp_new, ezlopi_persistent_str, req_devgrp_node->persistent);

    (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, ezlopi_followEntry_str))
        ? (CJSON_GET_VALUE_BOOL(cj_devgrp_new, ezlopi_followEntry_str, req_devgrp_node->follow_entry))
        : cJSON_AddBoolToObject(__FUNCTION__, cj_devgrp_new, ezlopi_followEntry_str, req_devgrp_node->follow_entry);

    (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, ezlopi_packageId_str))
        ? (CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_devgrp_new, ezlopi_packageId_str, &req_devgrp_node->package_id))
        : cJSON_AddStringToObject(__FUNCTION__, cj_devgrp_new, ezlopi_packageId_str, req_devgrp_node->package_id);

    // ------------- categories -------------
    {
        cJSON *cj_categories = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, ezlopi_categories_str);
        if (cj_categories && cJSON_IsArray(cj_categories))
        {
            cJSON_Delete(__FUNCTION__, req_devgrp_node->categories);
            req_devgrp_node->categories = cJSON_Duplicate(__FUNCTION__, cj_categories, true);
        }
        else
        {
            if (req_devgrp_node->categories)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_devgrp_new, ezlopi_categories_str, cJSON_Duplicate(__FUNCTION__, cj_categories, true));
            }
        }
    }

    // ------------- devices -------------
    {
        cJSON *cj_devices = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, ezlopi_devices_str);
        if (cj_devices && cJSON_IsArray(cj_devices))
        {
            cJSON_Delete(__FUNCTION__, req_devgrp_node->devices);
            req_devgrp_node->devices = cJSON_Duplicate(__FUNCTION__, cj_devices, true);
        }
        else
        {
            if (req_devgrp_node->devices)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_devgrp_new, ezlopi_devices_str, cJSON_Duplicate(__FUNCTION__, cj_devices, true));
            }
        }
    }

    // ------------- exceptions -------------
    {
        cJSON *cj_exceptions = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, ezlopi_exceptions_str);
        if (cj_exceptions && cJSON_IsArray(cj_exceptions))
        {
            cJSON_Delete(__FUNCTION__, req_devgrp_node->exceptions);
            req_devgrp_node->exceptions = cJSON_Duplicate(__FUNCTION__, cj_exceptions, true);
        }
        else
        {
            if (req_devgrp_node->exceptions)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_devgrp_new, ezlopi_exceptions_str, cJSON_Duplicate(__FUNCTION__, cj_exceptions, true));
            }
        }
    }

    // ------------- entryDelay -------------
    {
        cJSON *cj_entry_delay = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, ezlopi_entryDelay_str);
        if (cj_entry_delay && cj_entry_delay->valuestring && cj_entry_delay->str_value_len)
        {
            req_devgrp_node->entry_delay = ((EZPI_STRNCMP_IF_EQUAL(cj_entry_delay->valuestring, ezlopi_normal_str, cj_entry_delay->str_value_len, 7))           ? EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL
                                            : (EZPI_STRNCMP_IF_EQUAL(cj_entry_delay->valuestring, ezlopi_long_extended_str, cj_entry_delay->str_value_len, 13)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED
                                            : (EZPI_STRNCMP_IF_EQUAL(cj_entry_delay->valuestring, ezlopi_extended_str, cj_entry_delay->str_value_len, 9))       ? EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED
                                            : (EZPI_STRNCMP_IF_EQUAL(cj_entry_delay->valuestring, ezlopi_instant_str, cj_entry_delay->str_value_len, 8))        ? EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT
                                                                                                                                                                : EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE);
        }
        else
        {
            switch (req_devgrp_node->entry_delay)
            {
            case EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL:
                cJSON_AddStringToObject(__FUNCTION__, cj_devgrp_new, ezlopi_entryDelay_str, ezlopi_normal_str);
                break;
            case EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED:
                cJSON_AddStringToObject(__FUNCTION__, cj_devgrp_new, ezlopi_entryDelay_str, ezlopi_long_extended_str);
                break;
            case EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED:
                cJSON_AddStringToObject(__FUNCTION__, cj_devgrp_new, ezlopi_entryDelay_str, ezlopi_extended_str);
                break;
            case EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT:
                cJSON_AddStringToObject(__FUNCTION__, cj_devgrp_new, ezlopi_entryDelay_str, ezlopi_instant_str);
                break;
            case EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE:
            default:
                cJSON_AddStringToObject(__FUNCTION__, cj_devgrp_new, ezlopi_entryDelay_str, ezlopi_none_str);
                break;
            }
        }
    }

    // ------------- role -------------
    {
        // req_devgrp_node->role = EZLOPI_DEVICE_GRP_ROLE_USER;
        cJSON *cj_role = cJSON_GetObjectItem(__FUNCTION__, cj_devgrp_new, ezlopi_role_str);
        if (cj_role && cj_role->valuestring && cj_role->str_value_len)
        {
            req_devgrp_node->role = ((EZPI_STRNCMP_IF_EQUAL(cj_role->valuestring, ezlopi_house_modes_str, cj_role->str_value_len, 12)) ? EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES : EZLOPI_DEVICE_GRP_ROLE_USER);
        }
        else
        {
            switch (req_devgrp_node->role)
            {
            case EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES:
                cJSON_AddStringToObject(__FUNCTION__, cj_devgrp_new, ezlopi_role_str, ezlopi_house_modes_str);
                break;
            case EZLOPI_DEVICE_GRP_ROLE_USER:
            default:
                cJSON_AddStringToObject(__FUNCTION__, cj_devgrp_new, ezlopi_role_str, ezlopi_user_str);
                break;
            }
        }
    }
    __EZPI_core_device_group_print(req_devgrp_node);
}

static void __edit_itemgrp_from_ll(l_ezlopi_item_grp_t *req_itemgrp_node, cJSON *cj_itemgrp_new)
{
    (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_name_str))
        ? (CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_itemgrp_new, ezlopi_name_str, &req_itemgrp_node->name))
        : cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_name_str, req_itemgrp_node->name);

    (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_hasGetter_str))
        ? (CJSON_GET_VALUE_BOOL(cj_itemgrp_new, ezlopi_hasGetter_str, req_itemgrp_node->has_getter))
        : cJSON_AddBoolToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_hasGetter_str, req_itemgrp_node->has_getter);

    (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_hasSetter_str))
        ? (CJSON_GET_VALUE_BOOL(cj_itemgrp_new, ezlopi_hasSetter_str, req_itemgrp_node->has_setter))
        : cJSON_AddBoolToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_hasSetter_str, req_itemgrp_node->has_setter);

    (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_persistent_str))
        ? (CJSON_GET_VALUE_BOOL(cj_itemgrp_new, ezlopi_persistent_str, req_itemgrp_node->persistent))
        : cJSON_AddBoolToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_persistent_str, req_itemgrp_node->persistent);

    (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_valueType_str))
        ? (CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_itemgrp_new, ezlopi_valueType_str, &req_itemgrp_node->value_type))
        : cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_valueType_str, req_itemgrp_node->value_type);

    if (req_itemgrp_node->value_type && strlen(req_itemgrp_node->value_type))
    {
        (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_valueTypeFamily_str))
            ? (CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_itemgrp_new, ezlopi_valueTypeFamily_str, &req_itemgrp_node->value_type_family))
            : cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_valueTypeFamily_str, req_itemgrp_node->value_type_family);
    }

    // ------------- itemNames -------------
    {
        cJSON *cj_item_names = cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_itemNames_str);
        if (cj_item_names && cJSON_IsArray(cj_item_names))
        {
            cJSON_Delete(__FUNCTION__, req_itemgrp_node->item_names);
            req_itemgrp_node->item_names = cJSON_Duplicate(__FUNCTION__, cj_item_names, true);
        }
        else
        {
            if (req_itemgrp_node->item_names)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_itemNames_str, cJSON_Duplicate(__FUNCTION__, req_itemgrp_node->item_names, true));
            }
        }
    }

    // ------------- enums -------------
    {
        cJSON *cj_enums = cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_enum_str);
        if (cj_enums && cJSON_IsArray(cj_enums))
        {
            cJSON_Delete(__FUNCTION__, req_itemgrp_node->enum_values);
            req_itemgrp_node->enum_values = cJSON_Duplicate(__FUNCTION__, cj_enums, true);
        }
        else
        {
            if (req_itemgrp_node->enum_values)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_enum_str, cJSON_Duplicate(__FUNCTION__, req_itemgrp_node->enum_values, true));
            }
        }
    }

    // ------------- role -------------
    {
        // req_itemgrp_node->role = EZLOPI_ITEM_GRP_ROLE_EMPTY;
        cJSON *cj_role = cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_role_str);
        if (cj_role && cj_role->valuestring && cj_role->str_value_len)
        {
            req_itemgrp_node->role = ((EZPI_STRNCMP_IF_EQUAL(cj_role->valuestring, ezlopi_house_modes_str, cj_role->str_value_len, 12))  ? EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES
                                      : (EZPI_STRNCMP_IF_EQUAL(cj_role->valuestring, ezlopi_lifeSafety_str, cj_role->str_value_len, 11)) ? EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY
                                                                                                                                         : EZLOPI_ITEM_GRP_ROLE_EMPTY);
        }
        else
        {
            switch (req_itemgrp_node->role)
            {
            case EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES:
                cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_role_str, ezlopi_house_modes_str);
                break;
            case EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY:
                cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_role_str, ezlopi_lifeSafety_str);
                break;
            case EZLOPI_ITEM_GRP_ROLE_EMPTY:
            default:
                cJSON_AddStringToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_role_str, ezlopi_empty_str);
                break;
            }
        }
    }

    // ------------- info -------------
    {
        cJSON *cj_info = cJSON_GetObjectItem(__FUNCTION__, cj_itemgrp_new, ezlopi_info_str);
        if (cj_info && cJSON_IsArray(cj_info))
        {
            cJSON_Delete(__FUNCTION__, req_itemgrp_node->info);
            req_itemgrp_node->info = cJSON_Duplicate(__FUNCTION__, cj_info, true);
        }
        else
        {
            if (req_itemgrp_node->info)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_itemgrp_new, ezlopi_info_str, cJSON_Duplicate(__FUNCTION__, req_itemgrp_node->info, true));
            }
        }
    }
    __EZPI_core_item_group_print(req_itemgrp_node);
}
/**
 * @brief This function edits the populated 'devgrp_id' within linkedlist
 *
 * @param devgrp_id target 'devgrp_id'
 * @param cj_devgrp_new new cjson to replace old.
 * @return int
 */
static int __edit_and_update_ll_devgrp_by_id(uint32_t devgrp_id, cJSON *cj_devgrp_new)
{
    int ret = 0;

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    // CJSON_TRACE("cj_updated_devgrp_node :", cj_devgrp_new);
#endif

    if (devgrp_id && cj_devgrp_new)
    {
        l_ezlopi_device_grp_t *req_devgrp_node = EZPI_core_device_group_get_by_id(devgrp_id);
        if (req_devgrp_node)
        {
            __edit_devgrp_from_ll(req_devgrp_node, cj_devgrp_new);
            ret = 1;
        }
    }
    return ret;
}
static int __edit_and_update_ll_itemgrp_by_id(uint32_t itemgrp_id, cJSON *cj_itemgrp_new)
{
    int ret = 0;

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    CJSON_TRACE("cj_updated_itemgrp_node :", cj_itemgrp_new);
#endif

    if (itemgrp_id && cj_itemgrp_new)
    {
        l_ezlopi_item_grp_t *req_itemgrp_node = EZPI_core_item_group_get_by_id(itemgrp_id);
        if (req_itemgrp_node)
        {
            __edit_itemgrp_from_ll(req_itemgrp_node, cj_itemgrp_new);
            ret = 1;
        }
    }
    return ret;
}
/**
 * @brief This function replaces old with 'new_dev/item_grp'
 *
 * @param cj_grp_new 'new_grp' in cjson format
 * @param _id Target nvs '_id'.
 * @return ezlopi_error_t
 */
static ezlopi_error_t __edit_group_and_store_updated_to_nvs(uint32_t _id, cJSON *cj_grp_new)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (cj_grp_new && _id)
    {
        char grp_id_str[32];
        snprintf(grp_id_str, sizeof(grp_id_str), "%08x", _id);
        if (cJSON_AddStringToObject(__FUNCTION__, cj_grp_new, ezlopi__id_str, grp_id_str)) // since we need to replace 'id' with '_id' ; before storing in nvs .
        {
            cJSON_DeleteItemFromObject(__FUNCTION__, cj_grp_new, ezlopi_id_str);
        }

        char *update_grp_str = cJSON_PrintBuffered(__FUNCTION__, cj_grp_new, 1024, false);
        TRACE_D("length of 'update_grp_str': %d", strlen(update_grp_str));

        if (update_grp_str)
        {
            EZPI_core_nvs_delete_stored_data_by_name(grp_id_str);                              // delete --> '0x0..grp ' : '{}'
            ret = EZPI_core_nvs_write_str(update_grp_str, strlen(update_grp_str), grp_id_str); // write --> '0x0..grp ' : '{}'

            ezlopi_free(__FUNCTION__, update_grp_str);
        }
    }
    return ret;
}
/**
 * @brief   This function de-links a specific node having req '_id'
 *
 * @param _id Target 'device_group_id'
 * @return l_ezlopi_device_grp_t*
 */
static l_ezlopi_device_grp_t *__device_group_pop_by_id(uint32_t _id)
{
    l_ezlopi_device_grp_t *ret_devgrp = NULL;

    if (_id == l_device_grp_head->_id)
    {
        ret_devgrp = l_device_grp_head;
        l_device_grp_head = l_device_grp_head->next;
        ret_devgrp->next = NULL;
    }
    else
    {
        l_ezlopi_device_grp_t *curr_devgrp = l_device_grp_head;
        while (curr_devgrp->next)
        {
            if (_id == curr_devgrp->next->_id)
            {
                ret_devgrp = curr_devgrp->next;
                curr_devgrp->next = curr_devgrp->next->next;
                ret_devgrp->next = NULL;
                break;
            }
            curr_devgrp = curr_devgrp->next;
        }
    }

    return ret_devgrp;
}
static l_ezlopi_item_grp_t *__item_group_pop_by_id(uint32_t _id)
{
    l_ezlopi_item_grp_t *ret_itemgrp = NULL;

    if (_id == l_item_grp_head->_id)
    {
        ret_itemgrp = l_item_grp_head;
        l_item_grp_head = l_item_grp_head->next;
        ret_itemgrp->next = NULL;
    }
    else
    {
        l_ezlopi_item_grp_t *curr_itemgrp = l_item_grp_head;
        while (curr_itemgrp->next)
        {
            if (_id == curr_itemgrp->next->_id)
            {
                ret_itemgrp = curr_itemgrp->next;
                curr_itemgrp->next = curr_itemgrp->next->next;
                ret_itemgrp->next = NULL;
                break;
            }
            curr_itemgrp = curr_itemgrp->next;
        }
    }

    return ret_itemgrp;
}

static void __device_group_delete_node(l_ezlopi_device_grp_t *devgrp_node)
{
    if (devgrp_node)
    {
        if (NULL != devgrp_node->name)
        {
            ezlopi_free(__FUNCTION__, devgrp_node->name);
            devgrp_node->name = NULL;
        }

        if (NULL != devgrp_node->categories)
        {
            cJSON_Delete(__FUNCTION__, devgrp_node->categories);
            devgrp_node->categories = NULL;
        }

        if (NULL != devgrp_node->devices)
        {
            cJSON_Delete(__FUNCTION__, devgrp_node->devices);
            devgrp_node->devices = NULL;
        }

        if (NULL != devgrp_node->exceptions)
        {
            cJSON_Delete(__FUNCTION__, devgrp_node->exceptions);
            devgrp_node->exceptions = NULL;
        }

        if (NULL != devgrp_node->package_id)
        {
            ezlopi_free(__FUNCTION__, devgrp_node->package_id);
            devgrp_node->package_id = NULL;
        }

        __device_group_delete_node(devgrp_node->next);
        devgrp_node->next = NULL;
        ezlopi_free(__FUNCTION__, devgrp_node);
    }
}
static void __item_group_delete_node(l_ezlopi_item_grp_t *itemgrp_node)
{
    if (itemgrp_node)
    {
        if (NULL != itemgrp_node->name)
        {
            ezlopi_free(__FUNCTION__, itemgrp_node->name);
            itemgrp_node->name = NULL;
        }

        if (NULL != itemgrp_node->value_type)
        {
            ezlopi_free(__FUNCTION__, itemgrp_node->value_type);
            itemgrp_node->value_type = NULL;
        }

        if (NULL != itemgrp_node->item_names)
        {
            cJSON_Delete(__FUNCTION__, itemgrp_node->item_names);
            itemgrp_node->item_names = NULL;
        }

        if (NULL != itemgrp_node->value_type_family)
        {
            ezlopi_free(__FUNCTION__, itemgrp_node->value_type_family);
            itemgrp_node->value_type_family = NULL;
        }

        if (NULL != itemgrp_node->enum_values)
        {
            cJSON_Delete(__FUNCTION__, itemgrp_node->enum_values);
            itemgrp_node->enum_values = NULL;
        }

        if (NULL != itemgrp_node->info)
        {
            cJSON_Delete(__FUNCTION__, itemgrp_node->info);
            itemgrp_node->info = NULL;
        }

        __item_group_delete_node(itemgrp_node->next);
        itemgrp_node->next = NULL;
        ezlopi_free(__FUNCTION__, itemgrp_node);
    }
}

static l_ezlopi_device_grp_t *____device_grp_create_node(cJSON *cj_device_grp, uint32_t device_grp_id)
{
    l_ezlopi_device_grp_t *new_device_grp_node = NULL;
    if (cj_device_grp)
    {
        new_device_grp_node = ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_device_grp_t));
        if (new_device_grp_node)
        {
            memset(new_device_grp_node, 0, sizeof(l_ezlopi_device_grp_t));

            new_device_grp_node->_id = device_grp_id;

            CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_device_grp, ezlopi_name_str, &new_device_grp_node->name);
            CJSON_GET_VALUE_BOOL(cj_device_grp, ezlopi_persistent_str, new_device_grp_node->persistent);
            CJSON_GET_VALUE_BOOL(cj_device_grp, ezlopi_followEntry_str, new_device_grp_node->follow_entry);
            CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_device_grp, ezlopi_packageId_str, &new_device_grp_node->package_id);

            {
                cJSON *cj_categories = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, ezlopi_categories_str);
                if (cj_categories && cJSON_IsArray(cj_categories))
                {
                    new_device_grp_node->categories = cJSON_Duplicate(__FUNCTION__, cj_categories, true);
                }
            }

            {
                cJSON *cj_devices = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, ezlopi_devices_str);
                if (cj_devices && cJSON_IsArray(cj_devices))
                {
                    new_device_grp_node->devices = cJSON_Duplicate(__FUNCTION__, cj_devices, true);
                }
            }

            {
                cJSON *cj_exceptions = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, ezlopi_exceptions_str);
                if (cj_exceptions && cJSON_IsArray(cj_exceptions))
                {
                    new_device_grp_node->exceptions = cJSON_Duplicate(__FUNCTION__, cj_exceptions, true);
                }
            }

            {
                cJSON *cj_entry_delay = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, ezlopi_entryDelay_str);
                if (cj_entry_delay && cj_entry_delay->valuestring && cj_entry_delay->str_value_len)
                {
                    new_device_grp_node->entry_delay = ((EZPI_STRNCMP_IF_EQUAL(cj_entry_delay->valuestring, ezlopi_normal_str, cj_entry_delay->str_value_len, 7))           ? EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL
                                                        : (EZPI_STRNCMP_IF_EQUAL(cj_entry_delay->valuestring, ezlopi_long_extended_str, cj_entry_delay->str_value_len, 13)) ? EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED
                                                        : (EZPI_STRNCMP_IF_EQUAL(cj_entry_delay->valuestring, ezlopi_extended_str, cj_entry_delay->str_value_len, 9))       ? EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED
                                                        : (EZPI_STRNCMP_IF_EQUAL(cj_entry_delay->valuestring, ezlopi_instant_str, cj_entry_delay->str_value_len, 8))        ? EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT
                                                                                                                                                                            : EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE);
                }
            }

            {
                cJSON *cj_role = cJSON_GetObjectItem(__FUNCTION__, cj_device_grp, ezlopi_role_str);
                if (cj_role && cj_role->valuestring && cj_role->str_value_len)
                {
                    new_device_grp_node->role = ((EZPI_STRNCMP_IF_EQUAL(cj_role->valuestring, ezlopi_house_modes_str, cj_role->str_value_len, 12)) ? EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES : EZLOPI_DEVICE_GRP_ROLE_USER);
                }
            }

            // __EZPI_core_device_group_print(new_device_grp_node);
        }
    }
    return new_device_grp_node;
}
static l_ezlopi_item_grp_t *____item_grp_create_node(cJSON *cj_item_grp, uint32_t item_grp_id)
{
    l_ezlopi_item_grp_t *new_item_grp_node = NULL;
    if (cj_item_grp)
    {
        new_item_grp_node = ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_item_grp_t));
        if (new_item_grp_node)
        {
            memset(new_item_grp_node, 0, sizeof(l_ezlopi_item_grp_t));

            new_item_grp_node->_id = item_grp_id;

            CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_item_grp, ezlopi_name_str, &new_item_grp_node->name);
            CJSON_GET_VALUE_BOOL(cj_item_grp, ezlopi_hasGetter_str, new_item_grp_node->has_getter);
            CJSON_GET_VALUE_BOOL(cj_item_grp, ezlopi_hasSetter_str, new_item_grp_node->has_setter);
            CJSON_GET_VALUE_BOOL(cj_item_grp, ezlopi_persistent_str, new_item_grp_node->persistent);
            CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_item_grp, ezlopi_valueType_str, &new_item_grp_node->value_type);

            {
                cJSON *cj_item_names = cJSON_GetObjectItem(__FUNCTION__, cj_item_grp, ezlopi_itemNames_str);
                if (cj_item_names && cJSON_IsArray(cj_item_names))
                {
                    new_item_grp_node->item_names = cJSON_Duplicate(__FUNCTION__, cj_item_names, true);
                }
            }

            {
                cJSON *cj_enum = cJSON_GetObjectItem(__FUNCTION__, cj_item_grp, ezlopi_enum_str);
                if (cj_enum && cJSON_IsArray(cj_enum))
                {
                    new_item_grp_node->enum_values = cJSON_Duplicate(__FUNCTION__, cj_enum, true);
                }
            }

            { // Add ('value_type' is abscent)  valueType_Family [cjson]
                if (NULL == new_item_grp_node->value_type)
                {
                    CJSON_GET_VALUE_STRING_BY_COPY_INTO_PTR(cj_item_grp, ezlopi_valueTypeFamily_str, &new_item_grp_node->value_type_family);
                }
            }

            {
                cJSON *cj_role = cJSON_GetObjectItem(__FUNCTION__, cj_item_grp, ezlopi_role_str);
                if (cj_role && cj_role->valuestring && cj_role->str_value_len)
                {
                    new_item_grp_node->role = ((EZPI_STRNCMP_IF_EQUAL(cj_role->valuestring, ezlopi_house_modes_str, cj_role->str_value_len, 12))  ? EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES
                                               : (EZPI_STRNCMP_IF_EQUAL(cj_role->valuestring, ezlopi_lifeSafety_str, cj_role->str_value_len, 11)) ? EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY
                                                                                                                                                  : EZLOPI_ITEM_GRP_ROLE_EMPTY);
                }
            }

            {
                cJSON *cj_info = cJSON_GetObjectItem(__FUNCTION__, cj_item_grp, ezlopi_info_str);
                if (cj_info && cJSON_IsObject(cj_info))
                {
                    new_item_grp_node->info = cJSON_Duplicate(__FUNCTION__, cj_info, true);
                }
            }

            // __EZPI_core_item_group_print(new_item_grp_node);
        }
    }
    return new_item_grp_node;
}

static l_ezlopi_device_grp_t *__device_group_populate(cJSON *cj_device_grp, uint32_t device_grp_id)
{
    l_ezlopi_device_grp_t *new_device_grp_node = NULL;
    if (l_device_grp_head)
    {
        l_ezlopi_device_grp_t *current_device_grp = l_device_grp_head;
        while (current_device_grp->next)
        {
            current_device_grp = current_device_grp->next;
        }

        current_device_grp->next = ____device_grp_create_node(cj_device_grp, device_grp_id);
        new_device_grp_node = current_device_grp->next;
    }
    else
    {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        // CJSON_TRACE("new-device-group", cj_device_grp);
#endif

        l_device_grp_head = ____device_grp_create_node(cj_device_grp, device_grp_id);
        new_device_grp_node = l_device_grp_head;
    }

    return new_device_grp_node;
}
static l_ezlopi_item_grp_t *__item_group_populate(cJSON *cj_item_grp, uint32_t item_grp_id)
{
    l_ezlopi_item_grp_t *new_item_grp_node = NULL;
    if (l_item_grp_head)
    {
        l_ezlopi_item_grp_t *current_item_grp = l_item_grp_head;
        while (current_item_grp->next)
        {
            current_item_grp = current_item_grp->next;
        }

        current_item_grp->next = ____item_grp_create_node(cj_item_grp, item_grp_id);
        new_item_grp_node = current_item_grp->next;
    }
    else
    {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        // CJSON_TRACE("new-item-group", cj_item_grp);
#endif

        l_item_grp_head = ____item_grp_create_node(cj_item_grp, item_grp_id);
        new_item_grp_node = l_item_grp_head;
    }

    return new_item_grp_node;
}

//----------------------------------------------------------------------------------------------------------------------------------
static void ____remove_id_from_group_list(uint32_t _id, bool choice_of_trigger)
{
    char *grp_id_list = (choice_of_trigger) ? EZPI_core_nvs_read_item_groups() : EZPI_core_nvs_read_device_groups();
    if (grp_id_list)
    {
        cJSON *cj_grp_id_list = cJSON_Parse(__FUNCTION__, grp_id_list);
        if (cj_grp_id_list)
        {
            uint32_t idx = 0;
            cJSON *cj_id = NULL;
            cJSON_ArrayForEach(cj_id, cj_grp_id_list)
            {
                if (cj_id->valuedouble == _id)
                {
                    cJSON_DeleteItemFromArray(__FUNCTION__, cj_grp_id_list, idx);
                }
                // iterate upto the last element (incase -->  for redundant value )
                idx++;
            }

            // Now to rewrite the group list into NVS
            char *updated_grp_id_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_grp_id_list, 1024, false);
            TRACE_D("length of 'updated_grp_id_list_str': %d", strlen(updated_grp_id_list_str));

            if (updated_grp_id_list_str)
            {
                (choice_of_trigger) ? EZPI_core_nvs_write_item_groups(updated_grp_id_list_str) : EZPI_core_nvs_write_device_groups(updated_grp_id_list_str);
                ezlopi_free(__FUNCTION__, updated_grp_id_list_str);
            }

            cJSON_Delete(__FUNCTION__, cj_grp_id_list);
        }
        ezlopi_free(__FUNCTION__, grp_id_list);
    }
}
//----------------------------------------------------------------------------------------------------------------------------------
static uint32_t __grp_get_list(cJSON *cj_grp_array, bool choice_of_trigger)
{
    uint32_t ret = 0;
    char *grp_id_list = (choice_of_trigger) ? EZPI_core_nvs_read_item_groups() : EZPI_core_nvs_read_device_groups();
    if (grp_id_list)
    {
        cJSON *cj_grp_id_list = cJSON_Parse(__FUNCTION__, grp_id_list);
        if (cj_grp_id_list)
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            // CJSON_TRACE("In-cjson :", cj_grp_id_list);
#endif

            int array_size = cJSON_GetArraySize(cj_grp_id_list);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *cj_grp_id = cJSON_GetArrayItem(cj_grp_id_list, i);
                if (cj_grp_id && cj_grp_id->valuedouble)
                {
                    char grp_id_str[32];
                    snprintf(grp_id_str, sizeof(grp_id_str), "%08x", (uint32_t)cj_grp_id->valuedouble);
                    char *grp_str = EZPI_core_nvs_read_str(grp_id_str);
                    if (grp_str)
                    {
                        cJSON *cj_grp_node = cJSON_Parse(__FUNCTION__, grp_str);
                        if (cj_grp_node)
                        {
                            cJSON_AddStringToObject(__FUNCTION__, cj_grp_node, ezlopi__id_str, grp_id_str);

                            if (!cJSON_AddItemToArray(cj_grp_array, cj_grp_node))
                            {
                                cJSON_Delete(__FUNCTION__, cj_grp_node);
                            }
                            else
                            {
                                ret += 1; // return total number of grps added
                            }
                        }

                        ezlopi_free(__FUNCTION__, grp_str);
                    }
                }
            }

            cJSON_Delete(__FUNCTION__, cj_grp_id_list);
        }

        ezlopi_free(__FUNCTION__, grp_id_list);
    }

    return ret;
}
//----------------------------------------------------------------------------------------------------------------------------------
static uint32_t __store_new_grp_in_nvs(cJSON *cj_new_grp, bool choice_of_trigger)
{
    uint32_t new_grp_id = 0;

    if (cj_new_grp)
    {
        new_grp_id = (choice_of_trigger) ? EZPI_core_cloud_generate_item_group_id() : EZPI_core_cloud_generate_device_group_id();
        char grp_id_str[32];
        snprintf(grp_id_str, sizeof(grp_id_str), "%08x", new_grp_id);                  // convert (uint32_t) to (0xC002e....)
        cJSON_AddStringToObject(__FUNCTION__, cj_new_grp, ezlopi__id_str, grp_id_str); // "_id" in nvs

        char *new_grp_str = cJSON_PrintBuffered(__FUNCTION__, cj_new_grp, 1024, false);
        TRACE_D("length of 'new_grp_str': %d", strlen(new_grp_str));

        if (new_grp_str)
        {
            if (EZPI_SUCCESS == EZPI_core_nvs_write_str(new_grp_str, strlen(new_grp_str) + 1, grp_id_str)) // writes to nvs as --> '0xC002e....'
            {
                bool free_Grp_id_list_str = 1;
                char *grp_id_list_str = (choice_of_trigger) ? EZPI_core_nvs_read_item_groups() : EZPI_core_nvs_read_device_groups();
                if (NULL == grp_id_list_str)
                {
                    grp_id_list_str = "[]";
                    free_Grp_id_list_str = 0;
                    TRACE_W("_dev/item_ Group ids-list not found in NVS");
                }

                cJSON *cj_grp_id_list = cJSON_Parse(__FUNCTION__, grp_id_list_str);
                if (cj_grp_id_list)
                {
                    cJSON *cj_grp_id_str = cJSON_CreateNumber(__FUNCTION__, new_grp_id); // adds to nvs as --> '32220910...'
                    if (cj_grp_id_str)
                    {
                        if (!cJSON_AddItemToArray(cj_grp_id_list, cj_grp_id_str))
                        { // if 'new_grp_id' doesnot exist, then erase the falsly store 'new_grp_str' in nvs.
                            cJSON_Delete(__FUNCTION__, cj_grp_id_str);
                            EZPI_core_nvs_delete_stored_data_by_id(new_grp_id);
                            new_grp_id = 0;
                        }
                        else
                        {
                            char *updated_grp_id_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_grp_id_list, 1024, false);
                            TRACE_D("length of 'updated_grp_id_list_str': %d", strlen(updated_grp_id_list_str));

                            if (updated_grp_id_list_str)
                            {
                                TRACE_D("updated_grp_id_list_str: %s", updated_grp_id_list_str);
                                if ((choice_of_trigger) ? EZPI_core_nvs_write_item_groups(updated_grp_id_list_str) : EZPI_core_nvs_write_device_groups(updated_grp_id_list_str))
                                {
                                    TRACE_S("Group list updated.");
                                }
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                                else
                                {
                                    TRACE_E("Group list update failed!");
                                }
#endif
                                ezlopi_free(__FUNCTION__, updated_grp_id_list_str);
                            }
                        }
                    }

                    cJSON_Delete(__FUNCTION__, cj_grp_id_list);
                }
                if (free_Grp_id_list_str)
                {
                    ezlopi_free(__FUNCTION__, grp_id_list_str);
                }
            }
            else
            {
                new_grp_id = 0;
            }

            ezlopi_free(__FUNCTION__, new_grp_str);
        }
        else
        {
            new_grp_id = 0;
        }
    }

    return new_grp_id;
}
//----------------------------------------------------------------------------------------------------------------------------------
static bool ____check_for_category_in_devGrp(cJSON *cj_curr_devGrp_node, const char *req_category_name)
{
    bool ret = false;
    if (cj_curr_devGrp_node && req_category_name)
    {
        cJSON *cj_categories_arr = cJSON_GetObjectItem(__FUNCTION__, cj_curr_devGrp_node, ezlopi_categories_str);
        if (cj_categories_arr && cJSON_IsArray(cj_categories_arr))
        {
            cJSON *cj_cat_element = NULL;
            cJSON_ArrayForEach(cj_cat_element, cj_categories_arr)
            {
                // 1. compare category {string}
                cJSON *cj_category = cJSON_GetObjectItem(__FUNCTION__, cj_cat_element, ezlopi_category_str); // string
                if (cj_category && cj_category->valuestring)
                {
                    if (EZPI_STRNCMP_IF_EQUAL(cj_category->valuestring, req_category_name, cj_category->str_value_len, strlen(req_category_name) + 1))
                    {
                        ret = true;
                        // TRACE_S("found Category --> '%s'", req_category_name);
                        // TRACE_S("Category criteria completed... ; flags -> [%#x]", flags);
                        break;
                    }
                }
            }
        }
    }
    return ret;
}
static bool ____check_for_subcategory_in_devGrp(cJSON *cj_curr_devGrp_node, const char *req_subcategory_name)
{
    bool ret = false;
    if (cj_curr_devGrp_node && req_subcategory_name)
    {
        cJSON *cj_categories_arr = cJSON_GetObjectItem(__FUNCTION__, cj_curr_devGrp_node, ezlopi_categories_str);
        if (cj_categories_arr && cJSON_IsArray(cj_categories_arr))
        {
            cJSON *cj_cat_element = NULL;
            cJSON_ArrayForEach(cj_cat_element, cj_categories_arr)
            {
                // 2 compare sub-category {array}
                cJSON *cj_sub_category_arr = cJSON_GetObjectItem(__FUNCTION__, cj_cat_element, ezlopi_subcategory_str); // arr-object
                if (cj_sub_category_arr && cJSON_IsArray(cj_sub_category_arr))
                {
                    cJSON *cj_subcat_element = NULL;
                    cJSON_ArrayForEach(cj_subcat_element, cj_sub_category_arr)
                    {
                        if (EZPI_STRNCMP_IF_EQUAL(cj_subcat_element->valuestring, req_subcategory_name, cj_subcat_element->str_value_len, strlen(req_subcategory_name) + 1))
                        {
                            ret = true;
                            break;
                            // TRACE_S("found Sub-Category --> '%s'", req_subcategory_name);
                        }
                    }
                }

                // Break if the condition is valid.
                if (ret)
                {
                    // TRACE_S("Sub-Category criteria completed...  ");
                    break;
                }
            }
        }
    }
    return ret;
}
static bool ____check_for_device_id_in_devGrp(cJSON *cj_curr_devGrp_node, const char *req_device_id_str)
{
    bool ret = false;
    if (cj_curr_devGrp_node && req_device_id_str)
    {
        cJSON *cj_devices_arr = cJSON_GetObjectItem(__FUNCTION__, cj_curr_devGrp_node, ezlopi_devices_str); // arr-obj
        if (cj_devices_arr && cJSON_IsArray(cj_devices_arr))
        {
            cJSON *cj_device = NULL;
            cJSON_ArrayForEach(cj_device, cj_devices_arr)
            {
                // 3. compare devices string-elements {string}
                if (EZPI_STRNCMP_IF_EQUAL(cj_device->valuestring, req_device_id_str, cj_device->str_value_len, strlen(req_device_id_str) + 1))
                {
                    ret = true;
                    // TRACE_S("found Device-Id : %s", req_device_id_str);
                    // TRACE_S("DeviceGroupId criteria completed... ; ", ret);
                    break;
                }
            }
        }
    }
    return ret;
}
static bool ____check_for_deviceGroupIds_list(cJSON *cj_curr_devGrp_node, cJSON *deviceGroupIds_list)
{
    bool ret = false;
    if (cj_curr_devGrp_node && deviceGroupIds_list)
    {
        cJSON *cj_curr_devGrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_curr_devGrp_node, ezlopi_id_str); // "c02ea004"
        if (cj_curr_devGrp_id && cj_curr_devGrp_id->valuestring)
        {
            // 5. compare 'cj_curr_devGrp_id' match with the one in "deviceGroupIds_list".
            cJSON *cj_devId_element = NULL;
            cJSON_ArrayForEach(cj_devId_element, deviceGroupIds_list)
            {
                if (EZPI_STRNCMP_IF_EQUAL(cj_curr_devGrp_id->valuestring, cj_devId_element->valuestring, cj_curr_devGrp_id->str_value_len, strlen(cj_devId_element->valuestring) + 1))
                {
                    ret = true;
                    // TRACE_S("found list Device-Id matched : %s", cj_curr_devGrp_id->valuestring);
                    // TRACE_S("DeviceGroupIds criteria completed... ; ", ret);
                    break;
                }
            }
        }
    }
    return ret;
}
static bool ____check_for_deviceGroupId(cJSON *cj_curr_devGrp_node, const char *req_deviceGroupId)
{
    bool ret = true;
    if (cj_curr_devGrp_node && req_deviceGroupId)
    {
        cJSON *cj_curr_devGrp_id = cJSON_GetObjectItem(__FUNCTION__, cj_curr_devGrp_node, ezlopi_id_str); // string // "c02ea004"
        if (cj_curr_devGrp_id && cj_curr_devGrp_id->valuestring)
        {
            // 5. compare devices string-elements {string}
            if (EZPI_STRNCMP_IF_EQUAL(cj_curr_devGrp_id->valuestring, req_deviceGroupId, cj_curr_devGrp_id->str_value_len, strlen(req_deviceGroupId) + 1))
            {
                ret = true;
                // TRACE_S("DeviceGroupId criteria completed... ; ", ret);
                // TRACE_S("found Device-Id : %s", req_deviceGroupId);
            }
        }
    }
    return ret;
}
static bool __check_devgroup_validity(cJSON *cj_curr_devgrp_node, cJSON *cj_params)
{
    // # Check the condition from "cj_params"
    cJSON *cj_category_param = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_category_str);
    if (cj_category_param && cj_category_param->valuestring)
    {
        if (!____check_for_category_in_devGrp(cj_curr_devgrp_node, cj_category_param->valuestring))
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            trace_error(" error at --> [devGrp : category]");
#endif
            return false;
        }
    }

    cJSON *cj_subcategory_param = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_subcategory_str);
    if (cj_subcategory_param && cj_subcategory_param->valuestring)
    {
        if (!____check_for_subcategory_in_devGrp(cj_curr_devgrp_node, cj_subcategory_param->valuestring))
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            trace_error(" error at --> [devGrp : sub-category]");
#endif
            return false;
        }
    }

    cJSON *cj_deviceId_param = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str);
    if (cj_deviceId_param && cj_deviceId_param->valuestring)
    {
        if (!____check_for_device_id_in_devGrp(cj_curr_devgrp_node, cj_deviceId_param->valuestring))
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            trace_error(" error at --> [devGrp : deviceId]");
#endif
            return false;
        }
    }

    cJSON *cj_deviceGroupIds_param = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceGroupIds_str);
    if (cj_deviceGroupIds_param && cJSON_IsArray(cj_deviceGroupIds_param))
    {
        if (!____check_for_deviceGroupIds_list(cj_curr_devgrp_node, cj_deviceGroupIds_param))
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            trace_error(" error at --> [devGrp : deviceGroupIds]");
#endif
            return false;
        }
    }

    cJSON *cj_deviceGroupId_param = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceGroupId_str);
    if (cj_deviceGroupId_param && cj_deviceGroupId_param->valuestring)
    {
        if (!____check_for_deviceGroupId(cj_curr_devgrp_node, cj_deviceGroupId_param->valuestring))
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            trace_error(" error at --> [devGrp : deviceGroupId]");
#endif
            return false;
        }
    }

    return true;
}
//--------------------------------------------------------------------------------------------------------------------------------
static void ______add_items_list_to_dest_array(cJSON *cj_main_device_list, l_ezlopi_device_t *curr_dev_node)
{
    cJSON *cj_items_arr = cJSON_AddArrayToObject(__FUNCTION__, cj_main_device_list, ezlopi_items_str);
    if (cj_items_arr)
    {
        // Iterate through the items within "curr_device"
        l_ezlopi_item_t *curr_item = curr_dev_node->items;
        while (curr_item)
        {
            cJSON *cj_add_curr_item = cJSON_CreateObject(__FUNCTION__);
            if (cj_add_curr_item)
            {
                // 1. add : item_id
                char item_id_str[32];
                snprintf(item_id_str, sizeof(item_id_str), "%08x", curr_item->cloud_properties.item_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_add_curr_item, ezlopi_itemId_str, item_id_str);

                // 2. add : item_name
                cJSON_AddStringToObject(__FUNCTION__, cj_add_curr_item, ezlopi_name_str, curr_item->cloud_properties.item_name);

                // 3. delete if not added
                if (!cJSON_AddItemToArray(cj_items_arr, cj_add_curr_item))
                {
                    cJSON_Delete(__FUNCTION__, cj_add_curr_item);
                }
            }

            curr_item = curr_item->next;
        }
    }
}
static void ____compare_when_itemgrpid_is_given(cJSON *cj_main_device_list, l_ezlopi_device_t *curr_dev_node, const char *device_id_str, const char *itemgrp_id_str)
{
    if (cj_main_device_list && curr_dev_node && device_id_str && itemgrp_id_str)
    {
        uint32_t get_itemgrp_id = strtoul(itemgrp_id_str, NULL, 16); // "itemGroupId" : "2002ce001"

        l_ezlopi_item_grp_t *curr_itemgrp_ll = EZPI_core_item_group_get_by_id(get_itemgrp_id); // node <--- itemgrp_ll
        if (curr_itemgrp_ll && (NULL != curr_itemgrp_ll->item_names))                          // item_names --> cj_arr
        {
            int req_count = 0; // total_count of "item_name" condition we must fullfill
            int num_of_dev_items = 0;
            int num_of_matched_items = 0;

            l_ezlopi_item_t *curr_items_ll_node = curr_dev_node->items; // get items_ll <--- device_ll
            while (curr_items_ll_node)
            {
                num_of_dev_items++; // total_count of items within --> perticular device

                // B1 . Compare :-    devce_ll_[items]   -- with --    itemGrpId_[itemNames]
                cJSON *cj_item_name_compare = NULL; // holds   "item_name" [i.e.list_member] <----- [itemGroupId]
                cJSON_ArrayForEach(cj_item_name_compare, curr_itemgrp_ll->item_names)
                {
                    req_count++; // increase the count on items checked

                    if (EZPI_STRNCMP_IF_EQUAL(curr_items_ll_node->cloud_properties.item_name, // items_ll <---- device_ll
                                              cj_item_name_compare->valuestring,              // cj_item_name_compare ("temp") <--- itemNames[] <--- ll_itemgrp
                                              strlen(curr_items_ll_node->cloud_properties.item_name) + 1,
                                              cj_item_name_compare->str_value_len))
                    {
                        num_of_matched_items++;
                        break;
                    }
                }

                curr_items_ll_node = curr_items_ll_node->next;
            }

            // C1 . Check validity and Add -->  "device" with "Items".
            if ((num_of_dev_items >= num_of_matched_items) && (req_count == num_of_matched_items))
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_main_device_list, ezlopi_deviceId_str, device_id_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_main_device_list, ezlopi_name_str, curr_dev_node->cloud_properties.device_name);

                // D1 .  Add item of this valid "[device_id]"
                ______add_items_list_to_dest_array(cj_main_device_list, curr_dev_node);
            }
        }
    }
}
static void __generate_device_list_with_specific_itemgroup_id(cJSON *cj_destination_array, cJSON *cj_devices_arr, cJSON *cj_params)
{
    if (cj_destination_array && cj_devices_arr && cj_params)
    {
        cJSON *cj_device = NULL;

        cJSON_ArrayForEach(cj_device, cj_devices_arr)
        {
            if (cj_device->valuestring)
            {
                // TRACE_S("Inspecting --> [%s]", cj_device->valuestring);

                uint32_t curr_dev_id = strtoul(cj_device->valuestring, NULL, 16);
                l_ezlopi_device_t *curr_dev_node = EZPI_core_device_get_by_id(curr_dev_id);
                if (curr_dev_node)
                {
                    cJSON *cj_main_device_list = cJSON_CreateObject(__FUNCTION__);
                    if (cj_main_device_list)
                    {

                        cJSON *cj_itemGroupId_str = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_itemGroupId_str);
                        if (cj_itemGroupId_str && cj_itemGroupId_str->valuestring)
                        {
                            // A1 . If the itemGroupId condition is given ---> add only those devices to list, containing items that matches with  'ITEM_GROUP->[items_list]'
                            ____compare_when_itemgrpid_is_given(cj_main_device_list, curr_dev_node, cj_device->valuestring, cj_itemGroupId_str->valuestring);
                        }
                        else
                        {
                            // A2 . When ItemGroupId is abscent ; include all the devices & its items.
                            cJSON_AddStringToObject(__FUNCTION__, cj_main_device_list, ezlopi_deviceId_str, cj_device->valuestring);
                            cJSON_AddStringToObject(__FUNCTION__, cj_main_device_list, ezlopi_name_str, curr_dev_node->cloud_properties.device_name);

                            // B2 . Proceed only if "showItems == True"
                            cJSON *cj_showitems = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_showItems_str);
                            if (cj_showitems && (cj_showitems->type == cJSON_True))
                            {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                                TRACE_E("Since, [showItems==TRUE] ; attaching items_list ");
#endif

                                // C2 . add "items" : [...]
                                ______add_items_list_to_dest_array(cj_main_device_list, curr_dev_node);
                            }
                        }

                        //  Add array # ezlopi_devices_str  : [ {}, + <--"{ ... }" ]
                        if (!cJSON_AddItemToArray(cj_destination_array, cj_main_device_list))
                        {
                            cJSON_Delete(__FUNCTION__, cj_main_device_list);
                        }
                    }
                }
            }
        }
    }
}
#if 0 // may be required in future
/**
 * @brief This function (if present) removes any unwanted residue-IDs from the nvs_list
 *
 * @param choice_of_trigger
 */
static void __remove_residue_ids_from_list(bool choice_of_trigger)
{
    TRACE_D("---------- # Removing [%s_Group] residue-Ids # ----------", (choice_of_trigger) ? "Item" : "Device");
    // check --> nvs_devgrp_list for unncessary "residue-IDs" & update the list
    uint32_t residue_nvs_grp_id = 0;
    bool grp_list_has_residue = false; // this indicates absence of residue-IDs // those IDs which are still in the "nvs-list" but doesnot not exists in "nvs-body"
    char *list_ptr = NULL;

    do
    {
        if (grp_list_has_residue)
        {
            if (0 != residue_nvs_grp_id)
            {
                ____remove_id_from_group_list(residue_nvs_grp_id, choice_of_trigger);
            }
            grp_list_has_residue = false;
        }

        list_ptr = (choice_of_trigger) ? EZPI_core_nvs_read_item_groups() : EZPI_core_nvs_read_device_groups();
        if (list_ptr)
        {
            cJSON *cj_id_list = cJSON_Parse(__FUNCTION__, list_ptr);
            if (cj_id_list)
            {
                int array_size = cJSON_GetArraySize(cj_id_list);
                for (int i = 0; i < array_size; i++)
                {
                    cJSON *cj_id = cJSON_GetArrayItem(cj_id_list, i);
                    if (cj_id && cj_id->valuedouble)
                    {
                        char tmp_buffer[32];
                        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", (uint32_t)cj_id->valuedouble);

                        if (NULL == EZPI_core_nvs_read_str(tmp_buffer))
                        {
                            residue_nvs_grp_id = (uint32_t)cj_id->valuedouble; // A residue_id is found..
                            grp_list_has_residue = true;                       // this will trigger a removal of "invalid_nvs_devgrp_id" .
                            break;                                             // get out of for
                        }
                    }
                }
            }
        }
    } while (grp_list_has_residue);
    TRACE_D("---------- # --------------------------------- # ----------");
}
#endif

/*******************************************************************************
 *                          End of File
 *******************************************************************************/