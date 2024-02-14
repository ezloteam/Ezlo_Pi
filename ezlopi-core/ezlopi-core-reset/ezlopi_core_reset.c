#include <esp_system.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_service_webprov.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_util_trace.h"

void EZPI_CORE_reboot(void)
{
    web_provisioning_deinit();
    esp_restart();
}

void EZPI_CORE_factory_restore(void)
{
    int ret = ezlopi_factory_info_v3_factory_reset();
    if (ret)
    {
        TRACE_I("FLASH RESET WAS DONE SUCCESSFULLY");
    }

    ret = ezlopi_nvs_factory_reset();
    if (ret)
    {
        TRACE_I("NVS-RESET WAS DONE SUCCESSFULLY");
    }

    TRACE_S("factory reset done, rebooting now .............................................");
    vTaskDelay(2000 / portTICK_RATE_MS);
    EZPI_CORE_reboot();
}