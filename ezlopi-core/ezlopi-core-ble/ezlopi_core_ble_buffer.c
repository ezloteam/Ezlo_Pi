
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE

#include <string.h>

#include "ezlopi_core_ble_buffer.h"
#include "EZLOPI_USER_CONFIG.h"

s_linked_buffer_t* ezlopi_ble_buffer_create(esp_ble_gatts_cb_param_t* param)
{
    s_linked_buffer_t* linked_buffer = ezlopi_malloc(__FUNCTION__, sizeof(s_linked_buffer_t));
    if (linked_buffer)
    {
        memset(linked_buffer, 0, sizeof(s_linked_buffer_t));
        if ((NULL != param->write.value) && (param->write.len > 0))
        {
            linked_buffer->buffer = ezlopi_malloc(__FUNCTION__, param->write.len);
            if (linked_buffer->buffer)
            {
                linked_buffer->len = param->write.len;
                memcpy(linked_buffer->buffer, param->write.value, param->write.len);
            }
            else
            {
                ezlopi_free(__FUNCTION__, linked_buffer);
                linked_buffer = NULL;
            }
        }
        else
        {
            ezlopi_free(__FUNCTION__, linked_buffer);
            linked_buffer = NULL;
        }
    }

    return linked_buffer;
}

void ezlopi_ble_buffer_add_to_buffer(s_linked_buffer_t* buffer, esp_ble_gatts_cb_param_t* param)
{
    while (buffer->next)
    {
        buffer = buffer->next;
    }

    buffer->next = ezlopi_ble_buffer_create(param);
}

void ezlopi_ble_buffer_free_buffer(s_linked_buffer_t* l_buffer)
{
    if (l_buffer)
    {
        if (l_buffer->buffer)
        {
            ezlopi_free(__FUNCTION__, l_buffer->buffer);
            l_buffer->buffer = NULL;
        }
        ezlopi_ble_buffer_free_buffer(l_buffer->next);
        l_buffer->next = NULL;
        ezlopi_free(__FUNCTION__, l_buffer);
    }
}

void ezlopi_ble_buffer_accumulate_to_start(s_linked_buffer_t* l_buffer)
{
    if (l_buffer)
    {
        uint32_t tot_len = 0;
        s_linked_buffer_t* tmp_buffer = l_buffer;
        while (tmp_buffer)
        {
            tot_len += tmp_buffer->len;
            tmp_buffer = tmp_buffer->next;
        }

        uint8_t* tot_buffer = ezlopi_malloc(__FUNCTION__, tot_len + 1);
        if (tot_buffer)
        {
            memset(tot_buffer, 0, tot_len + 1);
            uint32_t pos = 0;
            tmp_buffer = l_buffer;
            while (tmp_buffer)
            {
                memcpy(&tot_buffer[pos], tmp_buffer->buffer, tmp_buffer->len);
                pos += tmp_buffer->len;
                tmp_buffer = tmp_buffer->next;
            }

            if (l_buffer->buffer)
            {
                ezlopi_free(__FUNCTION__, l_buffer->buffer);
                l_buffer->buffer = NULL;
            }

            l_buffer->buffer = tot_buffer;
            l_buffer->len = tot_len;
        }

        ezlopi_ble_buffer_free_buffer(l_buffer->next);
        l_buffer->next = NULL;
    }
}
#endif // CONFIG_EZPI_BLE_ENABLE