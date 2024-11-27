#ifndef __EZLOPI_SERVICE_MODES_H__
#define __EZLOPI_SERVICE_MODES_H__

void ezlopi_service_modes_init(void);

bool ezlopi_service_modes_stop(uint32_t wait_ms);
bool ezlopi_service_modes_start(uint32_t wait_ms);
bool ezlopi_service_modes_device_alert_start(uint32_t wait_ms, const char *alert_loop_name);
bool ezlopi_service_modes_device_alert_stop(uint32_t wait_ms, const char *alert_loop_name);

#endif // __EZLOPI_SERVICE_MODES_H__
