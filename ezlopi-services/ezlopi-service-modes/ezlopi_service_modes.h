#ifndef __EZLOPI_SERVICE_MODES_H__
#define __EZLOPI_SERVICE_MODES_H__

void ezlopi_service_modes_init(void);

bool ezlopi_service_modes_stop(uint32_t wait_ms);
bool ezlopi_service_modes_start(uint32_t wait_ms);

#endif // __EZLOPI_SERVICE_MODES_H__
