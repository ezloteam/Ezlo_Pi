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
static SemaphoreHandle_t sg_device_alert_smphr = NULL;

static void __modes_loop(void *pv);
static void __modes_alert_loop(void *pv);
// static void __modes_service(void *pv);

void ezlopi_service_modes_init(void)
{
    // initialize modes-loop
    sg_modes_loop_smphr = xSemaphoreCreateBinary();
    xSemaphoreGive(sg_modes_loop_smphr);
    ezlopi_service_modes_start(5000);
}

static void ezlopi_service_mode_alert_init(void)
{
    // initialize if the 'mode-loop' and 'semaphore' exists
    if (NULL != sg_modes_loop_smphr &&
        (true == ezlopi_service_is_mode_loop_acitve(__modes_loop)) &&      // must have mode-loop
        (false == ezlopi_service_is_mode_loop_acitve(__modes_alert_loop))) // must not have mode-alert-loop
    {
        // check - the bypass list
        sg_device_alert_smphr = xSemaphoreCreateBinary();
        xSemaphoreGive(sg_device_alert_smphr);
        ezlopi_service_modes_device_alert_start(5000);
    }
}

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

        // Stop the 'alert-loop' if possible.
        if (NULL != sg_device_alert_smphr && (true == ezlopi_service_is_mode_loop_acitve(__modes_alert_loop))) // the 'alert_semaphore' should not be null // 'alert-loop' should exist
        {
            ezlopi_service_modes_device_alert_stop(5000);
        }
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

        // Start the 'alert-loop' if possible.
        if (NULL != sg_device_alert_smphr && (false == ezlopi_service_is_mode_loop_acitve(__modes_alert_loop))) // the 'alert_semaphore' should not be null // 'alert-loop' should not exist
        {
            ezlopi_service_modes_device_alert_start(5000);
        }
    }

    return ret;
}

static bool ezlopi_service_modes_device_alert_start(uint32_t wait_ms)
{
    bool ret = false;

    if (ezlopi_core_modes_get_custom_modes() && xSemaphoreTake(sg_device_alert_smphr, wait_ms / portTICK_RATE_MS))
    {
        ret = true;
        xSemaphoreGive(sg_device_alert_smphr);
        ezlopi_service_loop_add("modes-ALERT-loop", __modes_alert_loop, 1000, NULL);
        TRACE_I("added modes-ALERT-loop");
    }

    return ret;
}

static bool ezlopi_service_modes_device_alert_stop(uint32_t wait_ms)
{
    bool ret = false;
    // uint32_t start_tick = xTaskGetTickCount() / portTICK_PERIOD_MS;

    if (sg_device_alert_smphr && (xSemaphoreTake(sg_device_alert_smphr, wait_ms / portTICK_RATE_MS)))
    {
        ezlopi_service_loop_remove(__modes_alert_loop);
        xSemaphoreGive(sg_device_alert_smphr);
        TRACE_W("removed modes-ALERT-loop");
        ret = true;
    }

    return ret;
}

static ezlopi_error_t __check_mode_switch_condition(s_ezlopi_modes_t *ez_mode)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;

    if (ez_mode->switch_to_mode_id)
    {
        if (ez_mode->time_is_left_to_switch_sec)
        {
            ez_mode->time_is_left_to_switch_sec--;
            TRACE_D("time_is_left_to_SWITCH_sec: %u", ez_mode->time_is_left_to_switch_sec);
        }
        else
        {
            // find the 'house-mode', to switch to using 'switch_to_mode_id'.
            s_house_modes_t *new_house_mode = ezlopi_core_modes_get_house_mode_by_id(ez_mode->switch_to_mode_id);
            if (new_house_mode)
            {
                uint8_t bypass_clear_modeId = ez_mode->current_mode_id;

                // switch to new 'MODE'
                TRACE_I("switching-to-mode: %s (id: %u)", new_house_mode->name, new_house_mode->_id);
                ez_mode->current_mode_id = ez_mode->switch_to_mode_id;
                ez_mode->switch_to_mode_id = 0;

                // After the transition, bypass devices list is cleared for the previous house mode.
                s_house_modes_t *tmp_house_mode = ezlopi_core_modes_get_house_mode_by_id(bypass_clear_modeId);
                if (tmp_house_mode && tmp_house_mode->cj_bypass_devices)
                {
                    cJSON_Delete(__FUNCTION__, tmp_house_mode->cj_bypass_devices);
                    tmp_house_mode->cj_bypass_devices = NULL;
                }

                // set the new 'house-mode' as the active 'custom-mode'
                if (EZPI_SUCCESS == ezlopi_core_modes_set_current_house_mode(new_house_mode))
                {
                    // 1. assign 'delayToSwitch'
                    ez_mode->time_is_left_to_switch_sec = ez_mode->switch_to_delay_sec = new_house_mode->switch_to_delay_sec;

                    // 2. assign 'alert/alarm_delay' mode.
                    if (new_house_mode->armed)
                    {
                        ez_mode->alarm_delay = new_house_mode->alarm_delay_sec; // 'Delay_sec' before sending alert if armed sensors (door/window or motion sensor) tripped
                    }

                    // 3. Refresh 'timeleftTo-exit-EntryDelay' second info for --->  non-zero 'entryDelay'
                    if ((0 < ez_mode->alarmed.entry_delay_sec) && (0 == ez_mode->alarmed.time_is_left_sec))
                    {
                        ez_mode->alarmed.time_is_left_sec = ez_mode->alarmed.entry_delay_sec;
                    }

                    // 4. Store to nvs
                    ezlopi_core_modes_store_to_nvs();

                    cJSON *cj_update = ezlopi_core_modes_cjson_changed();
                    CJSON_TRACE("----------------- broadcasting - cj_update", cj_update);

                    if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(cj_update))
                    {
                        cJSON_Delete(__FUNCTION__, cj_update);
                    }

                    ret = EZPI_SUCCESS;
                }
            }
        }
    }

    return ret;
}

static bool __check_if_device_is_bypassed(s_ezlopi_modes_t *ez_mode, s_house_modes_t *curr_house_mode, uint32_t device_id_str)
{
    bool ret = false;
    if (ez_mode && curr_house_mode && device_id_str)
    {
        cJSON *cj_element = NULL;
        cJSON_ArrayForEach(cj_element, curr_house_mode->cj_bypass_devices)
        {
            if (EZPI_STRNCMP_IF_EQUAL(device_id_str, cj_element->valuestring, strlen(device_id_str), cj_element->str_value_len))
            {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __check_mode_entry_delay(s_ezlopi_modes_t *ez_mode, s_house_modes_t *curr_house_mode)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (ez_mode && curr_house_mode)
    {
        bool trigger_broadcast = false;
        bool entryDelay_finish = false;

        // 1. First activate --> 'entry-delay' phase immediately
        if ((0 < ez_mode->alarmed.entry_delay_sec) && (0 < ez_mode->alarmed.time_is_left_sec)) // time-left to exit entry-delay
        {
            ez_mode->alarmed.phase = EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY;

            // First check if 'EntryDelay' is canceled.
            if (EZLOPI_MODES_ALARM_STATUS_CANCELED != ez_mode->alarmed.status) // begin or done
            {
                // a. Trigger entry-Delay 'begin' broadcast (at the start)
                if (EZLOPI_MODES_ALARM_STATUS_BEGIN != ez_mode->alarmed.status) // done
                {
                    ez_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_BEGIN;
                    trigger_broadcast = true;
                }

                // b. Check if the mode is given
                if (0 == ez_mode->alarmed.time_is_left_sec) // Number of seconds left to the end of the Entry delay.
                {
                    ez_mode->alarmed.status = EZLOPI_MODES_ALARM_STATUS_DONE;
                    trigger_broadcast = true;
                    entryDelay_finish = true;
                    // ezlopi_core_modes_store_to_nvs();
                }
                else
                {
                    ez_mode->alarmed.time_is_left_sec--;
                    TRACE_D("time_is_left_to_ALARM_sec: %u", ez_mode->alarmed.time_is_left_sec);
                }
            }
            else // for canceled entry-delay
            {
                TRACE_D("entryDelay_Status : 'Canceled' ");
                entryDelay_finish = true;
            }
        }
        else // Here if "entryDelay-time-left" is 0
        {
            entryDelay_finish = true;
        }

        //-------------------------------------------------------------------------------------
        // 2. Second  Enter --> Bypass or Main Phase.
        if (entryDelay_finish)
        {
        }

        //-------------------------------------------------------------------------------------
        // 3. Lastly Trigger a broadcast to provid --> phase and status info
        if (trigger_broadcast)
        {
            cJSON *cj_update = ezlopi_core_modes_cjson_alarmed();
            CJSON_TRACE("----------------- broadcasting - cj_update", cj_update);

            if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(cj_update))
            {
                cJSON_Delete(__FUNCTION__, cj_update);
            }
        }
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static void __modes_alert_loop(void *arg)
{
    if (pdTRUE == xSemaphoreTake(sg_device_alert_smphr, 1000 / portTICK_PERIOD_MS))
    {
        s_ezlopi_modes_t *ez_mode = ezlopi_core_modes_get_custom_modes();
        s_house_modes_t *curr_house_mode = ezlopi_core_modes_get_current_house_modes();
        if (ez_mode && curr_house_mode)
        {
            // find the devices_ids with alarm/security ability
            char device_id_str[32] = {0};
            l_ezlopi_device_t *device_node = ezlopi_device_get_head();
            while (device_node)
            {
                snprintf(device_id_str, 32, "%08x", device_node->cloud_properties.device_id);
                if (__check_if_device_is_bypassed(ez_mode, curr_house_mode, device_id_str))
                {
                    // 2. Now to enter alarming process; if 'house modes is alarmed, and HouseModes.alarmDelay > 0'
                    if (true == curr_house_mode->armed) //&& (0 < curr_house_mode->alarm_delay_sec)
                    {
                        __check_mode_alarm_trigger(ez_mode, curr_house_mode);
                    }
                }
                device_node = device_node->next;
            }
        }

        xSemaphoreGive(sg_device_alert_smphr);
    }
}

static void __modes_loop(void *arg)
{
    if (pdTRUE == xSemaphoreTake(sg_modes_loop_smphr, 1000 / portTICK_PERIOD_MS))
    {
        s_ezlopi_modes_t *ez_mode = ezlopi_core_modes_get_custom_modes();
        if (ez_mode)
        {
            // 1. check if the mode is to be switched.
            if (EZPI_SUCCESS == __check_mode_switch_condition(ez_mode))
            {
                TRACE_D("Mode - Switch completed to [%d]", ez_mode->current_mode_id);
            }

            if (NULL == sg_device_alert_smphr)
            { //  Only once: start the alert loop
                ezlopi_service_mode_alert_init();
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
