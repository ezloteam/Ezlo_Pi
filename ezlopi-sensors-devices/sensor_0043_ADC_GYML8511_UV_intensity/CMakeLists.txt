file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0043_ADC_GYML8511_UV_intensity.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES  
                trace 
                gpio_isr_service
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi-cloud-constants
                ezlopi_valueformatter)