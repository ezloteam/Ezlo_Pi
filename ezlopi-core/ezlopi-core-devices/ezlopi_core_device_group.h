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
 * @file    ezlopi_core_device_group.h
 * @brief   These function perform operation on device-groups
 * @author  Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */
#ifndef _EZLOPI_CORE_DEVICES_GROUP_H_
#define _EZLOPI_CORE_DEVICES_GROUP_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_core_devices.h"
#include "ezlopi_core_protect.h"

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
    // typedef enum e_ezlopi_device_grp_entrydelay_type
    // {
    //     EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE = 0,
    //     EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL,
    //     EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED,
    //     EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED,
    //     EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT,
    //     EZLOPI_DEVICE_GRP_ENTRYDELAY_MAX
    // } e_ezlopi_device_grp_entrydelay_type_t;

    typedef enum e_ezlopi_device_grp_role_type
    {
        EZLOPI_DEVICE_GRP_ROLE_NONE = 0,
        EZLOPI_DEVICE_GRP_ROLE_USER,
        EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES,
        EZLOPI_DEVICE_GRP_ROLE_MAX
    } e_ezlopi_device_grp_role_type_t;

    typedef enum e_ezlopi_item_grp_role_type
    {
        EZLOPI_ITEM_GRP_ROLE_EMPTY = 0,
        EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY,
        EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES,
        EZLOPI_ITEM_GRP_ROLE_MAX
    } e_ezlopi_item_grp_role_type_t;

    typedef struct l_ezlopi_device_grp
    {
        cJSON *categories; // array containing required categories/sub-categories
        cJSON *devices;    // array containing DeviceIds to be added
        cJSON *exceptions; // array of DeviceIds to exclude from this group
        char *name;        // device_group_name
        char *package_id;
        e_entry_delay_t entry_delay;
        e_ezlopi_device_grp_role_type_t role;
        uint32_t _id; // perticular device_group_id
        bool persistent;
        bool follow_entry;
        struct l_ezlopi_device_grp *next;
    } l_ezlopi_device_grp_t;

    typedef struct l_ezlopi_item_grp
    {
        cJSON *item_names;  // array containing 'Item_names' as filters
        cJSON *enum_values; // array of strings ; used to filter specifice 'tokens' [from valueType -> token]
        cJSON *info;        // Description of an item group.
        char *name;
        char *value_type;                   // 'valueType' -> https://api.ezlo.com/hub/items_api/index.html, // ezlopi-cloud/constants/values_str.h
        char *value_type_family;            // [yes, if there is no valueType field] 'valueType_family' :- one of ["numeric" / "string" / "valuesWithLess" / "valuesWithoutLess"]
        e_ezlopi_item_grp_role_type_t role; // default : empty
        uint32_t _id;
        bool has_getter;
        bool has_setter;
        bool persistent;
        struct l_ezlopi_item_grp *next;
    } l_ezlopi_item_grp_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief This function return 'device_group_head' node from ll
     */
    l_ezlopi_device_grp_t *EZPI_core_device_group_get_head(void);
    /**
     * @brief This function return 'item_group_head' node from ll
     */
    l_ezlopi_item_grp_t *EZPI_core_item_group_get_head(void);
    /**
     * @brief Returns the node associated with required 'device_group_id'
     *
     * @param _id  'device_group_id'
     * @return l_ezlopi_device_grp_t*
     */
    l_ezlopi_device_grp_t *EZPI_core_device_group_get_by_id(uint32_t _id);
    /**
     * @brief Returns the node associated with required 'item_group_id'
     *
     * @param _id  'item_group_id'
     * @return l_ezlopi_device_grp_t*
     */
    l_ezlopi_item_grp_t *EZPI_core_item_group_get_by_id(uint32_t _id);
    /**
     * @brief This function generates equivalent 'cjson' object of the input 'l_ezlopi_device_grp_t*'
     *
     * @param devgrp_node The node to be converted
     * @return cJSON* : ( NOTE!! Must be freed after use )
     */
    cJSON *EZPI_core_device_group_create_cjson(l_ezlopi_device_grp_t *devgrp_node);
    /**
     * @brief This function generates equivalent 'cjson' object of the input 'l_ezlopi_item_grp_t*'
     *
     * @param itemgrp_node The node to be converted
     * @return cJSON* : ( NOTE!! Must be freed after use )
     */
    cJSON *EZPI_core_item_group_create_cjson(l_ezlopi_item_grp_t *itemgrp_node);
    /**
     * @brief This function handles editing of req_id in ll & nvs
     *
     * @param devgrp_id required 'devgrp_id'
     * @param cj_devgrp_new cjson of 'new_devgrp'
     * @return int
     */
    int EZPI_core_device_group_edit_by_id(uint32_t devgrp_id, cJSON *cj_devgrp_new);
    /**
     * @brief This function handles editing of req_id in ll & nvs
     *
     * @param itemgrp_id required 'itemgrp_id'
     * @param cj_itemgrp_new cjson of 'new_itemgrp'
     * @return int
     */
    int EZPI_core_item_group_edit_by_id(uint32_t itemgrp_id, cJSON *cj_itemgrp_new);
    /**
     * @brief This function removes perticular 'devgrp_id' from nvs list
     *
     * @param _id Target_devgrp_id
     */
    void EZPI_core_device_group_remove_id_from_list(uint32_t _id);
    /**
     * @brief This function removes perticular 'item_grp_id' from nvs list
     *
     * @param _id Target_item_grp_id
     */
    void EZPI_core_item_group_remove_id_from_list(uint32_t _id);
    /**
     * @brief Depopulate perticular node with 'device-group-id'
     *
     * @param _id target 'device-group-id'
     */
    void EZPI_core_device_group_depopulate_by_id_v2(uint32_t _id);
    /**
     * @brief Depopulate perticular node with 'item-group-id'
     *
     * @param _id target 'item-group-id'
     */
    void EZPI_core_item_group_depopulate_by_id_v2(uint32_t _id);
    /**
     * @brief This function returns all the 'device_groups' stored in nvs
     *
     * @param cj_devgrp_array This will contain all the 'dev_grps' in cjson format
     * @return uint32_t -> Total devGrps in nvs
     */
    uint32_t EZPI_core_device_group_get_list(cJSON *cj_devgrp_array);
    /**
     * @brief This function returns all the 'item_groups' stored in nvs
     *
     * @param cj_itemgrp_array This will contain all the 'item_grps' in cjson format
     * @return uint32_t -> Total itemGrps in nvs
     */
    uint32_t EZPI_core_item_group_get_list(cJSON *cj_itemgrp_array);
    /**
     * @brief This function is responsible for storing 'new_device_group_node' in nvs
     *
     * @param cj_new_device_grp New_dev_grp in cjson format
     * @return uint32_t
     */
    uint32_t EZPI_core_device_group_store_nvs_devgrp(cJSON *cj_new_device_grp);
    /**
     * @brief This function is responsible for storing 'new_item_group_node' in nvs
     *
     * @param cj_new_item_grp New_item_grp in cjson format
     * @return uint32_t
     */
    uint32_t EZPI_core_item_group_store_nvs_itemgrp(cJSON *cj_new_item_grp);
    /**
     * @brief This function populates/links the 'new_device_grp' into ll_head
     *
     * @param cj_new_dev_grp This contains 'new_dev_grp' in cjson format.
     * @param new_device_grp_id This 'devgrp_id' indentifies the 'new_dev_grp_node'
     * @return l_ezlopi_device_grp_t*
     */
    l_ezlopi_device_grp_t *EZPI_core_device_group_new_devgrp_populate(cJSON *cj_new_dev_grp, uint32_t new_device_grp_id);
    /**
     * @brief This function populates/links the 'new_item_grp' into ll_head
     *
     * @param cj_new_item_grp This contains 'new_item_grp' in cjson format.
     * @param new_item_grp_id This 'itemgrp_id' indentifies the 'new_item_grp_node'
     * @return l_ezlopi_item_grp_t*
     */
    l_ezlopi_item_grp_t *EZPI_core_item_group_new_itemgrp_populate(cJSON *cj_new_item_grp, uint32_t new_item_grp_id);
    /**
     * @brief This function returns list of valid dev_groups
     *
     * @param cj_devgrp_array constains the array of valid dev_groups
     * @param params cjson with Conditions to be checked
     * @return uint32_t  Total count of valid device_groups returned
     */
    uint32_t EZPI_core_device_group_find(cJSON *cj_devgrp_array, cJSON *params);
    /**
     * @brief This function return list of device, that contains desired items
     *
     * @param cj_request The array to be written to.
     * @param cj_response contains conditions to be validated.
     * @return uint32_t
     */
    uint32_t EZPI_core_device_group_devitem_expand(cJSON *cj_destination_array, cJSON *cj_params);
    /**
     * @brief main functions to initialize the device-groups
     */
    void EZPI_device_group_init(void);
    /**
     * @brief main functions to initialize the item-groups
     */
    void EZPI_item_group_init(void);

#ifdef __cplusplus
}
#endif

#endif //_EZLOPI_CORE_DEVICES_GROUP_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/

// ----------------------------------------------------
