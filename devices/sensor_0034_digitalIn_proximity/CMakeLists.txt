file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0034_digitalIn_proximity.c"
                        INCLUDE_DIRS "."
                        REQUIRES 
                                    trace 
                                    items
                                    gpio_isr_service
                                    ezlopi_actions 
                                    ezlopi_timer
                                    ezlopi_valueformatter)