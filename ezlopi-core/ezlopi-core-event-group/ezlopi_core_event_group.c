#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_core_event_group.h"

static EventGroupHandle_t ezlopi_event_group_handle = NULL;

void ezlopi_event_group_create(void)
{
    if (NULL == ezlopi_event_group_handle)
    {
        ezlopi_event_group_handle = xEventGroupCreate();
    }
}

ezlopi_error_t ezlopi_event_group_set_event(e_ezlopi_event_t event)
{
    // if (ezlopi_event_group_handle && (event < BIT31))
    // {
    //     xEventGroupSetBits(ezlopi_event_group_handle, event);
    // }

    ezlopi_error_t error = EZPI_SUCCESS;
    if (ezlopi_event_group_handle)
    {
        if (event < BIT31)
        {
            xEventGroupSetBits(ezlopi_event_group_handle, event);
        }
        else
        {
            error = EZPI_ERR_EVENT_GROUP_BIT_UNDEFINED;
        }
    }
    else
    {
        error = EZPI_ERR_EVENT_GROUP_UNINITIALIZED;
    }
    return error;
}

ezlopi_error_t ezlopi_event_group_clear_event(e_ezlopi_event_t event)
{
    // if (ezlopi_event_group_handle && (event < BIT31))
    // {
    //     xEventGroupClearBits(ezlopi_event_group_handle, event);
    // }

    ezlopi_error_t error = EZPI_SUCCESS;
    if (ezlopi_event_group_handle)
    {
        if (event < BIT31)
        {
            xEventGroupClearBits(ezlopi_event_group_handle, event);
        }
        else
        {
            error = EZPI_ERR_EVENT_GROUP_BIT_UNDEFINED;
        }
    }
    else
    {
        error = EZPI_ERR_EVENT_GROUP_UNINITIALIZED;
    }
    return error;
}

ezlopi_error_t ezlopi_event_group_wait_for_event(e_ezlopi_event_t event, uint32_t wait_time_ms, uint32_t clear_on_exit)
{
    ezlopi_error_t error = EZPI_ERR_EVENT_GROUP_BIT_WAIT_FAILED;
    if (ezlopi_event_group_handle)
    {
        EventBits_t event_bits = xEventGroupWaitBits(ezlopi_event_group_handle, event, clear_on_exit ? pdTRUE : pdFALSE, pdFALSE, ((wait_time_ms == portMAX_DELAY) ? portMAX_DELAY : (wait_time_ms / portTICK_RATE_MS)));
        if (event_bits & event)
        {
            error = EZPI_SUCCESS;
        }
    }
    else
    {
        error = EZPI_ERR_EVENT_GROUP_UNINITIALIZED;
    }

    return error;
}

e_ezlopi_event_t ezlopi_get_event_bit_status()
{
    #warning("return statement always returns 0 i.e. OTA event at else")
        e_ezlopi_event_t ret = 0;
    if (ezlopi_event_group_handle)
    {
        ret = xEventGroupGetBits(ezlopi_event_group_handle);
    }
    return ret;
}


