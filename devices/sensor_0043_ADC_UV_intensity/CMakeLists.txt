idf_component_register(SRCS "sensor_0043_ADC_UV_intensity.c"
                        INCLUDE_DIRS "."
                        REQUIRES trace
                                ezlopi_actions
                                ezlopi_devices
                                ezlopi_cloud_constants
                                ezlopi_hal
                                ezlopi_timer)