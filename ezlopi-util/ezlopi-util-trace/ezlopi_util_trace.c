#include "ezlopi_util_trace.h"

#include <stdio.h>
#include <string.h>
#include "esp_system.h"

#if (1 == ENABLE_TRACE)
static f_ezlopi_log_upcall_t cloud_log_upcall_func = NULL;
static f_ezlopi_log_upcall_t serial_log_upcall_func = NULL;

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

    f_ezlopi_log_upcall_t log_upcall_func;

#if 0
    f_ezlopi_log_upcall_t log_upcall_func = ezlopi_util_get_cloud_log_upcall();

    if (log_upcall_func != NULL) {
        va_list args;
        va_start(args, format);
        char cloud_log_format[EZPI_CORE_LOG_BUFFER_SIZE];
        snprintf(cloud_log_format, sizeof(cloud_log_format), "[File: %s Line: %d]: ", file, line);
        vsnprintf(cloud_log_format + strlen(cloud_log_format), sizeof(cloud_log_format) - strlen(cloud_log_format), format, args);
        log_upcall_func(severity, cloud_log_format);
        va_end(args);
    }
#endif

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
    }
}

#endif // ENABLE_TRACE