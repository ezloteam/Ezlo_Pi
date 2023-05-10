# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_ethernet.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace ezlopi_hal ezlopi_boards)