#ifndef __GPIO_ISR_SERVICE_H__
#define __GPIO_ISR_SERVICE_H__

#include "ezlopi_devices_list.h"

typedef void (*f_interrupt_upcall_t)(void *);

void gpio_isr_service_init(void);
void gpio_isr_service_register_v3(l_ezlopi_item_t *item, f_interrupt_upcall_t __upcall, TickType_t debounce_ms);

#if 0 // v2.x
void gpio_isr_service_register(s_ezlopi_device_properties_t *properties, f_interrupt_upcall_t __upcall, TickType_t debounce_ms);
#endif

#endif // __GPIO_ISR_SERVICE_H__
