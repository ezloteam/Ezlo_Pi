
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "../../build/config/sdkconfig.h"
#include "esp_sntp.h"
#include "zones.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_nvs.h"

static time_t start_time = 0;

static void sntp_sync_time_call_back(struct timeval* tv)
{

    char strftime_buf[64];
    struct tm timeinfo;

    sntp_set_sync_interval(60 * 60 * 1000); // Sync every hour
    TRACE_I("Notification of a time synchronization event");

    time_t now;
    time(&now);
    if (0 == start_time)
    {
        start_time = now;
    }

    localtime_r(&now, &timeinfo);

    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    TRACE_I("Time now[%ld]: %s", now, strftime_buf);
}

void EZPI_CORE_sntp_init(void)
{
    int retry = 0;
    const int retry_count = 10;

    TRACE_I("Initializing SNTP");
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    // esp_sntp_setservername(1, "ntp-b.nist.gov");
    esp_sntp_setservername(2, "ntp-wwv.nist.gov");
    sntp_set_time_sync_notification_cb(sntp_sync_time_call_back);

    // sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);

    sntp_set_sync_interval(15 * 1000);
    esp_sntp_init();

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        TRACE_I("Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

int EZPI_CORE_sntp_set_location(const char* location)
{
    int ret = 1;
    if (location)
    {
        if (EZPI_CORE_nvs_write_time_location(location, strlen(location)))
        {

            const char* posix_str = micro_tz_db_get_posix_str(location);

            if (NULL == posix_str)
            {
                TRACE_I("%s is not a known timezone!\n", location);
                ret = 0;
            }
        }
        else
        {
            ret = 0;
        }
    }
    else
    {
        ret = 0;
    }
    return ret;
}

char* EZPI_CORE_sntp_get_location(void)
{
    return EZPI_CORE_nvs_read_time_location();
}

void EZPI_CORE_sntp_get_local_time(char* time_buf, uint32_t buf_len)
{
    if (time_buf && buf_len)
    {
        char* location = EZPI_CORE_nvs_read_time_location();
        const char* posix_str = (location) ? micro_tz_db_get_posix_str(location) : NULL;

        if (!posix_str)
        {
            TRACE_E("Unknown time zone location or invalid location, setting GMT0!!");
            posix_str = "GMT0";
        }

        if (setenv("TZ", posix_str, 1) != 0)
        {
            TRACE_E("Error setting time zone: %s", posix_str);
            return NULL;
        }
        else
        {
            tzset();

            TRACE_I("Timezone set: %s", posix_str);

            time_t now;
            struct tm timeinfo;

            time(&now);
            localtime_r(&now, &timeinfo);

            if (strftime(time_buf, buf_len, "%Y-%m-%dT%H:%M:%S%z", &timeinfo))
            {
                TRACE_I("The current date/time int %s is: %s", location ? location : posix_str, time_buf);
            }
            else
            {
                TRACE_E("Error - strftime");
            }
        }

        if (location)
        {
            free(location);
        }
    }
}

void EZPI_CORE_sntp_get_up_time(char* up_time_buf, uint32_t buf_len)
{
    if (up_time_buf && buf_len)
    {
        struct tm timeinfo;
        localtime_r(&start_time, &timeinfo);
        strftime(up_time_buf, buf_len, "%c", &timeinfo);
    }
}

void EZPI_CORE_sntp_epoch_to_iso8601(char* time_buf, uint32_t buf_len, time_t t)
{
    if (time_buf && buf_len)
    {
        struct tm* timeinfo;
        timeinfo = gmtime(&t);
        strftime(time_buf, buf_len, "%Y-%m-%dT%H:%M:%S+545", timeinfo);
        TRACE_I("Build Time: %s", time_buf);
    }
}

uint64_t EZPI_CORE_sntp_get_current_time_ms(void)
{
    time_t now;
    time(&now);
    return (now * 1000LL);
}