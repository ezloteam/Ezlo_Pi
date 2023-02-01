file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "029_IR_blaster_remote.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace library ezlopi_actions  ezlopi_timer ezlopi_devices ezlopi_cloud_constants ir_blaster_tools)
