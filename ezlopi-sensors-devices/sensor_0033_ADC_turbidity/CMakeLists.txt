file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0033_ADC_turbidity.c"
                        INCLUDE_DIRS "."
                        PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver"
                       REQUIRES trace  ezlopi_actions ezlopi_hal ezlopi_timer ezlopi_devices ezlopi-cloud-constants gpio_isr_service ezlopi_cloud)