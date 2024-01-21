
idf_component_register(SRCS "sensor_0053_UART_GYGPS6MV2.c" "gyGPS6MV2.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                 
                ezlopi-core-actions 
                 
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-cloud-constants 
                )