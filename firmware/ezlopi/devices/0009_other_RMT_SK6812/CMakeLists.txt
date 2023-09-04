

idf_component_register(SRCS "0009_other_RMT_SK6812.c"
                        INCLUDE_DIRS "."
                        REQUIRES trace 
                        ezlopi_actions 
                        ezlopi_hal 
                        ezlopi_timer 
                        ezlopi_devices 
                        ezlopi_cloud_constants 
                        gpio_isr_service
                        led_strip)

