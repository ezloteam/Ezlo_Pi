# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_COMPONENTS 
        trace
)

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES ${EZLOPI_COMPONENTS})