


idf_component_register(SRCS "sensor_sound.c"
                        INCLUDE_DIRS .
                        REQUIRES trace ezlopi_actions ADS131M08 ezlopi_timer items gpio_isr_service)