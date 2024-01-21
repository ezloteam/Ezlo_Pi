#include <esp_system.h>

#include "web_provisioning.h"

void ezlopi_reboot(void)
{
    web_provisioning_deinit();
    esp_restart();
}