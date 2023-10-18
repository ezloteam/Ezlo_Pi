

idf_component_register(SRCS "0061_sensor_digitalIn_reed_switch.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud_constants 
                gpio_isr_service)

