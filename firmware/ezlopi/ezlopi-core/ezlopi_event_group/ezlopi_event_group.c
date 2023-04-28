#include "string.h"

#include "esp_bit_defs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "ezlopi_event_group.h"

static EventGroupHandle_t ezlopi_event_group_handle = NULL;

void ezlopi_event_group_create(void)
{
    if (NULL == ezlopi_event_group_handle)
    {
        ezlopi_event_group_handle = xEventGroupCreate();
    }
}

void ezlopi_event_group_set_event(e_ezlopi_event_t event)
{
    if (ezlopi_event_group_handle && (event < BIT31))
    {
        xEventGroupSetBits(ezlopi_event_group_handle, event);
    }
}

int ezlopi_event_group_wait_for_event(e_ezlopi_event_t event, uint32_t wait_time_ms, uint32_t clear_on_exit)
{
    int ret = 0;
    if (ezlopi_event_group_handle)
    {
        EventBits_t event_bit = xEventGroupWaitBits(ezlopi_event_group_handle, event, clear_on_exit ? pdTRUE : pdFALSE, pdFALSE, wait_time_ms / portTICK_RATE_MS);
        if (event_bit & event)
        {
            ret = 1;
        }
    }
    else
    {
        ret = -1;
    }

    return ret;
}
