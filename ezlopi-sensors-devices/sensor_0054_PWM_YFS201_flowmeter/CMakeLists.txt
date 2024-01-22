

idf_component_register(SRCS "sensor_0054_PWM_YFS201_flowmeter.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter 
                ezlopi-hal-pwm
                ezlopi-cloud-constants 
                )

