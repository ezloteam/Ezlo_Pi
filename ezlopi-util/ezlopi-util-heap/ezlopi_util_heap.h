#ifndef __EZLOPI_UTIL_HEAP_TRACE_H__
#define __EZLOPI_UTIL_HEAP_TRACE_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>

void ezlopi_util_heap_flush(void);
void ezlopi_util_heap_trace(void);

void ezlopi_util_heap_free(const char * who, void *ptr, const char * __file_name, uint32_t line_number);
void* ezlopi_util_heap_malloc(const char * who, size_t size, const char * file_name, uint32_t line_no);
void* ezlopi_util_heap_calloc(const char * who, size_t count, size_t size, const char * file_name, uint32_t line_no);
void* ezlopi_util_heap_realloc(const char * who, void *ptr, size_t new_size, const char * file_name, uint32_t line_no);

#endif // __EZLOPI_UTIL_HEAP_TRACE_H__
