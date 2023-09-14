# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "device_0002_digitalOut_relay.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace ezlopi_actions  ezlopi_timer ezlopi_devices ezlopi_cloud_constants gpio_isr_service)
