


idf_component_register(SRCS "sensor_door.c"
                        INCLUDE_DIRS .
                        REQUIRES trace ezlopi_devices ezlopi_actions gpio_isr_service)
