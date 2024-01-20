idf_component_register(SRCS "sensor_0060_digitalIn_vibration_detector.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi-cloud-constants 
                gpio_isr_service)