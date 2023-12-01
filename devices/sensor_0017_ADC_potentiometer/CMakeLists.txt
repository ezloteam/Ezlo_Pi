file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0017_ADC_potentiometer.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud_constants 
                ezlopi_valueformatter)