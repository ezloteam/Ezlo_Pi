#include <esp_system.h>

#include "ezlopi_service_webprov.h"

void ezlopi_reboot(void)
{
    web_provisioning_deinit();
    esp_restart();
}