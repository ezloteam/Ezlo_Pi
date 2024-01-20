
idf_component_register(SRCS "sensor_0053_UART_GYGPS6MV2.c" "gyGPS6MV2.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                 
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi-cloud-constants 
                )