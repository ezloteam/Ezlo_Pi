# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_scenes_scripts.c" "ezlopi_scenes_then_methods.c" "ezlopi_scenes_when_methods.c" "ezlopi_scenes_methods.c" "ezlopi_scenes.c" "ezlopi_scenes_operators.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace json ezlopi_devices )