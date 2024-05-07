


idf_component_register(SRCS "ezlopi_service_led_indicator.c"
                        INCLUDE_DIRS "."
                        REQUIRES    ezlopi-util-trace
                                    led_strip
                                    ezlopi-core-event-group
                                    ezlopi-core-ping
                                    ezlopi-core-processes
)


