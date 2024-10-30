#ifndef __EZLOPI_SERVICE_OTEL_H__
#define __EZLOPI_SERVICE_OTEL_H__

typedef enum
{
    EZPI_OTEL_TELEMETRY_NONE = 0,
    EZPI_OTEL_TELEMETRY_LOG,
    EZPI_OTEL_TELEMETRY_TRACE,
    EZPI_OTEL_TELEMETRY_MATRICS,
    EZPI_OTEL_TELEMETRY_MAX
} e_ezlopi_otel_telemetry_type_t;

void ezlopi_service_otel_init(void);

#endif // __EZLOPI_SERVICE_OTEL_H__
