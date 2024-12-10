#ifndef __EZLOPI_SERVICE_OTEL_H__
#define __EZLOPI_SERVICE_OTEL_H__

typedef enum e_otel_type
{
    E_OTEL_NONE = 0,
    E_OTEL_LOGS,
    E_OTEL_TRACES,
    E_OTEL_MATRICS,
    E_OTEL_MAX,
} e_otel_type_t;

void ezlopi_service_otel_init(void);
int ezlopi_service_otel_add_trace_to_telemetry_queue(cJSON *cj_trace);

#endif // __EZLOPI_SERVICE_OTEL_H__
