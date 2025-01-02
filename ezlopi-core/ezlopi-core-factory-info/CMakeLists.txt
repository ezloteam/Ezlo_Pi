file(GLOB_RECURSE config_src "./*.c")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-nvs
        ezlopi-user-config
)

idf_component_register(SRCS  "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES ${EZLOPI_CORE_COMPONENTS})
