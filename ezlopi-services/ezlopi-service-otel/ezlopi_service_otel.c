#include "cjext.h"
#include "ezlopi_core_http.h"

static QueueHandle_t __telemetry_queue = NULL;

static void __otel_task(void *pv);
static void __post_telemetry_logs(char *telemetry_logs);

int ezlopi_service_otel_add_to_telemetry_queue(int telemetry_type, cJSON *cj_telemetry)
{
}

void ezlopi_service_otel_init(void)
{
    __telemetry_queue = xQueueCreate(10, sizeof(cJSON *));
    if (__telemetry_queue)
    {
        xTaskCreate(__otel_task, "otel-task", 4096, NULL, 3, NULL);
    }
}

static void __otel_task(void *pv)
{
    while (1)
    {
        cJSON *cj_telemetry = NULL;

        if (pdTRUE == xQueueReceive(__telemetry_queue, &cj_telemetry, portMAX_DELAY))
        {
            char *telemetry_str = cJSON_PrintBuffered(__FUNCTION__, cj_telemetry, 4096, false);
            cJSON_Delete(__FUNCTION__, cj_telemetry);

            if (telemetry_str)
            {
                int telemetry_type = 0; // need to define

                switch (telemetry_type)
                {
                case 0:
                {
                    __post_telemetry_logs(telemetry_str);
                    break;
                }
                case 0:
                {
                    __post_telemetry_traces(telemetry_str);
                    break;
                }
                case 0:
                {
                    __post_telemetry_matrics(telemetry_str);
                    break;
                }

                default:
                {
                    break;
                }
                }

                ezlopi_free(__FUNCTION__, telemetry_str);
            }
        }

        vTaskDelay(10 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

static void __post_telemetry_logs(char *telemetry_logs)
{
    static const char *default_log_path = "/v1/logs";

    // char tmp_url[64];
    // snprintf(tmp_url, sizeof(tmp_url), "%s/%s", CONFIG_EZPI_OTEL_COLLECTOR_ADDRESS, default_log_path);

    cJSON *cj_headers = cJSON_CreateObject(__FUNCTION__);
    if (cj_headers)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_headers, "Content-Type", "application/json");
    }

    ezlopi_http_post_request(CONFIG_EZPI_OTEL_COLLECTOR_ADDRESS, default_log_path, cj_headers, NULL, NULL, NULL);
}
