file(GLOB_RECURSE scene_src "*.c")

idf_component_register(SRCS "ezlopi_core_scenes_operators.c" "ezlopi_core_scenes_then_methods.c" "ezlopi_core_scenes_status_changed.c" "ezlopi_core_scenes_notifications.c" "ezlopi_core_scenes_expressions.c" "ezlopi_core_scenes_edit.c" "ezlopi_core_scenes_cjson.c" "ezlopi_core_scenes_v2.c" "ezlopi_core_scenes_print.c" "ezlopi_core_scenes_when_methods_helper_functions.c" "ezlopi_core_scenes_then_methods_helper_func.c" "ezlopi_core_scenes_when_methods.c" "ezlopi_core_scenes_scripts.c" "${scene_src}"
    INCLUDE_DIRS "."
    REQUIRES core 
)
