# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_scenes_populate.c" "ezlopi_scenes_edit.c" "ezlopi_scenes_when_methods.c" "ezlopi_scenes_operators.c" "ezlopi_scenes_then_methods.c" "ezlopi_scenes_print.c" "ezlopi_scenes_cjson.c" "ezlopi_scenes_v2.c" "ezlopi_scenes_notifications.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace json ezlopi_devices lua
    core ezlopi_meshbot_service)