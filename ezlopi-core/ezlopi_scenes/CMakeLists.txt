file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_scenes_v2.c" "ezlopi_scenes_edit.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace json ezlopi_devices lua ezlopi_meshbot_service)