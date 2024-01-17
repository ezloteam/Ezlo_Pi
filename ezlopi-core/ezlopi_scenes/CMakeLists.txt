file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace json ezlopi_devices lua core ezlopi_meshbot_service)