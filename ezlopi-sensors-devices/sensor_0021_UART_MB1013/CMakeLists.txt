file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "sensor_0021_UART_MB1013.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace ezlopi_actions ezlopi_hal ezlopi_timer ezlopi_devices ezlopi-cloud-constants gpio_isr_service)