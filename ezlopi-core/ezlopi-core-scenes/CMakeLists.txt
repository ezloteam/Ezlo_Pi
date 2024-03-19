# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_core_scenes_then_methods.c" "ezlopi_core_scenes_status_changed.c" "ezlopi_core_scenes_v2.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES core 
)