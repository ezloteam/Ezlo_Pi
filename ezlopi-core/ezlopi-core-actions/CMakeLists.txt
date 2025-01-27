file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_COMPONENTS
        ezlopi-util-trace
)

idf_component_register(SRCS "ezlopi_core_actions.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES ${EZLOPI_COMPONENTS})
