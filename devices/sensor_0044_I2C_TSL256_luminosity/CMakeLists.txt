
idf_component_register(SRCS "sensor_0044_I2C_TSL256_luminosity.c" "TSL256_interface.c"
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud_constants 
                gpio_isr_service)

