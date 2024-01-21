file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0017_ADC_potentiometer.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi-core-actions 
                 
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-cloud-constants 
                ezlopi-core-valueformatter)