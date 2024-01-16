file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0018_other_internal_hall_effect.c" "${config_src}"
                        INCLUDE_DIRS .
                        REQUIRES 
                        trace 
                        ezlopi_hal 
                        ezlopi_timer 
                        ezlopi_actions 
                        ezlopi_devices 
                        ezlopi_cloud_constants 
                        ezlopi_valueformatter
                        )
