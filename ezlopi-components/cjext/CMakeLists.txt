# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_COMPONENTS 
        ezlopi-util-trace
)

idf_component_register(SRCS "cjext.c" "cjext_utils.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES ${EZLOPI_COMPONENTS})