file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0041_ADC_FC28_soilMoisture.c" "${config_src}"
                    INCLUDE_DIRS "."
                    REQUIRES   trace 
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi-cloud-constants
                                ezlopi_valueformatter
                                )
