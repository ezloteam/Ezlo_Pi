# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}" "ezlopi_scenes_scripts.c"
    INCLUDE_DIRS "."
    REQUIRES trace json ezlopi_devices lua
    core ezlopi_meshbot_service)