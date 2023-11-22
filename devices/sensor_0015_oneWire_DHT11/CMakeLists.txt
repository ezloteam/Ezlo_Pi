idf_component_register(SRCS "sensor_0015_oneWire_DHT11.c" "dht11.c"
                        INCLUDE_DIRS "."
                        REQUIRES trace 
                                driver 
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants 
                                gpio_isr_service
                                ezlopi_valueformatter)