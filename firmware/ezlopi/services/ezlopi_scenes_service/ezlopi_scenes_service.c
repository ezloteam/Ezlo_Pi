#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"

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
        switch (when_block->block_options.method.arg_type)
        {
        case EZLOPI_SCENE_ARG_TYPE_DEVICE:
        {
            break;
        }
        case EZLOPI_SCENE_ARG_TYPE_HTTP_REQUEST:
        {
            break;
        }
        case EZLOPI_SCENE_ARG_TYPE_HOUSE_MODE:
        {
            break;
        }
        case EZLOPI_SCENE_ARG_TYPE_LUA_SCRIPT:
        {
            break;
        }
        default:
        {
            TRACE_E("Arg type not defined!");
            break;
        }
        }

        if (ret)
        {
            break;
        }

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
            if (__check_when_block(scenes_list->when))
            {
                __execute_then_block(scenes_list->then);
            }
            scenes_list = scenes_list->next;
        }
    }
}
