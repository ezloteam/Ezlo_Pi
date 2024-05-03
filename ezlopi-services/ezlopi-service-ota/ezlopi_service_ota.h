#ifndef _EZLOPI_SERVICE_OTA_H_
#define _EZLOPI_SERVICE_OTA_H_

#ifdef CONFIG_EZPI_ENABLE_OTA
void ezlopi_service_ota_init(void);
bool ezlopi_service_ota_get_busy_state(void);
#endif // CONFIG_EZPI_ENABLE_OTA

#endif // _EZLOPI_SERVICE_OTA_H_
