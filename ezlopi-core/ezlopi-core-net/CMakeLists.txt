# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-ethernet 
        ezlopi-core-wifi 
        ezlopi-core-ping 
)

idf_component_register(SRCS "ezlopi_core_net.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES ${EZLOPI_CORE_COMPONENTS})