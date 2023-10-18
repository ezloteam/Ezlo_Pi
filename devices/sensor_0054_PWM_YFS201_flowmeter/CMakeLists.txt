

idf_component_register(SRCS "sensor_0054_PWM_YFS201_flowmeter.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud_constants )

