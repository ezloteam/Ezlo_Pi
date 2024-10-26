#include "ezlopi_util_trace.h"

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "ezlopi_core_sntp.h"

#if (1 == CONFIG_EZPI_UTIL_TRACE_EN)

static const char *trace_name_str[] = {
    "NONE",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TRACE",
    "MAX",
};

typedef struct ll_trace_upcall
{
    f_trace_func_t upcall;
    struct ll_trace_upcall *next;
} ll_trace_upcall_t;

static ll_trace_upcall_t *trace_upcall_head = NULL;

static f_ezlopi_log_upcall_t cloud_log_upcall_func = NULL;
static f_ezlopi_log_upcall_t serial_log_upcall_func = NULL;

static void put_idump(uint8_t *buff, uint32_t ofs, uint32_t cnt);

const char *ezlopi_util_trace_get_severity_name_str(e_ezpi_trace_severity_t severity)
{
    const char *ret = trace_name_str[0];
    if (severity > E_TRACE_SEVERITY_NONE && severity < E_TRACE_SEVERITY_MAX)
    {
        ret = trace_name_str[severity];
    }
    return ret;
}

void ezlopi_util_trace_add_upcall(f_trace_func_t upcall)
{
    if (upcall)
    {
        if (trace_upcall_head)
        {
            ll_trace_upcall_t *node = trace_upcall_head;
            while (node->next)
            {
                node = node->next;
            }

            ll_trace_upcall_t *new_node = ezlopi_malloc(__FUNCTION__, sizeof(ll_trace_upcall_t));
            if (new_node)
            {
                new_node->next = NULL;
                new_node->upcall = upcall;
                node->next = new_node;
            }
        }
        else
        {
            trace_upcall_head = ezlopi_malloc(__FUNCTION__, sizeof(ll_trace_upcall_t));
            // trace_upcall_head = malloc(sizeof(ll_trace_upcall_t));
            if (trace_upcall_head)
            {
                trace_upcall_head->next = NULL;
                trace_upcall_head->upcall = upcall;
            }
        }
    }
}

void __dump(const char *file_name, uint32_t line, char *buffer_name, void *_buff, uint32_t ofs, uint32_t cnt)
{
    unsigned char *buff = _buff;
    int lines = cnt >> 4;
    int l;

    ets_printf("%s (%d):: %s: Total Size: %d\n", file_name, line, buffer_name, cnt);

    for (l = 0; l < lines; l++)
    {
        put_idump(&buff[l * 16], ofs + l * 16, 16);
    }

    if (cnt & 0x0F)
    {
        put_idump(&buff[l * 16], ofs + l * 16, cnt & 0x0F);
    }

    ets_printf("\n");
    fflush(stdout);
}

void ezlopi_util_set_log_upcalls(f_ezlopi_log_upcall_t cloud_log_upcall, f_ezlopi_log_upcall_t serial_log_upcall)
{
    if (cloud_log_upcall)
    {
        cloud_log_upcall_func = cloud_log_upcall;
    }

    if (serial_log_upcall)
    {
        serial_log_upcall_func = serial_log_upcall;
    }
}

f_ezlopi_log_upcall_t ezlopi_util_get_cloud_log_upcall()
{
    return cloud_log_upcall_func;
}

f_ezlopi_log_upcall_t ezlopi_util_get_serial_log_upcall()
{
    return serial_log_upcall_func;
}

void trace_color_print(const char *txt_color, uint8_t severity, const char *file, int line, const char *format, ...)
{
#if 0
    f_ezlopi_log_upcall_t log_upcall_func;

    log_upcall_func = ezlopi_util_get_serial_log_upcall();
    if (log_upcall_func != NULL)
    {
        va_list args;
        va_start(args, format);

#warning "No remedies for buffer over 'EZPI_CORE_LOG_BUFFER_SIZE'";

        static char serial_log_format[10240];
        // char serial_log_format[EZPI_CORE_LOG_BUFFER_SIZE];
        snprintf(serial_log_format, sizeof(serial_log_format), "\x1B[%sm %s[%d]: ", txt_color, file, line);
        vsnprintf(serial_log_format + strlen(serial_log_format), sizeof(serial_log_format) - strlen(serial_log_format), format, args);
        snprintf(serial_log_format + strlen(serial_log_format), sizeof(serial_log_format) - strlen(serial_log_format), "\x1B[0m");
        log_upcall_func(severity, serial_log_format);
        va_end(args);
#else
    va_list args;
    static char serial_log_format[10240];
    uint64_t time_now_ms = EZPI_CORE_sntp_get_current_time_ms();

    va_start(args, format);
    vsnprintf(serial_log_format, sizeof(serial_log_format), format, args);
    va_end(args);

    ll_trace_upcall_t *upcall_node = trace_upcall_head;
    while (upcall_node)
    {
        if (upcall_node->upcall)
        {
            upcall_node->upcall(severity, file, line, time_now_ms, serial_log_format);
        }

        upcall_node = upcall_node->next;
    }
#endif
}

static void put_idump(uint8_t *buff, uint32_t ofs, uint32_t cnt)
{
    int n;

    ets_printf("\n%08lX ", ofs);

    for (n = 0; n < cnt; n++)
    {
        ets_printf(" %02X", buff[n]);
    }

    if (cnt < 16)
    {
        do
        {
            ets_printf("   ");

        } while (++n < 16);
    }

    char temp_buff[17] = {0};

    memcpy(temp_buff, buff, cnt);
    temp_buff[16] = 0;

    ets_printf("%c%c", 0x09, 0x09);

    for (n = 0; n < cnt; n++)
    {
        if ((buff[n] < 0x20) || (buff[n] > 0x80))
        {
            ets_printf(".");
        }
        else
        {
            ets_printf("%c", buff[n]);
        }
    }
}

#endif // CONFIG_EZPI_UTIL_TRACE_EN