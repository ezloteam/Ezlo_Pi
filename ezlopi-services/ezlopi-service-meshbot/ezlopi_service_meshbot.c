#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_scenes_status_changed.h"
#include "ezlopi_core_scenes_v2.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_meshbot.h"
#include "pt.h"

typedef struct s_thread_ctx {
    struct pt pt;
    uint32_t curr_ticks;
    uint32_t start_cond;
    uint32_t stopped_cond;
    uint32_t delay_ms;
    l_action_block_v2_t* action_node;
} s_thread_ctx_t;

///////////// Static functions /////////////////////
static void __scenes_thread_process(void* pv);
static char __scene_proto_thread(l_scenes_list_v2_t* scene_node, uint32_t routine_delay_ms);

static int __execute_scene_stop(l_scenes_list_v2_t* scene_node);
static int __execute_scene_start(l_scenes_list_v2_t* scene_node);
static int __execute_action_block(l_scenes_list_v2_t* scene_node, l_action_block_v2_t* action_block);

////////// Global functions ///////////////////
uint32_t ezlopi_meshbot_service_stop_for_scene_id(uint32_t _id)
{
    uint32_t ret = 0;
    l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_by_id_v2(_id);
    if (scene_node)
    {
        if (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status)
        {
            scene_node->status = EZLOPI_SCENE_STATUS_STOP;
        }

        while (EZLOPI_SCENE_STATUS_STOPPED != scene_node->status)
        {
            vTaskDelay(50 / portTICK_RATE_MS);
        }

        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_stopped_str);

        ret = 1;
    }

    return ret;
}

uint32_t ezlopi_meshobot_service_stop_scene(l_scenes_list_v2_t* scene_node)
{
    int ret = 0;
    if (scene_node)
    {
        if (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status)
        {
            scene_node->status = EZLOPI_SCENE_STATUS_STOP;
        }

        while (EZLOPI_SCENE_STATUS_STOPPED != scene_node->status)
        {
            vTaskDelay(50 / portTICK_RATE_MS);
        }

        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_stopped_str);

        ret = 1;
    }
    return ret;
}

uint32_t ezlopi_meshbot_service_start_scene(l_scenes_list_v2_t* scene_node)
{
    int ret = 0;
    if (scene_node)
    {
        if ((EZLOPI_SCENE_STATUS_NONE == scene_node->status) ||
            (EZLOPI_SCENE_STATUS_STOPPED == scene_node->status))
        {
            // xTaskCreate(__scenes_process, scene_node->name, 2 * 2048, scene_node, 2, NULL);
            ret = 1;
        }
    }
    return ret;
}

uint32_t ezlopi_scenes_service_run_by_id(uint32_t _id)
{
    uint32_t ret = 0;
    TRACE_D("Scene-id: %d", _id);
    l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_by_id_v2(_id);

    if (scene_node)
    {
        if (__execute_scene_stop(scene_node))
        {
            if (scene_node->then_block)
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_started_str);

                if (1 == __execute_action_block(scene_node, scene_node->then_block))
                {
                    ezlopi_scenes_status_change_broadcast(scene_node, scene_status_finished_str);
                }
                else
                {
                    ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
                }
            }
            else if (scene_node->else_block)
            {
                if (1 == __execute_action_block(scene_node, scene_node->else_block))
                {
                    ezlopi_scenes_status_change_broadcast(scene_node, scene_status_finished_str);
                }
                else
                {
                    ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
                }
            }
            else
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
            }

            vTaskDelay(10 / portTICK_RATE_MS);
            __execute_scene_start(scene_node);
        }
        else
        {
            ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
        }
    }

    return ret;
}

uint32_t ezlopi_meshbot_execute_scene_else_action_group(uint32_t scene_id)
{
    int ret = 0;
    l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_by_id_v2(scene_id);
    if (scene_node)
    {
        if (scene_node->else_block)
        {
            ezlopi_scenes_status_change_broadcast(scene_node, scene_status_started_str);

            if (1 == __execute_action_block(scene_node, scene_node->else_block))
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_finished_str);
            }
            else
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
                ret = 1;
            }
        }
    }
    else
    {
        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
        ret = 1;
    }

    return ret;
}

void ezlopi_scenes_meshbot_init(void)
{
    uint32_t start_thread = 0;
    l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_scenes_head_v2();
    while (scene_node)
    {
        scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;
        // if (scene_node->enabled && scene_node->when_block && (scene_node->else_block || scene_node->then_block))
        if (scene_node->when_block && (scene_node->else_block || scene_node->then_block))
        {
            start_thread = 1;

            s_thread_ctx_t* thread_ctx = malloc(sizeof(s_thread_ctx_t));
            if (thread_ctx)
            {
                memset(thread_ctx, 0, sizeof(s_thread_ctx_t));
                PT_INIT(&thread_ctx->pt);
                scene_node->thread_ctx = (void*)thread_ctx;
                scene_node->status = EZLOPI_SCENE_STATUS_RUN;
            }
        }
        else
        {
            scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;
        }

        scene_node = scene_node->next;
    }

    if (start_thread)
    {
        TRACE_D("starting thread process");
        xTaskCreate(__scenes_thread_process, "Scene-task", 2 * 3048, NULL, 2, NULL);
    }
    else
    {
        TRACE_E("scene not available!");
    }
}

PT_THREAD(__scene_proto_thread(l_scenes_list_v2_t* scene_node, uint32_t routine_delay_ms))
{
    s_thread_ctx_t* ctx = scene_node->thread_ctx;
    PT_BEGIN(&ctx->pt);

    while (1)
    {
        if ((EZLOPI_SCENE_STATUS_RUN == scene_node->status) || (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status))
        {
            scene_node->status = EZLOPI_SCENE_STATUS_RUNNING;

            uint32_t when_condition_returned = 0;
            l_when_block_v2_t* when_condition_node = scene_node->when_block;

            if (when_condition_node)
            {
                f_scene_method_v2_t when_method = ezlopi_scene_get_method_v2(when_condition_node->block_options.method.type);
                if (when_method)
                {
                    when_condition_returned = when_method(scene_node, (void*)when_condition_node);
                    if (when_condition_returned)
                    {
                        if (ctx->start_cond < 2)
                        {
                            ctx->stopped_cond = 0;

                            if (ctx->start_cond)
                            {
                                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_started_str);
                            }

                            l_action_block_v2_t* then_block_node = scene_node->then_block;
                            while (then_block_node)
                            {
                                uint32_t delay_ms = (then_block_node->delay.days * (24 * 60 * 60)
                                    + then_block_node->delay.hours * (60 * 60)
                                    + then_block_node->delay.minutes * 60
                                    + then_block_node->delay.seconds) * 1000;

                                if (delay_ms)
                                {
                                    ctx->delay_ms = delay_ms;
                                    ctx->action_node = then_block_node;

                                    ctx->curr_ticks = xTaskGetTickCount();
                                    TRACE_D("entering delay: %d", ctx->curr_ticks);
                                    PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > ctx->delay_ms);
                                    TRACE_D("exiting delay: %d", xTaskGetTickCount());

                                    then_block_node = ctx->action_node;
                                }

                                f_scene_method_v2_t then_method = ezlopi_scene_get_method_v2(then_block_node->block_options.method.type);
                                TRACE_D("then-method: %p", then_method);
                                if (then_method)
                                {
                                    then_method(scene_node, (void*)then_block_node); //then method executed here

                                    if (then_block_node->next)
                                    {
                                        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_partially_finished_str);
                                    }
                                    else
                                    {
                                        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_finished_str);
                                    }

                                    ctx->start_cond += 1;
                                }
                                else
                                {
                                    ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
                                }

                                ctx->delay_ms = 10;
                                ctx->action_node = then_block_node;

                                ctx->curr_ticks = xTaskGetTickCount();
                                TRACE_D("entering delay: %d", ctx->curr_ticks);
                                PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > ctx->delay_ms);
                                TRACE_D("exiting delay: %d", xTaskGetTickCount());

                                then_block_node = ctx->action_node;

                                then_block_node = then_block_node->next;
                            }
                        }
                        else
                        {
                            TRACE_D("Meshobot '%s' is Idle.", scene_node->name);
                        }
                    }
                    else if (ctx->stopped_cond < 2)
                    {
                        l_action_block_v2_t* else_block_node = scene_node->else_block;
                        while (else_block_node)
                        {
                            uint32_t delay_ms = (else_block_node->delay.days * (24 * 60 * 60)
                                + else_block_node->delay.hours * (60 * 60)
                                + else_block_node->delay.minutes * 60
                                + else_block_node->delay.seconds) * 1000;

                            if (delay_ms)
                            {
                                ctx->delay_ms = delay_ms;
                                ctx->action_node = else_block_node;

                                ctx->curr_ticks = xTaskGetTickCount();
                                TRACE_D("entering delay: %d", ctx->curr_ticks);
                                PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > ctx->delay_ms);
                                TRACE_D("exiting delay: %d", xTaskGetTickCount());

                                else_block_node = ctx->action_node;
                            }

                            f_scene_method_v2_t else_method = ezlopi_scene_get_method_v2(else_block_node->block_options.method.type);
                            TRACE_D("else-method: %p", else_method);
                            if (else_method)
                            {
                                else_method(scene_node, (void*)else_block_node);
                            }

                            ctx->delay_ms = 10;
                            ctx->action_node = else_block_node;

                            ctx->curr_ticks = xTaskGetTickCount();
                            TRACE_D("entering delay: %d", ctx->curr_ticks);
                            PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > ctx->delay_ms);
                            TRACE_D("exiting delay: %d", xTaskGetTickCount());

                            else_block_node = ctx->action_node;

                            else_block_node = else_block_node->next;
                        }

                        ezlopi_scenes_status_change_broadcast(scene_node, scene_status_stopped_str);

                        ctx->stopped_cond += 1;
                        ctx->start_cond = 0;

                    }
                    else
                    {
                        TRACE_D("Meshobot '%s' is Idle.", scene_node->name);
                    }
                }
                else
                {
                    TRACE_E("method not found");
                }

                when_condition_node = when_condition_node->next;
            }
        }

        if (EZLOPI_SCENE_STATUS_STOP == scene_node->status)
        {
            ezlopi_scenes_status_change_broadcast(scene_node, scene_status_stopped_str);
            free(scene_node->thread_ctx);
            scene_node->thread_ctx = NULL;
            scene_node->status = EZLOPI_SCENE_STATUS_STOPPED;
            break;
        }

        ctx->curr_ticks = xTaskGetTickCount();
        TRACE_D("entering delay: %d", ctx->curr_ticks);
        PT_WAIT_UNTIL(&ctx->pt, (xTaskGetTickCount() - ctx->curr_ticks) > routine_delay_ms);
        TRACE_D("exiting delay: %d", xTaskGetTickCount());
    }

    PT_END(&ctx->pt);
}

static void __scenes_thread_process(void* pv)
{
    while (1)
    {
        l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_scenes_head_v2();
        while (scene_node)
        {
            if (scene_node->thread_ctx)
            {
                __scene_proto_thread(scene_node, 1000); // 
            }

            vTaskDelay(100);
            scene_node = scene_node->next;
        }
    }
}

static int __execute_scene_stop(l_scenes_list_v2_t* scene_node)
{
    int ret = 0;
    if (scene_node)
    {
        if ((EZLOPI_SCENE_STATUS_RUN == scene_node->status) || (EZLOPI_SCENE_STATUS_RUNNING == scene_node->status))
        {
            scene_node->status = EZLOPI_SCENE_STATUS_STOP;
        }

        uint32_t loop_count = 100;

        while (EZLOPI_SCENE_STATUS_STOPPED != scene_node->status && loop_count--)
        {
            vTaskDelay(50 / portTICK_RATE_MS);
        }

        if (EZLOPI_SCENE_STATUS_STOPPED == scene_node->status)
        {
            ret = 1;
        }
    }

    return ret;
}

static int __execute_scene_start(l_scenes_list_v2_t* scene_node)
{
    int ret = 0;
    if (scene_node && (NULL == scene_node->thread_ctx))
    {
        scene_node->thread_ctx = (void*)malloc(sizeof(s_thread_ctx_t));
        if (scene_node->thread_ctx)
        {
            memset(scene_node->thread_ctx, 0, sizeof(s_thread_ctx_t));
            scene_node->status = EZLOPI_SCENE_STATUS_RUN;
            ret = 1;
        }
    }

    return ret;
}

static int __execute_action_block(l_scenes_list_v2_t* scene_node, l_action_block_v2_t* action_block)
{
    int ret = 0;
    while (action_block)
    {
        uint32_t delay_ms = (action_block->delay.days * (24 * 60 * 60)
            + action_block->delay.hours * (60 * 60)
            + action_block->delay.minutes * 60
            + action_block->delay.seconds) * 1000;

        if (delay_ms)
        {
            TRACE_D("scene-delay_ms: %d", delay_ms);
            vTaskDelay(delay_ms / portTICK_RATE_MS);
        }

        f_scene_method_v2_t action_method = ezlopi_scene_get_method_v2(action_block->block_options.method.type);
        TRACE_D("action-method: %p", action_method);
        
        if (action_method)
        {
            action_method(scene_node, (void*)action_block);
            ret = 1;
        }

        if (NULL != action_block->next) // ((SCENE_BLOCK_TYPE_THEN == action_block->block_type))
        {
            ezlopi_scenes_status_change_broadcast(scene_node, scene_status_partially_finished_str);
        }

        action_block = action_block->next;
    }

    return ret;
}
