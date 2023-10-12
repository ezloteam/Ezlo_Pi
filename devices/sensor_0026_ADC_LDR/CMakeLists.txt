file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "sensor_0026_ADC_LDR.c" "${config_src}"
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