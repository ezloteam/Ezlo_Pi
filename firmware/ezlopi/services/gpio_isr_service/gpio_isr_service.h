#ifndef __GPIO_ISR_SERVICE_H__
#define __GPIO_ISR_SERVICE_H__

#include "ezlopi_devices_list.h"

typedef void(*f_interrupt_upcall_t)(s_ezlopi_device_properties_t*);

void gpio_isr_service_init(void);
void gpio_isr_service_register(s_ezlopi_device_properties_t *properties, f_interrupt_upcall_t __upcall, TickType_t debounce_ms);

#endif // __GPIO_ISR_SERVICE_H__
