#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"

#include "ezlopi_scenes.h"
#include "ezlopi_scenes_service.h"

static void __scenes_process(void *arg);

void ezlopi_scenes_service_init(void)
{
    l_scenes_list_t *curr_scene = ezlopi_scenes_get_scenes_list();
    while (curr_scene)
    {
        xTaskCreate(__scenes_process, curr_scene->name, 2 * 2048, curr_scene, 2, NULL);
        curr_scene = curr_scene->next;
    }
}

static void __scenes_process(void *arg)
{
    l_scenes_list_t *scene = (l_scenes_list_t *)arg;
    TRACE_B("task - '%s': Running", scene->name);

    while (1)
    {
        f_scene_method_t when_method = ezlopi_scene_get_method(scene->when->block_options.method.type);
        if (when_method)
        {
            int ret = when_method(scene);
            if (ret)
            {
                l_then_block_t *curr_then = scene->then;
                while (curr_then)
                {
                    uint32_t delay_ms = (curr_then->delay.days * (24 * 60 * 60) + curr_then->delay.hours * (60 * 60) + curr_then->delay.minutes * 60 + curr_then->delay.seconds) * 1000;
                    TRACE_D("delay_ms: %d", delay_ms);
                    if (delay_ms)
                    {
                        vTaskDelay(delay_ms / portTICK_RATE_MS);
                    }

                    f_scene_method_t then_method = ezlopi_scene_get_method(curr_then->block_options.method.type);
                    if (then_method)
                    {
                        ret = then_method(scene);
                    }

                    curr_then = curr_then->next;
                }
            }
        }

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
