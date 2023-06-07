# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "digital_io.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace library ezlopi_actions  ezlopi_timer ezlopi_devices ezlopi_cloud ezlopi_cloud_constants gpio_isr_service)
