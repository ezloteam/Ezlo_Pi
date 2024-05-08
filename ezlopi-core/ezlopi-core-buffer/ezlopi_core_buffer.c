#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_core_buffer.h"
#include "EZLOPI_USER_CONFIG.h"

static char *__buffer = NULL;
static uint32_t __buffer_len = 0;
static xSemaphoreHandle __buffer_lock = NULL;
static volatile e_buffer_state_t __buffer_lock_state = EZ_BUFFER_STATE_NOT_INITIATED;

e_buffer_state_t ezlopi_core_buffer_status(void)
{
    return __buffer_lock_state;
}

void ezlopi_core_buffer_deinit(void)
{
    if (__buffer_lock)
    {
        vSemaphoreDelete(__buffer_lock);
        __buffer_lock = NULL;
    }

    if (__buffer)
    {
        ezlopi_free(__FUNCTION__, __buffer);
        __buffer = NULL;
    }

    __buffer_len = 0;
    __buffer_lock_state = EZ_BUFFER_STATE_NOT_INITIATED;
}

void ezlopi_core_buffer_init(uint32_t len)
{
    if (__buffer_lock)
    {
        xSemaphoreTake(__buffer_lock, portMAX_DELAY);
    }

    if (__buffer)
    {
        ezlopi_free(__FUNCTION__, __buffer);
        __buffer = NULL;
        __buffer_len = 0;
        __buffer_lock_state = EZ_BUFFER_STATE_NOT_INITIATED;
    }

    __buffer = ezlopi_malloc(__FUNCTION__, len);

    if (__buffer)
    {
        __buffer_len = len;

        if (NULL == __buffer_lock)
        {
            __buffer_lock = xSemaphoreCreateMutex();
            if (__buffer_lock)
            {
                __buffer_lock_state = EZ_BUFFER_STATE_AVAILABLE;
                xSemaphoreGive(__buffer_lock);
            }
            else
            {
                __buffer_len = 0;
                ezlopi_free(__FUNCTION__, __buffer);
                __buffer = NULL;
                __buffer_lock_state = EZ_BUFFER_STATE_NOT_INITIATED;
            }
        }
        else
        {
            __buffer_lock_state = EZ_BUFFER_STATE_AVAILABLE;
            xSemaphoreGive(__buffer_lock);
        }
    }
    else
    {
        if (__buffer_lock)
        {
            vSemaphoreDelete(__buffer_lock);
            __buffer_lock = NULL;
        }

        __buffer_len = 0;
        __buffer_lock_state = EZ_BUFFER_STATE_NOT_INITIATED;
    }
}

char *ezlopi_core_buffer_acquire(uint32_t *len, uint32_t wait_to_acquired_ms)
{
    char *ret = NULL;
    uint32_t start_time = xTaskGetTickCount();

    if (__buffer_lock)
    {
        if (pdTRUE == xSemaphoreTake(__buffer_lock, wait_to_acquired_ms / portTICK_RATE_MS))
        {
            ret = __buffer;
            *len = __buffer_len;
            __buffer_lock_state = EZ_BUFFER_STATE_BUSY;
        }
    }

    return ret;
}

void ezlopi_core_buffer_release(void)
{
    if (__buffer_lock && (EZ_BUFFER_STATE_BUSY == __buffer_lock_state))
    {
        xSemaphoreGive(__buffer_lock);
        __buffer_lock_state = EZ_BUFFER_STATE_AVAILABLE;
    }
    else
    {
        // TRACE_E("buffer release failed!");
    }
}
