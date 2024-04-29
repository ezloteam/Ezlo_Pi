#ifndef _EZLOPI_CORE_BLE_BUFFER_H_
#define _EZLOPI_CORE_BLE_BUFFER_H_

#ifdef CONFIG_EZLOPI_BLE_ENABLE

#include "esp_gatts_api.h"

typedef struct s_linked_buffer
{
    struct s_linked_buffer* next;
    uint32_t len;
    uint8_t* buffer;
} s_linked_buffer_t;

s_linked_buffer_t* ezlopi_ble_buffer_create(esp_ble_gatts_cb_param_t* param);
void ezlopi_ble_buffer_add_to_buffer(s_linked_buffer_t* buffer, esp_ble_gatts_cb_param_t* param);
void ezlopi_ble_buffer_free_buffer(s_linked_buffer_t* l_buffer);
void ezlopi_ble_buffer_accumulate_to_start(s_linked_buffer_t* l_buffer);

#endif // CONFIG_EZLOPI_BLE_ENABLE

#endif //  _EZLOPI_CORE_BLE_BUFFER_H_