file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0043_ADC_GYML8511_UV_intensity.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES  
                trace 
                ezlopi-service-gpioisr
                ezlopi-core-actions 
                 
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-cloud-constants
                ezlopi-core-valueformatter)