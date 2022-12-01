#ifndef __EZLOPI_CONFIG_H__
#define __EZLOPI_CONFIG_H__

#include "esp_gatts_api.h"

typedef struct s_gatt_descr
{
    // s_gatt_char_t *characteristic;
    uint16_t handle;
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    // esp_attr_value_t *val;
    esp_attr_control_t *control;
    struct s_gatt_descr *next;
} s_gatt_descr_t;

typedef struct s_gatt_char
{
    // s_gatt_service_t *service;
    uint16_t handle;
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t property;
    // esp_attr_value_t *char_val;
    esp_attr_control_t *control;
    s_gatt_descr_t *descriptor;
    struct s_gatt_char *next;
} s_gatt_char_t;

typedef struct s_gatt_service
{
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t gatts_if;
    uint16_t num_handles; // Number of handles
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    struct s_gatt_char *characteristics;
    struct s_gatt_service *next;
} s_gatt_service_t;

#endif // __EZLOPI_CONFIG_H__
