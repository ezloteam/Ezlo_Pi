file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0043_ADC_GYML8511_UV_intensity.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES        trace 
                    ezlopi-core-timer 
                    ezlopi-core-cloud 
                    ezlopi-core-actions 
                    ezlopi-core-devices 
                    ezlopi-core-valueformatter 

                    ezlopi-hal-adc

                    ezlopi-cloud-items 
                    ezlopi-cloud-constants 
                    )