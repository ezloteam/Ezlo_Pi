# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-actions
        ezlopi-core-errors
)


idf_component_register(SRCS "ezlopi_core_event_queue.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES ${EZLOPI_CORE_COMPONENTS})
