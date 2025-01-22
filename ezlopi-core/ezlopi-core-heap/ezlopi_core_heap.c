/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_heap.c
 * @brief   Function to operate on heap
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_core_heap.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
typedef struct s_initiator
{
    const char *who;
    const char *file_name;
    uint32_t line_number;
} s_initiator_t;

typedef struct s_heap_trace
{
    void *ptr;
    bool freed;
    uint32_t size;
    uint32_t time_ms;
    s_initiator_t freer;
    s_initiator_t allocator;
    struct s_heap_trace *next;
} s_heap_trace_t;
/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static s_heap_trace_t *__internal_malloc(const char *who, size_t size, const char *file_name, uint32_t line_no);
static s_heap_trace_t *__create_list(void);
static void __remove_free_node(s_heap_trace_t *heap_node);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static s_heap_trace_t *heap_head = NULL;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_core_util_heap_free(const char *who, void *ptr, const char *__file_name, uint32_t line_number)
{
    if (ptr)
    {
        s_heap_trace_t *curr_node = heap_head;
        while (curr_node)
        {
            if ((curr_node->ptr == ptr) && (0 == curr_node->freed))
            {
                // printf("\x1B[34m*** freed at %s:%u, size: %u, ptr: %u ***\x1B[0m\r\n", __file_name, line_number, curr_node->size, (uint32_t)curr_node->ptr);
                free(curr_node->ptr);

                curr_node->freed = true;
                curr_node->freer.who = who;
                curr_node->freer.file_name = __file_name;
                curr_node->freer.line_number = line_number;
                curr_node->time_ms = (xTaskGetTickCount() - curr_node->time_ms);
                break;
            }

            curr_node = curr_node->next;
        }
    }
}

void *EZPI_core_util_heap_malloc(const char *who, size_t size, const char *file_name, uint32_t line_no)
{
    void *ret = NULL;

    if (size > 0)
    {
        s_heap_trace_t *new_heap = __internal_malloc(who, size, file_name, line_no);
        if (NULL != new_heap->ptr)
        {
            ret = new_heap->ptr;
        }
        else
        {
            new_heap->freed = 1;
        }
    }

    return ret;
}

void *EZPI_core_util_heap_calloc(const char *who, size_t count, size_t size, const char *file_name, uint32_t line_no)
{
    return EZPI_core_util_heap_malloc(who, (count * size), file_name, line_no);
}

void *EZPI_core_util_heap_realloc(const char *who, void *ptr, size_t new_size, const char *file_name, uint32_t line_no)
{
    s_heap_trace_t *curr_node = heap_head;
    while (curr_node)
    {
        if (curr_node->ptr == ptr)
        {
            break;
        }
        curr_node = curr_node->next;
    }

    void *new_ptr = realloc(ptr, new_size);

    if (curr_node)
    {
        curr_node->freed = false;
        curr_node->ptr = new_ptr;
        curr_node->size = new_size;
        curr_node->allocator.file_name = file_name;
        curr_node->allocator.line_number = line_no;
    }
    else
    {
        s_heap_trace_t *new_node = __internal_malloc(who, 0, file_name, line_no);
        if (new_node)
        {
            new_node->ptr = new_ptr;
            new_node->size = new_size;
        }
    }

    return new_ptr;
}

void EZPI_core_util_heap_flush(void)
{
    __remove_free_node(heap_head);

    if (heap_head->freed)
    {
        s_heap_trace_t *free_node = heap_head;
        heap_head = heap_head->next;
        free(free_node);
    }
}

void EZPI_core_util_heap_trace(bool print_freed)
{
    s_heap_trace_t *curr_node = heap_head;

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
                printf("\x1B[31m %s[%d]:: Error -> allocator.file_name: %p, allocator.who: %p, freer.who: %p \x1B[0m\r\n\r\n",
                       curr_node->freer.file_name, curr_node->freer.line_number, curr_node->freer.file_name, curr_node->allocator.who, curr_node->freer.who);
                assert(0);
            }
        }
        else
        {
            total_allocated_memory += curr_node->size;
            printf("\033[38:2:255:165:0m%d -> %s(%d):: who: %s, p: 0x%08x, l: %u, h-s: %u\x1B[0m\r\n", count + 1, curr_node->allocator.file_name, curr_node->allocator.line_number,
                   (curr_node->allocator.who ? curr_node->allocator.who : ""), (uint32_t)curr_node->ptr, curr_node->size, ((xTaskGetTickCount() - curr_node->time_ms) / 1000));
        }

        count++;
        curr_node = curr_node->next;

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    static uint32_t heap_watermark = 0;
    heap_watermark = heap_watermark < total_allocated_memory ? total_allocated_memory : heap_watermark;
    printf("--------> max used-heap:   %u\r\n", heap_watermark);
    printf("--------> total used-heap: %u\r\n", total_allocated_memory);
    printf("--------> total free-heap: %u\r\n", esp_get_free_heap_size());
    printf("**************************************************************************\r\n\r\n\r\n");
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
static s_heap_trace_t *__internal_malloc(const char *who, size_t size, const char *file_name, uint32_t line_no)
{
    s_heap_trace_t *new_heap = __create_list();

    if (new_heap)
    {
        new_heap->allocator.who = who;
        new_heap->time_ms = xTaskGetTickCount();
        new_heap->allocator.file_name = file_name;
        new_heap->allocator.line_number = line_no;
        new_heap->ptr = (size > 0) ? malloc(size) : NULL;
        new_heap->size = (NULL != new_heap->ptr) ? size : 0;
    }

    return new_heap;
}

static s_heap_trace_t *__create_list(void)
{
    s_heap_trace_t *new_node = (s_heap_trace_t *)malloc(sizeof(s_heap_trace_t));

    if (new_node)
    {
        memset(new_node, 0, sizeof(s_heap_trace_t));

        if (heap_head)
        {
            s_heap_trace_t *curr_node = heap_head;
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

static void __remove_free_node(s_heap_trace_t *heap_node)
{
    if (heap_node->next)
    {
        vTaskDelay(1);
        __remove_free_node(heap_node->next);

        if (heap_node->next->freed)
        {
            s_heap_trace_t *free_node = heap_node->next;
            heap_node->next = heap_node->next->next;
            free(free_node);
        }
    }
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
