file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "dht22.c" "sensor_0016_oneWire_DHT22.c" "${config_src}"
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