file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0015_oneWire_DHT11.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES trace 
                                driver 
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi-cloud-constants 
                                gpio_isr_service
                                ezlopi_valueformatter)