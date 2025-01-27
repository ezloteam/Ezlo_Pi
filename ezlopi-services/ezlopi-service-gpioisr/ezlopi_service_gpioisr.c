
/**
 * @file    ezlopi_service_gpioisr.c
 * @brief
 * @authors Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    ezlopi_service_gpioisr.c
 * @brief   Contains function definitions related to GPIO ISR
 * @author
 * @version 1.0
 * @date    November 23, 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include <string.h>

#include "../../build/config/sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "EZLOPI_USER_CONFIG.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_processes.h"

#include "ezlopi_service_gpioisr.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
/**
 * @brief Event argument struct for GPIO ISR
 *
 */
typedef struct s_event_arg
{
    TickType_t time;               /**< Holds the time detail to check if debounce has occured */
    TickType_t debounce_ms;        /**< Debounce ms to check for */
    l_ezlopi_item_t *item;         /**< Pointer to the item the interrupt belongs to */
    f_interrupt_upcall_t __upcall; /**< Upcall to call on interrupt */
} s_event_arg_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
/**
 * @brief Function is a task that receives from the interrrupt queue, checks for debounce and then call the upcall
 *
 * @param pv Task param
 */
static void ezpi_gpio_isr_process_v3(void *pv);
/**
 * @brief GPIO interrupt handler for the firmware, this will push to the event queue
 *
 * @param arg Handler arg
 */
static void IRAM_ATTR __gpio_isr_handler(void *arg);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static QueueHandle_t gpio_evt_queue = NULL;
static const uint32_t default_debounce_time = 1000;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

void EZPI_service_gpioisr_init(void)
{
    TRACE_S("Started gpio-isr service");
    gpio_evt_queue = xQueueCreate(20, sizeof(s_event_arg_t *));
    TaskHandle_t ezlopi_service_gpio_isr_task_handle = NULL;
    xTaskCreate(ezpi_gpio_isr_process_v3, "gpio_isr_process_v3", EZLOPI_SERVICE_GPIO_ISR_TASK_DEPTH, NULL, 3, &ezlopi_service_gpio_isr_task_handle);
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_process_info(ENUM_EZLOPI_SERVICE_GPIO_ISR_TASK, &ezlopi_service_gpio_isr_task_handle, EZLOPI_SERVICE_GPIO_ISR_TASK_DEPTH);
#endif
}

void EZPI_service_gpioisr_register_v3(l_ezlopi_item_t *item, f_interrupt_upcall_t __upcall, TickType_t debounce_ms)
{
    s_event_arg_t *event_arg = ezlopi_malloc(__FUNCTION__, sizeof(s_event_arg_t));

    if (event_arg)
    {
        event_arg->time = 0;
        // event_arg->properties = properties;
        event_arg->item = item;
        event_arg->__upcall = __upcall;
        event_arg->debounce_ms = (0 == debounce_ms) ? default_debounce_time : debounce_ms;
        gpio_intr_enable(item->interface.gpio.gpio_in.gpio_num);

        if (gpio_isr_handler_add(item->interface.gpio.gpio_in.gpio_num, __gpio_isr_handler, (void *)event_arg))
        {
            TRACE_E("Error while adding GPIO ISR handler.");
            gpio_reset_pin(item->interface.gpio.gpio_in.gpio_num);
        }
        else
        {
            TRACE_S("Successfully added GPIO ISR handler for pin: %d.", item->interface.gpio.gpio_in.gpio_num);
        }
    }
    else
    {
        TRACE_E("Malloc failed!");
    }
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

static void ezpi_gpio_isr_process_v3(void *pv)
{
    while (1)
    {
        s_event_arg_t *event_arg = NULL;
        xQueueReceive(gpio_evt_queue, &event_arg, portMAX_DELAY);

        if (NULL != event_arg)
        {
            TickType_t tick_now = xTaskGetTickCount();

            if ((tick_now - event_arg->time) > (event_arg->debounce_ms / portTICK_RATE_MS))
            {
                event_arg->__upcall(event_arg->item);
                event_arg->time = tick_now;
            }
        }

        // vTaskDelay(1);
    }
}

static void IRAM_ATTR __gpio_isr_handler(void *arg)
{
    void *tmp_arg = arg;
    if (gpio_evt_queue)
    {
        xQueueSendFromISR(gpio_evt_queue, &tmp_arg, NULL);
    }
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/