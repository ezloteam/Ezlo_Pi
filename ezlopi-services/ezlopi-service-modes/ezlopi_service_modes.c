#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_modes.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_cloud_modes.h"

#include "ezlopi_service_modes.h"

static TaskHandle_t sg_process_handle = NULL;

static void __modes_service(void *pv);

int ezlopi_service_modes_stop(void)
{
    if (sg_process_handle)
    {
        vTaskDelete(sg_process_handle);
        sg_process_handle = NULL;
    }

    return 1;
}

int ezlopi_service_modes_start(void)
{
    int ret = 0;

    if (sg_process_handle && ezlopi_core_modes_get_custom_modes())
    {
        ret = 1;
        xTaskCreate(__modes_service, "modes-service", 1024 * 4, NULL, 3, &sg_process_handle);
    }

    return ret;
}

void ezlopi_service_modes_init(void)
{
    ezlopi_service_modes_start();
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
                }
                else
                {
                    s_house_modes_t *current_house_mode = NULL;
                    ez_mode->current_mode_id = ez_mode->switch_to_mode_id;
                    ez_mode->switch_to_mode_id = 0;

                    switch (ez_mode->current_mode_id & 0x000000ff)
                    {
                    case EZLOPI_HOUSE_MODE_REF_ID_HOME:
                    {
                        current_house_mode = &ez_mode->mode_home;
                        break;
                    }
                    case EZLOPI_HOUSE_MODE_REF_ID_AWAY:
                    {
                        current_house_mode = &ez_mode->mode_home;
                        break;
                    }
                    case EZLOPI_HOUSE_MODE_REF_ID_NIGHT:
                    {
                        current_house_mode = &ez_mode->mode_home;
                        break;
                    }
                    case EZLOPI_HOUSE_MODE_REF_ID_VACATION:
                    {
                        current_house_mode = &ez_mode->mode_home;
                        break;
                    }
                    default:
                    {
                        TRACE_E("Undefined house-mode!");
                        break;
                    }
                    }

                    if (current_house_mode)
                    {
                        if (current_house_mode->cj_bypass_devices)
                        {
                            cJSON_Delete(current_house_mode->cj_bypass_devices);
                            current_house_mode->cj_bypass_devices = NULL;
                        }

                        TRACE_D("Switching to house-mode: '%s'", current_house_mode->name);
                        ezlopi_core_modes_set_current_house_mode(current_house_mode);
#warning "broadcast - mode-changed here"
                    }
                }
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}