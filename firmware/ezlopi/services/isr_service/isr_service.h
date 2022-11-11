

#ifndef __ISR_SERVICE_H__
#define __ISR_SERVICE_H__


// #include "driver/gpio.h"
#include "ezlopi_devices.h"

typedef int(*f_interrupt_upcall_t)(s_ezlopi_device_properties_t*);

typedef struct s_isr_event_args{
    TickType_t isr_timer;
    s_ezlopi_device_properties_t *properties; 
    f_interrupt_upcall_t upcall;  
}s_isr_event_args_t;

void isr_service_init(s_ezlopi_device_properties_t *properties, f_interrupt_upcall_t upcall);

#endif // __ISR_SERVICE_H__



