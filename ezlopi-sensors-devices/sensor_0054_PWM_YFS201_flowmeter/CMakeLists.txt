

idf_component_register(SRCS "sensor_0054_PWM_YFS201_flowmeter.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi-core-actions 
                ezlopi-hal-pwm
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-cloud-constants )

