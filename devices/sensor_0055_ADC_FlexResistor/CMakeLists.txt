
idf_component_register(SRCS "sensor_0055_ADC_FlexResistor.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud_constants 
                gpio_isr_service)

