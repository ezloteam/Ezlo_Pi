file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "pms5003.c" "sensor_0014_UART_PMS5003.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace ezlopi_actions ezlopi_hal ezlopi_timer ezlopi_devices ezlopi_cloud_constants gpio_isr_service)