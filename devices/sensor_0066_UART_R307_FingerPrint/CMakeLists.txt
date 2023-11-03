
idf_component_register(SRCS "sensor_0066_UART_R307_FingerPrint.c" "R307_AS606.c"
    INCLUDE_DIRS "."
    REQUIRES    trace 
                driver
                freertos
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud_constants )

