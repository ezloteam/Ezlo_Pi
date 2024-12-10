file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0033_ADC_turbidity.c"
                        INCLUDE_DIRS "."
                        PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver"
                       REQUIRES     ezlopi-util-trace
                                    # ezlopi-core-timer
                                    ezlopi-core-cloud
                                    ezlopi-core-actions
                                    ezlopi-core-devices
                                    ezlopi-core-valueformatter
                                    ezlopi-core-errors

                                    ezlopi-hal-adc

                                    ezlopi-cloud-items
                                    ezlopi-cloud-constants
                                    )               
