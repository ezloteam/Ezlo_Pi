#ifndef __GPIO_ISR_SERVICE_H__
#define __GPIO_ISR_SERVICE_H__

#include "ezlopi_devices_list.h"

void gpio_isr_service_init(void);
void gpio_isr_service_register(s_ezlopi_device_properties_t *properties, void (*__upcall)(s_ezlopi_device_properties_t *properties));

#endif // __GPIO_ISR_SERVICE_H__
