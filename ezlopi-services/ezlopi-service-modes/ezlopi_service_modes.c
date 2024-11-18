#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_cloud_modes.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_modes.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_modes_cjson.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_service_loop.h"
#include "ezlopi_service_modes.h"
// #include "ezlopi_service_webprov.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

static SemaphoreHandle_t sg_modes_loop_smphr = NULL;

static void __modes_loop(void *pv);
// static void __modes_service(void *pv);

bool ezlopi_service_modes_stop(uint32_t wait_ms)
{
    bool ret = false;
    uint32_t start_tick = xTaskGetTickCount() / portTICK_PERIOD_MS;

    if (sg_modes_loop_smphr && (xSemaphoreTake(sg_modes_loop_smphr, wait_ms / portTICK_RATE_MS)))
    {
        ezlopi_service_loop_remove(__modes_loop);
        xSemaphoreGive(sg_modes_loop_smphr);
        TRACE_W("removed modes-loop");
        ret = true;
    }

    return ret;
}

bool ezlopi_service_modes_start(uint32_t wait_ms)
{
    bool ret = false;

    if (ezlopi_core_modes_get_custom_modes() && xSemaphoreTake(sg_modes_loop_smphr, wait_ms / portTICK_RATE_MS))
    {
        ret = true;
        xSemaphoreGive(sg_modes_loop_smphr);
        ezlopi_service_loop_add("modes-loop", __modes_loop, 1000, NULL);
        TRACE_I("added modes-loop");
    }

    return ret;
}

void ezlopi_service_modes_init(void)
{
    sg_modes_loop_smphr = xSemaphoreCreateBinary();
    xSemaphoreGive(sg_modes_loop_smphr);
    ezlopi_service_modes_start(5000);
}

static void __modes_loop(void *arg)
{
    if (pdTRUE == xSemaphoreTake(sg_modes_loop_smphr, 1000 / portTICK_PERIOD_MS))
    {
        s_ezlopi_modes_t *ez_mode = ezlopi_core_modes_get_custom_modes();
        if (ez_mode)
        {
            if (ez_mode->switch_to_mode_id)
            {
                if (ez_mode->time_is_left_to_switch_sec)
                {
                    ez_mode->time_is_left_to_switch_sec--;
                    TRACE_D("time_is_left_to_SWITCH_sec: %u", ez_mode->time_is_left_to_switch_sec);
                }
                else
                {
                    // find the 'house-mode' to switch to using 'switch_to_mode_id'
                    s_house_modes_t *new_house_mode = ezlopi_core_modes_get_house_mode_by_id(ez_mode->switch_to_mode_id);
                    if (new_house_mode)
                    {
                        TRACE_I("switching-to-mode: %s (id: %u)", new_house_mode->name, new_house_mode->_id);
                        ez_mode->current_mode_id = ez_mode->switch_to_mode_id;
                        ez_mode->switch_to_mode_id = 0;

                        // set the new 'house-mode' as the active 'custom-mode'
                        if (EZPI_SUCCESS == ezlopi_core_modes_set_current_house_mode(new_house_mode))
                        {
                            // assign 'delayToSwitch'
                            ez_mode->time_is_left_to_switch_sec = ez_mode->switch_to_delay_sec = new_house_mode->switch_to_delay_sec;

                            // assign 'alarm_delay' mode.
                            ez_mode->time_left_to_alarm_sec = ez_mode->alarm_delay_sec = new_house_mode->alarm_delay_sec;

                            if (new_house_mode->cj_bypass_devices)
                            {
                                cJSON_Delete(__FUNCTION__, new_house_mode->cj_bypass_devices);
                                new_house_mode->cj_bypass_devices = NULL;
                            }

                            ezlopi_core_modes_store_to_nvs();

                            cJSON *cj_update = ezlopi_core_modes_cjson_changed();
                            CJSON_TRACE("----------------- broadcasting - cj_update", cj_update);

                            if (0 != ezlopi_core_broadcast_add_to_queue(cj_update))
                            {
                                cJSON_Delete(__FUNCTION__, cj_update);
                            }
                        }
                    }
                }
            }

            if (ez_mode->alarmed && 0 < ez_mode->alarm_delay_sec)
            {
                if (ez_mode->time_left_to_alarm_sec)
                {
                    ez_mode->time_left_to_alarm_sec--;
                    TRACE_D("time_is_left_to_ALARM_sec: %u", ez_mode->time_left_to_alarm_sec);

                }
                else    // broadcast --> 'alarmed state activation'
                {

                    ezlopi_core_modes_store_to_nvs();

                    cJSON *cj_update = ezlopi_core_modes_cjson_changed();
                    CJSON_TRACE("----------------- broadcasting - cj_update", cj_update);

                    if (0 != ezlopi_core_broadcast_add_to_queue(cj_update))
                    {
                        cJSON_Delete(__FUNCTION__, cj_update);
                    }
                }
            }
        }

        xSemaphoreGive(sg_modes_loop_smphr);
    }
}

#if 0
static TaskHandle_t sg_process_handle = NULL;

int ezlopi_service_modes_stop(void)
{
    if (sg_process_handle)
    {
        ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_MODES_TASK);
        vTaskDelete(sg_process_handle);
        sg_process_handle = NULL;
        TRACE_W("Modes-service: Stopped!");
    }

    return 1;
}

int ezlopi_service_modes_start(5000void)
{
    int ret = 0;

    if ((NULL == sg_process_handle) && ezlopi_core_modes_get_custom_modes())
    {
        ret = 1;
        xTaskCreate(__modes_service, "modes-service", EZLOPI_SERVICE_MODES_TASK_DEPTH, NULL, 3, &sg_process_handle);
        ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_MODES_TASK, &sg_process_handle, EZLOPI_SERVICE_MODES_TASK_DEPTH);
        TRACE_I("Starting modes-service");
    }

    return ret;
}

static void __modes_service(void *pv)
{
    while (1)
    {
        s_ezlopi_modes_t *ez_mode = ezlopi_core_modes_get_custom_modes();
        if (ez_mode)
        {
            if (ez_mode->switch_to_mode_id)
            {
                if (ez_mode->time_is_left_to_switch_sec)
                {
                    ez_mode->time_is_left_to_switch_sec--;
                    TRACE_D("time_is_left_to_switch_sec: %u", ez_mode->time_is_left_to_switch_sec);
                }
                else
                {
                    s_house_modes_t *new_house_mode = ezlopi_core_modes_get_house_mode_by_id(ez_mode->switch_to_mode_id);

                    if (new_house_mode)
                    {
                        TRACE_I("switching-to-mode: %s (id: %u)", new_house_mode->name, new_house_mode->_id);

                        ez_mode->current_mode_id = ez_mode->switch_to_mode_id;
                        ez_mode->switch_to_mode_id = 0;

                        ezlopi_core_modes_set_current_house_mode(new_house_mode);

                        if (new_house_mode->cj_bypass_devices)
                        {
                            cJSON_Delete(__FUNCTION__, new_house_mode->cj_bypass_devices);
                            new_house_mode->cj_bypass_devices = NULL;
                        }

                        ezlopi_core_modes_store_to_nvs();

                        cJSON *cj_update = ezlopi_core_modes_cjson_changed();
                        CJSON_TRACE("----------------- broadcasting - cj_update", cj_update);

                        if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(cj_update))
                        {
                            cJSON_Delete(__FUNCTION__, cj_update);
                        }
                    }
                }
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_MODES_TASK);
    vTaskDelete(NULL);
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MODES
