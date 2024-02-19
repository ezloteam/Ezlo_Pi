# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_core_scenes_when_methods.c" "ezlopi_core_scenes_cjson.c" "ezlopi_core_scenes_print.c" "ezlopi_core_scenes_value.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES core 
    )