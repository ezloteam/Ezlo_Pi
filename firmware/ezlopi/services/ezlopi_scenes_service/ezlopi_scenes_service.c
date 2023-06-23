#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_scenes.h"
#include "ezlopi_scenes_service.h"

static void __scenes_process(void *pv);

void ezlopi_scenes_service_init(void)
{
    xTaskCreate(__scenes_process, "Scenes Process", 2048, NULL, 2, NULL);
}

static int __execute_then_block(l_then_block_t *then_block)
{
    int ret = 0;
    while (then_block)
    {

        then_block = then_block->next;
    }

    return ret;
}

static int __check_when_block(l_when_block_t *when_block)
{
    int ret = 0;
    while (when_block)
    {

        when_block = when_block->next;
    }
    return ret;
}

static void __scenes_process(void *pv)
{
    while (1)
    {
        l_scenes_list_t *scenes_list = ezlopi_scenes_get_scenes_list();

        if (scenes_list)
        {
            if (__check_when_block())
            {
                __execute_then_block();
            }
            scenes_list = scenes_list->next;
        }
    }
}
