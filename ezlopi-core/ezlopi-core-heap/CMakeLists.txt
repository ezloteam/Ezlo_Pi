
file(GLOB_RECURSE config_src "./*.c")

idf_component_register(SRCS  "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES
                ezlopi-util-trace
                ezlopi-user-config
                ezlopi-cloud-constants
)
