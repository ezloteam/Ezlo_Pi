#ifndef __EZLOPI_CONFIG_H__
#define __EZLOPI_CONFIG_H__

#include "esp_gatts_api.h"

typedef enum e_gatt_status
{
    GATT_STATUS_NONE = 0,
    GATT_STATUS_PROCESSING,
    GATT_STATUS_DONE,
    GATT_STATUS_MAX
} e_gatt_status_t;

typedef void (*f_upcall_t)(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

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
    esp_attr_control_t *control;
    struct s_gatt_descr *next;
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
    s_gatt_descr_t *descriptor;
    struct s_gatt_char *next;
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
    struct s_gatt_char *characteristics;
    struct s_gatt_service *next;
} s_gatt_service_t;

#endif // __EZLOPI_CONFIG_H__