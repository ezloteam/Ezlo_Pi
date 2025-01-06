file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_core_event_group.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace
                ezlopi-core-errors
)


