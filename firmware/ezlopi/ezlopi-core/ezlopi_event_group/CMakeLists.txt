# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_event_group.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace )


