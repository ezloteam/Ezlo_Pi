file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "joystick_2_axis.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace library ezlopi_actions ezlopi_hal ezlopi_timer ezlopi_devices ezlopi_cloud_constants gpio_isr_service)