#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"

#include "ezlopi_scenes.h"
#include "ezlopi_scenes_service.h"

static void __scenes_process(void *arg);

uint32_t ezlopi_scenes_service_stop_for_scene_id(uint32_t _id)
{
    uint32_t ret = 0;
    l_scenes_list_t *curr_scene = ezlopi_scenes_get_scenes_list();
    while (curr_scene)
    {
        if (_id == curr_scene->_id)
        {
            curr_scene->enabled = 0;
            curr_scene->status = EZLOPI_SCENE_STATUS_STOP;
            ret = 1;
        }
        curr_scene = curr_scene->next;
    }

    return ret;
}

uint32_t ezlopi_scenes_service_pause_for_scene_id(uint32_t _id)
{
    uint32_t ret = 0;
    l_scenes_list_t *curr_scene = ezlopi_scenes_get_scenes_list();
    while (curr_scene)
    {
        if (_id == curr_scene->_id)
        {
            curr_scene->enabled = 0;
            curr_scene->status = EZLOPI_SCENE_STATUS_PAUSE;
            ret = 1;
        }
        curr_scene = curr_scene->next;
    }

    return ret;
}

uint32_t ezlopi_scenes_service_start_for_scene_id(uint32_t _id)
{
    uint32_t ret = 0;
    l_scenes_list_t *curr_scene = ezlopi_scenes_get_scenes_list();
    while (curr_scene)
    {
        if (_id == curr_scene->_id)
        {
            curr_scene->enabled = 0;
            curr_scene->status = EZLOPI_SCENE_STATUS_RUN;
            ret = 1;
        }
        curr_scene = curr_scene->next;
    }

    return ret;
}

void ezlopi_scenes_service_reinit(void)
{

    l_scenes_list_t *curr_scene = ezlopi_scenes_get_scenes_list();
    while (curr_scene)
    {
        curr_scene->status = EZLOPI_SCENE_STATUS_STOP;
        vTaskDelay(20);

        if (curr_scene->enabled)
        {
            xTaskCreate(__scenes_process, curr_scene->name, 2 * 2048, curr_scene, 2, NULL);
        }
        curr_scene = curr_scene->next;
    }
}

void ezlopi_scenes_service_init(void)
{
    l_scenes_list_t *curr_scene = ezlopi_scenes_get_scenes_list();
    while (curr_scene)
    {
        if (curr_scene->enabled)
        {
            xTaskCreate(__scenes_process, curr_scene->name, 2 * 2048, curr_scene, 2, NULL);
        }
        curr_scene = curr_scene->next;
    }
}

static void __scenes_process(void *arg)
{
    l_scenes_list_t *scene = (l_scenes_list_t *)arg;
    scene->status = EZLOPI_SCENE_STATUS_RUN;
    TRACE_B("task - '%s': Running", scene->name);

    while (1)
    {
        if (EZLOPI_SCENE_STATUS_RUN == scene->status)
        {
            l_when_block_t *curr_when = scene->when;
            while (curr_when)
            {
                f_scene_method_t when_method = ezlopi_scene_get_method(curr_when->block_options.method.type);
                if (when_method)
                {
                    int ret = when_method(scene, (void *)curr_when);
                    if (ret)
                    {
                        TRACE_B("When condition is true.");
                        l_then_block_t *curr_then = scene->then;
                        while (curr_then)
                        {
                            uint32_t delay_ms = (curr_then->delay.days * (24 * 60 * 60) + curr_then->delay.hours * (60 * 60) + curr_then->delay.minutes * 60 + curr_then->delay.seconds) * 1000;
                            if (delay_ms)
                            {
                                vTaskDelay(delay_ms / portTICK_RATE_MS);
                            }

                            TRACE_D("delay_ms: %d", delay_ms);
                            const char *method_name = ezlopi_scene_get_scene_method_name(curr_then->block_options.method.type);
                            if (method_name)
                            {
                                TRACE_D("Calling: %s", method_name);
                            }
                            else
                            {
                                TRACE_E("Error: Method is NULL!");
                            }

                            f_scene_method_t then_method = ezlopi_scene_get_method(curr_then->block_options.method.type);
                            if (then_method)
                            {
                                ret = then_method(scene, (void *)curr_then);
                            }

                            curr_then = curr_then->next;

                            if (EZLOPI_SCENE_STATUS_STOP == scene->status)
                            {
                                break;
                            }
                        }
                    }
                }

                if (EZLOPI_SCENE_STATUS_STOP == scene->status)
                {
                    break;
                }
                curr_when = curr_when->next;
            }

            if (EZLOPI_SCENE_STATUS_STOP == scene->status)
            {
                break;
            }
        }

        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    scene->enabled = false;
    scene->status = EZLOPI_SCENE_STATUS_STOP;
    vTaskDelete(NULL);
}
