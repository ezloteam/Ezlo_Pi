

idf_component_register(SRCS "ezlopi_service_system_temperature_sensor.c"
                        INCLUDE_DIRS .
                        REQUIRES    ezlopi-util-trace
                                    ezlopi-core-setting-commands
                                    ezlopi-core-devices
                                    ezlopi-cloud-constants
)

