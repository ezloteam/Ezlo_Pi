#include <esp_system.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_service_webprov.h"
#include "ezlopi_service_ws_server.h"

#include "ezlopi_util_trace.h"


void EZPI_CORE_reset_reboot(void)
{
#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    ezlopi_service_web_provisioning_deinit();
#endif // CONFIG_EZPI_WEBSOCKET_CLIENT

#ifdef CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    ezlopi_service_ws_server_stop();
#endif // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER

    esp_restart();
}

void EZPI_CORE_reset_factory_restore(void)
{
    ezlopi_error_t ret = ezlopi_factory_info_v3_factory_reset();
    if (EZPI_SUCCESS == ret)
    {
        TRACE_I("FLASH RESET WAS DONE SUCCESSFULLY");
    }

    ret = ezlopi_nvs_factory_reset();
    if (EZPI_SUCCESS == ret)
    {
        TRACE_I("NVS-RESET WAS DONE SUCCESSFULLY");
    }

    TRACE_S("factory reset done, rebooting now .............................................");
    vTaskDelay(2000 / portTICK_RATE_MS);
    EZPI_CORE_reset_reboot();
}