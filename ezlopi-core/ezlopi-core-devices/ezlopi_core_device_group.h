#ifndef EZLOPI_CORE_DEVICES_GROUP_H
#define EZLOPI_CORE_DEVICES_GROUP_H

#include "ezlopi_core_devices.h"

typedef enum e_ezlopi_device_grp_entrydelay_type
{
    EZLOPI_DEVICE_GRP_ENTRYDELAY_UNDEFINED = 0,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_MAX
}e_ezlopi_device_grp_entrydelay_type_t;

typedef enum e_ezlopi_device_grp_role_type
{
    EZLOPI_DEVICE_GRP_ROLE_NONE = 0,
    EZLOPI_DEVICE_GRP_ROLE_USER,
    EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES,
    EZLOPI_DEVICE_GRP_ROLE_MAX
}e_ezlopi_device_grp_role_type_t;

typedef enum e_ezlopi_item_grp_role_type
{
    EZLOPI_ITEM_GRP_ROLE_EMPTY = 0,
    EZLOPI_ITEM_GRP_ROLE_LIFE_SAFETY,
    EZLOPI_ITEM_GRP_ROLE_HOUSE_MODES,
    EZLOPI_ITEM_GRP_ROLE_MAX
}e_ezlopi_item_grp_role_type_t;

typedef struct l_ezlopi_device_grp
{
    uint32_t _id;               // perticular device_group_id
    char *name;             // device_group_name
    cJSON *categories;         // array containing required categories/sub-categories
    cJSON *devices;            // array containing DeviceIds to be added
    cJSON *exceptions;         // array of DeviceIds to exclude from this group
    bool persistent;
    e_ezlopi_device_grp_entrydelay_type_t entry_delay;
    bool follow_entry;
    e_ezlopi_device_grp_role_type_t role;
    char *package_id;
    struct l_ezlopi_device_grp *next;
}l_ezlopi_device_grp_t;

typedef struct l_ezlopi_item_grp
{
    uint32_t _id;
    char *name;
    bool has_getter;
    bool has_setter;
    bool persistent;
    cJSON *item_names;                 // array containing 'Item_names' as filters
    char *value_type;                // 'valueType' -> https://api.ezlo.com/hub/items_api/index.html, // ezlopi-cloud/constants/values_str.h 
    char *value_type_family;          // [yes, if there is no valueType field] 'valueType_family' :- one of ["numeric" / "string" / "valuesWithLess" / "valuesWithoutLess"]
    cJSON *enum_values;                // array of strings ; used to filter specifice 'tokens' [from valueType -> token]
    e_ezlopi_item_grp_role_type_t role; // default : empty
    cJSON *info;                       // Description of an item group.
    struct l_ezlopi_item_grp *next;
}l_ezlopi_item_grp_t;


// ----------------------------------------------------

/**
 * @brief This function return 'device_group_head' node from ll
 */
l_ezlopi_device_grp_t *ezlopi_core_device_group_get_head(void);
l_ezlopi_item_grp_t *ezlopi_core_item_group_get_head(void);

/**
 * @brief Returns the node associated with required 'device_group_id'
 *
 * @param _id  'device_group_id'
 * @return l_ezlopi_device_grp_t*
 */
l_ezlopi_device_grp_t *ezlopi_core_device_group_get_by_id(uint32_t _id);
l_ezlopi_item_grp_t *ezlopi_core_item_group_get_by_id(uint32_t _id);

/**
 * @brief This function generates equivalent 'cjson' object of the input 'l_ezlopi_device_grp_t*'
 *
 * @param devgrp_node The node to be converted
 * @return cJSON* : ( NOTE!! Must be freed after use )
 */
cJSON *ezlopi_core_device_group_create_cjson(l_ezlopi_device_grp_t *devgrp_node);
cJSON *ezlopi_core_item_group_create_cjson(l_ezlopi_item_grp_t *itemgrp_node);

/**
 * @brief This function handles editing of req_id in ll & nvs
 *
 * @param devgrp_id required 'devgrp_id'
 * @param cj_devgrp_new cjson of 'new_devgrp'
 * @return int
 */
int ezlopi_core_device_group_edit_by_id(uint32_t devgrp_id, cJSON *cj_devgrp_new);
int ezlopi_core_item_group_edit_by_id(uint32_t itemgrp_id, cJSON *cj_itemgrp_new);

/**
 * @brief This function removes perticular 'devgrp_id' from nvs list
 *
 * @param _id
 */
void ezlopi_core_device_group_remove_id_from_list(uint32_t _id);
void ezlopi_core_item_group_remove_id_from_list(uint32_t _id);

/**
 * @brief Depopulate perticular node with 'device-group-id'
 *
 * @param _id 'device-group-id'
 */
void ezlopi_core_device_group_depopulate_by_id_v2(uint32_t _id);
void ezlopi_core_item_group_depopulate_by_id_v2(uint32_t _id);

/**
 * @brief This function returns all the 'device_groups' stored in nvs
 *
 * @param cj_devgrp_array This will contain all the 'dev_grps' in cjson format
 * @return uint32_t -> Total devGrps in nvs
 */
uint32_t ezlopi_core_device_group_get_list(cJSON *cj_devgrp_array);
uint32_t ezlopi_core_item_group_get_list(cJSON *cj_itemgrp_array);

/**
 * @brief This function is responsible for storing 'new_device_group_node' in nvs
 *
 * @param cj_new_device_grp New_dev_grp in cjson format
 * @return uint32_t
 */
uint32_t ezlopi_core_device_group_store_nvs_devgrp(cJSON *cj_new_device_grp);
uint32_t ezlopi_core_item_group_store_nvs_itemgrp(cJSON *cj_new_item_grp);

/**
 * @brief This function populates/links the 'new_device_grp' into ll_head
 *
 * @param cj_new_dev_grp This contains 'new_dev_grp' in cjson format.
 * @param new_device_grp_id This 'devgrp_id' indentifies the 'new_dev_grp_node'
 * @return l_ezlopi_device_grp_t*
 */
l_ezlopi_device_grp_t *ezlopi_core_device_group_new_devgrp_populate(cJSON *cj_new_dev_grp, uint32_t new_device_grp_id);
l_ezlopi_item_grp_t *ezlopi_core_item_group_new_itemgrp_populate(cJSON *cj_new_item_grp, uint32_t new_item_grp_id);

/**
 * @brief This function returns list of valid dev_groups
 *
 * @param cj_devgrp_array constains the array of valid dev_groups
 * @param params cjson with Conditions to be checked
 * @return uint32_t  Total count of valid device_groups returned
 */
uint32_t ezlopi_core_device_group_find(cJSON *cj_devgrp_array, cJSON *params);

/**
 * @brief This function return list of device, that contains desired items
 *
 * @param cj_request The array to be written to.
 * @param cj_response contains conditions to be validated.
 * @return uint32_t
 */
uint32_t ezlopi_core_device_group_devitem_expand(cJSON *cj_destination_array, cJSON *cj_params);

/**
 * @brief main functions to initialize the device-groups
 */
void ezlopi_device_group_init(void);
void ezlopi_item_group_init(void);


#endif//EZLOPI_CORE_DEVICES_GROUP_H