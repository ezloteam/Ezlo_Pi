file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0018_other_internal_hall_effect.c" "${config_src}"
                        INCLUDE_DIRS .
                        REQUIRES 
                        trace 
                         
                        ezlopi-core-timer 
                        ezlopi-core-actions 
                        ezlopi-core-devices 
                        ezlopi-cloud-constants 
                        ezlopi-core-valueformatter
                        )
