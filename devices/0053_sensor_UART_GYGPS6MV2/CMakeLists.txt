
idf_component_register(SRCS "0053_sensor_UART_GYGPS6MV2.c" "gyGPS6MV2.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                library 
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud_constants 
                )