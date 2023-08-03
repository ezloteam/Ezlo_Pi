idf_component_register(SRCS "device_0038_digitalOut_RGB.c"
    INCLUDE_DIRS "."
    REQUIRES trace
            updated 
            gpio_isr_service 
             ezlopi_actions 
             ezlopi_hal 
             ezlopi_timer 
             ezlopi_devices 
             ezlopi_cloud_constants )