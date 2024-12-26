#ifndef __EZLOPI_SERVICE_OTEL_H__
#define __EZLOPI_SERVICE_OTEL_H__

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_ENABLE_OPENTELEMETRY
#include "ezlopi_core_log.h"
#include "ezlopi_core_api_methods.h"

typedef enum e_otel_type
{
    E_OTEL_NONE = 0,
    E_OTEL_LOGS,
    E_OTEL_TRACES,
    E_OTEL_TRACES_STRUCT,
    E_OTEL_MATRICS,
    E_OTEL_MAX,
} e_otel_type_t;

typedef enum e_otel_kind
{
    E_OTEL_KIND_INTERNAL = 0, // e.g.: meshbot, etc.
    E_OTEL_KIND_SERVER = 1,   // e.g.: websocket server, uart, ble, etc.
    E_OTEL_KIND_CLIENT = 2,   // e.g.: web-provisioning, http requests, etc.
    E_OTEL_KIND_PRODUCER = 3,
    E_OTEL_KIND_CONSUMER = 4
} e_otel_kind_t;

typedef enum e_attr_value_type
{
    E_ATTR_TYPE_NONE = 0,
    E_ATTR_TYPE_BOOL,
    E_ATTR_TYPE_STRING,
    E_ATTR_TYPE_STRING_CONST,
    E_ATTR_TYPE_NUMBER
} e_attr_value_type_t;

typedef struct s_otel_attr
{
    const char *key;
    union
    {
        char *string;
        double number;
    } value;

    e_attr_value_type_t type;
    struct s_otel_attr *next;
} s_otel_attr_t;

typedef struct s_otel_trace
{
    const char *name;
    char *id;
    char *method;
    char *msg_subclass;
    time_t start_time;
    time_t end_time;
    uint32_t free_heap;
    uint32_t heap_watermark;
    e_otel_kind_t kind;

    s_otel_attr_t *attributes;

} s_otel_trace_t;

void ezlopi_service_otel_init(void);
// int ezlopi_service_otel_add_trace_to_telemetry_queue(cJSON *cj_trace);
int ezlopi_service_otel_add_trace_to_telemetry_queue_struct(s_otel_trace_t *trace_obj);

#endif // CONFIG_EZPI_ENABLE_OPENTELEMETRY

#endif // __EZLOPI_SERVICE_OTEL_H__
