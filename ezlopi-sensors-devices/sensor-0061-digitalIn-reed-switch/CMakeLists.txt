

idf_component_register(SRCS "sensor_0061_digitalIn_reed_switch.c" 
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace 
                # ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter 
                ezlopi-core-errors
                
                ezlopi-hal-gpio
                
                ezlopi-cloud-items
                ezlopi-cloud-constants 
                
                ezlopi-service-gpioisr
                )

