#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_heap.h"

typedef struct s_initiator {
    const char * who;
    const char * file_name;
    uint32_t line_number;
} s_initiator_t;

typedef struct s_heap_trace {
    void* ptr;
    bool freed;
    uint32_t size;
    uint32_t time_ms;
    s_initiator_t freer;
    s_initiator_t allocator;

    struct s_heap_trace* next;

} s_heap_trace_t;


static s_heap_trace_t* heap_head = NULL;

static s_heap_trace_t * __create_list(void);
static void __remove_free_node(s_heap_trace_t * heap_node);

void ezlopi_util_heap_free(const char * who, void *ptr, const char * __file_name, uint32_t line_number)
{
    if (ptr)
    {
        s_heap_trace_t* curr_node = heap_head;
        while (curr_node)
        {
            if ((curr_node->ptr == ptr) && (0 == curr_node->freed))
            {
                // printf("\x1B[34m*** freed at %s:%u, size: %u, ptr: %u ***\x1B[0m\r\n", __file_name, line_number, curr_node->size, (uint32_t)curr_node->ptr);
                free(curr_node->ptr);

                curr_node->freed = true;
                curr_node->time_ms = (xTaskGetTickCount() - curr_node->time_ms);
                curr_node->freer.file_name = __file_name;
                curr_node->freer.line_number = line_number;
                curr_node->freer.who = who;
                break;
            }
            curr_node = curr_node->next;
        }

    }
}

void* ezlopi_util_heap_malloc(const char * who, size_t size, const char * file_name, uint32_t line_no)
{
    void* ret = NULL;

    if (size)
    {
        s_heap_trace_t * new_heap = __create_list();
        if (new_heap)
        {
            // \x1B[%sm %s[%d]:" X "\x1B[0m\r\n"
            ret = malloc(size);
            if (ret)
            {
                new_heap->ptr = ret;
                new_heap->size = size;
                new_heap->allocator.file_name = file_name;
                new_heap->allocator.line_number = line_no;
                new_heap->time_ms = xTaskGetTickCount();
                new_heap->allocator.who = who;
            }

            // printf("\x1B[34m### malloc at %s:%u, size: %u, ptr: %u ###\x1B[0m\r\n", file_name, line_no, size, (uint32_t)ret);
        }
    }

    return ret;
}

void* ezlopi_util_heap_calloc(const char * who, size_t count, size_t size, const char * file_name, uint32_t line_no)
{
    return ezlopi_util_heap_malloc(who, (count * size), file_name, line_no);
}

void* ezlopi_util_heap_realloc(const char * who, void *ptr, size_t new_size, const char * file_name, uint32_t line_no)
{
    ezlopi_util_heap_free(who, ptr, file_name, line_no);
    return ezlopi_util_heap_malloc(who, new_size, file_name, line_no);
}

void ezlopi_util_heap_flush(void)
{
    __remove_free_node(heap_head);

    if (heap_head->freed)
    {
        s_heap_trace_t * free_node = heap_head;
        heap_head = heap_head->next;
        free(free_node);
    }
}

void ezlopi_util_heap_trace(bool print_freed)
{
    s_heap_trace_t * curr_node = heap_head;

    printf("\r\n\r\n**************************************************************************\r\n");
    printf("****************************** CURRENT HEAP ******************************\r\n");

    uint32_t count = 0;
    uint32_t total_allocated_memory = 0;

    while (curr_node)
    {
        if (curr_node->freed && print_freed)
        {
            printf("%d. --\r\n", count);
            printf("%s(%d):: ptr: %u, size %u, hold-time-ms: %u\r\n", curr_node->allocator.file_name, curr_node->allocator.line_number, (uint32_t)curr_node->ptr, curr_node->size, curr_node->time_ms);

            if (curr_node->freer.file_name && curr_node->freer.who && curr_node->allocator.who)
            {
                printf("%s(%d):: freed, allocator: %s, freerer: %s\r\n", curr_node->freer.file_name, curr_node->freer.line_number, curr_node->allocator.who ? curr_node->allocator.who : "", curr_node->freer.who ? curr_node->freer.who : "");
            }
            else
            {
                printf("\x1B[31m %s[%d]:: Error -> allocator.file_name: %p, allocator.who: %p, freer.who: %p \x1B[0m\r\n\r\n", curr_node->freer.file_name, curr_node->freer.line_number,
                    curr_node->freer.file_name, curr_node->allocator.who, curr_node->freer.who);
                assert(0);
            }
        }
        else
        {
            total_allocated_memory += curr_node->size;
            printf("\033[38:2:255:165:0m%d. --\x1B[0m\r\n", count);
            printf("\033[38:2:255:165:0m%d -> %s(%d):: allocator: %s\r\n", count, curr_node->allocator.file_name, curr_node->allocator.line_number, curr_node->allocator.who ? curr_node->allocator.who : "");
            printf("\033[38:2:255:165:0m%d -> %s(%d):: ptr: %u, size %u, hold-time-ms: %u\x1B[0m\r\n", count, curr_node->allocator.file_name, curr_node->allocator.line_number, (uint32_t)curr_node->ptr, curr_node->size, (xTaskGetTickCount() - curr_node->time_ms));
        }

        count++;
        curr_node = curr_node->next;

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    printf("--------> total allocated ram: %u\r\n", total_allocated_memory);
    printf("**************************************************************************\r\n\r\n\r\n");
}

/// @brief ///////////////
/// @param  
/// @return 
static s_heap_trace_t * __create_list(void)
{
    s_heap_trace_t* new_node = (s_heap_trace_t*)malloc(sizeof(s_heap_trace_t));

    if (new_node)
    {
        memset(new_node, 0, sizeof(s_heap_trace_t));

        if (heap_head)
        {
            s_heap_trace_t* curr_node = heap_head;
            while (curr_node->next)
            {
                curr_node = curr_node->next;
            }

            curr_node->next = new_node;
        }
        else
        {
            heap_head = new_node;
        }
    }

    return new_node;
}


static void __remove_free_node(s_heap_trace_t * heap_node)
{
    if (heap_node->next)
    {
        __remove_free_node(heap_node->next);

        if (heap_node->next->freed)
        {
            s_heap_trace_t * free_node = heap_node->next;
            heap_node->next = heap_node->next->next;
            free(free_node);
        }
    }
}