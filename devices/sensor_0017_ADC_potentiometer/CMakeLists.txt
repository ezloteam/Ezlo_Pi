idf_component_register(SRCS "sensor_0017_ADC_potentiometer.c"
    INCLUDE_DIRS "."
    REQUIRES 
                trace 
                gpio_isr_service
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud_constants 
                ezlopi_valueformatter)