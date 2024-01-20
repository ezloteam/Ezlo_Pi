#if 0
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "esp_sntp.h"

#include "trace.h"
#include "core_sntp.h"
#include "../../build/config/sdkconfig.h"

static time_t start_time = 0;

time_t sntp_core_get_up_time(void)
{
    return start_time;
}

static void sntp_sync_time_call_back(struct timeval *tv)
{
    sntp_set_sync_interval(60 * 60 * 1000);
    TRACE_I("Notification of a time synchronization event");

    time_t now;
    time(&now);
    if (0 == start_time)
    {
        start_time = now;
    }

    char strftime_buf[64];
    struct tm timeinfo;
    setenv("TZ", "UTC-5:45", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    TRACE_B("Time now[%ld]: %s", now, strftime_buf);
}

void core_sntp_init(void)
{
    TRACE_I("Initializing SNTP");
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_setservername(1, "ntp-b.nist.gov");
    esp_sntp_setservername(2, "ntp-wwv.nist.gov");
    sntp_set_time_sync_notification_cb(sntp_sync_time_call_back);

    sntp_set_sync_interval(10 * 1000);
    esp_sntp_init();
}

#endif