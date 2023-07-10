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
        vTaskDelay(100);
    }
}
