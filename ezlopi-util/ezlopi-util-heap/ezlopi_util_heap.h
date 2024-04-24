#ifndef __EZLOPI_UTIL_HEAP_TRACE_H__
#define __EZLOPI_UTIL_HEAP_TRACE_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>

void ezlopi_util_heap_flush(void);
void ezlopi_util_heap_trace(void);

void ezlopi_util_heap_free(void *ptr, const char * __file_name, uint32_t line_number);
void* ezlopi_util_heap_malloc(uint32_t size, const char * file_name, uint32_t line_no);
void* ezlopi_util_heap_calloc(uint32_t count, uint32_t size, const char * file_name, uint32_t line_no);
void* ezlopi_util_heap_realloc(void *ptr, uint32_t new_size, const char * file_name, uint32_t line_no);

#endif // __EZLOPI_UTIL_HEAP_TRACE_H__
