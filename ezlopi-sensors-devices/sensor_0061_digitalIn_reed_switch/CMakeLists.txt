

idf_component_register(SRCS "sensor_0061_digitalIn_reed_switch.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi-core-actions 
                 
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-cloud-constants 
                ezlopi-service-gpioisr)

